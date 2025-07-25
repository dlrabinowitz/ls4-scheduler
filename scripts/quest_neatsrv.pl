#!/usr/bin/perl -w

# Server for NEAT-Yale communications
# 
# modified Jun 14 2003 by DLR to eliminate cammgr
# modification of camctl required.
#
# modified by DLR to take out call to
# camBiasOff  12-15-03
#
# modified by DLR to put Finger Positions and temperatures in FITS
# header on initialization. Finger Temperatures also put in after
# each exposure command
# DLR - 01-12-04


use Env;
use Cwd;
use FileHandle;
use IO::Socket;
use IO::Select;
use POSIX ":sys_wait_h";


sub docmd {
  my ($cmd) = @_;
#  printf STDERR "neatsrv.pl cmd: $cmd\n";
  my $ret = `$cmd`;
#  my $ret = system($cmd);
  return $ret;

}
######################################################################
# Routines for interacting with ccp
######################################################################

sub check_ccp {
    printf STDERR "camControl error set. Checking ccp status\n";
    
    my $wait = 120; #Time to wait for ccp to wake up
    sleep($wait);
    
    if (checkstatus()) {
	if ($nccp_starts >= $max_ccpstarts) {
	    printf STDERR "ERROR: Too many restarts\n";
	    return 1;
	}
	$nccp_starts = $nccp_starts + 1;
	printf STDERR "RESTARTING CCP\n";
        if ( $FAKE_OBS ) {
	   docmd "rsh -l quest quest7 start_ops.fake";
        } else {
	   docmd "rsh -l quest quest7 start_ops";
        }
	sleep(5);
	return checkstatus();
    }
    return 0;
}

sub checkstatus {
    # Check ccp status
    my $ret = docmd "echo STATUS | $QUESTOPSDIR/bin/camctl $OBSHOST";
    if ($ret =~ /DONE *([0-9]*.[0-9]*)/) {
	printf STDERR "ccp responding to status:\n$ret\n";
	return 0;
    } else {
	printf STDERR "ccp not responding to status:\n$ret\n";
	return 1
    }
}



sub camControl {
    my ($cmd) =  @_;
    printf STDERR "camControl command: $cmd\n";
    my $ret = docmd "echo $cmd | $QUESTOPSDIR/bin/camctl $OBSHOST";
#
#   if camctl reports an error in socket_transmit_loop and the max number 
#   of restarts has not been exceeded, then restart ccp. If the max 
#   number has been exceeded, then print a message and abort.
#
#    if ($ret =~ /error in socket_transmit_loop/) {
    if ($ret =~ /error in socket_transmit_loop/ ||
         $ret =~ /ERROR RESEND/) {
	printf STDERR "camControl:  $ret\n";
	
        $nccp_starts = $nccp_starts + 1;
        if ($nccp_starts >= $max_ccpstarts) {
	    printf STDERR "ERROR: Too many restarts. Aborting\n";
	    exit(-1); 
	}

# kill any ongoing read_camera or write_fits
#
	printf STDERR "Killing read_camera and write_fits\n";
        if ($p_read_camera > 0) {
#	 kill 9, $p_read_camera;
         system("$LS4_ROOT/bin/slay read_camera" );
        }
        if ($p_write_fits > 0) {
#	 kill 9, $p_write_fits;
         system("$LS4_ROOT/bin/slay write_fits" );
        }
        $p_read_camera = -1;
        $p_write_fits = -1;
#
	printf STDERR "RESTARTING CCP\n";
        if ( $FAKE_OBS ) {
	   docmd "rsh -l quest quest7 start_ops.fake";
        } else {
	   docmd "rsh -l quest quest7 start_ops";
        }
#
#       wait 5 seconds and see if ccp responds to status command
#
	sleep(5);
	printf STDERR "checking ccp status\n";
        if (checkstatus()){
          printf STDERR "RESTARTING CCP DID NOT HELP. ABORTING\n";
          exit(-1);
        }
#
#       resend the original command that failed
#   
	printf STDERR "resending command $cmd\n";
        $ret = docmd "echo $cmd | $QUESTOPSDIR/bin/camctl $OBSHOST";
#
#       If camctl stil reports an error in the socket_transmit_loop,
#       then print a message and abort.
#
        if ($ret =~ /error in socket_transmit_loop/) {
	   printf STDERR "camControl:  $ret\n";
	   printf STDERR "SAME PROBLEM ALL OVER. ABORTING\n";
           exit(-1);
        }
#
#       even if the command wa successful, set the return code to
#       -2 to to indicate that ccp was restarted.
#
        $ret = -2; 
    }
    printf STDERR "camControl:  $ret\n";
    return $ret;
}

sub fingControl {
    my ($cmd) =  @_;
    printf STDERR "fingControl command: $cmd\n";
    my $ret = docmd "echo $cmd | $QUESTOPSDIR/bin/fingctl $FINGHOST";
    if ($ret =~ /error in socket_transmit_loop/) {
 	printf STDERR "fingControl:  $ret\n";
 	return -1;
    }
    printf STDERR "fingControl:  $ret\n";
    return $ret;
}

sub tempControl {
    my ($cmd) =  @_;
    printf STDERR "tempControl command: $cmd\n";
#   my ($ret) = -100.0;
    my $ret = docmd "echo $cmd | $QUESTOPSDIR/bin/tempctl $TEMPHOST";
    if ($ret =~ /error in socket_transmit_loop/) {
	printf STDERR "tempControl:  $ret\n";
	return -1;
    }
    printf STDERR "tempControl:  $ret\n";
    return $ret;
}



######################################################################
# Camera initialization routines
######################################################################
# (This command should also make sure the fingers are in position for stares
#  and drop rates are set to zero)

sub camInit {

# Kill any old read/write processes

    if ($p_read_camera > 0) {
        system("$LS4_ROOT/bin/slay read_camera" );
#	kill 9, $p_read_camera;
    }
    if ($p_write_fits > 0) {
        system("$LS4_ROOT/bin/slay write_fits" );
#	 kill 9, $p_write_fits;
    }
    $p_read_camera = -1;
    $p_write_fits = -1;

# Refresh shared memory
    if (system("$LS4_ROOT/bin/check_shm $nccds 1 1")) {
	printf STDERR "check_shm failed: $!\n";
	return -1;
    }

# Check ccp?

    if ($camerr) {
	if (check_ccp()) {
	    return -1;
	} else {
	    $camerr = 0;
	}
    }


# Reset camera
 
    if ($ret = camReset()) {
	printf STDERR "camReset returns error.\n";
#	return -1;
    }

# Turn Amplifier Biases off on CCDs with glowing amplifiers
# They will be turned on automatically by the camera control
# program before each read out, and off again after each
# readout. Use camBiasOn to return to normal operation (bias
# voltages on all the time).
#
# Currently, the BiasOn option introduces a 10-sec overhead to the
# readout owing to the need to lower all 28 droplines to avoid noise
# pickup.

# if ($ret = camBiasOff()) {
#    printf STDERR "camBiasOff returns error.\n";
#    return -1;
# }
    
# Initialize headers
    if ($ret = headerInit()) {
	printf STDERR "headerInit returns error.\n";
	return -1;
    }
	
}

sub camBiasOn { 
    my $ret = camControl("BIASES_ON");
    if ($ret =~ /DONE *([0-9]*.[0-9]*)/) {
	return 0;
    }
    printf STDERR "camcontrol BIAS_ON returns %s\n",$ret;
    return -1;
}

sub camBiasOff { 
    my $ret = camControl("BIASES_OFF");
    if ($ret =~ /DONE *([0-9]*.[0-9]*)/) {
	return 0;
    }
    printf STDERR "camcontrol BIAS_OFF returns %s\n",$ret;
    return -1;
}

sub camReset { 
    my $ret = camControl("RESET");
    if ($ret =~ /DONE *([0-9]*.[0-9]*)/) {
	return 0;
    }
    return -1;
}
sub camSetPeriod { 
    my $ret = camControl("INIT 0");
    if ($ret =~ /DONE *([0-9]*.[0-9]*)/) {
	return 0;
    }
    return -1;
# reset all headers 
}

# Initialize header info
sub headerInit {
    if (system("$LS4_ROOT/bin/hinfo --offsets 0.0 --obsmode 'stare'")) {
	printf STDERR "hinfo failed: $!\n";
	return 1;
    }
    if (headerSetFingpos()) {
        printf STDERR "headerSetFingPos failed\n";
	return 1;
    }
#   if (headerSetTemps()) {
#       printf STDER "headerSetTemps failed\n";
#	return 1;
#  }

#    headerSetDrops;
#    headerSetReadout;
#    headerSetBiasvolts;
     return 0;
}

sub headerSetFingpos {
    
    my $fing = 1;
    my $fing_index=0;
    while ($fing <= $nfingers) {
	if (($pos = fingControl("WHERE $fing")) != -1) {
	    if (hinfoCommand("--finger $fing_index --fingpos $pos") == -1) {
		return(-1);
	    }
	    $fing_index++;
	    $fing++;
	} else {
	    return(-1);
	}
    }
    return(0);
}
	

#Just log temps in logfile for now since temp controller was returning 
# junk sometimes
sub headerSetTemps {
    
   my $fing = 1;
   my $fing_index = 0;
   while ($fing <= $nfingers) {
       $temp = tempControl("READ_TEMP $fing");
       $temp1 = substr($temp,0,6);
       printf STDERR "finger $fing temp = $temp1\n";
       if (hinfoCommand("--finger $fing_index --ccdtemp $temp1") == -1) {
	  return(-1);
       }
       $fing_index++;
       $fing++;



#
#	if (($temp = tempControl("READ_TEMP $fing")) != -1) {
#	    if (hinfoCommand("--finger $fing_index --ccdtemp $temp") == -1) {
#		return(-1);
#	    }
#            printf STDERR "finger $fing temp = $temp\n";
#	    $fing_index++;
#	    $fing++;
#	} else {
#	    return(-1);
#	}
   }
   return(0);
}
	

######################################################################
# Camera readout (and write) routines
#
# Tells ccp on CAMHOST to readout camera
sub camRead {
    my ($nlines) = @_;
    my $ret = camControl("READ $OBSHOST $nlines");
    printf STDERR "camcontrol READ returns %s\n",$ret;
    if ($ret =~ /DONE *([0-9]*.[0-9]*)/) {
	$stat = camStatus();
	printf STDERR "$stat\n";
	if ($stat =~ /([0-9]*) *([0-9]*) *([A-Z]*) *([A-Z]*) *([0-9]*.[0-9]*) *([0-9]*)  *([0-9]*.[0-9]*)/) {
	    return $5;
	} 
	$stat = camStatus();
	return -1;
    }
    $stat = camStatus();
    return -1;
}

# Forks call to read_camera on OBSHOST to read from socket into memory
sub read_camera {

    my ($nlines,$encode_flag) = @_;
    if ( $encode_flag == 1 ) {
       printf STDERR "$LS4_ROOT/bin/read_camera -b -s -l $nlines\n";
    } else {
       printf STDERR "$LS4_ROOT/bin/read_camera -s -l $nlines\n";
    }
    my $pid = 0;
    if ($pid = fork) {
	# parent returns with pid;
	printf STDERR "read_camera pid = %dd\n", $pid;
	return $pid;
    } elsif (defined $pid) {
        if ( $encode_flag == 1 ) {
	   exec "$LS4_ROOT/bin/read_camera -b -s -l $nlines";
	   #system ("$LS4_ROOT/bin/read_camera -b -s -l $nlines");
	   #printf STDERR "read_camera fork exitting\n";
	   #exit(0);
        } else {
	   exec "$LS4_ROOT/bin/read_camera -s -l $nlines";
	   #system("$LS4_ROOT/bin/read_camera -s -l $nlines");
	   #printf STDERR "read_camera fork exitting\n";
	   #exit(0);
        }
    }
    # if we got here there's a problem
    return 1;
}

# Forks call to write_fits on OBSHOST to read from memory and write to disk
sub write_fits {

    my ($nlines) = @_;
    printf STDERR "$LS4_ROOT/bin/write_fits -s -p $fileroot -n1  -l $nlines\n";
    my $pid = 0;
    if ($pid = fork) {
	# parent returns with pid;
	printf STDERR "write_fits pid = %d\n", $pid;
	return $pid;
    } elsif (defined $pid) {
	system("$LS4_ROOT/bin/write_fits -s -p $fileroot -n1  -l $nlines");
	printf STDERR "moving $fileroot to destdir \n";
        $wt1 = time();
	system ("mv $fileroot*.fits $destdir");
        $wt2 = time();
        $wt = $wt2 - $wt1;
	printf STDERR "move time = $wt\n";
	exit(0);
    }
    # if we got here there's a problem
    return 1;
}


######################################################################
# Routines called by socked commands
######################################################################
sub expose {
    my ($shutter,$exptime) = @_;

#    if ($exptime < 60) {
#        my $delay_time = 60 - $exptime;
#	printf STDERR "exposure time less than 60 seconds. Delaying exposure by $delay_time seconds\n";
#        sleep ($delay_time);
#    }  

    if ($shutter) {
	if (($t1=shutterOpen()) == -1) {
	    return -1;
	}
	printf STDERR "t1=$t1\n";
    } else {
	if (($t1=shutterClose()) == -1) {
	    return -1;
	}
	if (hinfoSet("shutter", "CLOSED")) {
	    return -1;
	}
    }
    $obstime = time();
    hinfoSet("scandate",$obstime);
    sleep($exptime);
    if (($t2 = shutterClose()) == -1) {
	return -1;
    }
    printf STDERR "t2=$t2\n";
    printf STDERR "t1=$t1\n";
    $exp= $t2-$t1;
    printf STDERR "exp=$exp\n";
    hinfoSet("exptime",$exp);

    if( $read_temps != 0 ) {
      if (headerSetTemps()) {
        printf STDERR "headerSetTemps failed\n";
	return 1;
      }
    }
    return($exp);
}

sub waitRead {

# Wait for readout of last exposure to complete 
    
#  get current time before starting wait loop. If time exceeds
#  current time by max_write_time, kill child process (write_camera first,
# then read_camera).
#
    $wt1 = time();
    $wt_prev = 0;
    $done=0;

    printf STDERR "p_write_fits = $p_write_fits\n";

    if ($p_write_fits > 0) {
	printf STDERR "Waiting for write_fits....\n";
 
        do { 
           $wt2 = time();
           $wt = $wt2 - $wt1;
	   $kid = waitpid $p_write_fits, &WNOHANG;
           if ( $kid == $p_write_fits || $kid < 0 ) {
               $done = 1;
           }
           elsif ( $wt > $max_write_time ) {
               printf STDERR "Wait time exceeded. Killing write_fits $p_write_fits\n";
               system("$LS4_ROOT/bin/slay write_fits");
               $done = -1;
           }
           elsif ( $wt > $wt_prev + 10 ) {
              printf STDERR "Still waiting $wt\n";
              $wt_prev = $wt
           }
        } until $done != 0;
 
	$wt2 = time();
	$wt = $wt2-$wt1;
	printf STDERR "T= $wt\n";

    }

    return($done);

}

sub readout {
#    my ($nlines, $fileroot) = @_;
    my ($nlines, $fileroot, $encode_flag) = @_;
    $root = $fileroot;
    $root =~ s|/.*/||;

# Make sure we're done writing last exposure;
    
#  get current time before starting wait loop. If time exceeds
#  current time by max_write_time, kill child process (write_camera first,
# then read_camera).
#
    $wt1 = time();
#
    if ($p_write_fits > 0) {
	printf STDERR "Waiting for write_fits....\n";
#
#       replace waitpid with loop that doesn't hang.
#	waitpid $p_write_fits, 0;
#
        $done=0;
        do { 
           $wt2 = time();
           $wt = $wt2 - $wt1;
	   $kid = waitpid $p_write_fits, &WNOHANG;
           if ( $kid == $p_write_fits || $kid < 0 ) {
               $done = 1;
           }
           elsif ( $wt > $max_write_time ) {
               printf STDERR "Wait time exceeded. Killing write_fits $p_write_fits\n";
#	       kill 9, $p_write_fits;
               system("$LS4_ROOT/bin/slay write_fits");
               $done = 1;
           }
        } until $done == 1;
#
	$wt2 = time();
	$wt = $wt2-$wt1;
	printf STDERR "T= $wt\n";
    }

    if ($p_read_camera > 0) {
	printf STDERR "Waiting for read camera....\n";
#
#       Replace waitpid with loop that doesn't hang.
#       If the wait time exceeds max_write_time, exit the wait loop,
#       and abort read_camera and write_fits.
#
#       This was the previous wait routine:
#	waitpid $p_read_camera, 0;
#
#       This is the new wait routine:
#
        $done=0;
        do { 
#          get time since wait loop was entered.
           $wt2 = time();
           $wt = $wt2 - $wt1;
#
#          Check if read_camera is done. If $kid = $p_read_camera
#          then read_camera has exited. If $kid < 0, read_camera
#          is not running.
#
	   $kid = waitpid $p_read_camera,&WNOHANG;
           if ( $kid == $p_read_camera || $kid < 0 ) {
               $done = 1;
           }
           elsif ( $wt > $max_write_time) {
               printf STDERR "Wait time exceeded. Killing read_camera $p_read_camera\n";
               system("$LS4_ROOT/bin/slay read_camera" );
#              kill 9, $p_read_camera;
               $done = 1;
           }
        } until $done == 1;
#
	$wt2 = time();
	$wt = $wt2-$wt1;
	printf STDERR "T= $wt\n";
    }
#


    hinfoSet("fileroot",$root);
    runHinfoCommands();
    if (system("$LS4_ROOT/bin/check_shm $nccds 1")) {
	printf STDERR "check_shm failed: $!\n";
	return -1;
    }
    $p_read_camera = read_camera($nlines,$encode_flag);
    if ($p_read_camera == 1)  {
	printf STDERR "read_camera failed: $!\n";
	return -1;
    }
    printf STDERR "read_camera launched: pid = $p_read_camera\n";
    $p_write_fits = write_fits($nlines, $fileroot);
    if ($p_write_fits == 1)  {
	printf STDERR "write_fits failed: $!\n";
        system("$LS4_ROOT/bin/slay read_camera" );
#	system("kill $p_read_camera");
	return -1;
    }
    printf STDERR "write_fits launched: pid = $p_write_fits\n";

    sleep(1);
    printf STDERR "Starting camRead.\n";
    if (($ret = camRead($nlines)) == -1) {
	printf STDERR "camRead failed $!\n";
        system("$LS4_ROOT/bin/slay read_camera" );
        system("$LS4_ROOT/bin/slay write_fits" );
#	system("kill $p_read_camera");
#	system("kill $p_write_fits");	
	return -1;
    }
    if (hinfoSet("shutter", "CLOSED")) {
	return -1;
    }
    printf STDERR "camRead returns %s\n",$ret;
    return $ret;
}
	

sub hinfoSet {
    my ($keyword, $value) = @_;
    if ($keyword eq 'dec') {
	hinfoCommand("--stareoffsets $value");
    }
    printf STDERR "$LS4_ROOT/bin/hinfo --$keyword $value\n";
    printf $hinfoFH "$LS4_ROOT/bin/hinfo --$keyword $value\n";
    if ( $keyword eq 'filtername' ) {
        hinfoCommand("--filtername $value --filtername $value --filtername $value --filtername $value");
    }
    return(0);
}

sub hinfoCommand {
    my ($command) = @_;
    printf STDERR "$LS4_ROOT/bin/hinfo $command\n";
    printf $hinfoFH "$LS4_ROOT/bin/hinfo $command\n"
}

sub openHinfoFile {
    $hinfo_commandfile = "hinfo.commands.$hinfo_filecount";
    printf STDERR "Opening $hinfo_commandfile\n";
    my $fh = new FileHandle "> $hinfo_commandfile"
	or die "Could not open $hinfo_commandfile:$!";
    $hinfo_filecount++;
    return $fh;
}

sub runHinfoCommands {
    printf STDERR "Running $hinfo_commandfile ...\n";
    $hinfoFH->close;
    if (system("chmod 755 $hinfo_commandfile")) {
	printf STDERR "Warning: could not chmod hinfo commandfile\n";
    } 
    if (system("$hinfo_commandfile")) {
	printf STDERR "Warning: could not exec hinfo commandfile\n";
    }
    printf STDERR "Ran commands, about to open new file ....\n";
    $hinfoFH = openHinfoFile();
    printf STDERR "filehandle is $hinfoFH\n";

}

# clear ccd 
sub camClear {
    my $ret = camControl("READ NULL $nlines");
    if ($ret =~ /DONE *([0-9]*.[0-9]*)/) {
#       return $1; 
	return "DONE 00000000"
    }
    return -1;
}


sub shutterOpen {
    my $ret = camControl("OPEN");
    if ($ret =~ /DONE *([0-9]*.[0-9]*)/) {
	if (hinfoSet("shutter", "OPEN") == -1) {
	    return -1;
	}
	return $1;
    }
    return -1;
}

sub shutterClose {
    my $ret = camControl("CLOSE");
    if ($ret =~ /DONE *([0-9]*.[0-9]*)/) {
	return $1;
    }
    return -1;
}

sub camStatus {
    my $ret = camControl("STATUS");
    if ($ret =~ /count: *(\S+).*lag: *(\S+).*state: *(\S+).*mode: *(\S+).*time: *(\S+).*code: *(\S+).*DONE *(\S+)/s) {
	$line_count = $1;
	$write_lag = $2;
	$shutter_state = $3;
	$camera_mode = $4;
	$read_time = $5;
	$error_code = $6;
	$ccp_time = $7;
	$status = "$line_count $write_lag $shutter_state $camera_mode $read_time $error_code $ccp_time";
	printf STDERR "$status\n";
	return $status;
    }
    return -1;
}

######################################################################
# Get socket routines
######################################################################
sub get_config_info {
    
    my $cfgfile = "$LS4_ROOT/config/quest_neat.cfg";
    my $neatcfg = new FileHandle "<  $cfgfile"
	or die "Could not open $cfgfile: $!";
    my $nmatch = 0;

    while(<$neatcfg>) {
	if(/^NEAT_COMMAND_PORT\s+(\S+)/) {
	    $neat_command_port = $1;
	    $nmatch++;
        } 
	if(/^NEAT_HOSTNAME\s+(\S+)/) {
	    $neat_hostname = $1;
	    $nmatch++;
        }
	if(/^TIMEOUT\s+(\S+)/) {
	    $timeout = $1/1000.;
	    $nmatch++;
        }
	if(/^MAX_WRITE_TIME\s+(\S+)/) {
	    $max_write_time= $1;
	    $nmatch++;
        }
    }
    if (!defined($neat_command_port)) {
	printf STDERR "Couldn't find NEAT_COMMAND_PORT in $cfgfile\n";
    }
    if (!defined($neat_hostname)) {
	printf STDERR "Couldn't find NEAT_HOSTNAME in $cfgfile\n";
    }
    if (!defined($timeout)) {
	printf STDERR "Couldn't find TIMEOUT in $cfgfile\n";
    }
    if (!defined($max_write_time)) {
	printf STDERR "Couldn't find MAX_WRITE_TIME in $cfgfile\n";
    }
    $neatcfg->close or die "Couldn't close neatcfg:$!\n";
	
    if ($nmatch != 4) {
	die "Ports undefined or incomplete config file";
    }
    return $nmatch;
}

sub open_neat_command_socket {
    $comsock = new IO::Socket::INET (
				     LocalHost =>	$neat_hostname,
				     LocalPort =>	$neat_command_port,
				     Proto =>	'tcp',
				     Listen =>	1,
				     Reuse =>	1,
				     );
    die "Could not create neat_command socket: $!\n" unless $comsock;
    $tsel = new IO::Select();
}

sub connect_neat_command_socket {
    $new_comsock = $comsock->accept();
    if (defined($new_comsock)) {
	$tsel->add($new_comsock);
	return 0;
    } else {
	return 1;
    }
}
    

sub close_neat_command_socket {
    $tsel->remove($new_comsock);
    close($new_comsock);
}

sub sel_write {
    my ($sel, $ret) = @_;
    if (@s=$sel->can_write($timeout)) {
	$w=$s[0];
	$ret_time = time();
	printf STDERR "RETURNING (%d) ==> $ret\n",$ret_time;
	my $diff = $ret_time - $cmd_time;
	printf STDERR "Time to return = $diff\n";
	print $w "$ret\n";
	
	return 0;
    } else {
	printf STDERR "timeout on socket write\n";
	return -1;
    }
}

sub sel_read {
    my ($sel) = @_;
    if (@s=$sel->can_read) {
	$w=$s[0];
    } else {
	printf STDERR "timeout on socket read\n";
    }
    return <$w>;
}

sub shutDown {
    close_neat_command_socket;

    exit(0);
}

######################################################################
##########################################################################
##########################################################################
# Main program
#
# Takes commands: i (init)
#                 o (open shutter)
#                 close (close shutter)
#                 c (clear ccd)
#                 s (status)
#                 x (shutdown)
#                 r nlines fileroot (read nlines to fileroot)
#                 e shutter exptime fileroot
#                 h keyword value (set header)
##########################################################################
##########################################################################
get_config_info;

if ($#ARGV != 1) {
   printf STDERR "syntax: quest_neatsrv.pl dest_subdir read_temps_flag[0 to skip/1 to read]\n";
   exit;
}
else{
    $dest_subdir = $ARGV[0];
    $read_temps = $ARGV[1];
    printf STDERR "Using host destination suddirectory $dest_subdir\n";
    if ( $read_temps == 0 ) {
         printf STDERR "Skipping temperature readout\n";
    }
}


# Perl magic variables setting input and output line separator to null:
$/ = "\0";
$\ = "\0";

# Run initialization sequence to make sure camera, disk, are ready

#if (system("$LS4_ROOT/gui/neatinit.tcl")) {
#    printf STDERR "Initialization failed.\n";
##    exit 1;
#}


## Use line below to make first write to neat data disks
#$tmpdata = "$QUESTNEATDATADIR/tmpdata";
#
# Otherwise, use the line below to write to local disk first, and
# then transfer for QUESTNEATDATADIR using the mv command.
$tmpdata = "$QUESTNEATTEMPDIR/tmpdata";
#
#
$destdir = "$QUESTNEATDATADIR/$dest_subdir";


if (! -e $tmpdata) {
  printf STDERR "neatsrv.pl: Creating directory $tmpdata\n";
  mkdir ("$tmpdata", 0777)
      or die "Could not create $tmpdata:$!";
}
chdir $tmpdata or die "Could not change to $tmpdata:$!";

$nccp_starts = 0;
$camerr = 0;
$max_ccpstarts = 10;
$nccds = 112;
$nfingers = 4;
$nlines = 2400;
$p_read_camera = -1;
$p_write_fits = -1;

open_neat_command_socket;
$hinfo_filecount=0;
$hinfoFH = openHinfoFile;

while(1) {
    $cmd_time=time();
    printf STDERR "neatsrv: waiting for socket connection: %d\n",$cmd_time;
    if (!connect_neat_command_socket) {
        $cmd_time=time();
         printf STDERR "neatsrv: reading next command: %d\n",$cmd_time;

	if (defined($msg=sel_read($tsel))) {

	    $cmd_time=time();
	    printf STDERR "COMMAND (%d)   ==> $msg\n",$cmd_time;
	    
	    if (($msg =~ /^i\0/) || ($msg =~ /^init\0/) ) {
		$ret = camInit;
		sel_write($tsel,$ret);
	    } elsif (($msg =~ /^o\0/) || ($msg =~ /^open shutter\0/) ) {
		$ret = shutterOpen;
		sel_write($tsel,$ret);
	    } elsif (($msg =~ /^close\0/) || ($msg =~ /^close shutter\0/) ) {
		$ret = shutterClose;
		sel_write($tsel,$ret);
	    } elsif (($msg =~ /^s\0/) || ($msg =~ /^status\0/) ) {
		$ret = camStatus;
		sel_write($tsel,$ret);
	    } elsif (($msg =~ /^w\0/) || ($msg =~ /^waitread\0/) ) {
		$ret = waitRead;
		sel_write($tsel,$ret);
	    } elsif (($msg =~ /^c\0/) || ($msg =~ /^clear\0/) ) {
		$ret = camClear;
	        sel_write($tsel,$ret);
	    } elsif (($msg =~ /^x\0/) || ($msg =~ /^shutown\0/) ) {
		shutDown();
	    } elsif ($msg =~ /^r\s+(\S+)\s+(\S+)\0/) {
		$n = $1;
		$fileroot = $2;
		$ret = readout($n,$fileroot,0);
		sel_write($tsel,$ret);
	    } elsif ($msg =~ /^h\s+(\S+)\s+(\S+.*)\0/) {
		$keyword = $1;
		$value = $2;
		hinfoSet($keyword,$value);
		sel_write($tsel,0);
	    } elsif ($msg =~ /^e\s+(\S+)\s+(\S+)\s+(\S+)\0/) {
		$shutter = $1;
		$exptime = $2;
		$fileroot = $3;
		if (($ret=expose($shutter,$exptime)) == -1) {
		    printf STDERR "expose returned -1";
		    sel_write($tsel,$ret);
		    sel_write($tsel,$ret);	    		    
		} else {
		    sel_write($tsel,$ret);	    
#                   if ($exptime == 150 ) {
#		       $ret=readout($nlines,$fileroot,1);
#                   }
#                   else {		    
		       $ret=readout($nlines,$fileroot,0);
#                   }
		    sel_write($tsel,$ret);	    
		}
	    } else {
		printf STDERR "server: bad command: $msg\n";
		sel_write($tsel,-1);
	    }
	} else {
    
           $cmd_time=time();
           printf STDERR "neatsrv: error reading next command: %d\n",$cmd_time;

        }

    } else {
    
        $cmd_time=time();
        printf STDERR "neatsrv: error waiting for connection: %d\n",$cmd_time;
  
    }
    close_neat_command_socket;
}



