/* scheduler_status.c

   routines to interpret camera status string.

*/

#include "scheduler.h"
#include <string.h>
#include <ctype.h>


char *state_name[NUM_STATES];

#define name (const char *[NUM_STATES])  {"NOSTATUS", "UNKNOWN", "IDLE", "EXPOSING", "READOUT_PENDING", "READING", "FETCHING", "FLUSHING", "ERASING", "PURGING", "AUTOCLEAR", "AUTOFLUSH", "POWERON", "POWEROFF", "POWERBAD", "FETCH_PENDING", "ERROR", "ACTIVE", "ERRORED"}


/*****************************************************/
int init_status_names()
{
  int i;
  for (i=0;i<NUM_STATES;i++){
      state_name[i]=(char *)malloc(1+sizeof(char)*strlen(name[i]));
      strcpy(state_name[i],name[i]);
  }
}

/*****************************************************/
/* parse  reply string to find status keyword and status value. If found, copy  
 * value to value_string and return length of value string.
 * If not found, return 0
*/
int get_value_string(char *reply, char *keyword, char *separator, char *value_string)
{
  char *strptr1, *strptr2;
  int n;

  strptr1 = NULL;
  strptr2 = NULL;
  n = 0;
  *value_string = 0;

  strptr1 = strstr(reply, keyword);
  if (strptr1 != NULL){
     strptr1 = strstr(strptr1,":");
     if (strptr1 != NULL) strptr2 = strstr(strptr1, "|");
  }
  if (strptr1 != NULL && strptr2 != NULL){
      strptr1 += 1;
      n=strptr2-strptr1;
      strncpy(value_string,strptr1,n);
      *(value_string + n) = 0;
  }
  else{
      fprintf(stderr,"ERROR: can not find keyword [%s] in status string [%s]\n",
             keyword,reply);
  }
  return n;
}

/*****************************************************/
/* convert a string to boolean  assuming the string is one of the following:
 * "True","true","TRUE","False","false", or "FALSE"
*/
bool string_to_bool(char *string)
{
  
  if(strstr(string,"True") != NULL || strstr(string,"true") != NULL ||
	strstr(string,"TRUE") != NULL){
    return True;
  }
  else if( strstr(string,"False") != NULL || strstr(string,"false") != NULL ||
	strstr(string,"FALSE") != NULL){
    return False;
  }
  else{
    fprintf(stderr,"WARNING: string [%s] does not express a boolean value\n",string);
    fflush(stderr);
    return False;
  }
}

/*****************************************************/
/* convert ascii string of zeros and ones to an integer*/
int binary_string_to_int(char *binaryString) 
{
    int result = 0;
    int len = strlen(binaryString);

    for (int i = 0; i < len; i++) {
        if (!isdigit(binaryString[i])) {
            return -1;
        }
        if (binaryString[i] != '0' && binaryString[i] != '1') {
	     fprintf(stderr,"ERROR: can not convert string [%s] to integer\n",binaryString);
	     fflush(stderr);
             return -1;
        }
        result = (result << 1) | (binaryString[i] - '0');
    }
    return result;
}

/*****************************************************/
/* find keyword in reply string and interpret value as  boolean */

bool get_bool_status(char *keyword, char *reply)
{
  char string[1024];
  int n;

  n = get_value_string(reply,keyword,"|",string);
  if (n>0) 
     return string_to_bool(string);
  else
     return False;
}

/*****************************************************/
/* look for specified keyword in reply string and copy the associastedi value
 * string  into the status string */

int  get_string_status(char *keyword, char *reply, char *status)
{
  char string[1024];
  int n;

  strcpy(status,"UNKNOWN");
  n = get_value_string(reply,keyword,"|",string);
  if (n>0) strcpy(status,string);

  return(n);
}

/*****************************************************/

/* parse the keyword values from the reply string and store in
 * Camera_Status record.
*/

int parse_status(char *reply,Camera_Status *status)
{
  int i;

  status->ready= get_bool_status("ready",reply);
  status->error= get_bool_status("error",reply);

  get_string_status("state",reply,status->state);
  get_string_status("comment",reply,status->comment);
  get_string_status("date",reply,status->date);

  for (i=0;i<NUM_STATES;i++){
     status->state_val[i]=-1;
     status->state_val[i]=binary_string_to_int(state_name[i]);
  }

  return (0);
}

/*****************************************************/

/* print the values in the specified Camera_Status record */
int print_camera_status(Camera_Status *status,FILE *output)
{
   fprintf(output,"date         : %s\n",status->date);  
   fprintf(output,"state        : %s\n",status->state);  
   fprintf(output,"comment      : %s\n",status->comment);  

   if (status->ready)
      fprintf(output,"ready        : %s\n","True");
   else
      fprintf(output,"ready        : %s\n","False");

   if (status->error)
      fprintf(output,"error        : %s\n","True");
   else
      fprintf(output,"error        : %s\n","False");

   int i;
   for (i=0;i<NUM_STATES;i++){
      fprintf(output,"%s        : %d\n",state_name[i],status->state_val[i]);
   }

   return(0);
}
