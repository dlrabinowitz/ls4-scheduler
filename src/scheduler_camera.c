/* scheduler_camera.c

   routines to allow scheduler to open a socket connection
   to the camera controller (neatsrv on quest17), send
   commands and read replies

*/

#include "scheduler.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#define MACHINE_NAME "pco-nuc"
#define COMMAND_PORT 5000  
#define STATUS_PORT 5001  


#define COMMAND_DELAY_USEC 100000 /* useconds to wait between commands */

/* first word in reply from telescope controller */
#define ERROR_REPLY "ERROR: 1"
#define DONE_REPLY "DONE"


/* Telescope Commands */

#define INIT_COMMAND "init"
#define OPEN_COMMAND "open shutter"
#define CLOSE_COMMAND "close shutter"
#define STATUS_COMMAND "status"
#define CLEAR_COMMAND "clear"
#define SHUTDOWN_COMMAND "shutdown"
#define READOUT_COMMAND "r" /* r num_lines fileroot */
#define HEADER_COMMAND "h" /* h keyword value */
#define EXPOSE_COMMAND "e" /* shutter exptime fileroot */

/* Timeout after 10 seconds if expecting quick response from
   a camera command */

#define CAMERA_TIMEOUT_SEC 10
#define READ_TIMEOUT_SEC 30

/* call to status command is used to wait for readout.
   Readout time is normally 33 sec. Timeout after 3 minutes */

#define CAMERA_STATUS_TIMEOUT_SEC 180
#define CAMERA_CLEAR_TIMEOUT_SEC 120
#define CAMERA_INIT_TIMEOUT_SEC 60 

/* error code or readtime exceeding these two values,
   respectively, indicate bad readout of camera */

#define BAD_ERROR_CODE 2
#define BAD_READOUT_TIME 60.0

typedef struct{
   char *command;
   char *reply;
   int timeout;
   sem_t *start_semaphore;
   sem_t *done_semaphore;
} Do_Command_Args;

// semaphores used to synchronize do_camera_command_thread
// with parent thread
sem_t command_done_semaphore; 
sem_t command_start_semaphore; 

/* set status_channel_active to True if ls4_ccp has been configured
 * to reply to status queries on a dedicated socket */

bool status_channel_active = False;

/* readout_pending will be True while an exposure is occuring  */
bool readout_pending = False;

extern int verbose;
extern int verbose1;

struct timeval t_exp_start;

Camera_Status cam_status;

/*****************************************************/


int take_exposure(Field *f, Fits_Header *header, double *actual_expt,
		    char *name, double *ut, double *jd,
		    bool wait_flag, int *exp_error_code)
{
    char command[MAXBUFSIZE],reply[MAXBUFSIZE];
    char filename[1024],date_string[1024],shutter_string[256],field_description[1024];
    char comment_line[1024];
    char s[256],code_string[1024],ujd_string[256],string[1024];
    struct tm tm;
    int e;
    double expt;
    int shutter;
    int timeout;
    pthread_t command_thread;

    int result = 0;
    *exp_error_code = 0;

    if(*actual_expt!=0.0){
         expt=*actual_expt*3600.0;
    }
    else {
         expt=f->expt*3600.0;
    }

    strcpy(name,"BAD");
    *name = 0;

    shutter = f->shutter;
    *ut=get_tm(&tm);
    *jd=get_jd();

    sprintf(ujd_string,"%14.6f",*jd);

    get_shutter_string(shutter_string,f->shutter,field_description);

    sprintf(date_string,"%04d%02d%02d %02d:%02d:%02d",
	tm.tm_year,tm.tm_mon,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);

    get_filename(filename,&tm,shutter);

    if(verbose){
         fprintf(stderr,
           "take_exposure: exposing %7.1f sec  shutter: %d filename: %s\n",
           expt,shutter,filename);
         fflush(stderr);
    }

    sprintf(string,"%d",f->n_done+1);

    if(update_fits_header(header,SEQUENCE_KEYWORD,string)<0){
          fprintf(stderr,"take_exposure: error updating fits header %s:%s\n",
		SEQUENCE_KEYWORD,string);
          return(-1);
    }
    else if(update_fits_header(header,IMAGETYPE_KEYWORD,field_description)<0){
          fprintf(stderr,"take_exposure: error updating fits header %s:%s\n",
		IMAGETYPE_KEYWORD,field_description);
          return(-1);
    }
    else if(update_fits_header(header,FLATFILE_KEYWORD,filename)<0){
          fprintf(stderr,"take_exposure: error updating fits header %s:%s\n",
		FLATFILE_KEYWORD,filename);
       return(-1);
    }

    /* if field is a sky field and there is a comment, then the string following
       the # sign should be the field id. Put this id string in the fieldid
       keyword of the fits header
    */

    /* update comment line in FITS header with comments from script record */

    if(strstr(f->script_line,"#")!=NULL){
        sprintf(comment_line,"                      ");
        sprintf(comment_line,"'%s",strstr(f->script_line,"#")+1);
        strcpy(comment_line+strlen(comment_line)-1,"'");
    }
    else{
        sprintf(comment_line,"                      ");
        sprintf(comment_line,"'no comment'");
    }
    if(update_fits_header(header,COMMENT_KEYWORD,comment_line)<0){
          fprintf(stderr,"take_exposure: error updating fits header %s:%s\n",
		COMMENT_KEYWORD,comment_line);
       return(-1);
    }

    if(verbose){
      fprintf(stderr,"take_exposure: imprinting fits header\n");
      fflush(stderr);
    }

    if(imprint_fits_header(header)!=0){
      fprintf(stderr,"take_exposure: could not imprint fits header\n");
      return(-1);
    }

    sprintf(command,"e %d %9.3f %s",shutter,expt,filename);

    if(verbose){
        fprintf(stderr,"take_exposure: %s sending command %s\n",date_string,command);
    }

    timeout = 2*expt + 30 ;


    /* initialize command semaphores to 0. 
     *
     * If do_camera_command thread executes the expose command (wait = False),
     * that thread will pause until the start semaphore is posted (changed to 1).
     * The thread will post the done semaphore when the expose command returns.
     *
     * If the thread is  not used, neither will be posted.
     *
    */
    sem_init(&command_start_semaphore,0,0);
    sem_init(&command_done_semaphore,0,0);
    readout_pending = True;

    /* If wait_flag is False, then launch a new thread (command_thread) to execute 
     * the call to do_camera_command. 
     *
     * Meanwhile, monitor the status of the camera. When the camera status show that  
     * the exposure time has elapsed (but the controller has not year read out the image),
     * return from the program.
     *
     * The command_thread will continue executing the exposure, and read out the
     * image when the exposure time has elapsed. It will post do the command_done_semaphore
     * just before it exists.
     *
     * If wait flag is True, do not launch a new thread. Just execute the call to 
     * to do_camera_command and wait for it to complete before returning. Set
     * exp_error_code appropriately if do_camera_command returns with an error.
    */


    if (! wait_flag){
       if(verbose){
         fprintf(stderr,"take_exposure: using a thread to execute the exposure command");
         fflush(stderr);
       }
       
       pthread_t thread_id;
       Do_Command_Args *command_args;

       command_args = (Do_Command_Args *)malloc(sizeof(Do_Command_Args));
       command_args->command = command;
       command_args->reply = reply;
       command_args->timeout = timeout;
       command_args->start_semaphore = &command_start_semaphore;
       command_args->done_semaphore = &command_done_semaphore;

       if(pthread_create(&thread_id,NULL,do_camera_command_thread,\
		       (void *)command_args)!=0){
         fprintf(stderr,"take_exposure: ERROR creating do_camera_command_thread\n");
         fflush(stderr);
	 return -1;
       }

       if(verbose){
           fprintf(stderr,"take_exposure: do_camera_command_thread running\n");
           fflush(stderr);
       }

       // post to command_start_semaphore. This tells the do_camera_command_thread to start
       // executing the exposre command
       sem_post(&command_start_semaphore);

       // record time at start of exposure
       gettimeofday(&t_exp_start, NULL);

       if(verbose){
           fprintf(stderr,"take_exposure: waiting for exposure time to end\n");
           fflush(stderr);
       }

       *actual_expt = wait_exp_done(expt);
       if (*actual_expt < 0){
          fprintf(stderr,"take_exposure: error waiting for exposure to end\n");
          fflush(stderr);
	  *actual_expt = 0;
	  return -1;
       }
       if(verbose){
           fprintf(stderr,"take_exposure: waited  %7.3f sec for exposure time to end\n",
                   *actual_expt);
           fflush(stderr);
       }
    }
    else{
       if(verbose){
         fprintf(stderr,"take_exposure: executing do_camera_command");
         fflush(stderr);
       }

       // record time at start of exposure
       gettimeofday(&t_exp_start, NULL);

       if(do_camera_command(command,reply,timeout)!=0){
         fprintf(stderr,"take_exposure: error sending exposure command : %s\n",command);
         fprintf(stderr,"take_exposure: reply was : %s\n",reply);
         *actual_expt=0.0;
         return -1;
       }
       else{
         sscanf(reply,"%lf",actual_expt);
       }

       readout_pending = False;

       if(verbose){
         fprintf(stderr,
           "take_exposure: exposure complete : filename %s  ut: %9.6f\n",
           filename,*ut);
         fflush(stderr);
       }
    }

    strncpy(name,filename,FILENAME_LENGTH);
    
    return(0);
}

/*****************************************************/

int get_filename(char *filename,struct tm *tm,int shutter)
{
    char shutter_string[3];
    int result;
    char field_description[1024];

    result=get_shutter_string(shutter_string,shutter,field_description);
    if(result!=0){
        fprintf(stderr,"get_filename: bad shutter code: %d\n",shutter);
        fflush(stderr);
    }

    sprintf(filename,"%04d%02d%02d%02d%02d%02d%s",
            tm->tm_year,tm->tm_mon,tm->tm_mday,
	    tm->tm_hour,tm->tm_min,tm->tm_sec,shutter_string);

    return(result);
}

/*****************************************************/

int imprint_fits_header(Fits_Header *header)
{
    char command[MAXBUFSIZE];
    char reply[MAXBUFSIZE];
    int i;

    for(i=0;i<header->num_words;i++){
      sprintf(command,"%s %s %s",
		HEADER_COMMAND,header->fits_word[i].keyword,
                header->fits_word[i].value);
      if(do_camera_command(command,reply,CAMERA_TIMEOUT_SEC)!=0){
        fprintf(stderr,
          "imprint_fits_header: error sending command %s\n",command);
        return(-1);
      }
    }

    return(0);
}
     
/*****************************************************/

/* wait for camera exposure to end while the command to take an
 * exposure occurs in a parallel thread (do_camera_command_thread). 
 *
 * The exposure state is determined by polling the camera status 
 * through a status socket distinct from the command socket.
 *
 * When the exposure has ended, it is then read out by
 * the controller. This may proceed in the parallel thread while
 * the main thread continues to perform other functions (such as
 * moving the telescope to the next position).
 *
 * The parallel thread posts to the done_semaphore just before it exits,
 * signifying the exposure command has completed.
*/
 
double wait_exp_done(int expt)
{
    struct timeval t_val;
    int t,t_start,t_end,timeout_sec;
    double act_expt;

    act_expt=0;
    timeout_sec = expt + 5;

    if(verbose){
         fprintf(stderr,"wait_exp_done: waiting for camera exposure to end\n");
         fflush(stderr);
    }

    sleep(expt);

    /* if the status channel is active, query the camera status to determine whenb
     * the exposure ends. Otherwise, just assume it has ended when the expected 
     * exposure time has been waited */

    if (status_channel_active){

	gettimeofday(&t_val,NULL);
	int t_start = t_val.tv_sec;
	int t_end = t_start + timeout_sec;

	t=t_start;
	int done = False;
	while (t < t_end && ! done){
	  if(update_camera_status(NULL)!=0){
	    fprintf(stderr,"wait_camera_readout: could not update camera status\n");
	    done = True;
	  }
	  else if (cam_status.state_val[EXPOSING] == ALL_NEGATIVE_VAL){
	     done = True;
	  }
	  else{
	    usleep(100000);
	    gettimeofday(&t_val,NULL);
	    t = t_val.tv_sec;
	  }
	}

	if (! done ){
	   if (t < t_end){
	     fprintf(stderr,"wait_exp_done: ERROR waiting for exposure to end\n");
	   }
	   else{
	     fprintf(stderr,"wait_exp_done: timeout waiting for exposure to end\n");
	   }
	   fflush(stderr);
	}
	else{

	   if(verbose){
	     fprintf(stderr,
	       "wait_camera_readout: exposure successfully ended in  %d sec\n",
		t - t_start);
	     fflush(stderr);
	   }
	}
	if( (!done) || cam_status.error){
	    fprintf(stderr,
		 "wait_exp_done: camera error\n");
	    print_camera_status(&cam_status,stderr);
	    fflush(stderr);
	    act_expt=-1.0;
	}
    }
    else{
        act_expt = expt;
    }

    return act_expt;

}

     
/*****************************************************/

int init_camera()
{
     char reply[MAXBUFSIZE];
     int result = 0;

     if(verbose){
       fprintf(stderr,"initializing camera\n");
       fflush(stderr);
     }

     if(do_camera_command(INIT_COMMAND,reply,CAMERA_INIT_TIMEOUT_SEC)!=0){
       fprintf(stderr,"error sending camera init command\n");
       result = -1;
     }
     else if (update_camera_status(NULL) != 0){
       fprintf(stderr,"error updating camera status\n");
       result = -1;
     }
     else if (cam_status.error){
       fprintf(stderr,"camera is in error state\n");
       result = -1;
     }
     else if(verbose){
       fprintf(stderr,"success initializing camera\n");
     }

     fflush(stderr);
     return (result);

}

/*****************************************************/

int update_camera_status(Camera_Status *status)
{
     char reply[MAXBUFSIZE];
     int error_flag;

     error_flag=0;

     if(do_status_command(STATUS_COMMAND,reply,CAMERA_STATUS_TIMEOUT_SEC)!=0){
       return(-1);
     }
     else {
       parse_status(reply,&cam_status);
       if (status != NULL) *status = cam_status;
     }

     return(0);

}
/*****************************************************/
// thread wrapper
//
void *do_camera_command_thread(void *args){

     int *result = malloc(sizeof(int));
     Do_Command_Args *command_args;
     int timeout_sec;
     char *command;
     char *reply;
     sem_t *done_semaphore;
     sem_t *start_semaphore;

     command_args = (Do_Command_Args *)args;
     command = command_args->command;
     reply = command_args->reply;
     timeout_sec =  command_args->timeout;
     start_semaphore = command_args->start_semaphore;
     done_semaphore = command_args->done_semaphore;


     *result = 0;

     if(verbose1){
          fprintf(stderr,"do_camera_command_thread: waiting for start_semaphore to post\n");
          fflush(stderr);
     }

     struct timespec t;
     t.tv_sec = timeout_sec;
     t.tv_nsec = 0;

     /* wait for the start_semaphore to post, or else timeout */
     if ( sem_timedwait(start_semaphore,&t) != 0){
        fprintf(stderr,"do_camera_command_thread: error waiting for start_semaphore to post\n");
	perror("timeout waiting for start_semaphore to post in do_camera_command");
        fflush(stderr);
        *result = -1;
     }
     /* once the start semaphore posts, execute the command and then post to the
      * done semaphre */
     else{
        *result = do_camera_command(command,reply, timeout_sec);
	sem_post(done_semaphore);
     }

     pthread_exit(result);
}

int do_status_command(char *command, char *reply, int timeout_sec){
    return do_command(command, reply, timeout_sec, STATUS_PORT);
}

int do_camera_command(char *command, char *reply, int timeout_sec){
    return do_command(command, reply, timeout_sec, COMMAND_PORT);
}

int do_command(char *command, char *reply, int timeout_sec, int port){

     int returnval = 0;

     if(verbose1){
          fprintf(stderr,"do_camera_command: sending command %s\n",command);
          fflush(stderr);
     }

     if(send_command(command,reply,MACHINE_NAME,port, timeout_sec)!=0){
         fprintf(stderr,
          "do_camera_command: error sending command %s\n", command);
         returnval = -1;
         fflush(stderr);
     }

     usleep(COMMAND_DELAY_USEC);

     if (returnval == 0){
       //if(strstr(reply,ERROR_REPLY)!=NULL || strlen(reply) == 0 ){
       if( strstr(reply,DONE_REPLY)==NULL || strlen(reply) == 0  || strstr(reply,"ERROR_REPLY") != NULL){
         fprintf(stderr,
            "do_camera_command: command [%s] returns error: %s\n", 
             command,reply);
         fflush(stderr);
	 returnval = -1;
       }
       else {
         if(verbose1){
           fprintf(stderr,"do_camera_command: reply was %s\n",reply);
           fflush(stderr);
         }
       }
     }


     return(returnval);

}
/*****************************************************/

/* wait for camera readout to end.
 *
 * It a readout is not pending, wait the the command_done_semaphore
 * to post. This signifies the exposure and readout have completed.
 * Return with result = 0 if no timeout or other error occurs while
 * waiting for the readout. Otherwise return -1.
 *
 * If no readout is pending, just return with result = 0
*/

int wait_camera_readout(Camera_Status *status)
{
    struct timeval t1,t2;
    int dt;
    int result=0;
    int timeout_sec = READ_TIMEOUT_SEC;

    if (readout_pending){
      gettimeofday(&t1,NULL);
      if(verbose){
	fprintf(stderr,"wait_camera_readout: waiting for readout to complete\n");
	fflush(stderr);
      }
      /* wait for the done_semphore to post, or else timeout */
      struct timespec t;
      t.tv_sec = timeout_sec;
      t.tv_nsec = 0;

      /* If a timeout or error occurs while waiting for ther done_semaphore to
       * post, return -1.
      */
      if ( sem_timedwait(&command_done_semaphore,&t) != 0){
        fprintf(stderr,"wait_camera_readout: error waiting for done_semaphore to post\n");
	perror("timeout waiting for done_semaphore to post in wait_camera_readout");
        fflush(stderr);
        result = -1;
      }
      /* Otherwise if done_semaphore posts, the readout is is done.  Set readout_pending to
       * False and return 0
      */
      else{
	readout_pending = False;
        gettimeofday(&t2,NULL);
        dt = t2.tv_sec - t1.tv_sec;
        if(verbose){
     	  fprintf(stderr,"wait_camera_readout: waited %d sec for readout to end\n",dt);
	  fflush(stderr);
        }
      }
    }
    else{
       if(verbose){
	  fprintf(stderr,"wait_camera_readout: no exposure currently reading out\n");
	  fflush(stderr);
       }
    }

    if(update_camera_status(NULL)!=0){
	result=-1;
	fprintf(stderr,"wait_camera_readout: could not update camera status\n");
	fflush(stderr);
    }
    else{
	*status  = cam_status;
    }

    return(result);

}

/*****************************************************/

int clear_camera()
{
     char reply[MAXBUFSIZE];


     if(do_camera_command(CLEAR_COMMAND,reply,CAMERA_CLEAR_TIMEOUT_SEC)!=0){
       return(-1);
     }
     else {
       return(0);
     }

}

/*****************************************************/

int bad_readout()
{
    if(cam_status.error_code>BAD_ERROR_CODE){
      fprintf(stderr,"bad_readout: bad error code %d\n",cam_status.error_code);
      return(1);
    }
    else if (cam_status.read_time>=BAD_READOUT_TIME){
      fprintf(stderr,"bad_readout: long readout time : %7.3f\n",
           cam_status.read_time);
      return(1);
    }

    return(0);
}

/*****************************************************/
