/* Utilities for Sky Calc 

       Modified by N. Ellman to make a callable library
*/

//#define EARTHQUAKE_DST_ON // define to keep DST going past 2nd Sunday in March owing to Govt
			  // decision after Mar 27 Earthquake 2010

/* SKY CALCULATOR PROGRAM

	John Thorstensen, Dartmouth College.

   This program computes many quantities frequently needed by
   the observational astronomer.  It is written as a completely
   self-contained program in standard c, so it should be
   very transportable; the only issue I know of that really affects
   portability is the adequacy of the double-precision floating
   point accuracy on the machine.  Experience shows that c compilers
   on various systems have idiosyncracies, though, so be sure
   to check carefully.

   This is intended as an observatory utility program; I assume the
   user is familiar with astronomical coordinates and nomenclature.
   While the code should be very transportable, I also
   assume it will be installed by a conscientious person who
   will run critical tests before it is released at a new site.
   Experience shows that some c compilers generate unforseen errors
   when the code is ported, so the output should be checked meticulously
   against data from other sites.

   The first part (the almanac) lists the phenomena for a single night (sunset,
   twilight, moonrise, mooset, etc.) in civil clock time.
   The rise-set and twilight times given are good
   to a minute or two; the moon ephemeris used for rise/set is good to
   +- 0.3 degrees or so; it's from the Astronomical Almanac's
   low precision formulae, (with topocentric corrections included).
   The resulting moon rise/set times are generally good to better than
   two minutes.  The moon coordinates for midnight and in the 'calculator
   mode' are from a more accurate routine and are generally better than
   1 arcmin.  The elevation of an observatory above its effective
   horizon can be specified; if it is non-zero, rise/set times are
   corrected approximately for depression of the horizon.

   After displaying the phenomena for one night, the program goes
   into a 'calculator mode', in which one can -

	- enter RA, dec, proper motion, epoch, date, time,
	     new site parameters, etc. ...

	- compute and display circumstances of observation for the
	   current parameters, including precessed coordinates,
	   airmass, interference from moon or twilight, parallactic
	   angle, etc; the program also gives calendar date in
	   both UT and local, Julian date, and barycentric corrections.

	- compute and display a table of airmasses (etc) at
	   hourly intervals through the night.  This is very useful
	   at the telescope.  Also, if one has a modest number of
	   objects, it may be convenient (using system utilities)
	   to redirect the output and print a hard copy of these
	   tables for ready reference.

	- compute and display galactic and ecliptic coordinates.

	- compute and display rough (of order 0.1 degree, but often
	  much better) positions of the major planets.

	- display the almanac for the current night.

    The program is self-contained.  It was developed on a VMS system,
   but should adapt easily to any system with a c compiler.  It has
   been ported to, and tested on, several popular workstations.

	** BUT CAUTION ... **
   Because many of the routines take a double-precision floating point
   Julian Date as their time argument, one must be sure that the machine
   and compiler carry sufficient mantissa to reach the desired accuracy.
   On VAX/VMS, the time resolution is of order 0.01 second.  This has also
   proven true on Sun and IBM workstations.

LEGALITIES:

   I make no guarantee as to the accuracy, reliability, or
   appropriateness of this program, though I have found it to be
   reasonably accurate and quite useful to the working astronomer.

   The program is COPYRIGHT 1993 BY JOHN THORSTENSEN.
   Permission is hereby granted for non-profit scientific or educational use.
   For-profit use (e. g., by astrologers!) must be through negotiated
   license.  The author requests that observatories and astronomy
   departments which install this as a utility notify the author
   by paper mail, just so I know how widely it is used.

   Credits:
    * The julian date and sidereal time routines were
    originally coded in PL/I by  Steve Maker of Dartmouth College.
    They were based on routines in the old American Ephemeris.
    * The conversion from julian date to calendar date is adapted
    from Numerical Recipes in c, by Press et al. (Cambridge University
    Press). I highly recommend this excellent, very useful book.


    APOLOGIES/DISCLAIMER:
    I am aware that the code here does not always conform to
    the best programming practices.  Not every possible error condition
    is anticipated, and no guarantee is given that this is bug-free.
    Nonetheless, most of this code has been shaken down at several
    hundred sites for several years, and I have never received any
    actual bug reports.  Many users have found this program
    to be useful.

    CHANGES SINCE THE ORIGINAL DISTRIBUTION ....

	The program as listed here is for the most part similar to that
	posted on the IRAF bulletin board in 1990.  Some changes
	include:

	01 In the original code, many functions returned structures, which
	   some c implementations do not like.  These have been eliminated.

	02 The original main() was extremely cumbersome; much of it has
	   been broken into smaller (but still large) functions.

	03 The hourly airmass includes a column for the altitude of the
	   sun, which is printed if it is greater than -18 degrees.

	04 The planets are included (see above).  As part of this, the
	   circumstances calculator issues a warning when one is within
	   three degrees of a major planet.  This warning is now also
	   included in the hourly-airmass table.

	05 The changeover from standard to daylight time has been rationalized.
	   Input times between 2 and 3 AM on the night when DST starts (which
	   are skipped over and  hence don't exist) are now disallowed; input
	   times between 1 and 2 AM on the night when DST ends (which are
	   ambiguous) are interpreted as standard times.  Warnings are printed
	   in both the almanac and calculator mode when one is near to the
	   changeover.

	06 a much more accurate moon calculation has been added; it's used
	   when the moon's coordinates are given explicitly, but not for
	   the rise/set times, which iterate and for which a lower precision
	   is adequate.

	07 It's possible now to set the observatory elevation; in a second
	   revision there are now two separate elevation parameters specified.
	   The elevation above the horizon used only in rise/set calculations
	   and adjusts rise/set times assuming the parameter is the elevation
	   above flat surroundings (e. g., an ocean).  The true elevation above
	   sea level is used (together with an ellipsoidal earth figure) in
	   determining the observatory's geocentric coordinates for use in
	   the topocentric correction of the moon's position and in the
	   calculation of the diurnal rotation part of the barycentric velocity
	   correction.  These refinements are quite small.

	08 The moon's altitude above the horizon is now printed in the
	   hourly airmass calculation; in the header line, its illuminated
	   fraction and angular separation from the object are included,
	   as computed for local midnight.

	09 The helio/barycentric corrections have been revised and improved.
	   The same routines used for planetary positions are used to
	   compute the offset from heliocentric to solar-system
	   barycentric positions and velocities.  The earth's position
	   (and the sun's position as well) have been improved somewhat
	   as well.

	10 The printed day and date are always based on the same truncation
	   of the julian date argument, so they should now always agree
	   arbitrarily close to midnight.

	11 A new convention has been adopted by which the default is that the
	   date specified is the evening date for the whole night.  This way,
	   calculating an almanac for the night of July 3/4 and then specifying
	   a time after midnight gives the circumstances for the *morning of
	   July 4*.  Typing 'n' toggles between this interpretation and a
	   literal interpretation of the date.

	12 The planetary proximity warning is now included in the hourly airmass
	   table.

	13 A routine has been added which calculates how far the moon is from
	   the nearest cardinal phase (to coin a phrase) and prints a
	   description.  This information is now included in both the almanac
	   and the calculator mode.

	14 The output formats have been changed slightly; it's hoped this
	   will enhance comprehensibility.

	15 A substantial revision affecting the user interface took place
	   in September of 1993.  A command 'a' has been added to the
	   'calculator' menu, which simply prints the almanac (rise, set,
	   and so on) for the current night.  I'd always found that it was
	   easy to get disoriented using the '=' command -- too much
	   information about the moment, not enough about the time
	   context.  Making the almanac info *conveniently* available
	   in the calculator mode helps your get oriented.

	   When the 'a' almanac is printed, space is saved over the
	   almanac printed on entry, because there does not need
	   to be a banner introducing the calculator mode.  Therefore some
	   extra information is included with the 'a' almanac; this includes
	   the length of the night from sunset to sunrise, the number of
	   hours the sun is below -18 degrees altitude, and the number of hours
	   moon is down after twilight.  In addition, moonrise and moonset
	   are printed in the order in which they occur, and the occasional
	   non-convergence of the rise/set algorithms at high latitude are
	   signalled more forcefully to the user.

	16 I found this 'a' command to be convenient in practice, and never
	   liked the previous structure of having to 'quit' the calculator
	   mode to see almanac information for a different night.  D'Anne
	   Thompson of NOAO also pointed out how hokey this was, especially the
	   use of a negative date to exit. So, I simply removed the outer
	   'almanac' loop and added a 'Q' to the main menu for 'quit'.  The
	   use of upper case -- for this one command only --  should guard
	   against accidental exit.

	17 The menu has been revised to be a little more readable.

	18 More error checking was added in Nov. 1993, especially for numbers.
	   If the user gives an illegal entry (such as a number which isn't
	   legal), the rest of the command line is thrown away (to avoid
	   having scanf simply chomp through it) and the user is prompted
	   as to what to do next.  This seems to have stopped all situations
	   in which execution could run away.  Also, typing repeated carriage
	   returns with nothing on the line -- which a frustrated novice
	   user may do because of the lack of any prompts -- causes a
	   little notice to be printed to draw attention to the help and menu
	   texts.

	19 I found in practice that, although the input parameters and
	   conditions are deducible *in principle* from such things as the
	   'a' and '=' output, it takes too much digging to find them.  So
	   I instituted an 'l' command to 'look' at the current parameter
	   values.  To make room for this I put the 'Cautions and legalities'
	   into the 'w' (inner workings) help text.  This looks as though
	   it will be be very helpful to the user.

	20 The more accurate moon calculation is used for moonrise and
	   moonset; the execution time penalty appears to be trivial.
	   Low precision moon is still used for the summary moon information
	   printed along with the hourly airmass table.

	21 A calculation of the expected portion of the night-sky
	   brightness due to moonlight has been added.  This is based on
	   Krisciunas and Schaefer's analytic fits (PASP, 1991).  Obviously,
	   it's only an estimate which will vary considerably depending on
	   atmospheric conditions.

	22 A very crude estimator of the zenith sky brightness in twilight
	   has been added.

	23 A topocentric correction has been added for the sun, in anticipation
	   of adding eclipse prediction.

	24 The code now checks for eclipses of the sun and moon, by making
	   very direct use of the predicted positions.  If an eclipse is
	   predicted, a notice is printed in print_circumstances; also, a
	   disclaimer is printed for the lunar sky brightness if a lunar
	   eclipse is predicted to be under way.

	25 In the driver of the main calculator loop, a provision has been
	   added for getting characters out of a buffer rather than reading
	   them directly off the command line.  This allows one to type any
	   valid command character (except Q for quit) directly after a number
	   in an argument without generating a complaint from the program
	   (see note 18).  This had been an annoying flaw.

	26 In 1993 December/1994 January, the code was transplanted
	   to a PC and compiled under Borland Turbo C++, with strict
	   ANSI rules.  The code was cut into 9 parts -- 8 subroutine
	   files, the main program, and an incude file containing
	   global variables and function prototypes.

	27 An "extra goodies" feature has been added -- at present it
	   computes geocentric times of a repeating phenomenon as
	   viewed from a site.  This can be used for relatively little-used
           commands to save space on the main menu.

	28 The sun and moon are now included in the "major planets"
	   printout.  This allows one to find their celestial positions
	   even when they are not visible from the selected site.

	29 A MAJOR new feature was added in February 1994, which computes
           the observability of an object at new and full moon over a
           range of dates.  The galactic/ecliptic coordinate converter
           was moved to the extra goodies menu to make room for this.

	30 Inclusion of a season-long timescale means that it's not
           always necessary to specify a date on entry to the program.
           Accordingly, the program immediately starts up in what used
           to be called "calculator" mode -- only the site is prompted
           for.  It is thought that the site will be relevant to nearly
           all users.

	31 Because the user is not led by the hand as much as before, the
           startup messages were completely revised to direct new users
           toward a short `guided tour' designed to show the program's
	   command structure and capabilities very quickly.  Tests on
	   volunteers showed that users instinctively go for anything
	   called the `menu', despite the fact that that's a slow way to
	   learn, so all mention of the menu option is removed from the
	   startup sequence; they'll catch on soon enough.

	32 Code has been added to automatically set the time and
           date to the present on startup.  A menu option 'T' has been
           added to set the time and date to the present plus a settable
           offset.  This should be very useful while observing.

	33 Because Sun machines apparently do not understand ANSI-standard
           function declarations, the code has been revised back to K&R
           style.  It's also been put back in a monolithic block for
           simplicity in distribution.

	34 The startup has been simplified still further, in that the
           coordinates are set automatically to the zenith on startup.
	   An 'xZ' menu item sets to the zenith for the currently specified
           time and date (not necessarily the real time and date.)

	35 Another MAJOR new capability was added in early 1994 --
           the ability to read in a list of objects and set the current
	   coordinates to an object on the list.  The list can be sorted
           in a number of ways using information about the site, date
           and time.

	35 Calculator-like commands were added to the extra goodies menu
           to do precessions and to convert Julian dates to calendar
           dates.  An option to set the date and time to correspond to
           a given julian date was also added.

	36 Another substantial new capability was added Aug 94 -- one can
           toggle open a log file (always named "skyclg") and keep
           a record of the output.  This is done simply by replacing
           most occurrences of "printf" with "oprintf", which mimics
           printf but writes to a log file as well if it is open.
	   This appears to slow down execution somewhat.

	37 12-degree twilight has been added to the almanac.  While the
	   awkward "goto" statements have been retained, the statement
           labels have been revised to make them a little clearer.
*/

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "sky_utils.h"
//#define FAKE_TELESCOPE

FILE *sclogfl = NULL;

int no_print=1; /* global variable. If = 1, shut off oprntf */
int no_print_prev = 1;

void oprntf(char *fmt, ...)

/* This routine should look almost exactly like printf in terms of its
   arguments (format list, then a variable number of arguments
   to be formatted and printed).  It is designed to behave just
   like printf (though perhaps not all format types are supported yet)
   EXCEPT that IF the globally-defined file pointer "sclogfl" is
   defined, IT ALSO WRITES TO THAT FILE using fprintf.  The skeleton
   for this came from Kernighan and Ritchie, 2nd edition, page 156 --
   their "minprintf" example.  I modified it to include the
   entire format string (e.g., %8.2f, %7d) and to write to the
   file as well as standard output.  */

{
	va_list ap;        /* see K&R for explanation of these macros */
	char *p, *sval;
	char outform[10];  /* an item's output format, e.g. %8.2f */
	char strout[150];
	int ival, i;
	short shval;
	char cval;
	double dval;

    if (no_print != no_print_prev ){
       fprintf(stderr,"oprntf: no_print has changed from %d to %d\n",no_print_prev,no_print);
       no_print_prev = no_print;
    }
    if(no_print==1)return;

	va_start(ap,fmt);
	for (p = fmt; *p; p++) {
		if (*p != '%') {
			putchar(*p);
/* overkill to put in these preprocessor flags, perhaps. */
#if LOG_FILES_OK == 1
			if(sclogfl != NULL) fputc(*p,sclogfl);
#endif
			continue;
		}
		i = 0;
		outform[i] = '%';
		p++;
		while(*p != 'd' && *p != 'f' && *p != 's' && *p != 'c' && 
		    *p != 'h') {
			outform[++i] = *p++;
		}
		switch (*p) {
		case 'd':
			ival = va_arg(ap, int);
			outform[++i] = *p;
			outform[++i] = '\0';
			printf(outform, ival);
#if LOG_FILES_OK == 1
			if(sclogfl != NULL)
				fprintf(sclogfl,outform,ival);
#endif
			break;
		case 'h':    /* signals short argument ... */
			shval = va_arg(ap, int);
			outform[++i] = 'd';
			outform[++i] = '\0';
			++p;  /* skip the 'd' in '%hd' */
			printf(outform, shval);
#if LOG_FILES_OK == 1
			if(sclogfl != NULL)
				fprintf(sclogfl,outform,shval);
#endif
			break;
		case 'c':
			/* cval = va_arg(ap, char); */
			cval = va_arg(ap, int);
			outform[++i] = *p;
			outform[++i] = '\0';
			printf(outform, cval);
#if LOG_FILES_OK == 1
			if(sclogfl != NULL)
				fprintf(sclogfl,outform,cval);
#endif
			break;
		case 'f':
			dval = va_arg(ap, double);
			outform[++i] = *p;
			outform[++i] = '\0';
			printf(outform, dval);
#if LOG_FILES_OK == 1
			if(sclogfl != NULL)
				fprintf(sclogfl,outform,dval);
#endif
			break;
		case 's':
			outform[++i] = *p;
			outform[++i] = '\0';
			i = 0;
			for (sval = va_arg(ap, char *); *sval; sval++) {
				strout[i++] = *sval;
			}
			strout[i] = '\0';
			printf(outform,strout);
#if LOG_FILES_OK == 1
			if(sclogfl != NULL) fprintf(sclogfl,outform,strout);
#endif
			break;
		default:
	                ;
		}
	}
    fflush(stderr);
    fflush(stdout);
	va_end(ap);
}

/* elements of K&R hp calculator, basis of commands */

char buf[BUFSIZE];
int bufp=0;


char getch() /* get a (possibly pushed back) character */
{
	return((bufp > 0) ? buf[--bufp] : getchar());
}

void ungetch(c) /* push character back on input */
	int c;
{
	if(bufp > BUFSIZE)
		printf("Ungetch -- too many characters.\n");
	else
		buf[bufp++] = c;
}

/* some functions for getting well-tested input. */

int legal_num_part(c)
	char c;

{
	if((c != '.') && (c != '-') && (c < '0' || c > '9'))
		return(-1);  /* not a legal number part */
	else return(0);
}

int legal_int_part(c)
	char c;

{
	if((c != '-') && (c < '0' || c > '9'))
		return(-1);  /* not a legal number part */
	else return(0);
}

int legal_command_char(c)
	char c;
{
	/* Allows more sophisticated argument checking by seeing if
	      a character appended to an argument is actually a
	      legal commmand. */
	switch(c) {
	  case '?': return(1);
		break;
	  case 'i': return(1);
		break;
	  case 'f': return(1);
		break;
	  case 'w': return(1);
		break;
	  case 'r': return(1);
		break;
	  case 'd': return(1);
		break;
	  case 'y': return(1);
		break;
	  case 't': return(1);
		break;
	  case 'T': return(1);
		break;
	  case 'n': return(1);
		break;
	  case 'g': return(1);
		break;
	  case 'e': return(1);
		break;
	  case 'p': return(1);
		break;
	  case 's': return(1);
		break;
	  case 'l': return(1);
		break;
	  case '=': return(1);
		break;
	  case 'a': return(1);
		break;
	  case 'h': return(1);
		break;
	  case 'o': return(1);
		break;
	  case 'm': return(1);
		break;
	  case 'c': return(1);
		break;
	  case 'x': return(1);
		break;
	  /* let's not allow 'Q' here! */
	  default: return(0);
	}
}

int parsedouble(char *s, double *d)
     /* return values 0 = ok, with number, 1 = found a valid command,
	but no number, and -1 = an error of some sort (unexpected char)*/
{
   short i=0, legal = 0;

   while((*(s+i) != '\0') && (legal == 0)) {
	if(legal_num_part(*(s+i)) == 0) i++;
	else if(legal_command_char(*(s+i)) == 1) {
		/* to allow command to follow argument without blanks */
		ungetch(s[i]);
		*(s+i) = '\0';  /* will terminate on next pass. */
	}
	else legal = -1;
   }

   if(legal == 0) {
	if(i > 0) {
		sscanf(s,"%lf",d);
		return(0);
	}
	else if (i == 0) { /* ran into a command character -- no input */
		*d = 0.;
		return(1);  /* ok, actually */
	}
   }
   else {
	printf("%s is not a legal number!! Try again!\n",s);
	return(-1);
   }
   return(0);
}


int getdouble(d,least,most,errprompt)

	double *d,least,most;
	char *errprompt;


{
    char s[30], buf[200], c;
    short success = -1, ndiscard = 0;

    scanf("%s",s);
    while(success < 0) {
	success = parsedouble(s,d);
	if((success == 0) && ((*d < least) || (*d > most))) {
	   printf("%g is out of range; allowed %g to %g -- \n",
			*d,least,most);
	   success = -1;
	}
	if(success < 0) {
	   /* if there's error on input, clean out the rest of the line */
	   ndiscard = 0;
	   while((c = getchar()) != '\n')  {
		buf[ndiscard] = c;
		ndiscard++;
	   }
	   if(ndiscard > 0) {
		buf[ndiscard] = '\0';  /* terminate the string */
		printf("Rest of input (%s) has been discarded.\n",buf);
	   }
	   printf("%s",errprompt);
	   printf("\nTry again:");
	   scanf("%s",s);
	}
    }
    return((int) success);
}

int parseshort(s,d)

	char *s;
	short *d;

{
   short i=0, legal = 0;

   while((*(s+i) != '\0') && (legal == 0)) {
	if(legal_int_part(*(s+i)) == 0) i++;
	else if(legal_command_char(*(s+i)) == 1) {
		/* to allow command to follow argument without blanks */
		ungetch(s[i]);
		*(s+i) = '\0';  /* will terminate on next pass. */
	}
	else legal = -1;
   }

   if(legal == 0) {
	if(i > 0) {
		sscanf(s,"%hd",d);
		return(0);
	}
	else if (i == 0) { /* ran into a command character -- no input */
		*d = 0.;
		return(1);  /* didn't get a number, but something else legal */
	}
   }
   else {
	printf("%s is not a legal integer number!! Try again!\n",s);
	return(-1);
   }
   return(0);
   
}

int getshort(d,least,most,errprompt)

	short *d,least,most;
	char *errprompt;

{
    char s[30];
    short success = -1, ndiscard = 0;
    char c, buf[200];

    scanf("%s",s);
    while(success < 0) {
	success = parseshort(s,d);
	if((success == 0) && ((*d < least) || (*d > most))) {
	   printf("%d is out of range; allowed %d to %d -- try again.\n",
			*d,least,most);
	   success = -1;
	}
	if(success < 0) {
	   /* if there's error on input, clean out the rest of the line */
	   ndiscard = 0;
	   while((c = getchar()) != '\n')  {
		buf[ndiscard] = c;
		ndiscard++;
	   }
	   if(ndiscard > 0) {
		buf[ndiscard] = '\0';  /* cap the string */
		printf("Rest of input (%s) has been discarded.\n",buf);
	   }
	   printf("%s",errprompt);
	   printf("Try again:");
	   scanf("%s",s);
	}
    }
    return( (int) success);
}


double bab_to_dec(bab)

	struct coord bab;

   /* converts a "babylonian" (sexigesimal) structure into
      double-precision floating point ("decimal") number. */
   {
   double x;
   x = bab.sign * (bab.hh + bab.mm / 60. + bab.ss / 3600.);
   return(x);
   }

void dec_to_bab (deci,bab)

	double deci;
	struct coord *bab;

   /* function for converting decimal to babylonian hh mm ss.ss */

{
   int hr_int, min_int;

   if (deci >= 0.) bab->sign = 1;
   else {
      bab->sign = -1;
      deci = -1. * deci;
   }
   hr_int = deci;   /* use conversion conventions to truncate */
   bab->hh = hr_int;
   min_int = 60. * (deci - bab->hh);
   bab->mm = min_int;
   bab->ss = 3600. * (deci - bab->hh - bab->mm / 60.);
}

short get_line(s)

	char *s;

/* gets a line terminated by end-of-line and returns number of characters. */
{
	char c;
	short i = 0;

	c = getchar(); /* get the first character */
	/* chew through until you hit non white space */
	while((c == '\n') || (c == ' ') || (c == '\t')) c = getchar();

	s[i]=c;
	i++;

	/* keep going til the next newline */
	while((c=getchar()) != '\n') {
		s[i]=c;
		i++;
	}
	s[i]='\0';  /* terminate with null */
	return(i);
}

double get_coord()

/* Reads a string from the terminal and converts it into
   a double-precision coordinate.  This is trickier than
   it appeared at first, since a -00 tests as non-negative;
   the sign has to be picked out and handled explicitly. */
/* Prompt for input in the calling routine.*/
{
   short sign;
   double hrs, mins, secs;
   char hh_string[6];  /* string with the first coord (hh) */
   char hh1[1];
   short i = 0;
   int end_in = 0;

   /* read and handle the hour (or degree) part with sign */

   scanf("%s",hh_string);
   hh1[0] = hh_string[i];

   while(hh1[0] == ' ') {
       /* discard leading blanks */
       i++;
       hh1[0] = hh_string[i];
   }

   if(hh1[0] == '-') sign = -1;

     else sign = 1;

   if((end_in = parsedouble(hh_string,&hrs)) < 0) {
	printf("Didn't parse correctly -- set parameter to zero!!\n");
	return(0.);
   }

   if(sign == -1) hrs = -1. * hrs;

   /* read in the minutes and seconds normally */
   if(end_in == 0)
       	end_in = getdouble(&mins,0.,60.,
	  "Give minutes again, then seconds; no further prompts.\n");
   else return(sign * hrs);
   if(end_in == 0) end_in = getdouble(&secs,0.,60.,
     "Give seconds again, no further prompts.\n");
   else if(end_in == 1) secs = 0.;
   return(sign * (hrs + mins / 60. + secs / 3600.));
}


void put_coords(deci, precision)

	double deci;
	short precision;

/* prints out a struct coord in a nice format; precision
   is a code for how accurate you want it.  The options are:
     precision = 0;   minutes rounded to the nearest minute
     precision = 1;   minutes rounded to the nearest tenth.
     precision = 2;   seconds rounded to the nearest second
     precision = 3;   seconds given to the tenth
     precision = 4;   seconds given to the hundredth
   The program assumes that the line is ready for the coord
   to be printed and does NOT deliver a new line at the end
   of the output. */

{

   double minutes;  /* for rounding off if necess. */
   struct coord out_coord, coords;
   char out_string[20];  /* for checking for nasty 60's */

   dec_to_bab(deci,&coords);  /* internally convert to coords*/

   if(precision == 0) {   /* round to nearest minute */
      minutes = coords.mm + coords.ss / 60.;
	   /* check to be sure minutes aren't 60 */
      sprintf(out_string,"%.0f %02.0f",coords.hh,minutes);
      sscanf(out_string,"%lf %lf",&out_coord.hh,&out_coord.mm);
      if(fabs(out_coord.mm - 60.) < 1.0e-7) {
	 out_coord.mm = 0.;
	 out_coord.hh = out_coord.hh + 1.;
      }
      if(out_coord.hh < 100.) oprntf(" ");  /* put in leading blanks explicitly
	  for 'h' option below. */
      if(out_coord.hh < 10.) oprntf(" ");
      if(coords.sign == -1) oprntf("-");
	else oprntf(" ");   /* preserves alignment */
      oprntf("%.0f %02.0f",out_coord.hh,out_coord.mm);
   }

   else if(precision == 1) {    /* keep nearest tenth of a minute */
      minutes = coords.mm + coords.ss / 60.;
	   /* check to be sure minutes are not 60 */
      sprintf(out_string,"%.0f %04.1f",coords.hh,minutes);
      sscanf(out_string,"%lf %lf",&out_coord.hh, &out_coord.mm);
      if(fabs(out_coord.mm - 60.) < 1.0e-7) {
	 out_coord.mm = 0.;
	 out_coord.hh = out_coord.hh + 1.;
      }
      if(out_coord.hh < 10.) oprntf(" ");
      if(coords.sign == -1) oprntf("-");
	else oprntf(" ");   /* preserves alignment */
      oprntf("%.0f %04.1f", out_coord.hh, out_coord.mm);
   }
   else if(precision == 2) {
	  /* check to be sure seconds are not 60 */
      sprintf(out_string,"%.0f %02.0f %02.0f",coords.hh,coords.mm,coords.ss);
      sscanf(out_string,"%lf %lf %lf",&out_coord.hh,&out_coord.mm,
	   &out_coord.ss);
      if(fabs(out_coord.ss - 60.) < 1.0e-7) {
	  out_coord.mm = out_coord.mm + 1.;
	  out_coord.ss = 0.;
	  if(fabs(out_coord.mm - 60.) < 1.0e-7) {
	      out_coord.hh = out_coord.hh + 1.;
	      out_coord.mm = 0.;
	  }
      }
      if(out_coord.hh < 10.) oprntf(" ");
      if(coords.sign == -1) oprntf("-");
	 else oprntf(" ");   /* preserves alignment */
      oprntf("%.0f %02.0f %02.0f",out_coord.hh,out_coord.mm,out_coord.ss);
   }
   else if(precision == 3) {
	  /* the usual shuffle to check for 60's */
      sprintf(out_string,"%.0f %02.0f %04.1f",coords.hh, coords.mm, coords.ss);
      sscanf(out_string,"%lf %lf %lf",&out_coord.hh,&out_coord.mm,
	   &out_coord.ss);
      if(fabs(out_coord.ss - 60.) < 1.0e-7) {
	  out_coord.mm = out_coord.mm + 1.;
	  out_coord.ss = 0.;
	  if(fabs(out_coord.mm - 60.) < 1.0e-7) {
	     out_coord.hh = out_coord.hh + 1.;
	     out_coord.mm = 0.;
	  }
      }
      if(out_coord.hh < 10.) oprntf(" ");
      if(coords.sign == -1) oprntf("-");
	 else oprntf(" ");   /* preserves alignment */
      oprntf("%.0f %02.0f %04.1f",out_coord.hh,out_coord.mm,out_coord.ss);
   }
   else {
      sprintf(out_string,"%.0f %02.0f %05.2f",coords.hh,coords.mm,coords.ss);
      sscanf(out_string,"%lf %lf %lf",&out_coord.hh,&out_coord.mm,
	   &out_coord.ss);
      if(fabs(out_coord.ss - 60.) < 1.0e-6) {
	 out_coord.mm = out_coord.mm + 1.;
	 out_coord.ss = 0.;
	 if(fabs(out_coord.mm - 60.) < 1.0e-6) {
	    out_coord.hh = out_coord.hh + 1.;
	    out_coord.mm = 0.;
	 }
      }
      if(out_coord.hh < 10.) oprntf(" ");
      if(coords.sign == -1) oprntf("-");
	 else oprntf(" ");   /* preserves alignment */
      oprntf("%.0f %02.0f %05.2f",out_coord.hh, out_coord.mm, out_coord.ss);
   }
}

void load_site(double *longit,double *lat,double *stdz,short *use_dst,
	       char *zone_name,char* zabr,double *elevsea,double *elev,
	       double *horiz,char *site_name)

/* sets the site-specific quantities; these are
		longit     = W longitude in decimal hours
		lat        = N latitude in decimal degrees
		stdz       = standard time zone offset, hours
		elevsea    = elevation above sea level (for absolute location)
		elev       = observatory elevation above horizon, meters
		horiz      = (derived) added zenith distance for rise/set due
				to elevation
		use_dst    = 0 don't use it
			     1 use USA convention
			     2 use Spanish convention
			     < 0 Southern hemisphere (reserved, unimplimented)
		zone_name  = name of time zone, e. g. Eastern
		zabr       = single-character abbreviation of time zone
		site_name  = name of site.  */

{
	short nch;
	char obs_code[3];  /* need only one char, but why not? */
	char errprompt[50];

    /* if site_name is preset to "DEFAULT", then use ESO La Silla
       (obs_code="e") as the site */

    if(strstr(site_name,"DEFAULT")!=NULL){
      obs_code[0]=DEFAULT_OBSCODE;
    }
    else if(strstr(site_name,"Fake")!=NULL ||
        strstr(site_name,"fake")!=NULL ||
        strstr(site_name,"FAKE")!=NULL){
      obs_code[0]=FAKE_OBSCODE;
    }

	if(obs_code[0] == 'x') {
		printf("No action taken.  Current site = %s.\n",site_name);
		return;
	}
	else if(obs_code[0] == 'k') {
		strcpy(site_name,"Kitt Peak [MDM Obs.]");
		strcpy(zone_name, "Mountain");
		*zabr = 'M';
		*use_dst = 0;
		*longit = 7.44111; /* decimal hours */
		*lat = 31.9533;    /* decimal degrees */
		*stdz = 7.;
		*elevsea = 1925.;  /* for MDM observatory, strictly */
		*elev = 700.;  /* approximate -- to match KPNO tables */
	}
	else if (obs_code[0] == 's') {
		strcpy(site_name, "Shattuck Observatory");
		strcpy(zone_name,"Eastern");
		*zabr = 'E';
		*use_dst = 1;
		*longit = 4.81889;
		*lat = 43.7033;
		*stdz = 5.;
		*elevsea = 183.;
		*elev = 0.;  /* below surrouding horizon */
	}
        else if (obs_code[0] == 'K') {
                strcpy(site_name, "Keele Observatory");
                strcpy(zone_name,"Greenwich");
                *zabr = 'G';
                *use_dst = 3;
                *longit = 0.15109;
                *lat = 53.0036;
                *stdz = 0.;
                *elevsea = 200.; /* From ordnance survey map */
                *elev = 0.;      /* In rolling hills */
        }
	else if (obs_code[0] == 'a') {
		strcpy(site_name, "Anglo-Australian Tel., Siding Spring");
		strcpy(zone_name, "Australian");
		*zabr = 'A';
		*use_dst = -2;
		*longit = -9.937739;
		*lat = -31.277039;
		*elevsea = 1149.;
		*elev = 670.;
		*stdz = -10.;
	}
	else if (obs_code[0] == 'e') {
		strcpy(site_name, "ESO_SCHMIDT");
		strcpy(zone_name, "Chilean");
		*zabr = 'C';
		*use_dst = -1;
		*longit = 4.7153;
		*stdz = 4.;
		*lat = -29.257;
		*elevsea = 2347.;
		*elev = 2347.; /* for ocean horizon, not Andes! */
		printf("\n\n** Will use daylght time, Chilean date conventions. \n\n");
		fflush(stdout);
	}
	else if (obs_code[0] == 'f') {
		strcpy(site_name, "Fake Site");
		strcpy(zone_name, "Fake Location");
		*zabr = 'F';
		*longit = 16.7153;
		*stdz = 16.;
		*lat = -29.257;
		*elevsea = 2347.;
		*elev = 2347.; /* for ocean horizon, not Andes! */
		printf("\n\n** Will use daylght time, Fakedate conventions. \n\n");
		fflush(stdout);
	}
	else if (obs_code[0] == 'b') {
		strcpy(site_name, "Black Moshannon Observatory");
		strcpy(zone_name, "Eastern");
		*zabr = 'E';
		*use_dst = 1;
		*longit = 5.20033;
		*lat = 40.92167;
		*elevsea = 738.;
		*elev = 0.;  /* not set */
		*stdz = 5.;
	}
	else if (obs_code[0] == 'd') {
		strcpy(site_name, "DAO, Victoria, BC");
		strcpy(zone_name, "Pacific");
		*zabr = 'P';
		*use_dst = 1;
		printf("\n\nWARNING: United States conventions for DST assumed.\n\n");
		*longit = 8.22778;
		*lat = 48.52;
		*elevsea = 74.;
		*elev = 74.;  /* not that it makes much difference */
		*stdz = 8.;
	}
        else if (obs_code[0] == 'H') {
		strcpy(site_name,"Harvard College Observatory");
		strcpy(zone_name,"Eastern");
		*zabr = 'E';
		*use_dst = 1;
		*longit = 4.742;
		*lat = 42.38;
		*elevsea = 0.;  /* small, anyhow */
		*elev = 0.;
		*stdz = 5.;
	}                   
	else if (obs_code[0] == 'h') {
		strcpy(site_name, "Mount Hopkins, Arizona");
		strcpy(zone_name, "Mountain");
		*zabr = 'M';
		*use_dst = 0;
		*longit = 7.39233;
		*lat = 31.6883;
		*elevsea = 2608.;
		*elev = 500.;  /* approximate elevation above horizon mtns */
		*stdz = 7.;
	}
	else if (obs_code[0] == 'l') {
		strcpy(site_name, "Lick Observatory");
		strcpy(zone_name, "Pacific");
		*zabr = 'P';
		*use_dst = 1;
		*longit = 8.10911;
		*lat = 37.3433;
		*elevsea = 1290.;
		*elev = 1290.; /* for those nice Pacific sunsets */
		*stdz = 8.;
	}
 	else if (obs_code[0] == 'm') {
		strcpy(site_name, "Mauna Kea, Hawaii");
		strcpy(zone_name, "Hawaiian");
		*zabr = 'H';
		*use_dst = 0;
		*longit = 10.36478;
		*lat = 19.8267;
		*elevsea = 4215.;
		*elev = 4215.;  /* yow! */
		*stdz = 10.;
	}
	else if (obs_code[0] == 'p') {
		strcpy(site_name, "Palomar Observatory");
		strcpy(zone_name, "Pacific");
		*zabr = 'P';
		*use_dst = 1;
		*longit = 7.79089;
		*lat = 33.35667;
		*elevsea = 1706.;
		*elev = 1706.;  /* not clear if it's appropriate ... */
		*stdz = 8.;
	}
        else if (obs_code[0] == 'P') {
                strcpy(site_name, "Pic du Midi, Pyrenees");
                strcpy(zone_name, "Central European");
                *zabr = 'C';
                *use_dst = 2;  /* Same as in Spain, I checked */
                *longit = -0.00967;
                *lat = 42.93667;
                *elevsea = 2865.;
                *elev = 2865.; /* OK for West; for East, it's more like 0 */
                *stdz = -1.;
        }
	else if (obs_code[0] == 'r') {
		strcpy(site_name, "Roque de los Muchachos");
		strcpy(zone_name, "pseudo-Greenwich");
		*zabr = 'G';
		*use_dst = 2;
		*longit = 1.192;
		*lat = 28.75833;
		*elevsea = 2326.;
		*elev = 2326.;
		*stdz = 0.;
	}
	else if (obs_code[0] == 't') {
		strcpy(site_name, "Cerro Tololo");
		strcpy(zone_name, "Chilean");
		*zabr = 'C';
		*use_dst = -1;
		*longit = 4.721;
		*lat = -30.165;
		*stdz = 4.;
		*elevsea = 2215.;
		*elev = 2215.; /* for ocean horizon, not Andes! */
		printf("\n\n** Will use daylght time, Chilean date conventions. \n\n");
		fflush(stdout);
	}
        else if (obs_code[0] == 'T') {
		strcpy(site_name,"McDonald Observatory");
		strcpy(zone_name,"Central");
		*zabr = 'C';
		*use_dst = 1;
		*longit = 6.93478;
                *lat = 30.6717;
                *elevsea = 2075;
                *elev = 1000.;  /* who knows? */
                *stdz = 6.;
        }
        else if (obs_code[0] == 'z') {
                strcpy(site_name, "SAAO, Sutherland");
                strcpy(zone_name, "South African");
                *zabr = 'S';
                *use_dst = 0; /* They don't use it */
                *longit = -1.387444;
                *lat = -32.37833;
                *elevsea = 1758.;
                *elev = 1000.;  /* Who knows? */
                *stdz = -2.;
        }
	else if (obs_code[0] == 'n') {
		printf("Enter new site parameters; the prompts give current values.\n");
		printf("(Even if current value is correct you must re-enter explicitly.)\n");
		printf("WEST longitude, (HOURS min sec); current value ");
		put_coords(*longit,3);
		printf(": ");
		*longit = get_coord();
		printf("Latitude, (d m s); current value ");
		put_coords(*lat,2);
		printf(": ");
		*lat = get_coord();
		printf("Actual elevation (meters) above sea level,");
		printf(" currently %5.0f:",*elevsea);
		strcpy(errprompt,"Because of error,");
		    /* situation is uncomplicated, so simple errprompt */
		getdouble(elevsea,-1000.,100000.,errprompt);
		printf("Effective elevation, meters (for rise/set),");
		printf(" currently %5.0f:",*elev);
		getdouble(elev,-1000.,20000.,errprompt); /* limits of approx. ... */
		printf("Site name (< 30 char): ");
		nch=get_line(site_name);
		printf("Std time zone, hours W; currently %3.0f :",*stdz);
		getdouble(stdz,-13.,13.,errprompt);
		printf("Time zone name, e. g., Central: ");
		nch = get_line(zone_name);
		printf("Single-character zone abbreviation, currently %c : ",*zabr);
		scanf("%c",zabr);
		printf("Type daylight savings time option --- \n");
		printf("   0  ... don't use it, \n");
		printf("   1  ... use United States convention for clock change.\n");
		printf("   2  ... use Spanish (Continental?) convention.\n");
		printf("  -1  ... use Chilean convention.\n");
		printf("  -2  ... use Australian convention.\n");
		printf("(Other options would require new code). Answer: --> ");
		getshort(use_dst,-100,100,errprompt);
	}
		else {
		printf("UNKNOWN SITE '%c' -- left as %s. Note input is case-sensitive.\n",
			      obs_code[0],site_name);
	}
	/* now compute derived quantity "horiz" = depression of horizon.*/
	*horiz = sqrt(2. * *elev / 6378140.) * DEG_IN_RADIAN;
}


double atan_circ(x,y)

	double x,y;

{
	/* returns radian angle 0 to 2pi for coords x, y --
	   get that quadrant right !! */

	double theta;

	if(x == 0.) {
		if(y > 0.) theta = PI / 2.;
		else if(y < 0.) theta = 3.* PI / 2.;
		else theta = 0.;   /* x and y zero */
	}
	else theta = atan(y/x);
	if(x < 0.) theta = theta + PI;
	if(theta < 0.) theta = theta + 2.* PI;
	return(theta);
}

void min_max_alt(lat,dec,min,max)

	double lat,dec,*min,*max;

{
	/* computes minimum and maximum altitude for a given dec and
	    latitude. */

	double x;
	lat = lat / DEG_IN_RADIAN; /* pass by value! */
	dec = dec / DEG_IN_RADIAN;
	x = cos(dec)*cos(lat) + sin(dec)*sin(lat);
	if(fabs(x) <= 1.) {
		*max = asin(x) * DEG_IN_RADIAN;
	}
	else oprntf("Error in min_max_alt -- arcsin(>1)\n");
	x = sin(dec)*sin(lat) - cos(dec)*cos(lat);
	if(fabs(x) <= 1.) {
		*min = asin(x) * DEG_IN_RADIAN;
	}
	else oprntf("Error in min_max_alt -- arcsin(>1)\n");
}

double altit(dec,ha,lat,az)

	double dec,ha,lat,*az;

/* returns altitude(degr) for dec, ha, lat (decimal degr, hr, degr);
    also computes and returns azimuth through pointer argument. */
{
	double x,y,z;
	dec = dec / DEG_IN_RADIAN;
	ha = ha / HRS_IN_RADIAN;
	lat = lat / DEG_IN_RADIAN;  /* thank heavens for pass-by-value */
	x = DEG_IN_RADIAN * asin(cos(dec)*cos(ha)*cos(lat) + sin(dec)*sin(lat));
	y =  sin(dec)*cos(lat) - cos(dec)*cos(ha)*sin(lat); /* due N comp. */
	z =  -1. * cos(dec)*sin(ha); /* due east comp. */
	*az = atan_circ(y,z) * DEG_IN_RADIAN;
	return(x);
}

double secant_z(alt)
	double alt;
{
	/* Computes the secant of z, assuming the object is not
           too low to the horizon; returns 100. if the object is
           low but above the horizon, -100. if the object is just
           below the horizon. */

	double secz;
	if(alt != 0) secz = 1. / sin(alt / DEG_IN_RADIAN);
	else secz = 100.;
	if(secz > 100.) secz = 100.;
	if(secz < -100.) secz = -100.;
	return(secz);
}

double ha_alt(dec,lat,alt)

	double dec,lat,alt;

{
	/* returns hour angle at which object at dec is at altitude alt.
	   If object is never at this altitude, signals with special
	   return values 1000 (always higher) and -1000 (always lower). */

	double x,coalt,min,max;

	min_max_alt(lat,dec,&min,&max);
	if(alt < min)
		return(1000.);  /* flag value - always higher than asked */
	if(alt > max)
		return(-1000.); /* flag for object always lower than asked */
	dec = (0.5*PI) - dec / DEG_IN_RADIAN;
	lat = (0.5*PI) - lat / DEG_IN_RADIAN;
	coalt = (0.5*PI) - alt / DEG_IN_RADIAN;
	x = (cos(coalt) - cos(dec)*cos(lat)) / (sin(dec)*sin(lat));
	if(fabs(x) <= 1.) return(acos(x) * HRS_IN_RADIAN);
	else {
		oprntf("Error in ha_alt ... acos(>1).\n");
		return(1000.);
	}
}

double subtend(ra1,dec1,ra2,dec2)

	double ra1,dec1,ra2,dec2;

  /*args in dec hrs and dec degrees */

{
	/* angle subtended by two positions in the sky --
	   return value is in radians.  Hybrid algorithm works down
	   to zero separation except very near the poles. */

	double x1, y1, z1, x2, y2, z2;
	double theta;

	ra1 = ra1 / HRS_IN_RADIAN;
	dec1 = dec1 / DEG_IN_RADIAN;
	ra2 = ra2 / HRS_IN_RADIAN;
	dec2 = dec2 / DEG_IN_RADIAN;
	x1 = cos(ra1)*cos(dec1);
	y1 = sin(ra1)*cos(dec1);
	z1 = sin(dec1);
	x2 = cos(ra2)*cos(dec2);
	y2 = sin(ra2)*cos(dec2);
	z2 = sin(dec2);
	theta = acos(x1*x2+y1*y2+z1*z2);
     /* use flat Pythagorean approximation if the angle is very small
	*and* you're not close to the pole; avoids roundoff in arccos. */
	if(theta < 1.0e-5) {  /* seldom the case, so don't combine test */
		if(fabs(dec1) < (PI/2. - 0.001) &&
		    fabs(dec2) < (PI/2. - 0.001))    {
			/* recycled variables here... */
			x1 = (ra2 - ra1) * cos((dec1+dec2)/2.);
			x2 = dec2 - dec1;
			theta = sqrt(x1*x1 + x2*x2);
		}
	}
	return(theta);
}

int get_pm(dec, mura, mudec)

double dec, *mura, *mudec;

{
	/* This gets the proper motions.  New routine
	(2/94) assumes that primary PM convention will
	be rate of change of right ascension in seconds
	of time per year.  Either one can be entered here,
	but the value passed out is seconds of time per year at the
        equator (i.e., rate of change of RA itself). */

	char pmtype[3];
        int status;

	printf("Note -- two different conventions for RA proper motion.\n");
	printf("Enter RA p.m., either as delta RA(sec) or arcsec per yr.:");
	scanf("%lf",mura);
        if(*mura != 0.) {
	  printf("Type s if this is RA change in time sec per yr,\n");
	  printf("or a if this is motion in arcsec per yr:");
	  scanf("%s",pmtype);
        }
	else pmtype[0] = 's';  /* if pm is zero, it doesn't matter. */

	if(pmtype[0] == 's') status = 0;
	else if (pmtype[0] == 'a') {
		*mura = *mura /(15. * cos(dec / DEG_IN_RADIAN));
		printf("Equivalent to %8.5f sec of time per yr. at current dec.\n",
		     *mura);
                               printf("(Will only be correct at this dec.)\n");
                status = 0;
	}
	else {
		printf("UNRECOGNIZED PM TYPE ... defaults to sec/yr\n");
		status = -1;
        }
	printf("Give declination PM in arcsec/yr:");
	scanf("%lf",mudec);
	printf("(Note: Proper motion correction only ");
	printf("by mu * delta t; inaccurate near pole.)\n");
	printf("Don't forget to reset for new object.\n");

        return(status);  /* 0 = success */
}


int get_date(date)

	struct date_time *date;
{

	int valid_date = 0;

	while(valid_date == 0) {
	 	getshort(&(date->y),-10,2100,
           	   "Give year again, then month and day.\n");
		if(date->y <= 0) return(-1);

	/* scan for mo and day here, *then* error check. */
		getshort(&(date->mo),1,12,
		   "Give month again (as number 1-12), then day.\n");
		getshort(&(date->d),0,32,"Give day again.\n");
		/* a lot of this error checking is redundant with the
		    checks in the new getshort routine.... */
		if(date->y < 100)  {
			date->y = date->y + 1900;
			printf("(Your answer assumed to mean %d)\n",date->y);
		}
		else if((date->y <= 1900 ) | (date->y >= 2100)){
			printf("Date out of range: only 1901 -> 2099 allowed.\n");
			printf("Try it again!\n");
		}
		/* might be nice to allow weird input dates, but calendrical
		   routines will not necessarily handle these right ... */
		else if((date->d < 0) || (date->d > 32))
			printf("day-of-month %d not allowed -- try again!\n",
				date->d);
		else if((date->mo < 1) || (date->mo > 12))
			printf("month %d doesn't exist -- try again!\n",
				date->mo);
		else {
			valid_date = 1;
			return(0);  /* success */
		}
	}
	return(0);
	
}

int get_time(date)

	struct date_time *date;

{

	struct coord ttime;

	dec_to_bab(get_coord(),&ttime);
	date->h = (short) ttime.hh;
		/* awkward, because h and m of date are short. */
	date->mn = (short) ttime.mm;
	date->s = ttime.ss;
	return(0);
}

double date_to_jd(struct date_time date)

/* Converts a date (structure) into a julian date.
   Only good for 1900 -- 2100. */

{
	short yr1=0, mo1=1;
	long jdzpt = 1720982, jdint, inter;
	double jd,jdfrac;


	if((date.y <= 1900) | (date.y >= 2100)) {
		printf("Date out of range.  1900 - 2100 only.\n");
		return(0.);
	}

	if(date.mo <= 2) {
		yr1 = -1;
		mo1 = 13;
	}

	jdint = 365.25*(date.y+yr1);  /* truncates */
	inter = 30.6001*(date.mo+mo1);
	jdint = jdint+inter+date.d+jdzpt;
	jd = jdint;
	jdfrac=date.h/24.+date.mn/1440.+date.s/SEC_IN_DAY;
	if(jdfrac < 0.5) {
		jdint--;
		jdfrac=jdfrac+0.5;
	}
	else jdfrac=jdfrac-0.5;
	jd=jdint+jdfrac;
	return(jd);
}

short day_of_week(jd)
	double jd;
{
	/* returns day of week for a jd, 0 = Mon, 6 = Sun. */

	double x;
	long i;
	short d;

	jd = jd+0.5;
	i = jd; /* truncate */
	x = i/7.+0.01;
	d = 7.*(x - (long) x);   /* truncate */
	return(d);
}


void caldat(jdin,date,dow)

	double jdin;
	struct date_time *date;
	short *dow;

#define IGREG 2299161

{
	/* Returns date and time for a given julian date;
	   also returns day-of-week coded 0 (Mon) through 6 (Sun).
	   Adapted from Press, Flannery, Teukolsky, &
	   Vetterling, Numerical Recipes in C, (Cambridge
	   University Press), 1st edn, p. 12. */

	int mm, id, iyyy;  /* their notation */
	long ja, jdint, jalpha, jb, jc, jd, je;
	float jdfrac;
	double x;

	jdin = jdin + 0.5;  /* adjust for 1/2 day */
	jdint = jdin;
	x = jdint/7.+0.01;
	*dow = 7.*(x - (long) x);   /* truncate for day of week */
	jdfrac = jdin - jdint;
	date->h = jdfrac * 24; /* truncate */
	date->mn = (jdfrac - ((float) date->h)/24.) * 1440.;
	date->s = (jdfrac - ((float) date->h)/24. -
			((float) date->mn)/1440.) * SEC_IN_DAY;

	if(jdint > IGREG) {
		jalpha=((float) (jdint-1867216)-0.25)/36524.25;
		ja=jdint+1+jalpha-(long)(0.25*jalpha);
	}
	else
		ja=jdint;
	jb=ja+1524;
	jc=6680.0+((float) (jb-2439870)-122.1)/365.25;
	jd=365*jc+(0.25*jc);
	je=(jb-jd)/30.6001;
	id=jb-jd-(int) (30.6001*je);
	mm=je-1;
	if(mm > 12) mm -= 12;
	iyyy=jc-4715;
	if(mm > 2) --iyyy;
	if (iyyy <= 0) --iyyy;
	date->y = iyyy;
	date->mo = mm;
	date->d = id;
}


void print_day(d)
	short d;

{
	/* prints day of week given number 0=Mon,6=Sun */
	char *days = "MonTueWedThuFriSatSun";
	char day_out[4];

	day_out[0] = *(days+3*d);
	day_out[1] = *(days+3*d+1);
	day_out[2] = *(days+3*d+2);
	day_out[3] = '\0';  /* terminate with null char */

	oprntf("%s",day_out);
}


void print_all(double jdin)
{
	/* given a julian date,
	prints a year, month, day, hour, minute, second */

	struct date_time date;
        int ytemp, dtemp; /* compiler bug workaround ... SUN
         and silicon graphics */
	char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char mo_out[4];
	double out_time;
	short dow;

	caldat(jdin,&date,&dow);

	print_day(dow);
	oprntf(", ");

	mo_out[0] = *(months + 3*(date.mo - 1));
	mo_out[1] = *(months + 3*(date.mo - 1) + 1);
	mo_out[2] = *(months + 3*(date.mo - 1) + 2);
	mo_out[3] = '\0';

	/* going through the rigamarole to avoid 60's */

	out_time = date.h + date.mn / 60. + date.s / 3600.;

	ytemp = (int) date.y;
	dtemp = (int) date.d;
	oprntf("%d %s %2d, time ",
		ytemp,mo_out,dtemp);
	put_coords(out_time,3);
}

void print_current(date,night_date,enter_ut)
        struct date_time date;
	short night_date, enter_ut;
{
	/* just prints out the date & time and a little statement
           of whether time is "local" or "ut".  Functionalized to
           compactify some later code. */

	double jd;

	jd = date_to_jd(date);
        if((night_date == 1) && (date.h < 12)) jd = jd + 1.0;
	print_all(jd);
	if(enter_ut == 0) oprntf(" local time.");
	else oprntf(" Universal time.");
}

void print_calendar(jdin,dow)

	double jdin;
	short *dow;

{
	/* given a julian date prints a year, month, day.
	   Returns day of week (0 - 6) for optional printing ---
	   idea is to do one roundoff only to eliminate possibility
	   of day/date disagreement. */

	struct date_time date;
	char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char mo_out[4];
	int ytemp, dtemp;  /* compiler bug workaround -- SUN and
           Silicon Graphics machines */

	caldat(jdin,&date,dow);
	mo_out[0] = *(months + 3*(date.mo - 1));
	mo_out[1] = *(months + 3*(date.mo - 1) + 1);
	mo_out[2] = *(months + 3*(date.mo - 1) + 2);
	mo_out[3] = '\0';
        ytemp = (int) date.y;
        dtemp = (int) date.d;
	oprntf("%d %s %d",ytemp,mo_out,dtemp);
}

void print_time1(jdin,prec)

	double jdin;
	short prec;

{
	struct date_time date;
	double temptime;
	short dow;

	caldat(jdin,&date,&dow);

	temptime = date.h + date.mn/60. + date.s/3600.;

	oprntf("%9.6lf",temptime); 
}

void print_time(jdin,prec)

	double jdin;
	short prec;

{
	/* given a julian day, prints time only;
	   special precision of "-1" prints only hours!  */
	struct date_time date;
	double temptime;
	short dow;

	caldat(jdin,&date,&dow);

	temptime = date.h + date.mn/60. + date.s/3600.;

	if(prec >= 0) put_coords(temptime,prec);
	else if(date.mn < 30) oprntf("%2.0hd hr",date.h);
	else oprntf("%2.0hd hr",(date.h+1)); /* round it up */
}

double frac_part(x)

	double x;
{
	long i;
	i = x;
	x = x - i;
	return(x);
}


double lst(jd,longit)

	double jd,longit;

{
	/* returns the local MEAN sidereal time (dec hrs) at julian date jd
	   at west longitude long (decimal hours).  Follows
	   definitions in 1992 Astronomical Almanac, pp. B7 and L2.
	   Expression for GMST at 0h ut referenced to Aoki et al, A&A 105,
	   p.359, 1982.  On workstations, accuracy (numerical only!)
	   is about a millisecond in the 1990s. */

	double t, ut, jdmid, jdint, jdfrac, sid_g;
	long jdin, sid_int;

	jdin = jd;         /* fossil code from earlier package which
			split jd into integer and fractional parts ... */
	jdint = jdin;
	jdfrac = jd - jdint;
	if(jdfrac < 0.5) {
		jdmid = jdint - 0.5;
		ut = jdfrac + 0.5;
	}
	else {
		jdmid = jdint + 0.5;
		ut = jdfrac - 0.5;
	}
	t = (jdmid - J2000)/36525;
	sid_g = (24110.54841+8640184.812866*t+0.093104*t*t-6.2e-6*t*t*t)/SEC_IN_DAY;
	sid_int = sid_g;
	sid_g = sid_g - (double) sid_int;
	sid_g = sid_g + 1.0027379093 * ut - longit/24.;
	sid_int = sid_g;
	sid_g = (sid_g - (double) sid_int) * 24.;
	if(sid_g < 0.) sid_g = sid_g + 24.;
	return(sid_g);
}

double adj_time(x)
	double x;

{
	/* adjusts a time (decimal hours) to be between -12 and 12,
	   generally used for hour angles.  */

	if(fabs(x) < 100000.) {  /* too inefficient for this! */
		while(x > 12.) {
			x = x - 24.;
		}
		while(x < -12.) {
			x = x + 24.;
		}
	}
	else oprntf("Out of bounds in adj_time!\n");
	return(x);
}

void lpmoon(jd,lat,sid,ra,dec,dist)

	double jd,lat,sid,*ra,*dec,*dist;

/* implements "low precision" moon algorithms from
   Astronomical Almanac (p. D46 in 1992 version).  Does
   apply the topocentric correction.
Units are as follows
jd,lat, sid;   decimal hours
*ra, *dec,   decimal hours, degrees
	*dist;      earth radii */
{

	double T, lambda, beta, pie, l, m, n, x, y, z, alpha, delta,
		rad_lat, rad_lst, distance, topo_dist;
	char dummy[40];  /* to fix compiler bug on IBM system */

	T = (jd - J2000) / 36525.;  /* jul cent. since J2000.0 */

	lambda = 218.32 + 481267.883 * T
	   + 6.29 * sin((134.9 + 477198.85 * T) / DEG_IN_RADIAN)
	   - 1.27 * sin((259.2 - 413335.38 * T) / DEG_IN_RADIAN)
	   + 0.66 * sin((235.7 + 890534.23 * T) / DEG_IN_RADIAN)
	   + 0.21 * sin((269.9 + 954397.70 * T) / DEG_IN_RADIAN)
	   - 0.19 * sin((357.5 + 35999.05 * T) / DEG_IN_RADIAN)
	   - 0.11 * sin((186.6 + 966404.05 * T) / DEG_IN_RADIAN);
	lambda = lambda / DEG_IN_RADIAN;
	beta = 5.13 * sin((93.3 + 483202.03 * T) / DEG_IN_RADIAN)
	   + 0.28 * sin((228.2 + 960400.87 * T) / DEG_IN_RADIAN)
	   - 0.28 * sin((318.3 + 6003.18 * T) / DEG_IN_RADIAN)
	   - 0.17 * sin((217.6 - 407332.20 * T) / DEG_IN_RADIAN);
	beta = beta / DEG_IN_RADIAN;
	pie = 0.9508
	   + 0.0518 * cos((134.9 + 477198.85 * T) / DEG_IN_RADIAN)
	   + 0.0095 * cos((259.2 - 413335.38 * T) / DEG_IN_RADIAN)
	   + 0.0078 * cos((235.7 + 890534.23 * T) / DEG_IN_RADIAN)
	   + 0.0028 * cos((269.9 + 954397.70 * T) / DEG_IN_RADIAN);
	pie = pie / DEG_IN_RADIAN;
	distance = 1 / sin(pie);

	l = cos(beta) * cos(lambda);
	m = 0.9175 * cos(beta) * sin(lambda) - 0.3978 * sin(beta);
	n = 0.3978 * cos(beta) * sin(lambda) + 0.9175 * sin(beta);

	x = l * distance;
	y = m * distance;
	z = n * distance;  /* for topocentric correction */
	/* lat isn't passed right on some IBM systems unless you do this
	   or something like it! */
	sprintf(dummy,"%f",lat);
	rad_lat = lat / DEG_IN_RADIAN;
	rad_lst = sid / HRS_IN_RADIAN;
	x = x - cos(rad_lat) * cos(rad_lst);
	y = y - cos(rad_lat) * sin(rad_lst);
	z = z - sin(rad_lat);


	topo_dist = sqrt(x * x + y * y + z * z);

	l = x / topo_dist;
	m = y / topo_dist;
	n = z / topo_dist;

	alpha = atan_circ(l,m);
	delta = asin(n);
	*ra = alpha * HRS_IN_RADIAN;
	*dec = delta * DEG_IN_RADIAN;
	*dist = topo_dist;
}


void lpsun(jd,ra,dec)

	double jd, *ra, *dec;

/* Low precision formulae for the sun, from Almanac p. C24 (1990) */
/* ra and dec are returned as decimal hours and decimal degrees. */

{
	double n, L, g, lambda,epsilon,x,y,z;

	n = jd - J2000;
	L = 280.460 + 0.9856474 * n;
	g = (357.528 + 0.9856003 * n)/DEG_IN_RADIAN;
	lambda = (L + 1.915 * sin(g) + 0.020 * sin(2. * g))/DEG_IN_RADIAN;
	epsilon = (23.439 - 0.0000004 * n)/DEG_IN_RADIAN;

	x = cos(lambda);
	y = cos(epsilon) * sin(lambda);
	z = sin(epsilon)*sin(lambda);

	*ra = (atan_circ(x,y))*HRS_IN_RADIAN;
	*dec = (asin(z))*DEG_IN_RADIAN;
}

void eclrot(jd, x, y, z)

	double jd, *x, *y, *z;

/* rotates ecliptic rectangular coords x, y, z to
   equatorial (all assumed of date.) */

{
	double incl;
	double ypr,zpr;
	double T;

	T = (jd - J2000) / 36525;  /* centuries since J2000 */

	incl = (23.439291 + T * (-0.0130042 - 0.00000016 * T))/DEG_IN_RADIAN;
		/* 1992 Astron Almanac, p. B18, dropping the
		   cubic term, which is 2 milli-arcsec! */
	ypr = cos(incl) * *y - sin(incl) * *z;
	zpr = sin(incl) * *y + cos(incl) * *z;
	*y = ypr;
	*z = zpr;
	/* x remains the same. */
}

double circulo(x)
	double x;
{
	/* assuming x is an angle in degrees, returns
	   modulo 360 degrees. */

	int n;

	n = (int)(x / 360.);
	return(x - 360. * n);
}


void geocent(geolong,geolat,height,x_geo,y_geo,z_geo)

	double geolong, geolat, height, *x_geo, *y_geo, *z_geo;

/* computes the geocentric coordinates from the geodetic
(standard map-type) longitude, latitude, and height.
These are assumed to be in decimal hours, decimal degrees, and
meters respectively.  Notation generally follows 1992 Astr Almanac,
p. K11 */


{

	double denom, C_geo, S_geo;

	geolat = geolat / DEG_IN_RADIAN;
	geolong = geolong / HRS_IN_RADIAN;
	denom = (1. - FLATTEN) * sin(geolat);
	denom = cos(geolat) * cos(geolat) + denom*denom;
	C_geo = 1. / sqrt(denom);
	S_geo = (1. - FLATTEN) * (1. - FLATTEN) * C_geo;
	C_geo = C_geo + height / EQUAT_RAD;  /* deviation from almanac
		       notation -- include height here. */
	S_geo = S_geo + height / EQUAT_RAD;
	*x_geo = C_geo * cos(geolat) * cos(geolong);
	*y_geo = C_geo * cos(geolat) * sin(geolong);
	*z_geo = S_geo * sin(geolat);
}


double etcorr(jd)

double jd;

{

	/* Given a julian date in 1900-2100, returns the correction
           delta t which is:
		TDT - UT (after 1983 and before 1993)
		ET - UT (before 1983)
		an extrapolated guess  (after 1993).

	For dates in the past (<= 1993) the value is linearly
        interpolated on 5-year intervals; for dates after the present,
        an extrapolation is used, because the true value of delta t
	cannot be predicted precisely.  Note that TDT is essentially the
	modern version of ephemeris time with a slightly cleaner
	definition.

	Where the algorithm shifts there is an approximately 0.1 second
        discontinuity.  Also, the 5-year linear interpolation scheme can
        lead to errors as large as 0.5 seconds in some cases, though
 	usually rather smaller. */

	double dates[20] = {1900,1905,1910,1915,1920,1925,1930,1935,1940,1945,
		    1950,1955,1960,1965,1970,1975,1980,1985,1990,1993};
	double delts[20]={-2.72,3.86,10.46,17.20,21.16,23.62,24.02,23.93,24.33,26.77,
		  29.15,31.07,33.15,35.73,40.18,45.48,50.54,54.34,56.86,59.12};
	double year, delt=0.0;
	short i;

	year = 1900. + (jd - 2415019.5) / 365.25;

	if(year < 1993.0 && year >= 1900.) {
		i = (year - 1900) / 5;
		delt = delts[i] +
		 ((delts[i+1] - delts[i])/(dates[i+1] - dates[i])) * (year - dates[i]);
	}

	else if (year > 1993. && year < 2100.)
		delt = 33.15 + (2.164e-3) * (jd - 2436935.4);  /* rough extrapolation */

	else if (year < 1900) {
		oprntf("etcorr ... no ephemeris time data for < 1900.\n");
       		delt = 0.;
	}

	else if (year >= 2100.) {
		oprntf("etcorr .. very long extrapolation in delta T - inaccurate.\n");
		delt = 180.; /* who knows? */
	}

	return(delt);
}


void accumoon(jd,geolat,lst,elevsea,geora,geodec,geodist,
     topora,topodec,topodist)

	double jd,geolat,lst,elevsea;
     	double *geora,*geodec,*geodist,*topora,*topodec,*topodist;

  /* jd, dec. degr., dec. hrs., meters */
/* More accurate (but more elaborate and slower) lunar
   ephemeris, from Jean Meeus' *Astronomical Formulae For Calculators*,
   pub. Willman-Bell.  Includes all the terms given there. */

{
/*      double *eclatit,*eclongit, *pie,*ra,*dec,*dist; geocent quantities,
		formerly handed out but not in this version */
	double pie, dist;  /* horiz parallax */
	double Lpr,M,Mpr,D,F,Om,T,Tsq,Tcb;
	double e,lambda,B,beta,om1,om2;
	double sinx, x, y, z, l, m, n;
	double x_geo, y_geo, z_geo;  /* geocentric position of *observer* */

	jd = jd + etcorr(jd)/SEC_IN_DAY;   /* approximate correction to ephemeris time */
	T = (jd - 2415020.) / 36525.;   /* this based around 1900 ... */
	Tsq = T * T;
	Tcb = Tsq * T;

	Lpr = 270.434164 + 481267.8831 * T - 0.001133 * Tsq
			+ 0.0000019 * Tcb;
	M = 358.475833 + 35999.0498*T - 0.000150*Tsq
			- 0.0000033*Tcb;
	Mpr = 296.104608 + 477198.8491*T + 0.009192*Tsq
			+ 0.0000144*Tcb;
	D = 350.737486 + 445267.1142*T - 0.001436 * Tsq
			+ 0.0000019*Tcb;
	F = 11.250889 + 483202.0251*T -0.003211 * Tsq
			- 0.0000003*Tcb;
	Om = 259.183275 - 1934.1420*T + 0.002078*Tsq
			+ 0.0000022*Tcb;

	Lpr = circulo(Lpr);
	Mpr = circulo(Mpr);
	M = circulo(M);
	D = circulo(D);
	F = circulo(F);
	Om = circulo(Om);


	sinx =  sin((51.2 + 20.2 * T)/DEG_IN_RADIAN);
	Lpr = Lpr + 0.000233 * sinx;
	M = M - 0.001778 * sinx;
	Mpr = Mpr + 0.000817 * sinx;
	D = D + 0.002011 * sinx;

	sinx = 0.003964 * sin((346.560+132.870*T -0.0091731*Tsq)/DEG_IN_RADIAN);

	Lpr = Lpr + sinx;
	Mpr = Mpr + sinx;
	D = D + sinx;
	F = F + sinx;

	sinx = sin(Om/DEG_IN_RADIAN);
	Lpr = Lpr + 0.001964 * sinx;
	Mpr = Mpr + 0.002541 * sinx;
	D = D + 0.001964 * sinx;
	F = F - 0.024691 * sinx;
	F = F - 0.004328 * sin((Om + 275.05 -2.30*T)/DEG_IN_RADIAN);

	e = 1 - 0.002495 * T - 0.00000752 * Tsq;

	M = M / DEG_IN_RADIAN;   /* these will all be arguments ... */
	Mpr = Mpr / DEG_IN_RADIAN;
	D = D / DEG_IN_RADIAN;
	F = F / DEG_IN_RADIAN;

	lambda = Lpr + 6.288750 * sin(Mpr)
		+ 1.274018 * sin(2*D - Mpr)
		+ 0.658309 * sin(2*D)
		+ 0.213616 * sin(2*Mpr)
		- e * 0.185596 * sin(M)
		- 0.114336 * sin(2*F)
		+ 0.058793 * sin(2*D - 2*Mpr)
		+ e * 0.057212 * sin(2*D - M - Mpr)
		+ 0.053320 * sin(2*D + Mpr)
		+ e * 0.045874 * sin(2*D - M)
		+ e * 0.041024 * sin(Mpr - M)
		- 0.034718 * sin(D)
		- e * 0.030465 * sin(M+Mpr)
		+ 0.015326 * sin(2*D - 2*F)
		- 0.012528 * sin(2*F + Mpr)
		- 0.010980 * sin(2*F - Mpr)
		+ 0.010674 * sin(4*D - Mpr)
		+ 0.010034 * sin(3*Mpr)
		+ 0.008548 * sin(4*D - 2*Mpr)
		- e * 0.007910 * sin(M - Mpr + 2*D)
		- e * 0.006783 * sin(2*D + M)
		+ 0.005162 * sin(Mpr - D);

		/* And furthermore.....*/

	lambda = lambda + e * 0.005000 * sin(M + D)
		+ e * 0.004049 * sin(Mpr - M + 2*D)
		+ 0.003996 * sin(2*Mpr + 2*D)
		+ 0.003862 * sin(4*D)
		+ 0.003665 * sin(2*D - 3*Mpr)
		+ e * 0.002695 * sin(2*Mpr - M)
		+ 0.002602 * sin(Mpr - 2*F - 2*D)
		+ e * 0.002396 * sin(2*D - M - 2*Mpr)
		- 0.002349 * sin(Mpr + D)
		+ e * e * 0.002249 * sin(2*D - 2*M)
		- e * 0.002125 * sin(2*Mpr + M)
		- e * e * 0.002079 * sin(2*M)
		+ e * e * 0.002059 * sin(2*D - Mpr - 2*M)
		- 0.001773 * sin(Mpr + 2*D - 2*F)
		- 0.001595 * sin(2*F + 2*D)
		+ e * 0.001220 * sin(4*D - M - Mpr)
		- 0.001110 * sin(2*Mpr + 2*F)
		+ 0.000892 * sin(Mpr - 3*D)
		- e * 0.000811 * sin(M + Mpr + 2*D)
		+ e * 0.000761 * sin(4*D - M - 2*Mpr)
		+ e * e * 0.000717 * sin(Mpr - 2*M)
		+ e * e * 0.000704 * sin(Mpr - 2 * M - 2*D)
		+ e * 0.000693 * sin(M - 2*Mpr + 2*D)
		+ e * 0.000598 * sin(2*D - M - 2*F)
		+ 0.000550 * sin(Mpr + 4*D)
		+ 0.000538 * sin(4*Mpr)
		+ e * 0.000521 * sin(4*D - M)
		+ 0.000486 * sin(2*Mpr - D);

/*              *eclongit = lambda;  */

	B = 5.128189 * sin(F)
		+ 0.280606 * sin(Mpr + F)
		+ 0.277693 * sin(Mpr - F)
		+ 0.173238 * sin(2*D - F)
		+ 0.055413 * sin(2*D + F - Mpr)
		+ 0.046272 * sin(2*D - F - Mpr)
		+ 0.032573 * sin(2*D + F)
		+ 0.017198 * sin(2*Mpr + F)
		+ 0.009267 * sin(2*D + Mpr - F)
		+ 0.008823 * sin(2*Mpr - F)
		+ e * 0.008247 * sin(2*D - M - F)
		+ 0.004323 * sin(2*D - F - 2*Mpr)
		+ 0.004200 * sin(2*D + F + Mpr)
		+ e * 0.003372 * sin(F - M - 2*D)
		+ 0.002472 * sin(2*D + F - M - Mpr)
		+ e * 0.002222 * sin(2*D + F - M)
		+ e * 0.002072 * sin(2*D - F - M - Mpr)
		+ e * 0.001877 * sin(F - M + Mpr)
		+ 0.001828 * sin(4*D - F - Mpr)
		- e * 0.001803 * sin(F + M)
		- 0.001750 * sin(3*F)
		+ e * 0.001570 * sin(Mpr - M - F)
		- 0.001487 * sin(F + D)
		- e * 0.001481 * sin(F + M + Mpr)
		+ e * 0.001417 * sin(F - M - Mpr)
		+ e * 0.001350 * sin(F - M)
		+ 0.001330 * sin(F - D)
		+ 0.001106 * sin(F + 3*Mpr)
		+ 0.001020 * sin(4*D - F)
		+ 0.000833 * sin(F + 4*D - Mpr);
     /* not only that, but */
	B = B + 0.000781 * sin(Mpr - 3*F)
		+ 0.000670 * sin(F + 4*D - 2*Mpr)
		+ 0.000606 * sin(2*D - 3*F)
		+ 0.000597 * sin(2*D + 2*Mpr - F)
		+ e * 0.000492 * sin(2*D + Mpr - M - F)
		+ 0.000450 * sin(2*Mpr - F - 2*D)
		+ 0.000439 * sin(3*Mpr - F)
		+ 0.000423 * sin(F + 2*D + 2*Mpr)
		+ 0.000422 * sin(2*D - F - 3*Mpr)
		- e * 0.000367 * sin(M + F + 2*D - Mpr)
		- e * 0.000353 * sin(M + F + 2*D)
		+ 0.000331 * sin(F + 4*D)
		+ e * 0.000317 * sin(2*D + F - M + Mpr)
		+ e * e * 0.000306 * sin(2*D - 2*M - F)
		- 0.000283 * sin(Mpr + 3*F);

	om1 = 0.0004664 * cos(Om/DEG_IN_RADIAN);
	om2 = 0.0000754 * cos((Om + 275.05 - 2.30*T)/DEG_IN_RADIAN);

	beta = B * (1. - om1 - om2);
/*      *eclatit = beta; */

	pie = 0.950724
		+ 0.051818 * cos(Mpr)
		+ 0.009531 * cos(2*D - Mpr)
		+ 0.007843 * cos(2*D)
		+ 0.002824 * cos(2*Mpr)
		+ 0.000857 * cos(2*D + Mpr)
		+ e * 0.000533 * cos(2*D - M)
		+ e * 0.000401 * cos(2*D - M - Mpr)
		+ e * 0.000320 * cos(Mpr - M)
		- 0.000271 * cos(D)
		- e * 0.000264 * cos(M + Mpr)
		- 0.000198 * cos(2*F - Mpr)
		+ 0.000173 * cos(3*Mpr)
		+ 0.000167 * cos(4*D - Mpr)
		- e * 0.000111 * cos(M)
		+ 0.000103 * cos(4*D - 2*Mpr)
		- 0.000084 * cos(2*Mpr - 2*D)
		- e * 0.000083 * cos(2*D + M)
		+ 0.000079 * cos(2*D + 2*Mpr)
		+ 0.000072 * cos(4*D)
		+ e * 0.000064 * cos(2*D - M + Mpr)
		- e * 0.000063 * cos(2*D + M - Mpr)
		+ e * 0.000041 * cos(M + D)
		+ e * 0.000035 * cos(2*Mpr - M)
		- 0.000033 * cos(3*Mpr - 2*D)
		- 0.000030 * cos(Mpr + D)
		- 0.000029 * cos(2*F - 2*D)
		- e * 0.000029 * cos(2*Mpr + M)
		+ e * e * 0.000026 * cos(2*D - 2*M)
		- 0.000023 * cos(2*F - 2*D + Mpr)
		+ e * 0.000019 * cos(4*D - M - Mpr);

	beta = beta/DEG_IN_RADIAN;
	lambda = lambda/DEG_IN_RADIAN;
	l = cos(lambda) * cos(beta);
	m = sin(lambda) * cos(beta);
	n = sin(beta);
	eclrot(jd,&l,&m,&n);

	dist = 1/sin((pie)/DEG_IN_RADIAN);
	x = l * dist;
	y = m * dist;
	z = n * dist;

	*geora = atan_circ(l,m) * HRS_IN_RADIAN;
	*geodec = asin(n) * DEG_IN_RADIAN;
	*geodist = dist;

	geocent(lst,geolat,elevsea,&x_geo,&y_geo,&z_geo);

	x = x - x_geo;  /* topocentric correction using elliptical earth fig. */
	y = y - y_geo;
	z = z - z_geo;

	*topodist = sqrt(x*x + y*y + z*z);

	l = x / (*topodist);
	m = y / (*topodist);
	n = z / (*topodist);

	*topora = atan_circ(l,m) * HRS_IN_RADIAN;
	*topodec = asin(n) * DEG_IN_RADIAN;

}

void flmoon(n,nph,jdout)

	int n,nph;
	double *jdout;

/* Gives jd (+- 2 min) of phase nph on lunation n; replaces
less accurate Numerical Recipes routine.  This routine
implements formulae found in Jean Meeus' *Astronomical Formulae
for Calculators*, 2nd edition, Willman-Bell.  A very useful
book!! */

/* n, nph lunation and phase; nph = 0 new, 1 1st, 2 full, 3 last
 *jdout   jd of requested phase */

{
	double jd, cor;
	double M, Mpr, F;
	double T;
	double lun;

	lun = (double) n + (double) nph / 4.;
	T = lun / 1236.85;
	jd = 2415020.75933 + 29.53058868 * lun
		+ 0.0001178 * T * T
		- 0.000000155 * T * T * T
		+ 0.00033 * sin((166.56 + 132.87 * T - 0.009173 * T * T)/DEG_IN_RADIAN);
	M = 359.2242 + 29.10535608 * lun - 0.0000333 * T * T - 0.00000347 * T * T * T;
	M = M / DEG_IN_RADIAN;
	Mpr = 306.0253 + 385.81691806 * lun + 0.0107306 * T * T + 0.00001236 * T * T * T;
	Mpr = Mpr / DEG_IN_RADIAN;
	F = 21.2964 + 390.67050646 * lun - 0.0016528 * T * T - 0.00000239 * T * T * T;
	F = F / DEG_IN_RADIAN;
	if((nph == 0) || (nph == 2)) {/* new or full */
		cor =   (0.1734 - 0.000393*T) * sin(M)
			+ 0.0021 * sin(2*M)
			- 0.4068 * sin(Mpr)
			+ 0.0161 * sin(2*Mpr)
			- 0.0004 * sin(3*Mpr)
			+ 0.0104 * sin(2*F)
			- 0.0051 * sin(M + Mpr)
			- 0.0074 * sin(M - Mpr)
			+ 0.0004 * sin(2*F+M)
			- 0.0004 * sin(2*F-M)
			- 0.0006 * sin(2*F+Mpr)
			+ 0.0010 * sin(2*F-Mpr)
			+ 0.0005 * sin(M+2*Mpr);
		jd = jd + cor;
	}
	else {
		cor = (0.1721 - 0.0004*T) * sin(M)
			+ 0.0021 * sin(2 * M)
			- 0.6280 * sin(Mpr)
			+ 0.0089 * sin(2 * Mpr)
			- 0.0004 * sin(3 * Mpr)
			+ 0.0079 * sin(2*F)
			- 0.0119 * sin(M + Mpr)
			- 0.0047 * sin(M - Mpr)
			+ 0.0003 * sin(2 * F + M)
			- 0.0004 * sin(2 * F - M)
			- 0.0006 * sin(2 * F + Mpr)
			+ 0.0021 * sin(2 * F - Mpr)
			+ 0.0003 * sin(M + 2 * Mpr)
			+ 0.0004 * sin(M - 2 * Mpr)
			- 0.0003 * sin(2*M + Mpr);
		if(nph == 1) cor = cor + 0.0028 -
				0.0004 * cos(M) + 0.0003 * cos(Mpr);
		if(nph == 3) cor = cor - 0.0028 +
				0.0004 * cos(M) - 0.0003 * cos(Mpr);
		jd = jd + cor;

	}
	*jdout = jd;
}

float lun_age(jd, nlun)

	double jd;
	int *nlun;

{
	/* compute age in days of moon since last new,
	   and lunation of last new moon. */

	int nlast;
	double newjd, lastnewjd;
	short kount=0;
	float x;

	nlast = (jd - 2415020.5) / 29.5307 - 1;

	flmoon(nlast,0,&lastnewjd);
	nlast++;
	flmoon(nlast,0,&newjd);
	while((newjd < jd) && (kount < 40)) {
		lastnewjd = newjd;
		nlast++;
		flmoon(nlast,0,&newjd);
	}
 	if(kount > 35) {
		oprntf("Didn't find phase in lun_age!\n");
		x = -10.;
                *nlun = 0;
	}
	else {
	  x = jd - lastnewjd;
	  *nlun = nlast - 1;
        }

	return(x);
}

void print_phase(jd)
	double jd;

{
	/* prints a verbal description of moon phase, given the
	   julian date.  */

	int nlast, noctiles;
	double newjd, lastnewjd;
	double fqjd, fljd, lqjd;  /* jds of first, full, and last in this lun.*/
	short kount=0;
	float x;

	nlast = (jd - 2415020.5) / 29.5307 - 1;  /* find current lunation */

	flmoon(nlast,0,&lastnewjd);
	nlast++;
	flmoon(nlast,0,&newjd);
	while((newjd < jd) && (kount < 40)) {
		lastnewjd = newjd;
		nlast++;
		flmoon(nlast,0,&newjd);
	}
	if(kount > 35) {  /* oops ... didn't find it ... */
		oprntf("Didn't find phase in print_phase!\n");
		x = -10.;
	}
	else {     /* found lunation ok */
		x = jd - lastnewjd;
		nlast--;
		noctiles = x / 3.69134;  /* 3.69134 = 1/8 month; truncate. */
		if(noctiles == 0) oprntf("%3.1f days since new moon",x);
		else if (noctiles <= 2) {  /* nearest first quarter */
			flmoon(nlast,1,&fqjd);
			x = jd - fqjd;
			if(x < 0.)
			  oprntf("%3.1f days before first quarter",(-1.*x));
			else
			  oprntf("%3.1f days since first quarter",x);
		}
		else if (noctiles <= 4) {  /* nearest full */
			flmoon(nlast,2,&fljd);
			x = jd - fljd;
			if(x < 0.)
			  oprntf("%3.1f days until full moon",(-1.*x));
			else
			  oprntf("%3.1f days after full moon",x);
		}
		else if (noctiles <= 6) {  /* nearest last quarter */
			flmoon(nlast,3,&lqjd);
			x = jd - lqjd;
			if(x < 0.)
			  oprntf("%3.1f days before last quarter",(-1.*x));
			else
			  oprntf("%3.1f days after last quarter",x);
		}
		else oprntf("%3.1f days before new moon",(newjd - jd));
	}
}

double lunskybright(alpha,rho,kzen,altmoon,alt, moondist)

	double alpha,rho,kzen,altmoon,alt,moondist;

/* Evaluates predicted LUNAR part of sky brightness, in
   V magnitudes per square arcsecond, following K. Krisciunas
   and B. E. Schaeffer (1991) PASP 103, 1033.

   alpha = separation of sun and moon as seen from earth,
   converted internally to its supplement,
   rho = separation of moon and object,
   kzen = zenith extinction coefficient,
   altmoon = altitude of moon above horizon,
   alt = altitude of object above horizon
   moondist = distance to moon, in earth radii

   all are in decimal degrees. */

{

    double istar,Xzm,Xo,Z,Zmoon,Bmoon,fofrho,rho_rad;

    rho_rad = rho/DEG_IN_RADIAN;
    alpha = (180. - alpha);
    Zmoon = (90. - altmoon)/DEG_IN_RADIAN;
    Z = (90. - alt)/DEG_IN_RADIAN;
    moondist = moondist/(60.27);  /* divide by mean distance */

    istar = -0.4*(3.84 + 0.026*fabs(alpha) + 4.0e-9*pow(alpha,4.)); /*eqn 20*/
    istar =  pow(10.,istar)/(moondist * moondist);
    if(fabs(alpha) < 7.)   /* crude accounting for opposition effect */
	istar = istar * (1.35 - 0.05 * fabs(istar));
	/* 35 per cent brighter at full, effect tapering linearly to
	   zero at 7 degrees away from full. mentioned peripherally in
	   Krisciunas and Scheafer, p. 1035. */
    fofrho = 229087. * (1.06 + cos(rho_rad)*cos(rho_rad));
    if(fabs(rho) > 10.)
       fofrho=fofrho+pow(10.,(6.15 - rho/40.));            /* eqn 21 */
    else if (fabs(rho) > 0.25)
       fofrho= fofrho+ 6.2e7 / (rho*rho);   /* eqn 19 */
    else fofrho = fofrho+9.9e8;  /*for 1/4 degree -- radius of moon! */
    Xzm = sqrt(1.0 - 0.96*sin(Zmoon)*sin(Zmoon));
    if(Xzm != 0.) Xzm = 1./Xzm;
	  else Xzm = 10000.;
    Xo = sqrt(1.0 - 0.96*sin(Z)*sin(Z));
    if(Xo != 0.) Xo = 1./Xo;
	  else Xo = 10000.;
    Bmoon = fofrho * istar * pow(10.,(-0.4*kzen*Xzm))
	  * (1. - pow(10.,(-0.4*kzen*Xo)));   /* nanoLamberts */
    if(Bmoon > 0.001)
      return(22.50 - 1.08574 * log(Bmoon/34.08)); /* V mag per sq arcs-eqn 1 */
    else return(99.);
}

void accusun(jd,lst,geolat,ra,dec,dist,topora,topodec,x,y,z)

	double jd,lst,geolat,*ra,*dec,*dist,*topora,*topodec;
 	double *x, *y, *z;
{
      /*  implemenataion of Jean Meeus' more accurate solar
	  ephemeris.  For ultimate use in helio correction! From
	  Astronomical Formulae for Calculators, pp. 79 ff.  This
	  gives sun's position wrt *mean* equinox of date, not
	  *apparent*.  Accuracy is << 1 arcmin.  Positions given are
	  geocentric ... parallax due to observer's position on earth is
	  ignored. This is up to 8 arcsec; routine is usually a little
	  better than that.
          // -- topocentric correction *is* included now. -- //
	  Light travel time is apparently taken into
	  account for the ra and dec, but I don't know if aberration is
	  and I don't know if distance is simlarly antedated.

	  x, y, and z are heliocentric equatorial coordinates of the
	  EARTH, referred to mean equator and equinox of date. */

	double L, T, Tsq, Tcb;
	double M, e, Cent, nu, sunlong;
	double Lrad, Mrad, nurad, R;
	double A, B, C, D, E, H;
	double xtop, ytop, ztop, topodist, l, m, n, xgeo, ygeo, zgeo;

	jd = jd + etcorr(jd)/SEC_IN_DAY;  /* might as well do it right .... */
	T = (jd - 2415020.) / 36525.;  /* 1900 --- this is an oldish theory*/
	Tsq = T*T;
	Tcb = T*Tsq;
	L = 279.69668 + 36000.76892*T + 0.0003025*Tsq;
	M = 358.47583 + 35999.04975*T - 0.000150*Tsq - 0.0000033*Tcb;
	e = 0.01675104 - 0.0000418*T - 0.000000126*Tsq;

	L = circulo(L);
	M = circulo(M);
/*      printf("raw L, M: %15.8f, %15.8f\n",L,M); */

	A = 153.23 + 22518.7541 * T;  /* A, B due to Venus */
	B = 216.57 + 45037.5082 * T;
	C = 312.69 + 32964.3577 * T;  /* C due to Jupiter */
		/* D -- rough correction from earth-moon
			barycenter to center of earth. */
	D = 350.74 + 445267.1142*T - 0.00144*Tsq;
	E = 231.19 + 20.20*T;    /* "inequality of long period .. */
	H = 353.40 + 65928.7155*T;  /* Jupiter. */

	A = circulo(A) / DEG_IN_RADIAN;
	B = circulo(B) / DEG_IN_RADIAN;
	C = circulo(C) / DEG_IN_RADIAN;
	D = circulo(D) / DEG_IN_RADIAN;
	E = circulo(E) / DEG_IN_RADIAN;
	H = circulo(H) / DEG_IN_RADIAN;

	L = L + 0.00134 * cos(A)
	      + 0.00154 * cos(B)
	      + 0.00200 * cos(C)
	      + 0.00179 * sin(D)
	      + 0.00178 * sin(E);

	Lrad = L/DEG_IN_RADIAN;
	Mrad = M/DEG_IN_RADIAN;

	Cent = (1.919460 - 0.004789*T -0.000014*Tsq)*sin(Mrad)
	     + (0.020094 - 0.000100*T) * sin(2.0*Mrad)
	     + 0.000293 * sin(3.0*Mrad);
	sunlong = L + Cent;


	nu = M + Cent;
	nurad = nu / DEG_IN_RADIAN;

	R = (1.0000002 * (1 - e*e)) / (1. + e * cos(nurad));
	R = R + 0.00000543 * sin(A)
	      + 0.00001575 * sin(B)
	      + 0.00001627 * sin(C)
	      + 0.00003076 * cos(D)
	      + 0.00000927 * sin(H);
/*      printf("solar longitude: %10.5f  Radius vector %10.7f\n",sunlong,R);
	printf("eccentricity %10.7f  eqn of center %10.5f\n",e,Cent);   */

	sunlong = sunlong/DEG_IN_RADIAN;

	*dist = R;
	*x = cos(sunlong);  /* geocentric */
	*y = sin(sunlong);
	*z = 0.;
	eclrot(jd, x, y, z);

/*      --- code to include topocentric correction for sun .... */

	geocent(lst,geolat,0.,&xgeo,&ygeo,&zgeo);

	xtop = *x - xgeo*EQUAT_RAD/ASTRO_UNIT;
	ytop = *y - ygeo*EQUAT_RAD/ASTRO_UNIT;
	ztop = *z - zgeo*EQUAT_RAD/ASTRO_UNIT;

	topodist = sqrt(xtop*xtop + ytop*ytop + ztop*ztop);

	l = xtop / (topodist);
	m = ytop / (topodist);
	n = ztop / (topodist);

	*topora = atan_circ(l,m) * HRS_IN_RADIAN;
	*topodec = asin(n) * DEG_IN_RADIAN;

	*ra = atan_circ(*x,*y) * HRS_IN_RADIAN;
	*dec = asin(*z) * DEG_IN_RADIAN;

	*x = *x * R * -1;  /* heliocentric */
	*y = *y * R * -1;
	*z = *z * R * -1;

}

double jd_moon_alt(alt,jdguess,lat,longit,elevsea)

	double alt,jdguess,lat,longit,elevsea;

{
	/* returns jd at which moon is at a given
	altitude, given jdguess as a starting point. In current version
	uses high-precision moon -- execution time does not seem to be
	excessive on modern hardware.  If it's a problem on your machine,
	you can replace calls to 'accumoon' with 'lpmoon' and remove
	the 'elevsea' argument. */

	double jdout;
	double deriv, err, del = 0.002;
	double ra,dec,dist,geora,geodec,geodist,sid,ha,alt2,alt3,az;
	short i = 0;

	/* first guess */

	sid=lst(jdguess,longit);
	accumoon(jdguess,lat,sid,elevsea,&geora,&geodec,&geodist,
				&ra,&dec,&dist);
	ha = lst(jdguess,longit) - ra;
	alt2 = altit(dec,ha,lat,&az);
	jdguess = jdguess + del;
	sid = lst(jdguess,longit);
	accumoon(jdguess,lat,sid,elevsea,&geora,&geodec,&geodist,
				&ra,&dec,&dist);
	alt3 = altit(dec,(sid - ra),lat,&az);
	err = alt3 - alt;
	deriv = (alt3 - alt2) / del;
	while((fabs(err) > 0.1) && (i < 10)) {
		jdguess = jdguess - err/deriv;
		sid=lst(jdguess,longit);
		accumoon(jdguess,lat,sid,elevsea,&geora,&geodec,&geodist,
				&ra,&dec,&dist);
		alt3 = altit(dec,(sid - ra),lat,&az);
		err = alt3 - alt;
		i++;
		if(i == 9) oprntf("Moonrise or -set calculation not converging!!...\n");
	}
	if(i >= 9) jdguess = -1000.;
	jdout = jdguess;
	return(jdout);
}

double jd_sun_alt(alt,jdguess,lat,longit)

	double alt,jdguess,lat,longit;

{
	/* returns jd at which sun is at a given
	altitude, given jdguess as a starting point. Uses
	low-precision sun, which is plenty good enough. */

	double jdout;
	double deriv, err, del = 0.002;
	double ra,dec,ha,alt2,alt3,az;
	short i = 0;

	/* first guess */

	lpsun(jdguess,&ra,&dec);
	ha = lst(jdguess,longit) - ra;
	alt2 = altit(dec,ha,lat,&az);
	jdguess = jdguess + del;
	lpsun(jdguess,&ra,&dec);
	alt3 = altit(dec,(lst(jdguess,longit) - ra),lat,&az);
	err = alt3 - alt;
	deriv = (alt3 - alt2) / del;
	while((fabs(err) > 0.1) && (i < 10)) {
		jdguess = jdguess - err/deriv;
		lpsun(jdguess,&ra,&dec);
		alt3 = altit(dec,(lst(jdguess,longit) - ra),lat,&az);
		err = alt3 - alt;
		i++;
		if(i == 9) oprntf("Sunrise, set, or twilight calculation not converging!\n");
	}
	if(i >= 9) jdguess = -1000.;
	jdout = jdguess;
	return(jdout);
}

float ztwilight(alt)
	double alt;
{

/* evaluates a polynomial expansion for the approximate brightening
   in magnitudes of the zenith in twilight compared to its
   value at full night, as function of altitude of the sun (in degrees).
   To get this expression I looked in Meinel, A.,
   & Meinel, M., "Sunsets, Twilight, & Evening Skies", Cambridge U.
   Press, 1983; there's a graph on p. 38 showing the decline of
   zenith twilight.  I read points off this graph and fit them with a
   polynomial; I don't even know what band there data are for! */
/* Comparison with Ashburn, E. V. 1952, JGR, v.57, p.85 shows that this
   is a good fit to his B-band measurements.  */

	float y, val;

	y = (-1.* alt - 9.0) / 9.0;  /* my polynomial's argument...*/
	val = ((2.0635175 * y + 1.246602) * y - 9.4084495)*y + 6.132725;
	return(val);
}


void find_dst_bounds(yr,stdz,use_dst,jdb,jde)

	short yr;
	double stdz;
	short use_dst;
  	double *jdb,*jde;

{
	/* finds jd's at which daylight savings time begins
	    and ends.  The parameter use_dst allows for a number
	    of conventions, namely:
		0 = don't use it at all (standard time all the time)
		1 = use USA convention (1st Sun in April to
		     last Sun in Oct after 1986; last Sun in April before)
		2 = use Spanish convention (for Canary Islands)
		-1 = use Chilean convention (CTIO).
		-2 = Australian convention (for AAT).
	    Negative numbers denote sites in the southern hemisphere,
	    where jdb and jde are beginning and end of STANDARD time for
	    the year.
	    It's assumed that the time changes at 2AM local time; so
	    when clock is set ahead, time jumps suddenly from 2 to 3,
	    and when time is set back, the hour from 1 to 2 AM local
	    time is repeated.  This could be changed in code if need be. */

	struct date_time trial;

	if((use_dst == 1) || (use_dst == 0)) {
#if 0 /* this only worked until 2007 */
	    /* USA Convention, and including no DST to be defensive */
	    /* Note that this ignores various wrinkles such as the
		brief Nixon administration flirtation with year-round DST,
		the extended DST of WW II, and so on. */
		trial.y = yr;
		trial.mo = 4;
		if(yr >= 1986) trial.d = 1;
		else trial.d = 30;
		trial.h = 2;
		trial.mn = 0;
		trial.s = 0;

		/* Find first Sunday in April for 1986 on ... */
		if(yr >= 1986)
			while(day_of_week(date_to_jd(trial)) != 6)
				trial.d++;

		/* Find last Sunday in April for pre-1986 .... */
		else while(day_of_week(date_to_jd(trial)) != 6)
				trial.d--;

		*jdb = date_to_jd(trial) + stdz/24.;

		/* Find last Sunday in October ... */
		trial.mo = 10;
		trial.d = 31;
		while(day_of_week(date_to_jd(trial)) != 6) {
			trial.d--;
		}
		*jde = date_to_jd(trial) + (stdz - 1.)/24.;
#else
	    /* USA Convention, and including no DST to be defensive */
	    /* Note that this ignores various wrinkles such as the
		brief Nixon administration flirtation with year-round DST,
		the extended DST of WW II, and so on. */
		trial.y = yr;
		trial.mo = 4;
		if(yr >= 1986) trial.d = 1;
		else trial.d = 30;
		trial.h = 2;
		trial.mn = 0;
		trial.s = 0;

		/* Find second Sunday in March for 2007 on ... */
                if(yr >= 2007){
                        trial.mo=3;
			while(day_of_week(date_to_jd(trial)) != 6)
				trial.d++;
                        trial.d++;
			while(day_of_week(date_to_jd(trial)) != 6)
				trial.d++;
                }
		/* Find first Sunday in April for 1986 on ... */
		else if(yr >= 1986){
			while(day_of_week(date_to_jd(trial)) != 6)
				trial.d++;
                }

		/* Find last Sunday in April for pre-1986 .... */
		else {
                    while(day_of_week(date_to_jd(trial)) != 6) trial.d--;
                }

		*jdb = date_to_jd(trial) + stdz/24.;


                if(yr >= 2007){
                    /* First Sunday in November */
		    trial.mo = 11;
		    trial.d = 1 ;
		    while(day_of_week(date_to_jd(trial)) != 6) {
                         trial.d++;
                    }
                }
                else{ 
		    /* Find last Sunday in October ... */
		    trial.mo = 10;
		    trial.d = 31;
		    while(day_of_week(date_to_jd(trial)) != 6) {
			trial.d--;
		    }
                }
		*jde = date_to_jd(trial) + (stdz - 1.)/24.;
#endif
	}
	else if (use_dst == 2) {  /* Spanish, for Canaries */
		trial.y = yr;
		trial.mo = 3;
		trial.d = 31;
		trial.h = 2;
		trial.mn = 0;
		trial.s = 0;

		while(day_of_week(date_to_jd(trial)) != 6) {
			trial.d--;
		}
		*jdb = date_to_jd(trial) + stdz/24.;
		trial.mo = 9;
		trial.d = 30;
		while(day_of_week(date_to_jd(trial)) != 6) {
			trial.d--;
		}
		*jde = date_to_jd(trial) + (stdz - 1.)/24.;
	}
	else if (use_dst == -1) {  /* Chilean, for CTIO, etc.  */
#ifdef EARTHQUAKE_DST_ON
		// In 2010 Chile changed end date of Daylight Savings Time
		// to Apr 4, 0h in order to aid Earthquake recovery.
		trial.y = yr;
		trial.mo = 3;
		trial.d = 4; 
		trial.h = 0;
		trial.mn = 0;
		trial.s = 0;
		*jdb = date_to_jd(trial) + (stdz - 1.)/24.;
#else
	   /* off daylight 2nd Sun in March, onto daylight 2nd Sun in October */
		trial.y = yr;
		trial.mo = 3;
		trial.d = 8;  /* earliest possible 2nd Sunday */
		trial.h = 2;
		trial.mn = 0;
		trial.s = 0;

		while(day_of_week(date_to_jd(trial)) != 6) {
			trial.d++;
		}
		*jdb = date_to_jd(trial) + (stdz - 1.)/24.;
			/* note jdb is beginning of STANDARD time in south,
				hence use stdz - 1. */
#endif
		trial.mo = 10;
		trial.d = 8;
		while(day_of_week(date_to_jd(trial)) != 6) {
			trial.d++;
		}
		*jde = date_to_jd(trial) + stdz /24.;
	}
	else if (use_dst == -2) {  /* For Anglo-Australian Telescope  */
	   /* off daylight 1st Sun in March, onto daylight last Sun in October */
		trial.y = yr;
		trial.mo = 3;
		trial.d = 1;  /* earliest possible 1st Sunday */
		trial.h = 2;
		trial.mn = 0;
		trial.s = 0;

		while(day_of_week(date_to_jd(trial)) != 6) {
			trial.d++;
		}
		*jdb = date_to_jd(trial) + (stdz - 1.)/24.;
			/* note jdb is beginning of STANDARD time in south,
				hence use stdz - 1. */
		trial.mo = 10;
		trial.d = 31;
		while(day_of_week(date_to_jd(trial)) != 6) {
			trial.d--;
		}
		*jde = date_to_jd(trial) + stdz /24.;
	}
}


double zone(use_dst,stdz,jd,jdb,jde)

	short use_dst;
	double stdz,jd,jdb,jde;

{
	/* Returns zone time offset when standard time zone is stdz,
	   when daylight time begins (for the year) on jdb, and ends
	   (for the year) on jde.  This is parochial to the northern
	   hemisphere.  */
	/* Extension -- specifying a negative value of use_dst reverses
	   the logic for the Southern hemisphere; then DST is assumed for
	   the Southern hemisphere summer (which is the end and beginning
	   of the year. */

	if(use_dst == 0) return(stdz);
	else if((jd > jdb) && (jd < jde) && (use_dst > 0)) return(stdz-1.);
	   /* next line .. use_dst < 0 .. for Southern Hemisphere sites. */
	else if(((jd < jdb) || (jd > jde)) && (use_dst < 0)) return(stdz-1.);
	else return(stdz);
}

double true_jd(date, use_dst, enter_ut, night_date, stdz)

/* takes the values in the date-time structure, the standard time
   zone (in hours west), the prevailing conventions for date and
   time entry, and returns the value of the true julian date. */

	struct date_time date;
	short use_dst, enter_ut, night_date;
	double stdz;
{
	double jd, jdb, jde, test;

	if(enter_ut == 0) {
           find_dst_bounds(date.y,stdz,use_dst,&jdb,&jde);
	   jd = date_to_jd(date);
	   if((night_date == 1)  && (date.h < 12)) jd = jd + 1.;
	   if(use_dst != 0)  {  /* check at time changes */
		test = jd + stdz/24. - jdb;
		if((test > 0.) && (test < 0.041666666))   {
			/* 0.0416 = 1 hr; nonexistent time */
			oprntf("Error in true_jd -- nonexistent input time during std->dst change.\n");
			oprntf("Specify as 1 hour later!\n");
			return(-1.); /* signal of nonexistent time */
		}
		test = jd + stdz/24. - jde;
		if((test > 0.) && (test < 0.041666666))   {
			oprntf("WARNING ... ambiguous input time during dst->std change!\n");
		}
	   }
	   jd = jd + zone(use_dst,stdz,(jd+stdz/24.),jdb,jde)/24.;
			/* effect should be to default to standard time. */
        }
	else jd = date_to_jd(date);

	return(jd);
}


void print_tz(jd,use,jdb,jde,zabr)

	double jd;
	short use;
	double jdb,jde;
	char zabr;

{
	/* prints correct time abbreviation, given zabr as the
	   single character abbreviation for the time zone,
	   "D" or "S" depending on daylight or standard (dst
	    begins at jdb, ends at jde) and current jd. */

	oprntf(" %c", zabr);
	if((jd > jdb) && (jd < jde) && (use > 0)) oprntf("D");
	  else if(((jd < jdb) || (jd > jde)) && (use < 0)) oprntf("D");
	  else oprntf("S");
	oprntf("T");
}

void xyz_cel(x, y, z, r, d)

	double x, y, z, *r, *d;

     /* Cartesian coordinate triplet */

{
   /* converts a coordinate triplet back to a standard ra and dec */

   double mod;    /* modulus */
   double xy;     /* component in xy plane */
   double radian_ra, radian_dec;

   /* this taken directly from pl1 routine - no acos or asin available there,
       as it is in c. Easier just to copy, though */

   mod = sqrt(x*x + y*y + z*z);
   x = x / mod;
   y = y / mod;
   z = z / mod;   /* normalize 'em explicitly first. */

   xy = sqrt(x*x + y*y);

   if(xy < 1.0e-10) {
      radian_ra = 0.;  /* too close to pole */
      radian_dec = PI / 2.;
      if(z < 0.) radian_dec = radian_dec * -1.;
   }
   else {
      if(fabs(z/xy) < 3.) radian_dec = atan(z / xy);
	 else if (z >= 0.) radian_dec = PI / 2. - atan(xy / z);
	 else radian_dec = -1. * PI / 2. - atan(xy / z);
      if(fabs(x) > 1.0e-10) {
	 if(fabs(y / x) < 3.) radian_ra = atan(y / x);
	 else if ((x * y ) >= 0.) radian_ra = PI / 2. - atan(x/y);
	 else radian_ra = -1. *  PI / 2. - atan(x / y);
      }
      else {
	 radian_ra = PI / 2.;
	 if((x * y)<= 0.) radian_ra = radian_ra * -1.;
      }
      if(x <0.) radian_ra = radian_ra + PI ;
      if(radian_ra < 0.) radian_ra = radian_ra + 2. * PI ;
   }

   *r = radian_ra * HRS_IN_RADIAN;
   *d = radian_dec * DEG_IN_RADIAN;

}

void precrot(rorig, dorig, orig_epoch, final_epoch, rf, df)

	double rorig, dorig, orig_epoch, final_epoch, *rf, *df;

/*  orig_epoch, rorig, dorig  years, decimal hours, decimal degr.
    final_epoch;
    *rf, *df final ra and dec */

   /* Takes a coordinate pair and precesses it using matrix procedures
      as outlined in Taff's Computational Spherical Astronomy book.
      This is the so-called 'rigorous' method which should give very
      accurate answers all over the sky over an interval of several
      centuries.  Naked eye accuracy holds to ancient times, too.
      Precession constants used are the new IAU1976 -- the 'J2000'
      system. */

{
   double ti, tf, zeta, z, theta;  /* all as per  Taff */
   double cosz, coszeta, costheta, sinz, sinzeta, sintheta;  /* ftns */
   double p11, p12, p13, p21, p22, p23, p31, p32, p33;
      /* elements of the rotation matrix */
   double radian_ra, radian_dec;
   double orig_x, orig_y, orig_z;
   double fin_x, fin_y, fin_z;   /* original and final unit ectors */

   ti = (orig_epoch - 2000.) / 100.;
   tf = (final_epoch - 2000. - 100. * ti) / 100.;

   zeta = (2306.2181 + 1.39656 * ti + 0.000139 * ti * ti) * tf +
    (0.30188 - 0.000344 * ti) * tf * tf + 0.017998 * tf * tf * tf;
   z = zeta + (0.79280 + 0.000410 * ti) * tf * tf + 0.000205 * tf * tf * tf;
   theta = (2004.3109 - 0.8533 * ti - 0.000217 * ti * ti) * tf
     - (0.42665 + 0.000217 * ti) * tf * tf - 0.041833 * tf * tf * tf;

   /* convert to radians */

   zeta = zeta / ARCSEC_IN_RADIAN;
   z = z / ARCSEC_IN_RADIAN;
   theta = theta / ARCSEC_IN_RADIAN;

   /* compute the necessary trig functions for speed and simplicity */

   cosz = cos(z);
   coszeta = cos(zeta);
   costheta = cos(theta);
   sinz = sin(z);
   sinzeta = sin(zeta);
   sintheta = sin(theta);

   /* compute the elements of the precession matrix */

   p11 = coszeta * cosz * costheta - sinzeta * sinz;
   p12 = -1. * sinzeta * cosz * costheta - coszeta * sinz;
   p13 = -1. * cosz * sintheta;

   p21 = coszeta * sinz * costheta + sinzeta * cosz;
   p22 = -1. * sinzeta * sinz * costheta + coszeta * cosz;
   p23 = -1. * sinz * sintheta;

   p31 = coszeta * sintheta;
   p32 = -1. * sinzeta * sintheta;
   p33 = costheta;

   /* transform original coordinates */

   radian_ra = rorig / HRS_IN_RADIAN;
   radian_dec = dorig / DEG_IN_RADIAN;

   orig_x = cos(radian_dec) * cos(radian_ra);
   orig_y = cos(radian_dec) *sin(radian_ra);
   orig_z = sin(radian_dec);
      /* (hard coded matrix multiplication ...) */
   fin_x = p11 * orig_x + p12 * orig_y + p13 * orig_z;
   fin_y = p21 * orig_x + p22 * orig_y + p23 * orig_z;
   fin_z = p31 * orig_x + p32 * orig_y + p33 * orig_z;

   /* convert back to spherical polar coords */

   xyz_cel(fin_x, fin_y, fin_z, rf, df);

}

void mass_precess() {

    double rorig = 1., dorig, orig_epoch, final_epoch, rf, df,
            mura = 0., mudec = 0., dt;
    short do_proper = 0;

    printf("Mass precession.  The '=' command does precessions in a pinch, but\n");
    printf("the present command is convenient for doing several (all with same\n");
    printf("starting and ending epochs).  This routine does not affect parameters\n");
    printf("in the rest of the program.\n\n");
    printf("Type 1 if you need proper motions (They're a pain!), or 0:");
    scanf("%hd",&do_proper);
    if(do_proper == 1) {
	printf("\nA proper motion correction will be included -- it's a simple\n");
	printf("linear correction (adds mu * dt to coordinate).  Proper motion\n");
	printf("itself is NOT rigorously transformed (as in B1950->J2000, which\n");
	printf("involves a small change of inertial reference frame!)\n\n");
    }
    printf("Give epoch to precess from:");
    scanf("%lf",&orig_epoch);
    printf("Give epoch to precess to:");
    scanf("%lf",&final_epoch);
    dt = final_epoch - orig_epoch;
    while(rorig >= 0.) {
	printf("\nGive RA for %7.2f (h m s, -1 0 0 to exit):",orig_epoch);
        rorig = get_coord();
        if(rorig < 0.) {
		printf("Exiting precession routine.  Type '?' if you want a menu.\n");
		return;
        }
        printf("Give dec (d m s):");
        dorig = get_coord();
	if(do_proper == 1) get_pm(dorig,&mura,&mudec);
        precrot(rorig,dorig,orig_epoch,final_epoch,&rf,&df);
	rf = rf + mura * dt / 3600.;
        df = df + mudec * dt / 3600.;
        oprntf("\n\n %7.2f : RA = ",orig_epoch);
        put_coords(rorig,4);
        oprntf(", dec = ");
        put_coords(dorig,3);
        oprntf("\n %7.2f : RA = ",final_epoch);
        put_coords(rf,4);
        oprntf(", dec = ");
        put_coords(df,3);
        if(do_proper == 1) oprntf("\n RA p.m. = %8.4f sec/yr, dec = %8.3f arcsec/yr",mura,mudec);
	oprntf("\n");
    }
}

void galact(ra,dec,epoch,glong,glat)

	double ra,dec,epoch,*glong,*glat;

{
	/* Homebrew algorithm for 3-d Euler rotation into galactic.
	   Perfectly rigorous, and with reasonably accurate input
	   numbers derived from original IAU definition of galactic
	   pole (12 49, +27.4, 1950) and zero of long (at PA 123 deg
	   from pole.) */

	double  p11= -0.066988739415,
		p12= -0.872755765853,
		p13= -0.483538914631,
		p21=  0.492728466047,
		p22= -0.450346958025,
		p23=  0.744584633299,
		p31= -0.867600811168,
		p32= -0.188374601707,
		p33=  0.460199784759;  /* derived from Euler angles of
		theta   265.610844031 deg (rotates x axis to RA of galact center),
		phi     28.9167903483 deg (rotates x axis to point at galact cent),
		omega   58.2813466094 deg (rotates z axis to point at galact pole) */

	double r1950,d1950,
	  x0,y0,z0,x1,y1,z1;


/*   EXCISED CODE .... creates matrix from Euler angles. Resurrect if
     necessary to create new Euler angles for better precision.
     Program evolved by running and calculating angles,then initializing
     them to the values they will always have, thus saving space and time.

	cosphi = cos(phi); and so on
	p11 = cosphi * costhet;
	p12 = cosphi * sinthet;
	p13 = -1. * sinphi;
	p21 = sinom*sinphi*costhet - sinthet*cosom;
	p22 = cosom*costhet + sinthet*sinphi*sinom;
	p23 = sinom*cosphi;
	p31 = sinom*sinthet + cosom*sinphi*costhet;
	p32 = cosom*sinphi*sinthet - costhet*sinom;
	p33 = cosom * cosphi;

	printf("%15.10f %15.10f %15.10f\n",p11,p12,p13);
	printf("%15.10f %15.10f %15.10f\n",p21,p22,p23);
	printf("%15.10f %15.10f %15.10f\n",p31,p32,p33);

	check = p11*(p22*p33-p32*p23) - p12*(p21*p33-p31*p23) +
		p13*(p21*p32-p22*p31);
	printf("Check: %lf\n",check);  check determinant .. ok

    END OF EXCISED CODE..... */

	/* precess to 1950 */
	precrot(ra,dec,epoch,1950.,&r1950,&d1950);
	r1950 = r1950 / HRS_IN_RADIAN;
	d1950 = d1950 / DEG_IN_RADIAN;

	/* form direction cosines */
	x0 = cos(r1950) * cos(d1950);
	y0 = sin(r1950) * cos(d1950);
	z0 = sin(d1950);

	/* rotate 'em */
	x1 = p11*x0 + p12*y0 + p13*z0;
	y1 = p21*x0 + p22*y0 + p23*z0;
	z1 = p31*x0 + p32*y0 + p33*z0;

	/* translate to spherical polars for Galactic coords. */
	*glong = atan_circ(x1,y1)*DEG_IN_RADIAN;
	*glat = asin(z1)*DEG_IN_RADIAN;
}

void eclipt(ra,dec,epoch,jd,curep,eclong,eclat)

           double ra,dec,epoch,jd,*curep,*eclong,*eclat;

 /* ra in decimal hrs, other coords in dec. deg. */

/* converts ra and dec to ecliptic coords -- precesses to current
   epoch first (and hands current epoch back for printing.)  */

{
	double incl, T;
	double racur, decur;
	double x0,y0,z0,x1,y1,z1;

	T = (jd - J2000)/36525.;  /* centuries since J2000 */
	*curep = 2000. + (jd - J2000) / 365.25;

	incl = (23.439291 + T * (-0.0130042 - 0.00000016 * T))/DEG_IN_RADIAN;
		/* 1992 Astron Almanac, p. B18, dropping the
		   cubic term, which is 2 milli-arcsec! */

	precrot(ra,dec,epoch,*curep,&racur,&decur);
	racur = racur / HRS_IN_RADIAN;
	decur = decur / DEG_IN_RADIAN;

	x0=cos(decur)*cos(racur);
	y0=cos(decur)*sin(racur);
	z0=sin(decur);

	x1=x0;  /* doesn't change */
	y1 = cos(incl)*y0 + sin(incl)*z0;
	z1 = -1 * sin(incl)*y0 + cos(incl)*z0;
	*eclong = atan_circ(x1,y1) * DEG_IN_RADIAN;
	*eclat = asin(z1) * DEG_IN_RADIAN;
}

double parang(ha,dec,lat)

	double ha,dec,lat;

 /* decimal hours, degrees, and degrees. */

{
	/* finds the parallactic angle.  This is a little
	   complicated (see Filippenko PASP 94, 715 (1982) */

	double colat,codec,hacrit,sineta,denom;

	ha = ha / HRS_IN_RADIAN;
	dec = dec / DEG_IN_RADIAN;
	lat = lat / DEG_IN_RADIAN;

	/* Filippenko eqn 10 follows -- guarded against division by zero
             at the exact zenith .... */
	denom =
	   sqrt(1.-pow((sin(lat)*sin(dec)+cos(lat)*cos(dec)*cos(ha)),2.));
	if(denom != 0.)
	    sineta = sin(ha)*cos(lat)/denom;
	else sineta = 0.;

	if(lat >= 0.) {
		/* northern hemisphere case */

		/* If you're south of zenith, no problem. */

		if(dec<lat) return (asin(sineta)*DEG_IN_RADIAN);

		else {
			/* find critical hour angle -- where parallactic
				angle becomes 90 deg.  After that,
				take another root of expression. */
			colat = PI /2. - lat;
			codec = PI /2. - dec;
			hacrit = 1.-pow(cos(colat),2.)/pow(cos(codec),2.);
			hacrit = sqrt(hacrit)/sin(colat);
			if(fabs(hacrit) <= 1.00) hacrit = asin(hacrit);
			else oprntf("Error in parang..asin(>1)\n");
			if(fabs(ha) > fabs(hacrit))
				return(asin(sineta)*DEG_IN_RADIAN);
				/* comes out ok at large hour angle */
			else if (ha > 0)
				return((PI - asin(sineta))*DEG_IN_RADIAN);
			else return((-1.* PI - asin(sineta))*DEG_IN_RADIAN);
		}
	}
	else {  /* Southern hemisphere case follows */
		/* If you're north of zenith, no problem. */
		if(dec>lat) {
			if(ha >= 0)
				return ((PI - asin(sineta))*DEG_IN_RADIAN);
			else return(-1*(PI + asin(sineta)) * DEG_IN_RADIAN);
		}
		else {
			/* find critical hour angle -- where parallactic
				angle becomes 90 deg.  After that,
				take another root of expression. */
			colat = -1*PI/2. - lat;
			codec = PI/2. - dec;
			hacrit = 1.-pow(cos(colat),2.)/pow(cos(codec),2.);
			hacrit = sqrt(hacrit)/sin(colat);
			if(fabs(hacrit) <= 1.00) hacrit = asin(hacrit);
			else oprntf("Error in parang..asin(>1)\n");
			if(fabs(ha) > fabs(hacrit)) {
				if(ha >= 0)
				    return((PI - asin(sineta))*DEG_IN_RADIAN);
				else return(-1. * (PI + asin(sineta))*DEG_IN_RADIAN);
			}
			else return(asin(sineta)*DEG_IN_RADIAN);
		}
	}
}

/* Planetary part, added 1992 August.  The intention of this is
   to compute low-precision planetary positions for general info
   and to inform user if observation might be interfered with by
   a planet -- a rarity, but it happens.  Also designed to make
   handy low-precision planet positions available for casual
   planning purposes.  Do not try to point blindly right at the
   middle of a planetary disk with these routines!  */

double jd_el;     /* ************** */

/* elements of planetary orbits */
struct elements {
	char name[9];
	double incl;
	double Omega;
	double omega;
	double a;
	double daily;
	double ecc;
	double L_0;
	double mass;
};

struct elements el[10];

void comp_el(jd)

	double jd;
{

   double T, Tsq, Tcb, d;
   double ups, P, Q, S, V, W, G, H, zeta, psi; /* Meeus p. 110 ff. */
   double sinQ,sinZeta,cosQ,cosZeta,sinV,cosV,
	sin2Zeta,cos2Zeta;

   jd_el = jd;   /* true, but not necessarily; set explicitly */
   d = jd - 2415020.;
   T = d / 36525.;
   Tsq = T * T;
   Tcb = Tsq * T;

/* computes and loads mean elements for planets.  */

/* Mercury, Venus, and Mars from Explanatory Suppl., p. 113 */

   strcpy(el[1].name,"Mercury");
   el[1].incl = 7.002880 + 1.8608e-3 * T - 1.83e-5 * Tsq;
   el[1].Omega = 47.14594 + 1.185208 * T + 1.74e-4 * Tsq;
   el[1].omega = 75.899697 + 1.55549 * T + 2.95e-4 * Tsq;
   el[1].a = .3870986;
   el[1].daily = 4.0923388;
   el[1].ecc = 0.20561421 + 0.00002046 * T;
   el[1].L_0 = 178.179078 + 4.0923770233 * d  +
	 0.0000226 * pow((3.6525 * T),2.);

   strcpy(el[2].name,"Venus  ");
   el[2].incl = 3.39363 + 1.00583e-03 * T - 9.722e-7 * Tsq;
   el[2].Omega = 75.7796472 + 0.89985 * T + 4.1e-4 * Tsq;
   el[2].omega = 130.16383 + 1.4080 * T + 9.764e-4 * Tsq;
   el[2].a = .723325;
   el[2].daily = 1.60213049;
   el[2].ecc = 0.00682069 - 0.00004774 * T;
   el[2].L_0 = 342.767053 + 1.6021687039 * 36525 * T +
	 0.000023212 * pow((3.6525 * T),2.);

/* Earth from old Nautical Almanac .... */

   strcpy(el[5].name,"Earth  ");
   el[3].ecc = 0.01675104 - 0.00004180*T + 0.000000126*Tsq;
   el[3].incl = 0.0;
   el[3].Omega = 0.0;
   el[3].omega = 101.22083 + 0.0000470684*d + 0.000453*Tsq + 0.000003*Tcb;
   el[3].a = 1.0000007;;
   el[3].daily = 0.985599;
   el[3].L_0 = 358.47583 + 0.9856002670*d - 0.000150*Tsq - 0.000003*Tcb +
	    el[3].omega;

   strcpy(el[4].name,"Mars   ");
   el[4].incl = 1.85033 - 6.75e-04 * T - 1.833e-5 * Tsq;
   el[4].Omega = 48.786442 + .770992 * T + 1.39e-6 * Tsq;
   el[4].omega = 334.218203 + 1.840758 * T + 1.299e-4 * Tsq;
   el[4].a = 1.5236915;
   el[4].daily = 0.5240329502 + 1.285e-9 * T;
   el[4].ecc = 0.09331290 - 0.000092064 * T - 0.000000077 * Tsq;
   el[4].L_0 = 293.747628 + 0.5240711638 * d  +
	 0.000023287 * pow((3.6525 * T),2.);

/* Outer planets from Jean Meeus, Astronomical Formulae for
   Calculators, 3rd edition, Willman-Bell; p. 100. */

   strcpy(el[5].name,"Jupiter");
   el[5].incl = 1.308736 - 0.0056961 * T + 0.0000039 * Tsq;
   el[5].Omega = 99.443414 + 1.0105300 * T + 0.0003522 * Tsq
		- 0.00000851 * Tcb;
   el[5].omega = 12.720972 + 1.6099617 * T + 1.05627e-3 * Tsq
	- 3.43e-6 * Tcb;
   el[5].a = 5.202561;
   el[5].daily = 0.08312941782;
   el[5].ecc = .04833475  + 1.64180e-4 * T - 4.676e-7*Tsq -
	1.7e-9 * Tcb;
   el[5].L_0 = 238.049257 + 3036.301986 * T + 0.0003347 * Tsq -
	1.65e-6 * Tcb;

   /* The outer planets have such large mutual interactions that
      even fair accuracy requires lots of perturbations --- here
      are some of the larger ones, from Meeus' book. */

   ups = 0.2*T + 0.1;
   P = (237.47555 + 3034.9061 * T) / DEG_IN_RADIAN;
   Q = (265.91650 + 1222.1139 * T) / DEG_IN_RADIAN;
   S = (243.51721 + 428.4677 * T) / DEG_IN_RADIAN;
   V = 5*Q - 2*P;
   W = 2*P - 6*Q + 3*S;
   zeta = Q - P;
   psi = S - Q;
   sinQ = sin(Q);
   cosQ = cos(Q);
   sinV = sin(V);
   cosV = cos(V);
   sinZeta = sin(zeta);
   cosZeta = cos(zeta);
   sin2Zeta = sin(2*zeta);
   cos2Zeta = cos(2*zeta);

   el[5].L_0 = el[5].L_0
	+ (0.331364 - 0.010281*ups - 0.004692*ups*ups)*sinV
	+ (0.003228 - 0.064436*ups + 0.002075*ups*ups)*cosV
	- (0.003083 + 0.000275*ups - 0.000489*ups*ups)*sin(2*V)
	+ 0.002472 * sin(W) + 0.013619 * sinZeta + 0.018472 * sin2Zeta
	+ 0.006717 * sin(3*zeta)
	+ (0.007275  - 0.001253*ups) * sinZeta * sinQ
	+ 0.006417 * sin2Zeta * sinQ
	- (0.033839 + 0.001253 * ups) * cosZeta * sinQ
	- (0.035681 + 0.001208 * ups) * sinZeta * sinQ;
	/* only part of the terms, the ones first on the list and
	   selected larger-amplitude terms from farther down. */

   el[5].ecc = el[5].ecc + 1e-7 * (
	  (3606 + 130 * ups - 43 * ups*ups) * sinV
	+ (1289 - 580 * ups) * cosV - 6764 * sinZeta * sinQ
	- 1110 * sin2Zeta * sin(Q)
	+ (1284 + 116 * ups) * cosZeta * sinQ
	+ (1460 + 130 * ups) * sinZeta * cosQ
	+ 6074 * cosZeta * cosQ);

   el[5].omega = el[5].omega
	+ (0.007192 - 0.003147 * ups) * sinV
	+ ( 0.000197*ups*ups - 0.00675*ups - 0.020428) * cosV
	+ 0.034036 * cosZeta * sinQ + 0.037761 * sinZeta * cosQ;

   el[5].a = el[5].a + 1.0e-6 * (
	205 * cosZeta - 263 * cosV + 693 * cos2Zeta + 312 * sin(3*zeta)
	+ 147 * cos(4*zeta) + 299 * sinZeta * sinQ
	+ 181 * cos2Zeta * sinQ + 181 * cos2Zeta * sinQ
	+ 204 * sin2Zeta * cosQ + 111 * sin(3*zeta) * cosQ
	- 337 * cosZeta * cosQ - 111 * cos2Zeta * cosQ
	);

   strcpy(el[6].name,"Saturn ");
   el[6].incl = 2.492519 - 0.00034550*T - 7.28e-7*Tsq;
   el[6].Omega = 112.790414 + 0.8731951*T - 0.00015218*Tsq - 5.31e-6*Tcb ;
   el[6].omega = 91.098214 + 1.9584158*T + 8.2636e-4*Tsq;
   el[6].a = 9.554747;
   el[6].daily = 0.0334978749897;
   el[6].ecc = 0.05589232 - 3.4550e-4 * T - 7.28e-7*Tsq;
   el[6].L_0 = 266.564377 + 1223.509884*T + 0.0003245*Tsq - 5.8e-6*Tcb
	+ (0.018150*ups - 0.814181 + 0.016714 * ups*ups) * sinV
	+ (0.160906*ups - 0.010497 - 0.004100 * ups*ups) * cosV
	+ 0.007581 * sin(2*V) - 0.007986 * sin(W)
	- 0.148811 * sinZeta - 0.040786*sin2Zeta
	- 0.015208 * sin(3*zeta) - 0.006339 * sin(4*zeta)
	- 0.006244 * sinQ
	+ (0.008931 + 0.002728 * ups) * sinZeta * sinQ
	- 0.016500 * sin2Zeta * sinQ
	- 0.005775 * sin(3*zeta) * sinQ
	+ (0.081344 + 0.003206 * ups) * cosZeta * sinQ
	+ 0.015019 * cos2Zeta * sinQ
	+ (0.085581 + 0.002494 * ups) * sinZeta * cosQ
	+ (0.025328 - 0.003117 * ups) * cosZeta * cosQ
	+ 0.014394 * cos2Zeta * cosQ;   /* truncated here -- no
		      terms larger than 0.01 degrees, but errors may
		      accumulate beyond this.... */
   el[6].ecc = el[6].ecc + 1.0e-7 * (
	  (2458 * ups - 7927.) * sinV + (13381. + 1226. * ups) * cosV
	+ 12415. * sinQ + 26599. * cosZeta * sinQ
	- 4687. * cos2Zeta * sinQ - 12696. * sinZeta * cosQ
	- 4200. * sin2Zeta * cosQ +(2211. - 286*ups) * sinZeta*sin(2*Q)
	- 2208. * sin2Zeta * sin(2*Q)
	- 2780. * cosZeta * sin(2*Q) + 2022. * cos2Zeta*sin(2*Q)
	- 2842. * sinZeta * cos(2*Q) - 1594. * cosZeta * cos(2*Q)
	+ 2162. * cos2Zeta*cos(2*Q) );  /* terms with amplitudes
	    > 2000e-7;  some secular variation ignored. */
   el[6].omega = el[6].omega
	+ (0.077108 + 0.007186 * ups - 0.001533 * ups*ups) * sinV
	+ (0.045803 - 0.014766 * ups - 0.000536 * ups*ups) * cosV
	- 0.075825 * sinZeta * sinQ - 0.024839 * sin2Zeta*sinQ
	- 0.072582 * cosQ - 0.150383 * cosZeta * cosQ +
	0.026897 * cos2Zeta * cosQ;  /* all terms with amplitudes
	    greater than 0.02 degrees -- lots of others! */
   el[6].a = el[6].a + 1.0e-6 * (
	2933. * cosV + 33629. * cosZeta - 3081. * cos2Zeta
	- 1423. * cos(3*zeta) + 1098. * sinQ - 2812. * sinZeta * sinQ
	+ 2138. * cosZeta * sinQ  + 2206. * sinZeta * cosQ
	- 1590. * sin2Zeta*cosQ + 2885. * cosZeta * cosQ
	+ 2172. * cos2Zeta * cosQ);  /* terms with amplitudes greater
	   than 1000 x 1e-6 */

   strcpy(el[7].name,"Uranus ");
   el[7].incl = 0.772464 + 0.0006253*T + 0.0000395*Tsq;
   el[7].Omega = 73.477111 + 0.4986678*T + 0.0013117*Tsq;
   el[7].omega = 171.548692 + 1.4844328*T + 2.37e-4*Tsq - 6.1e-7*Tcb;
   el[7].a = 19.21814;
   el[7].daily = 1.1769022484e-2;
   el[7].ecc = 0.0463444 - 2.658e-5 * T;
   el[7].L_0 = 244.197470 + 429.863546*T + 0.000316*Tsq - 6e-7*Tcb;
   /* stick in a little bit of perturbation -- this one really gets
      yanked around.... after Meeus p. 116*/
   G = (83.76922 + 218.4901 * T)/DEG_IN_RADIAN;
   H = 2*G - S;
   el[7].L_0 = el[7].L_0 + (0.864319 - 0.001583 * ups) * sin(H)
	+ (0.082222 - 0.006833 * ups) * cos(H)
	+ 0.036017 * sin(2*H);
   el[7].omega = el[7].omega + 0.120303 * sin(H)
	+ (0.019472 - 0.000947 * ups) * cos(H)
	+ 0.006197 * sin(2*H);
   el[7].ecc = el[7].ecc + 1.0e-7 * (
	20981. * cos(H) - 3349. * sin(H) + 1311. * cos(2*H));
   el[7].a = el[7].a - 0.003825 * cos(H);

   /* other corrections to "true longitude" are ignored. */

   strcpy(el[8].name,"Neptune");
   el[8].incl = 1.779242 - 9.5436e-3 * T - 9.1e-6*Tsq;
   el[8].Omega = 130.681389 + 1.0989350 * T + 2.4987e-4*Tsq - 4.718e-6*Tcb;
   el[8].omega = 46.727364 + 1.4245744*T + 3.9082e-3*Tsq - 6.05e-7*Tcb;
   el[8].a = 30.10957;
   el[8].daily = 6.020148227e-3;
   el[8].ecc = 0.00899704 + 6.33e-6 * T;
   el[8].L_0 = 84.457994 + 219.885914*T + 0.0003205*Tsq - 6e-7*Tcb;
   el[8].L_0 = el[8].L_0
	- (0.589833 - 0.001089 * ups) * sin(H)
	- (0.056094 - 0.004658 * ups) * cos(H)
	- 0.024286 * sin(2*H);
   el[8].omega = el[8].omega + 0.024039 * sin(H)
	- 0.025303 * cos(H);
   el[8].ecc = el[8].ecc + 1.0e-7 * (
	4389. * sin(H) + 1129. * sin(2.*H)
	+ 4262. * cos(H) + 1089. * cos(2.*H));
   el[8].a = el[8].a + 8.189e-3 * cos(H);

/* crummy -- osculating elements a la Sept 15 1992 */

   d = jd - 2448880.5;  /* 1992 Sep 15 */
   T = d / 36525.;
   strcpy(el[9].name,"Pluto  ");
   el[9].incl = 17.1426;
   el[9].Omega = 110.180;
   el[9].omega = 223.782;
   el[9].a = 39.7465;
   el[9].daily = 0.00393329;
   el[9].ecc = 0.253834;
   el[9].L_0 = 228.1027 + 0.00393329 * d;
/*   printf("inc Om om : %f %f %f\n",el[9].incl,el[9].Omega,el[9].omega);
   printf("a  dail ecc: %f %f %f\n",el[9].a,el[9].daily,el[9].ecc);
   printf("L_0 %f\n",el[9].L_0);
*/
   el[1].mass = 1.660137e-7;  /* in units of sun's mass, IAU 1976 */
   el[2].mass = 2.447840e-6;  /* from 1992 *Astron Almanac*, p. K7 */
   el[3].mass = 3.040433e-6;  /* earth + moon */
   el[4].mass = 3.227149e-7;
   el[5].mass = 9.547907e-4;
   el[6].mass = 2.858776e-4;
   el[7].mass = 4.355401e-5;
   el[8].mass = 5.177591e-5;
   el[9].mass = 7.69e-9;  /* Pluto+Charon -- ? */

}

void planetxyz(p, jd, x, y, z)

	int p;
	double jd, *x, *y, *z;

/* produces ecliptic x,y,z coordinates for planet number 'p'
   at date jd. */

{
	double M, omnotil, nu, r;
	double e, LL, Om, om, ii;

/* see 1992 Astronomical Almanac, p. E 4 for these formulae. */

	ii = el[p].incl/DEG_IN_RADIAN;
	e = el[p].ecc;

	LL = (el[p].daily * (jd - jd_el) + el[p].L_0) / DEG_IN_RADIAN;
	Om = el[p].Omega / DEG_IN_RADIAN;
	om = el[p].omega / DEG_IN_RADIAN;

	M = LL - om;
	omnotil = om - Om;
	nu = M + (2.*e - 0.25 * pow(e,3.)) * sin(M) +
	     1.25 * e * e * sin(2 * M) +
	     1.08333333 * pow(e,3.) * sin(3 * M);
	r = el[p].a * (1. - e*e) / (1 + e * cos(nu));

	*x = r *
	     (cos(nu + omnotil) * cos(Om) - sin(nu +  omnotil) *
		cos(ii) * sin(Om));
	*y = r *
	     (cos(nu +  omnotil) * sin(Om) + sin(nu +  omnotil) *
		cos(ii) * cos(Om));
	*z = r * sin(nu +  omnotil) * sin(ii);
}


void planetvel(p, jd, vx, vy, vz)
	int p;
	double jd, *vx, *vy, *vz;
{
/* numerically evaluates planet velocity by brute-force
numerical differentiation. Very unsophisticated algorithm. */

	double dt; /* timestep */
	double x1,y1,z1,x2,y2,z2;

	dt = 0.1 / el[p].daily; /* time for mean motion of 0.1 degree */
	planetxyz(p, (jd - dt), &x1, &y1, &z1);
	planetxyz(p, (jd + dt), &x2, &y2, &z2);
	*vx = 0.5 * (x2 - x1) / dt;
	*vy = 0.5 * (y2 - y1) / dt;
	*vz = 0.5 * (z2 - z1) / dt;
	/* answer should be in ecliptic coordinates, in AU per day.*/
}

void xyz2000(jd,x,y,z)
	double jd, x, y, z;

/* simply transforms a vector x, y, and z to 2000 coordinates
   and prints -- for use in diagnostics. */
{
	double r1, d1, ep1, r2, d2, mod;

	mod = sqrt(x*x + y*y + z*z);
	xyz_cel(x,y,z,&r1,&d1);
	ep1 = 2000. + (jd - J2000)/365.25;
	precrot(r1,d1,ep1,2000.,&r2,&d2);
	x = mod * cos(r2/HRS_IN_RADIAN) * cos(d2/DEG_IN_RADIAN);
	y = mod * sin(r2/HRS_IN_RADIAN) * cos(d2/DEG_IN_RADIAN);
	z = mod * sin(d2/DEG_IN_RADIAN);
	printf("%f to 2000 -->  %f %f %f \n",ep1,x,y,z);

}

void earthview(x, y, z, i, ra, dec)

	double *x, *y, *z;
	int i;
	double *ra, *dec;

/* given computed planet positions for planets 1-10, computes
   ra and dec of i-th planet as viewed from earth (3rd) */

{
	double dx, dy, dz;

	dx = x[i] - x[3];
	dy = y[i] - y[3];
	dz = z[i] - z[3];


	xyz_cel(dx,dy,dz,ra,dec);

}

void pposns(jd,lat,sid,print_option,planra,plandec)

	double jd,lat,sid;
	short print_option;
	double *planra, *plandec;

/* computes and optionally prints positions for all the planets. */
/*  print_option 1 = print positions, 0 = silent */

{
	int i=0;
	double x[10], y[10], z[10], ha, alt, az, secz;
	double rasun, decsun, distsun, topora,topodec;
	double georamoon,geodecmoon,geodistmoon,toporamoon,topodecmoon,
	      topodistmoon;

	accusun(jd,0.,0.,&rasun,&decsun,&distsun,&topora,&topodec,x+3,y+3,z+3);
/*      planetxyz(3,jd,x+3,y+3,z+3);   get the earth first (EarthFirst!?)
	eclrot(jd,x+3,y+3,z+3);  */

	accumoon(jd,lat,sid,0.,&georamoon,&geodecmoon,&geodistmoon,
			 &toporamoon,&topodecmoon,&topodistmoon);

	if(print_option == 1) {  /* set up table header */
		oprntf("\n\nPlanetary positions (epoch of date), accuracy about 0.1 deg:\n");
		oprntf("\n             RA       dec       HA");
		oprntf("       sec.z     alt   az\n\n");

	/* Throw in the sun and moon here ... */
		oprntf("Sun    : ");
		put_coords(topora,1);
		oprntf("  ");
		put_coords(topodec,0);
		ha = adj_time(sid - topora);
		oprntf("  ");
		put_coords(ha,0);
		alt = altit(topodec,ha,lat,&az);
		secz = secant_z(alt);
 		if(fabs(secz) < 100.) oprntf("   %8.2f  ",secz);
		else oprntf("  (near horiz)");
		oprntf(" %5.1f  %5.1f\n",alt,az);
		oprntf("Moon   : ",el[i].name);
		put_coords(toporamoon,1);
		oprntf("  ");
		put_coords(topodecmoon,0);
		ha = adj_time(sid - toporamoon);
		oprntf("  ");
		put_coords(ha,0);
		alt=altit(topodecmoon,ha,lat,&az);
		secz = secant_z(alt);
 		if(fabs(secz) < 100.) oprntf("   %8.2f  ",secz);
		else oprntf("  (near horiz)");
		oprntf(" %5.1f  %5.1f\n\n",alt,az);
        }
	for(i = 1; i <= 9; i++) {
		if(i == 3) goto SKIP;  /* skip the earth */
		planetxyz(i,jd,x+i,y+i,z+i);
		eclrot(jd,x+i,y+i,z+i);
		earthview(x,y,z,i,planra+i,plandec+i);
		if(print_option == 1) {
			oprntf("%s: ",el[i].name);
			put_coords(planra[i],1);
			oprntf("  ");
			put_coords(plandec[i],0);
			ha = adj_time(sid - planra[i]);
			oprntf("  ");
			put_coords(ha,0);
			alt=altit(plandec[i],ha,lat,&az);
			secz = secant_z(alt);
 			if(fabs(secz) < 100.) oprntf("   %8.2f  ",secz);
			else oprntf("  (near horiz)");
			oprntf(" %5.1f  %5.1f",alt,az);
			if(i == 9) oprntf(" <-(least accurate)\n");
			else oprntf("\n");
		}
		SKIP: ;
	}
	if(print_option == 1) printf("Type command, or ? for menu:");
}

void barycor(jd,x,y,z,xdot,ydot,zdot)

	double jd,*x,*y,*z;
	double *xdot,*ydot,*zdot;

/* This routine takes the position
   x,y,z and velocity xdot,ydot,zdot, assumed heliocentric,
   and corrects them to the solar system barycenter taking into
   account the nine major planets.  Routine evolved by inserting
   planetary data (given above) into an earlier, very crude
   barycentric correction.  */

{

	int p;
	double xp, yp, zp, xvp, yvp, zvp;

	double xc=0.,yc=0.,zc=0.,xvc=0.,yvc=0.,zvc=0.;

	comp_el(jd);

	for(p=1;p<=9;p++) { /* sum contributions of the planets */
		planetxyz(p,jd,&xp,&yp,&zp);
		xc = xc + el[p].mass * xp;  /* mass is fraction of solar mass */
		yc = yc + el[p].mass * yp;
		zc = zc + el[p].mass * zc;
		planetvel(p,jd,&xvp,&yvp,&zvp);
		xvc = xvc + el[p].mass * xvp;
		yvc = yvc + el[p].mass * yvp;
		zvc = zvc + el[p].mass * zvc;
	/* diagnostic commented out ..... nice place to check planets if needed
		printf("%d :",p);
		xo = xp;
		yo = yp;
		zo = zp;
		eclrot(jd,&xo,&yo,&zo);
		xyz2000(jd,xo,yo,zo);
		printf("    ");
		xo = xvp;
		yo = yvp;
		zo = zvp;
		eclrot(jd,&xo,&yo,&zo);
		xyz2000(jd,xo,yo,zo);    */
	}
	/* normalize properly and rotate corrections to equatorial coords */
	xc = xc / SS_MASS;
	yc = yc / SS_MASS;
	zc = zc / SS_MASS;     /* might as well do it right ... */

	eclrot(jd, &xc, &yc, &zc);

/*      printf("posn corrn:");   diagnostic commented out
	xyz2000(jd,xc,yc,zc);
	printf(" vel corrn:");
	xyz2000(jd,(1.0e9 * xvc),(1.0e9 * yvc),(1.0e9 * zvc));  */

	xvc = xvc * KMS_AUDAY / SS_MASS;
	yvc = yvc * KMS_AUDAY / SS_MASS;
	zvc = zvc * KMS_AUDAY / SS_MASS;
	eclrot(jd, &xvc, &yvc, &zvc);

	/* add them in */
	*x = *x - xc;  /* these are in AU -- */
	*y = *y - yc;
	*z = *z - zc;
/*      xyz2000(jd,*x,*y,*z);   */
	*xdot = *xdot - xvc;
	*ydot = *ydot - yvc;
	*zdot = *zdot - zvc;

/* diagnostic -- trash variables
	-- scale for direct comparison with almanac
	xp = 1.0e9 * *xdot / KMS_AUDAY;
	yp = 1.0e9 * *ydot / KMS_AUDAY;
	zp = 1.0e9 * *zdot / KMS_AUDAY;
	xyz2000(jd,xp,yp,zp);
					*/
}

void helcor(jd,ra,dec,ha,lat,elevsea,tcor,vcor)

	double jd,ra,dec,ha;
  	double lat,elevsea,*tcor,*vcor;

/* finds heliocentric correction for given jd, ra, dec, ha, and lat.
   tcor is time correction in seconds, vcor velocity in km/s, to
   be added to the observed values.
   Input ra and dec assumed to be at current epoch */

{
	double x, y, z, xdot, ydot, zdot;
	double xobj,yobj,zobj;
	double ras, decs, dists, jd1, jd2, x1, x2, y1, y2, z1, z2;
	double topora, topodec;
	double x_geo, y_geo, z_geo;  /* geocentric coords of observatory */
	double a=499.0047837;  /* light travel time for 1 AU, sec  */

	dec=dec/DEG_IN_RADIAN; /* pass by value! */
	ra = ra/HRS_IN_RADIAN;
	ha = ha/HRS_IN_RADIAN;

	xobj = cos(ra) * cos(dec);
	yobj = sin(ra) * cos(dec);
	zobj = sin(dec);

/* diagnostic -- temporarily trash jd1
	jd1 = jd1 + etcorr(jd1) / SEC_IN_DAY;
	printf("TDT: %20f\n",jd1);  */

	jd1 = jd - EARTH_DIFF;
	jd2 = jd + EARTH_DIFF;

	accusun(jd1,0.,0.,&ras,&decs,&dists,&topora,&topodec,&x1,&y1,&z1);
	accusun(jd2,0.,0.,&ras,&decs,&dists,&topora,&topodec,&x2,&y2,&z2);
	accusun(jd,0.,0.,&ras,&decs,&dists,&topora,&topodec,&x,&y,&z);

/*      printf("ra dec distance:");  diagnostic -- commented out
	put_coords(ras,3);
	printf(" ");
	put_coords(decs,2);
	printf(" %f\n",dists);   */

	xdot = KMS_AUDAY*(x2 - x1)/(2.*EARTH_DIFF);  /* numerical differentiation */
	ydot = KMS_AUDAY*(y2 - y1)/(2.*EARTH_DIFF);  /* crude but accurate */
	zdot = KMS_AUDAY*(z2 - z1)/(2.*EARTH_DIFF);

/*      printf("Helio earth:");  diagnostic -- commmented out
	xyz2000(jd,x,y,z);
	xyz2000(jd,xdot,ydot,zdot);   */

	barycor(jd,&x,&y,&z,&xdot,&ydot,&zdot);
	*tcor = a * (x*xobj + y*yobj + z*zobj);
	*vcor = xdot * xobj + ydot * yobj + zdot * zobj;
/* correct diurnal rotation for elliptical earth including obs. elevation */
	geocent(0., lat, elevsea, &x_geo, &y_geo, &z_geo);
/* longitude set to zero arbitrarily so that x_geo = perp. distance to axis */
	*vcor = *vcor - 0.4651011 * x_geo * sin(ha) * cos(dec);
/* 0.4651011 = 6378.137 km radius * 2 pi / (86164.1 sec per sidereal day) */
/* could add time-of-flight across earth's radius here -- but rest of
   theory is not good to 0.02 seconds anyway. */
}

/* A couple of eclipse predictors.... */

float overlap(r1, r2, sepn)

	double r1, r2, sepn;
{


/* for two circles of radii r1 and r2,
   computes the overlap area in terms of the area of r1
   if their centers are separated by
   sepn. */

	float result, a, rlarge, rsmall;

	if(sepn > r1+r2) return(0.); /* They don't overlap */
	if((r2 < 0.) || (r1 < 0.)) return(-1.);  /* oops -- r's > 0.*/
	if(r2 > r1) {
		rlarge = r2;
		rsmall = r1;
	}
	else {
		rlarge = r1;
		rsmall = r2;
	}
	if(sepn < (rlarge - rsmall)) { /* small circle completely contained */
		if(r2 > r1) return(1.);  /* r1 is completely covered */
		else return((r2*r2) / (r1*r1)); /* ratio of areas */
	}
	a = (rlarge*rlarge + sepn*sepn - rsmall*rsmall) / (2.*sepn);
	   /* a is the distance along axis of centers to where circles cross */
	result = PI * rlarge * rlarge / 4.
	     - a * sqrt(rlarge * rlarge - a*a) / 2.
	     - rlarge * rlarge * asin(a/rlarge) / 2;
	a = sepn - a;
	result = result + PI * rsmall * rsmall / 4.
	     - a * sqrt(rsmall * rsmall - a*a) / 2.
	     - rsmall * rsmall * asin(a/rsmall) / 2.;
	result = result * 2.;
	return(result / (PI*r1*r1)); /* normalize to circle 1's area. */
}

void solecl(sun_moon,distmoon,distsun)

	double sun_moon,distmoon,distsun;


{
	double ang_sun, ang_moon; /* angular sizes */
	float magnitude; /* fraction of sun covered */

	ang_moon = DEG_IN_RADIAN * asin(RMOON / (distmoon * EQUAT_RAD));
	ang_sun = DEG_IN_RADIAN * asin(RSUN / (distsun * ASTRO_UNIT));

	if(sun_moon >= (ang_sun + ang_moon)) return ;

	else if(ang_sun >= ang_moon) {                   /* annular */
		magnitude = overlap(ang_sun,ang_moon,sun_moon);
		if(sun_moon > (ang_sun - ang_moon))
			oprntf("PARTIAL ECLIPSE OF THE SUN, %4.2f covered.\n",
				magnitude);
		else oprntf("ANNULAR ECLIPSE OF THE SUN, %4.2f covered.\n",
				magnitude);
	}
	else {
		magnitude = overlap(ang_sun,ang_moon,sun_moon);
		if(sun_moon > (ang_moon - ang_sun))
			oprntf("PARTIAL ECLIPSE OF THE SUN, %4.2f covered.\n",
				magnitude);
		else oprntf("TOTAL ECLIPSE OF THE SUN!\n");
	}
}

short lunecl(georamoon,geodecmoon,geodistmoon,rasun,decsun,distsun)

	double georamoon,geodecmoon,geodistmoon,rasun,decsun,distsun;


{
	/* quickie lunar eclipse predictor -- makes a number of
	   minor assumptions, e. g. small angle approximations, plus
	   projects phenomena onto a plane at distance = geocentric
	   distance of moon . */

	double ang_sun, ang_moon; /* angular sizes */
	double radius_um, radius_penum;
	double ra_shadow, dec_shadow;
	double lun_to_shadow;  /* angular separation of centerline of shadow
		  from center of moon ... */
	float magnitude;  /* portion covered */

	ang_sun = asin(RSUN / (distsun * ASTRO_UNIT));  /* radians */

	ra_shadow = rasun + 12.;
	if(ra_shadow > 24.) ra_shadow = ra_shadow - 24.; /* not strictly
	   necessary, perhaps, but wise */
	dec_shadow = -1. * decsun;
	radius_um = (1./geodistmoon - ang_sun) * DEG_IN_RADIAN;
	radius_penum = (1./geodistmoon + ang_sun) * DEG_IN_RADIAN;


	ang_moon = DEG_IN_RADIAN * asin(RMOON / (geodistmoon * EQUAT_RAD));
	lun_to_shadow = DEG_IN_RADIAN *
		subtend(georamoon,geodecmoon,ra_shadow,dec_shadow);
	if(lun_to_shadow > (radius_penum + ang_moon)) return (0);
	else if(lun_to_shadow >= (radius_um + ang_moon))  {
		if(lun_to_shadow >= (radius_penum - ang_moon)) {
			oprntf("PARTIAL PENUMBRAL (BRIGHT) ECLIPSE OF THE MOON.\n");
			return (1);
		}
		else  {
			oprntf("PENUMBRAL (BRIGHT) ECLIPSE OF THE MOON.\n");
			return (2);
		}
	}
	else if (lun_to_shadow >= (radius_um - ang_moon)) {
		magnitude = overlap(ang_moon,radius_um,lun_to_shadow);
		oprntf("PARTIAL UMBRAL (DARK) ECLIPSE OF THE MOON, %4.2f covered.\n",
		   magnitude);
		return(3);
	}
	else {
		oprntf("TOTAL ECLIPSE OF THE MOON!\n");
		return(4);
	}
}

void planet_alert(jd,ra,dec,tolerance)

	double jd,ra,dec,tolerance;

/* given a jd, ra, and dec, this computes rough positions
   for all the planets, and alerts the user if any of them
   are within less than a settable tolerance of the ra and dec. */

{
	double pra[10],pdec[10], angle;
	int i;

	comp_el(jd);
	pposns(jd,0.,0.,0,pra,pdec);
	for(i = 1; i<=9 ; i++) {
		if(i == 3) goto SKIP;
		angle = subtend(pra[i],pdec[i],ra,dec) * DEG_IN_RADIAN;
		if(angle < tolerance) {
			oprntf("-- CAUTION -- proximity to %s -- low-precision calculation shows\n ",
				el[i].name);
			oprntf("this direction as %5.2f deg away from %s ---\n",
				angle,el[i].name);
		}
		SKIP: ;
	}
}

short setup_time_place(date,longit,lat,stdz,use_dst,zone_name,
        zabr,site_name,enter_ut,night_date,jdut,jdlocal,jdb,jde,sid,
	curepoch)

struct date_time date;
double lat, longit, stdz, *jdut, *jdlocal, *jdb, *jde, *sid, *curepoch;
short use_dst, enter_ut, night_date;
char zabr;
char *site_name;
char *zone_name;

/* This takes the date (which contains the time), and the site parameters,
   and prints out a banner giving the various dates and times; also
   computes and returns various jd's, the sidereal time, and the epoch.
   Returns negative number to signal error if date is out of range of
   validity of algorithms, or if you specify a bad time during daylight-time
   change; returns zero if successful.  */

{

	double jd, jdloc, jdtest;
	if((date.y <= 1900) | (date.y >= 2100)) {
		oprntf("Date out of range - 1901 -> 2099\n");
		return(-1);
	}
	find_dst_bounds(date.y,stdz,use_dst,jdb,jde);
	oprntf("\nW Long (hms):");
	put_coords(longit,3);
	oprntf(", lat (dms):");
	put_coords(lat,2);
	oprntf(", std time zone %3.0f hr W\n",stdz);
	oprntf("\n");

        /* Establish the correct time from the input values ...
             This is done more compactly in the true_jd function, but
             the present code prints more warnings, etc, so leave it.*/

	if(enter_ut==0) {
		/* "night date" convention -- adjust date up by one if local
		      time is in morning. */
		if((night_date == 1) && (date.h < 12))
		      date.d = date.d + 1;  /* pass-by-value, I hope */
		jdloc = date_to_jd(date); /* local */
		jdtest = jdloc + stdz/24.; /* almost */
		if(use_dst != 0) {
		   if((fabs(jdtest-*jdb)<0.5) | (fabs(jdtest-*jde)<0.5)) {
		   oprntf("** be careful -- near standard and daylight change. ** \n");
		   }
		}
		if(use_dst > 0) {  /* handle exceptions during dst -- std change */
			if((jdtest - *jdb > 0.)
				&& (jdtest - *jdb < 0.041666667)) {
				/* this time doesn't really exist */
			  oprntf("Nonexistent input time during change to DST!\n");
			  oprntf("Please respecify time as 1 hour later.\n");
			  return(-1);  /* return error flag. */
			}
			else if((jdtest - *jde >= 0.0) &&
				(jdtest - *jde < 0.04166667)) {
			  oprntf("\nCAUTION - DST --> STD -- ambiguous input time defaults to STD!\n");
			  oprntf("Use 'g' option to specify UT unambiguously.\n");
			  jd = jdloc + stdz / 24.;
			}
			else jd = jdloc +
				zone(use_dst,stdz,jdtest,*jdb,*jde) / 24.;
		}
		else if(use_dst < 0) {  /* south -- reverse jdb & jde */
			if((jdtest - *jde > 0.)
				&& (jdtest - *jde < 0.041666667)) {
				/* this time doesn't really exist */
			  oprntf("Nonexistent input time during change to DST!\n");
			  oprntf("Please respecify time as 1 hour later.\n");
			  return(-1);  /* return error flag. */
			}
			else if((jdtest - *jdb >= 0.0) &&
				(jdtest - *jdb < 0.04166667)) {
			  oprntf("\nCAUTION - DST --> STD -- ambiguous input time defaults to STD!\n");
			  oprntf("Use 'g' option to specify UT unambiguously.\n");
			  jd = jdloc + stdz / 24.;
			}
			else jd = jdloc +
				zone(use_dst,stdz,jdtest,*jdb,*jde) / 24.;
		}
		else jd = jdtest;  /* not using dst ... no problem. */
		oprntf("Local Date and time: ");
		print_all(jdloc);
		oprntf(" ");
		print_tz(jd,use_dst,*jdb,*jde,zabr);
/*              if(fabs(frac_part(jdloc) - 0.5) < MIDN_TOL)
		   oprntf("\n(Watch for possible day error near midnight.)");
 .... this really shouldn't happen now. */
		if(use_dst != 0) {
		}

		oprntf("\n   UT Date and time: ");
		print_all(jd);
		if(fabs(frac_part(jd) - 0.5) < 0.00001)
		   oprntf("\n(Watch for possible day error near midnight.)");
	}
	else {  /* times entered are ut already */

		jd = date_to_jd(date);
		oprntf("   UT Date and time: ");
		print_all(jd);
		if(fabs(frac_part(jd) - 0.5) < 0.00001)
		   oprntf("\n(Watch for possible day error near midnight.)");
		jdloc = jd - zone(use_dst,stdz,jd,*jdb,*jde)/24.;
		if(use_dst != 0) {
		   if((fabs(jd-*jdb)<0.05) | (fabs(jd-*jde)<0.05))
			oprntf("\nCAUTION..dst/std time changing!");
		}
		oprntf("\nlocal Date and time: ");
		print_all(jdloc);
		oprntf(" ");
		print_tz(jd,use_dst,*jdb,*jde,zabr);
		if(fabs(frac_part(jdloc) - 0.5) < 0.00001)
		   oprntf("\n(Watch for possible day error near midnight.)");

	}
	if(use_dst != 0) {
		if(stdz==zone(use_dst,stdz,jd,*jdb,*jde))
			oprntf("\nDST assumed not in effect\n");
		else oprntf("\nDST assumed in effect.\n");
	}
	oprntf("\nJulian date: %lf",jd);
	*sid=lst(jd,longit);
	oprntf("   LMST: ");
	put_coords(*sid,3);
	*jdut = jd;
	*jdlocal = jdloc;
	*curepoch = 2000.+(jd-J2000)/365.25;
	return(0);
}

double ut_from_jd(double jd) {
  
  double tmp;
  struct date_time date;
  short dow;


  caldat(jd,&date,&dow);
  tmp = date.h + date.mn/60. + date.s/3600.;
  return(tmp);
}


void print_tonight(struct date_time date, double lat, double longit,
		   double elevsea, double elev, double horiz,
		   char *site_name, double stdz, char *zone_name,
		   char zabr, short use_dst, double *jdb, double *jde,
		   short short_long, Night_Times *ntimes, int print_flag)

/* Given site and time information, prints a summary of
   the important phenomena for a single night.

   The coding in this routine is extremely tortuous ... I even use
   the dreaded goto statement!  It's inelegant, but (a) the logic is
   indeed somewhat complicated and (b) it works.  */

{

	double jd, jdmid, jdcent, stmid, ramoon, decmoon, distmoon;
	double geora, geodec, geodist;  /* geocent for moon, not used here.*/
	double locjdb, locjde;
	double rasun, decsun, min_alt, max_alt;
	double hasunset, jdsunset=0.0, jdsunrise=0.0, sid;
	double hamoonset, tmoonrise=0.0, tmoonset,
		jdmoonrise, jdmoonset=0.0;
	double hatwilight, jdtwilight, jdetw18=0.0, jdmtw18=0.0;
	double jdetw12=0.0, jdmtw12=0.0;
	double ill_frac;
	short dow; /* day of week */
	float set_to_rise=0.0, twi_to_twi=0.0, moon_print;
		
    no_print= 1 - print_flag;   
    fprintf(stderr,"print_flag: %d  no_print %d:\n",print_flag,no_print);

	find_dst_bounds(date.y,stdz,use_dst,jdb,jde);
	locjdb = *jdb-stdz/24.;
	locjde = *jde-(stdz-1)/24.;
	date.h = 18;  /* local afternoon */
	date.mn = 0;
	date.s = 0;  /* afternoon */

	oprntf("\n#Almanac for %s:\n#long. ",site_name);
	put_coords(longit,2);
	oprntf(" (h.m.s) W, lat. ");
	put_coords(lat,1);
	oprntf(" (d.m), elev. %5.0f m\n",elevsea);
	jd = date_to_jd(date); /* not really jd; local equivalent */
	if(use_dst > 0) {
		oprntf("#%s Daylight Savings Time assumed from 2 AM on\n#",zone_name);
		print_calendar(locjdb,&dow);
		oprntf(" to 2 AM on ");
		print_calendar(locjde,&dow);
		oprntf("; standard zone = %3.0f hrs W",stdz);
		if((fabs(jd - locjdb) < 0.45) || (fabs(jd - locjde) < 0.45))
			oprntf("\n#   ** TIME CHANGE IS TONIGHT! **");
	}
	else if (use_dst < 0) {
		oprntf("#%s Daylight Savings Time used before 2 AM \n#",zone_name);
		print_calendar(*jdb-stdz/24.,&dow);
		oprntf(" and after 2 AM ");
		print_calendar(*jde-(stdz-1)/24.,&dow);
		oprntf("; standard zone = %3.0f hrs W",stdz);
		if((fabs(jd - locjdb) < 0.45) || (fabs(jd - locjde) < 0.45))
			oprntf("\n#   ** TIME CHANGE IS TONIGHT! **");
	}
	else oprntf("#%s Standard Time (%3.0f hrs W) in use all year.",zone_name,stdz);

	oprntf("\n#\n");

	oprntf("#For the night of: ");
	print_day(day_of_week(jd));
	oprntf(", ");
	print_calendar(jd,&dow); /* translate back e.g. 11/31-12/1 */
	jd = jd + .5;  /* local morning */
	oprntf(" ---> ");
	print_day(day_of_week(jd));
	oprntf(", ");
	print_calendar(jd,&dow);
	oprntf("\n");
	jd = jd - 0.25;  /* local midnight */
	jdmid = jd + zone(use_dst,stdz,jd,*jdb,*jde) / 24.;
					/* corresponding ut */
	oprntf("#Local midnight = ");
	print_calendar(jdmid,&dow);
	oprntf(", ");
	print_time(jdmid,-1); /* just the hours! */
	oprntf(" UT, or JD %11.3f\n",jdmid);
	oprntf("#Local Mean Sidereal Time at midnight = ");
	stmid = lst(jdmid,longit);
	put_coords(stmid,3);
	oprntf("\n#\n");

	accumoon(jdmid,lat,stmid,elevsea,
	   &geora,&geodec,&geodist,&ramoon,&decmoon,&distmoon);
	lpsun(jdmid,&rasun,&decsun);
	hasunset = ha_alt(decsun,lat,-(0.83+horiz));
	if(hasunset > 900.) {  /* flag for never sets */
		oprntf("#Sun up all night!\n");
		set_to_rise = 0.;
		twi_to_twi = 0.;
		jdcent = -1.;
		goto DO_MOON_ETC;  /* aargh! Ugly flow of control!! But
                   twilight certainly irrelevant if sun up all night, so
		   skip it. */
	}
	if(hasunset < -900.) {
		oprntf("#Sun down all day!\n");
		set_to_rise = 24.;
		jdcent = -1.;
		goto CHECK_TWI18;  /* More ugly flow  -
			checks for twilight even if sun down. */
	}
	jdsunset = jdmid + adj_time(rasun+hasunset-stmid)/24.;
		/* initial guess */
	jdsunset = jd_sun_alt(-(0.83+horiz),jdsunset,lat,longit);
	if(jdsunset > 0.) {
		fprintf(stdout,"jdsunset = %12.6f\n",jdsunset);
		oprntf("#Sunset (%5.0f m horizon): ",elev);
		print_time((jdsunset-zone(use_dst,stdz,jdsunset,*jdb,*jde)/24.),0);
		print_tz(jdsunset,use_dst,*jdb,*jde,zabr);
	}
	else oprntf("#Sunset not correctly computed; ");
	jdsunrise = jdmid + adj_time(rasun-hasunset-stmid)/24.;
	jdsunrise = jd_sun_alt(-(0.83+horiz),jdsunrise,lat,longit);
	if(jdsunrise > 0.) {
		oprntf("; Sunrise: ");
		print_time((jdsunrise-zone(use_dst,stdz,jdsunrise,*jdb,*jde)/24.),0);
		print_tz(jdsunrise,use_dst,*jdb,*jde,zabr);
	}
	if((jdsunrise > 0.) && (jdsunset > 0.)) {
		set_to_rise = (jdsunrise - jdsunset) * 24.;
		jdcent = (jdsunrise + jdsunset) / 2.;
	}
	else {
		oprntf("# Sunrise not correctly computed.");
		jdcent = -1.;
	}

	/* This block checks for and prints out 18-degree twilight
           as appropriate. */

	CHECK_TWI18: hatwilight = ha_alt(decsun,lat,-18.);
	if(hatwilight < -900.) {
		oprntf("\n#Full darkness all day (sun below -18 deg).\n");
		twi_to_twi = 24.;
		goto DO_MOON_ETC;  /* certainly no 12-degree twilight */
	}
	if(hatwilight > 900.) {
		oprntf("\n#Sun higher than 18-degree twilight all night.\n");
		twi_to_twi = 0.;
		goto CHECK_TWI12;  /* but maybe 12-degree twilight occurs ...*/
	}

	/* compute & print evening twilight and LST at eve. twilight */
	jdtwilight = jdmid + adj_time(rasun+hatwilight-stmid)/24.;  /* rough */
	jdtwilight = jd_sun_alt(-18.,jdtwilight,lat,longit);  /* accurate */
	jdetw18 = jdtwilight;
	if(jdtwilight > 0.) {
		oprntf("\n#Evening twilight: ");
		print_time((jdtwilight-zone(use_dst,stdz,jdtwilight,*jdb,*jde)/24.),0);
		sid = lst(jdtwilight,longit);
		print_tz(jdtwilight,use_dst,*jdb,*jde,zabr);
		oprntf(";  LMST at evening twilight: ");
		put_coords(sid,0);
		oprntf("\n");
	}
	else oprntf("#Evening twilight incorrectly computed.\n");

        /* Now do morning twilight */
	jdtwilight = jdmid + adj_time(rasun-hatwilight-stmid)/24.;
	jdtwilight = jd_sun_alt(-18.,jdtwilight,lat,longit);
	jdmtw18 = jdtwilight;
	if(jdtwilight > 0.) {
		oprntf("#Morning twilight: ");
		print_time((jdtwilight-zone(use_dst,stdz,jdtwilight,*jdb,*jde)/24.),0);
		sid = lst(jdtwilight,longit);
		print_tz(jdtwilight,use_dst,*jdb,*jde,zabr);
		oprntf(";  LMST at morning twilight: ");
		put_coords(sid,0);
	}
	else oprntf("#Morning twilight incorrectly computed.");
	if((jdetw18 > 0.) && (jdmtw18 > 0.)) twi_to_twi = 24. * (jdmtw18 - jdetw18);

        /* Now do same for 12-degree twilight */
	CHECK_TWI12: hatwilight = ha_alt(decsun,lat,-12.);
	if(hatwilight < -900.) {
		oprntf("\n#Sun always below 12-degree twilight...\n");
		goto DO_MOON_ETC;
	}
	if(hatwilight > 900.) {
		oprntf("\n#Sun always above 12-degree twilight...\n");
		goto DO_MOON_ETC;
	}
	jdtwilight = jdmid + adj_time(rasun+hatwilight-stmid)/24.;
	jdtwilight = jd_sun_alt(-12.,jdtwilight,lat,longit);
	jdetw12 = jdtwilight;
	if(jdtwilight > 0.) {
		oprntf("\n#12-degr twilight:");
		print_time((jdtwilight-zone(use_dst,stdz,jdtwilight,*jdb,*jde)/24.),0);
		print_tz(jdtwilight,use_dst,*jdb,*jde,zabr);
	}
	else oprntf("#Evening 12-degree twilight incorrectly computed.\n");

	jdtwilight = jdmid + adj_time(rasun-hatwilight-stmid)/24.;
	jdtwilight = jd_sun_alt(-12.,jdtwilight,lat,longit);
	jdmtw12 = jdtwilight;
	if(jdtwilight > 0.) {
		oprntf(" -->");
		print_time((jdtwilight-zone(use_dst,stdz,jdtwilight,*jdb,*jde)/24.),0);
		print_tz(jdtwilight,use_dst,*jdb,*jde,zabr);
		oprntf("; ");
	}
	else oprntf("#Morning 12-degree twilight incorrectly computed.");
	if((jdetw12 > 0.) && (jdmtw12 > 0.)) twi_to_twi = 24. * (jdmtw12 - jdetw12);

        DO_MOON_ETC:
        if(jdcent > 0.) {
		oprntf("#night center: ");
		print_time((jdcent-zone(use_dst,stdz,jdcent,*jdb,*jde)/24.),0);
		print_tz(jdcent,use_dst,*jdb,*jde,zabr);
	}
	oprntf("\n#\n");
	min_max_alt(lat,decmoon,&min_alt,&max_alt);  /* rough check -- occurs? */
	if(max_alt < -(0.83+horiz)) {
		oprntf("#Moon's midnight position does not rise.\n");
		jdmoonrise = -1.;
		goto MORE_MOON;
	}
	if(min_alt > -(0.83+horiz)) {
		oprntf("#Moon's midnight position does not set.\n");
		jdmoonrise = 1.;
		goto MORE_MOON;
	}

	/* compute moonrise and set if they're likely to occur */

	hamoonset = ha_alt(decmoon,lat,-(0.83+horiz)); /* rough approx. */
	tmoonrise = adj_time(ramoon-hamoonset-stmid);
	tmoonset = adj_time(ramoon+hamoonset-stmid);
	jdmoonrise = jdmid + tmoonrise / 24.;
	jdmoonrise = jd_moon_alt(-(0.83+horiz),jdmoonrise,lat,longit,elevsea);
	jdmoonset = jdmid + tmoonset / 24.;
	jdmoonset = jd_moon_alt(-(0.83+horiz),jdmoonset,lat,longit,elevsea);
	if(fabs(set_to_rise) > 10.) moon_print = 0.65*set_to_rise;
		 else moon_print = 6.5;

	/* more lousy flow of control, but it's nice to see the event which happens
	   first printed first .... */

	if(jdmoonset < jdmoonrise) goto PRINT_MOONSET;  /* OUCH!! */
	PRINT_MOONRISE: if((jdmoonrise > 0.) && (fabs(tmoonrise) < moon_print)) {
	  /* print it if computed correctly and more-or-less at night */
		oprntf("#Moonrise: ");
		print_time((jdmoonrise-zone(use_dst,stdz,jdmoonrise,*jdb,*jde)/24.),0);
		print_tz(jdmoonrise,use_dst,*jdb,*jde,zabr);
		oprntf("   ");
	}
	else if (jdmoonrise < 0.) oprntf("Moonrise incorrectly computed. ");
	if(jdmoonset < jdmoonrise) goto MORE_MOON;
	PRINT_MOONSET: if((jdmoonset > 0.) && (fabs(tmoonset) < moon_print)) {
		oprntf("#Moonset : ");
		print_time((jdmoonset-zone(use_dst,stdz,jdmoonset,*jdb,*jde)/24.),0);
		print_tz(jdmoonset,use_dst,*jdb,*jde,zabr);
		oprntf("   ");
	}
	else if (jdmoonset < 0.) oprntf("Moonset incorrectly computed. ");
	if(jdmoonset < jdmoonrise) goto PRINT_MOONRISE;

     MORE_MOON: ill_frac=0.5*(1.-cos(subtend(ramoon,decmoon,rasun,decsun)));
	oprntf("\n#Moon at civil midnight: ");
	oprntf("illuminated fraction %5.3f\n#",ill_frac);
	print_phase(jdmid);
	oprntf(", RA and dec: ");
	put_coords(ramoon,2);
	oprntf(", ");
	put_coords(decmoon,1);
	oprntf("\n#\n");

     /* print more information if desired */
     if(short_long == 2) {  /* wacky indenting here ... */
	oprntf("#The sun is down for %4.1f hr; %4.1f hr from eve->morn 18 deg twilight.\n#",
		set_to_rise,twi_to_twi);
	if((jdmoonrise > 100.) && (jdmoonset > 100.) &&
	   (twi_to_twi > 0.) && (twi_to_twi < 24.)) {
	  /* that is, non-pathological */
		if((jdmoonrise > jdetw18) && (jdmoonrise < jdmtw18)) /* rises at night */
			oprntf("%4.1f dark hours after end of twilight and before moonrise.\n",
			  (24.*(jdmoonrise - jdetw18)));
		if((jdmoonset > jdetw18) && (jdmoonset < jdmtw18)) /* sets at night */
			oprntf("%4.1f dark hours after moonset and before beginning of twilight.\n",
			  (24.*(jdmtw18 - jdmoonset)));
		if((jdmoonrise < jdetw18) && (jdmoonset > jdmtw18))
			oprntf("Bright all night (moon up from evening to morning twilight).\n");
		if((jdmoonrise > jdmtw18) && (jdmoonset < jdetw18))
			oprntf("Dark all night (moon down from evening to morning twilight).\n");
	}
     }  /* closes the wacky indent. */

#if 0
     printf("# UT_SUNSET ");print_time1(jdsunset,1);printf("\n");
     printf("# UT_EVE_18 ");print_time1(jdetw18,1);printf("\n");
     printf("# UT_EVE_12 ");print_time1(jdetw12,1);printf("\n");
     printf("# UT_MIDNIGHT ");print_time1(jdmid,1);printf("\n");
     printf("# UT_MORN_18 ");print_time1(jdmtw18,1);printf("\n");
     printf("# UT_MORN_12 ");print_time1(jdmtw12,1);printf("\n");
     printf("# UT_SUNRISE ");print_time1(jdsunrise,1);printf("\n");
     printf("# UT_MOONRISE ");print_time1(jdmoonrise,1);printf("\n");
     printf("# UT_MOONSET ");print_time1(jdmoonset,1);printf("\n");
     printf("# LST_SUNSET %9.6f\n",lst(jdsunset,longit));
     printf("# LST_EVE_18 %9.6f\n",lst(jdetw18,longit));
     printf("# LST_EVE_12 %9.6f\n",lst(jdetw12,longit));
     printf("# LST_MIDNIGHT %9.6f\n",lst(jdmid,longit));
     printf("# LST_MORN_18 %9.6f\n",lst(jdmtw18,longit));
     printf("# LST_MORN_12 %9.6f\n",lst(jdmtw12,longit));
     printf("# LST_SUNRISE %9.6f\n",lst(jdsunrise,longit));
     printf("# LST_MOONRISE %9.6f\n",lst(jdmoonrise,longit));
     printf("# LST_MOONSET %9.6f\n",lst(jdmoonset,longit));
#endif
     if (ntimes != NULL) {
       ntimes->jd_evening12 = jdetw12;
       ntimes->jd_morning12 = jdmtw12;
       ntimes->jd_evening18 = jdetw18;
       ntimes->jd_morning18 = jdmtw18;
       ntimes->jd_sunrise = jdsunrise;
       ntimes->jd_sunset = jdsunset;
       ntimes->ut_sunset = ut_from_jd(jdsunset);
       ntimes->ut_evening12 = ut_from_jd(jdetw12);
       ntimes->ut_evening18 = ut_from_jd(jdetw18);
       ntimes->ut_midnight = ut_from_jd(jdmid);
       ntimes->ut_morning12 = ut_from_jd(jdmtw12);
       ntimes->ut_morning18 = ut_from_jd(jdmtw18);
       ntimes->ut_sunrise = ut_from_jd(jdsunrise);
       ntimes->ut_moonrise = ut_from_jd(jdmoonrise);
       ntimes->ut_moonset = ut_from_jd(jdmoonset);
       ntimes->lst_sunset = lst(jdsunset,longit);
       ntimes->lst_midnight = lst(jdmid,longit);
       ntimes->lst_evening12 = lst(jdetw12,longit);
       ntimes->lst_morning12 = lst(jdmtw12,longit);
       ntimes->lst_evening18 = lst(jdetw18,longit);
       ntimes->lst_morning18 = lst(jdmtw18,longit);
       ntimes->lst_sunrise = lst(jdsunrise,longit);
       ntimes->lst_moonrise = lst(jdmoonrise,longit);
       ntimes->lst_moonset = lst(jdmoonset,longit);
       ntimes->ra_moon=ramoon;
       ntimes->dec_moon=decmoon;
       ntimes->percent_moon=ill_frac;
    }

    fflush(stderr);
    fflush(stdout);
    no_print=1;
}

void print_circumstances(objra,objdec,objepoch,jd,curep,
	mura_arcs,mura_sec,mudec,sid,lat,elevsea,horiz)

double objra,objdec,objepoch,curep,mura_arcs,mura_sec,mudec,lat,horiz;
double jd,sid,elevsea;

/* Given object, site, and time information, prints the circumstances
   of an observation.  The heart of the "calculator" mode. */

{
	double objra_adj,objdec_adj,curra,curdec,ha,alt,az,secz,par;
	float ill_frac;
	double ramoon,decmoon,distmoon,ang_moon,
		georamoon, geodecmoon, geodistmoon,
		rasun,decsun,distsun,x,y,z,
		toporasun,topodecsun,
		moon_alt,sun_alt,obj_moon,obj_lunlimb,sun_moon,Vmoon,
		obj_alt,eclong,eclat,tcor,vcor;
	short luneclcode = 0;

	oprntf("\n\nStd epoch--> RA:");
	put_coords(objra,3);
	oprntf(", dec:");
	put_coords(objdec,2);
	oprntf(", ep %7.2f\n",objepoch);
	if((mura_sec != 0.) | (mura_arcs != 0.) |(mudec != 0.)) {
		objra_adj = objra + (curep-objepoch)* mura_sec/3600.;
		objdec_adj = objdec + (curep-objepoch)*mudec/3600.;
		oprntf("Adj for p.m: RA:");
		put_coords(objra_adj,3);
		oprntf(", dec:");
		put_coords(objdec_adj,2);
		oprntf(", epoch %7.2f, equinox %7.2f\n", curep,objepoch);
		oprntf("(Annual proper motions:");
		mura_arcs = mura_sec * 15. *
		     cos(objdec / DEG_IN_RADIAN);
      		oprntf(" RA: %8.4f sec //%7.3f arcsec, ",mura_sec,mura_arcs);
		oprntf("dec: %7.3f)\n",mudec);
		precrot(objra_adj,objdec_adj,objepoch,curep,&curra,&curdec);
	}
	else precrot(objra,objdec,objepoch,curep,&curra,&curdec);
	oprntf("Current  --> RA:");
	put_coords(curra,3);
	oprntf(", dec:");
	put_coords(curdec,2);
	oprntf(", ep %7.2f\n",curep);
	ha = adj_time(sid - curra);
	oprntf("HA: ");
	put_coords(ha,2);
	alt=altit(curdec,ha,lat,&az);
	obj_alt = alt;
	/* test size of sec z to avoid overflowing space provided */
	secz = secant_z(alt);
 	if(fabs(secz) < 100.) oprntf("; sec.z = %8.3f",secz);
	else oprntf(" Obj very near horizon.");
	if(secz > 3.) oprntf(" -- Large airmass!\n");
	else if(secz < 0.) oprntf(" -- BELOW HORIZON.\n");
	else oprntf("\n");
	oprntf("altitude %6.2f, azimuth %6.2f, ",alt,az);
	par = parang(ha,curdec,lat);
	oprntf("parallactic angle %4.1f",par);
	/* also give +- 180 ..... */
	if((par <= 180.) && (par > 0.)) oprntf("  [%4.1f]\n\n",par - 180.);
	 else oprntf("  [%4.1f]\n\n",par + 180.);
	accumoon(jd,lat,sid,elevsea,&georamoon,&geodecmoon,&geodistmoon,
			&ramoon,&decmoon,&distmoon);
	accusun(jd,sid,lat,&rasun,&decsun,&distsun,
		&toporasun,&topodecsun,&x,&y,&z);
	alt=altit(topodecsun,(sid-toporasun),lat,&az);
	sun_moon = subtend(ramoon,decmoon,toporasun,topodecsun);
	ill_frac= 0.5*(1.-cos(sun_moon)); /* ever so slightly inaccurate ...
	   basis of ancient Greek limit on moon/sun distance ratio! */
	sun_moon = sun_moon * DEG_IN_RADIAN;
/*      printf("geocentric moon: ");
	put_coords(georamoon,4);
	printf("  ");
	put_coords(geodecmoon,3);
	printf(" %f \n",geodistmoon);  */
	sun_alt = alt;
	if(alt < -18.) oprntf("The sun is down; there is no twilight.\n");
	else {
		if (alt < -(0.83+horiz))
			oprntf("In twilight, sun alt %4.1f, az %5.1f ",alt,az);
		else oprntf("The sun is up, alt %4.1f, az %4.1f",alt,az);
		oprntf("; Sun at ");
		put_coords(toporasun,3);
		oprntf(", ");
		put_coords(topodecsun,2);
		oprntf("\n");
		if(sun_moon < 1.5) solecl(sun_moon,distmoon,distsun);
			/* check for solar eclipse if it's close */
		if (alt < -(0.83+horiz))
		  oprntf("Clear zenith twilight (blue) approx %4.1f  mag over dark night sky.\n",
				ztwilight(alt));
	}
	moon_alt=altit(decmoon,(sid-ramoon),lat,&az);
	if(moon_alt > -2.) {
		oprntf("Moon :");
		put_coords(ramoon,2);
		oprntf(",");
		put_coords(decmoon,1);
		oprntf(", alt %5.1f, az %5.1f;",moon_alt,az);
		oprntf("%6.3f illum.\n",ill_frac);
		print_phase(jd);
		obj_moon = DEG_IN_RADIAN * subtend(ramoon,decmoon,curra,curdec);
		if(fabs(obj_moon) > 10.) {
		  oprntf(".  Object is %5.1f degr. from moon.\n",obj_moon);
		}
		else  {
			ang_moon = DEG_IN_RADIAN * asin(RMOON / (distmoon * EQUAT_RAD));
			if((obj_lunlimb = obj_moon - ang_moon) > 0.)
			  oprntf(" ** NOTE ** Object %4.1f degr. from lunar limb!\n",obj_lunlimb);
			else oprntf(" ** NOTE ** You're looking AT the moon!\n");
		}
		if(sun_moon > 176.)
			luneclcode = lunecl(georamoon,geodecmoon,geodistmoon,
			rasun,decsun,distsun);
		if((moon_alt > 0.) && (obj_alt > 0.5) && (sun_alt < -9.)) {
		  /*if it makes sense to estimate a lunar sky brightness */
		  Vmoon =
		     lunskybright(sun_moon,obj_moon,KZEN,moon_alt,
				obj_alt,distmoon);

		     oprntf("Lunar part of sky bright. = %5.1f V mag/sq.arcsec (estimated).\n",Vmoon);
		     if(luneclcode != 0)
			oprntf(" NOT including effect of LUNAR ECLIPSE ...!\n");
		}
	}
	else {
		print_phase(jd);
		oprntf(".  The moon is down.\n");
	}
	eclipt(objra,objdec,objepoch,jd,&curep,&eclong,&eclat);
	if(fabs(eclat)<10.) {
		oprntf("Ecliptic latitude %4.1f; ",eclat);
		oprntf("watch for low-flying minor planets.\n");
	}
	planet_alert(jd,curra,curdec,PLANET_TOL);
	/* tolerance set to 3 degrees earlier. */
	helcor(jd,curra,curdec,ha,lat,elevsea,&tcor,&vcor);
	oprntf("Barycentric corrections: add %6.1f sec, %5.2f",tcor,vcor);
	oprntf(" km/sec to observed values.\n");
	oprntf("Barycentric Julian date = %14.6f\n",(jd+tcor/SEC_IN_DAY));
	printf("\nType command, 'f' for fast tour, '?' for a menu:");
}

void hourly_airmass(date,stdz,lat,longit,horiz,use_dst,objra,objdec,
  objepoch, mura_sec,mura_arcs,mudec)

/* Given a slew of information, prints a table of hourly airmass, etc.
   for use in scheduling observations.  Also prints sun and moon
   altitude when these are relevant.  Precesses coordinates as well. */

struct date_time date;
double stdz,lat,longit,horiz,objra,objdec,objepoch,mura_sec,mura_arcs,mudec;
short use_dst;

{

	double jd, jdb, jde, jdmid, curra, curdec, curep;
	double sid, ha, alt, az, secz, par, rasun, decsun, ramoon,
		decmoon,distmoon;
 	double ill_frac,sepn;
        long int jdclong;
	int nch;
	short i, hr_span, dow;
	char obj_name[40];
        double hasset, jdsset, jdsrise, jdcent;

	if((date.y <= 1900) | (date.y >= 2100)) {
		printf("Date out of range - 1901 -> 2099\n");
		return;
	}
	printf("Name of object:");
	nch = get_line(obj_name);

	find_dst_bounds(date.y,stdz,use_dst,&jdb,&jde);
	date.h = 24; /* local midn */
	date.mn = 0;
	date.s = 0;
	jdmid = date_to_jd(date) + stdz/24.;
	/* first approx.-imperfect near time change */
	jdmid = date_to_jd(date) +
		zone(use_dst,stdz,jdmid,jdb,jde)/24.;
	curep = 2000.+(jdmid - J2000)/365.25;
	oprntf("\n\n*** Hourly airmass for %s ***\n\n",obj_name);
	if((use_dst != 0) &&
		((fabs(jdmid - jdb) < 0.5) || (fabs(jdmid - jde) < 0.5)))
		oprntf("*** NOTE STD/DAYLIGHT TIME CHANGE TONIGHT ***\n");
	oprntf("Epoch %7.2f: RA ",objepoch);
	put_coords(objra,3);
	oprntf(", dec ");
	put_coords(objdec,2);
	oprntf("\n");
	oprntf("Epoch %7.2f: RA ",curep);
	precrot(objra,objdec,objepoch,curep,&curra,&curdec);
	put_coords(curra,3);
	oprntf(", dec ");
	put_coords(curdec,2);
	if((mura_sec != 0.) | (mura_arcs != 0.) | (mudec != 0))
	   oprntf("\n Caution .. proper motion ignored\n\n");
	else oprntf("\n\n");
	oprntf("At midnight: UT date ");
	print_calendar(jdmid,&dow);
	lpsun(jdmid,&rasun,&decsun);
	sid=lst(jdmid,longit);
	lpmoon(jdmid,lat,sid,&ramoon,&decmoon,&distmoon); /* close enuf */
	ill_frac=0.5*(1.-cos(subtend(ramoon,decmoon,rasun,decsun)));
	sepn = DEG_IN_RADIAN * subtend(ramoon,decmoon,curra,curdec);
	oprntf(", Moon %4.2f illum, %3.0f degr from obj\n",ill_frac,sepn);
	planet_alert(jdmid,curra,curdec,PLANET_TOL);   /* better know about it .... */
	oprntf("\n  Local      UT      LMST");
	oprntf("      HA     secz   par.angl. SunAlt MoonAlt\n\n");

        /* figure out how much to print ... */

        hasset = ha_alt(decsun,lat,-0.83);
        jdsset = jd_sun_alt(-0.83,(jdmid - (12. - hasset) / 24.),lat,longit);
        jdsrise = jd_sun_alt(-0.83,(jdmid + (12. - hasset) / 24.),lat,longit);
        jdcent = (jdsset + jdsrise) / 2.;   /* center of night .. not local mid */
        hr_span = (short) (12. * (jdsrise - jdsset) + 0.5);
 /*       printf("jdsset %f jdsrise %f jdcent %f hr_span %d\n",
                jdsset,jdsrise,jdcent,hr_span);   ... diagnostic */
        jdclong = (long) ((24. * jdcent) + 0.5);  /* round to nearest hour */
        jdcent = jdclong / 24. + 0.00001;  /* add a hair to prevent "24 00"
              rounding ugliness in time table. */
 /*       printf("jdcent .... %f\n",jdcent);  ... diagnostic */
	for(i=(-1 * hr_span);i<=hr_span;i++) {
		jd = jdcent + i/24.;
		sid=lst(jd,longit);
		lpsun(jd,&rasun,&decsun);
		if(altit(decsun,(sid-rasun),lat,&az) > 0.) goto SKIP;
		print_time((jd-zone(use_dst,stdz,jd,jdb,jde)/24.),0);
		oprntf("  ");
		print_time(jd,0);
		oprntf("  ");
	     /*	sid=lst(jd,longit); */
		put_coords(sid,0);
		oprntf("  ");
		ha = adj_time(sid - curra);
		put_coords(ha,0);
		oprntf("  ");
		alt=altit(curdec,ha,lat,&az);
	        if(alt < -(horiz)) oprntf(" (down)");
		else if(alt < 1.0) oprntf("(v.low)");
		else {
		     secz=secant_z(alt);
      		     oprntf(" %6.3f",secz);
		}
		par = parang(ha,curdec,lat);
		oprntf("  %6.1f ",par);
		/* lpsun(jd,&rasun,&decsun); */
		alt=altit(decsun,(sid-rasun),lat,&az);
		if(alt < -18.) oprntf("    ... ");
		   else oprntf("   %5.1f",alt);
		lpmoon(jd,lat,sid,&ramoon,&decmoon,&distmoon); /* close enuf */
		alt=altit(decmoon,(sid-ramoon),lat,&az);
		if(alt < -2.)  oprntf("    ... \n");
		   else oprntf("   %5.1f\n",alt);
                SKIP: ;
	}
}

void print_params(date,enter_ut,night_date,stdz,lat,longit,site_name,
    elevsea,elev,use_dst,objra,objdec,objepoch,mura_sec,mura_arcs,
    mudec)

    struct date_time date;
    short enter_ut;
    short night_date;
    double stdz;
    double lat;
    double longit;
    char *site_name;
    double elevsea;
    double elev;
    short use_dst;
    double objra;
    double objdec;
    double objepoch;
    double mura_sec;
    double mura_arcs;
    double mudec;

/* This simply prints a nicely formatted list of the *input* parameters
   without doing any computations.  Helpful for the perplexed user, and
   for checking things. */
{
	short dow;
	double jd;

	jd = date_to_jd(date);
	oprntf("\nCurrent INPUT parameter values:\n\n");
	oprntf("      DATE: ");
	print_calendar(jd,&dow);
	oprntf("\n      TIME:");
	print_time(jd,3);
	if(night_date == 1)
		oprntf("\nNIGHT_DATE:  ON    -- date applies all evening & next morning.\n");
	else oprntf("\nNIGHT_DATE:  OFF   -- date is taken literally.\n");
	if(enter_ut == 1)
		oprntf("  UT_INPUT:  ON    -- input times taken to be UT.\n");
	else oprntf("  UT_INPUT:  OFF   -- input times taken to be local.\n");
	if(use_dst == 0)
		oprntf("   USE_DST:   0    -- Standard time in use all year.\n");
	else oprntf("   USE_DST: %3d    -- Daylight savings used part of year.\n",
			use_dst);
	oprntf("\n");
	oprntf("            RA: ");
	put_coords(objra,4);
	oprntf("\n           DEC: ");
	put_coords(objdec,3);
	oprntf("\n   INPUT EPOCH:   %8.2f\n",objepoch);
	if((mura_sec == 0.) && (mura_arcs == 0.) && (mudec == 0.))
		oprntf("PROPER MOTIONS:  OFF\n");
	else    {
          mura_arcs = mura_sec * 15. * cos(objdec / DEG_IN_RADIAN);
	  oprntf("PROPER MOTIONS: RA(sec)=%6.3f, RA(arc)=%6.2f, DEC=%6.2f\n",
		mura_sec,mura_arcs,mudec);
        }
	oprntf("\nSITE: %s\n",site_name);
	oprntf("      E.longit. = ");
	put_coords(-15. * longit,1);
	oprntf(", latit. = ");
	put_coords(lat,1);
	oprntf(" (degrees)\n");
	oprntf("      Standard zone = %3.0f hrs ",fabs(stdz));
	if(stdz >= 0.) oprntf(" West\n");
	  else oprntf(" East\n");
	oprntf("      Elevation above horizon = %4.0f m, True elevation = %4.0f m\n",
		elev,elevsea);
}

void print_menu()
	{
	if(sclogfl != NULL) fprintf(sclogfl,"\n\n *** Menu Choices *** \n");
	oprntf("Circumstance calculator, type '=' for output.\n");
	oprntf("Commands are SINGLE (lower-case!) CHARACTERS as follows:\n");
	oprntf(" ? .. prints this menu; other information options are:\n");
	oprntf("i,f . 'i' prints brief Instructions and examples, 'f' fast tour\n");
	oprntf(" w .. prints info on internal Workings, accuracy & LEGALITIES\n");
	oprntf("TO SET PARAMETERS & OPTIONS, use these (follow the formats!):\n");
	oprntf(" r .. enter object RA, in hr min sec,  example: r 3 12 12.43\n");
	oprntf(" d .. enter object Dec in deg min sec, example: d -0 18 0\n");
	oprntf(" y .. enter date, starting with Year   example: y 1994 10 12\n");
#if SYS_CLOCK_OK == 1
     	oprntf("t,T: t = enter time, e.g.: t 22 18 02 [see 'g' and 'n']; T = right now+\n");
#else
     	oprntf(" t .. enter time, example: t 22 18 02 ; see 'g' and 'n' below\n");
#endif
     	oprntf(" n .. *toggles* whether date is used as 'evening' (default) or literal\n");
	oprntf(" g .. *toggles* whether time is used as Greenwich or local\n");
	oprntf(" e .. enter Epoch used to interpret input coords (default = 1950)\n");
	oprntf(" p .. enter object Proper motions (complicated, follow prompts).\n");
	oprntf(" s .. change Site (again, follow prompts).\n");
	oprntf(" l .. Look at current parameter values (no computation).\n");
	oprntf("TO CALCULATE AND SEE RESULTS, use these commands: \n");
	oprntf(" = .. type out circumstances for specified instant of time, ra, dec, etc.\n");
	oprntf(" a .. type out night's Almanac for specified (evening) date\n");
	oprntf(" h .. type out Hourly airmass table for specified date, ra, dec\n");
        oprntf(" o .. tabulate Observability at 2-week intervals (at full&new moon)\n");
	oprntf(" m .. Major planets -- print 0.1 deg positions for specified instant\n");
        oprntf(" x .. eXtra goodies: log file, object files, other calculators; try x? \n");
	oprntf(" Q .. QUIT .. STOPS PROGRAM. ---> ");
}

void print_tutorial()
{
	if(sclogfl != NULL) fprintf(sclogfl,"\n  *** Fast Guided Tour listing ***\n\n");
	oprntf("FAST GUIDED TOUR: (type 'f' to see this again).\n\n");
	oprntf("To explore this program quickly, try the following (in order):\n\n");
	oprntf("--> Specify an evening date (e.g., 1995 March 22) and then display\n");
	oprntf("    the almanac for that night by typing \n");
	oprntf("        y 1995 3 22   a \n");
	oprntf("--> Specify an ra and dec and then make an hourly airmass table\n");
	oprntf("    by typing, for instance \n");
	oprntf("        r 15 28 27  d 12 13 14  h \n");
	oprntf("    (Notice how the date you specified earlier is still in effect.)\n");
	oprntf("--> Specify a time (e.g., 4 50 00 AM local time) and display the\n");
	oprntf("    circumstances at that instant by typing\n");
	oprntf("        t 4 50 0  =\n");
	oprntf("--> Type 'o' and follow the prompts to show observability\n");
	oprntf("    through a season (tabulated at new & full moon).\n");
	oprntf("--> Type 'l' to list the current input parameters and options.\n");
	oprntf("--> Type 'm' to list planetary positions.\n");
#if SYS_CLOCK_OK == 1
 	oprntf("--> Type 'T' to set time & date to right now (+ settable offset).\n");
        oprntf("--> Type 'x?' for eXtra goodies menu; log file, object lists etc..\n");
	oprntf("--> Read the help texts; '?' prints a short menu, 'i' gives\n");
#else
	oprntf("--> Read the help texts; '?' prints a short menu, 'i' gives\n");
#endif
	oprntf("    more complete information, 'w' tells you about inner workings.\n");
	oprntf("    An upper-case 'Q' exits the program.\n");
	oprntf(" Give a command: ");
}

void print_examples() {

	char cdum;

	if(sclogfl != NULL) fprintf(sclogfl,"\n\n *** On-Line Documentation listing ***");
	oprntf("\n\nMost parameters are entered as a single character followed\n");
	oprntf("by a value; you then type an EQUALS SIGN to compute circumstances\n");
	oprntf("for current site, time, and celestial position. FOR EXAMPLE:\n");
	oprntf("You observe an object at RA 19 02 23.3, dec -5 18 17\n");
	oprntf("epoch 2000., on 1993 June 18, at 21 32 18 local time, from\n");
	oprntf("the site you've selected.  To calculate circumstances, type\n\n");
	oprntf("r 19 02 23.3 d -5 18 17 e 2000 y 1993 6 18 t 21 32 18 =\n\n");
	oprntf("Note: coords, dates and times are entered as TRIPLETS OF NUMBERS\n");
	oprntf("separated by blanks (not colons), and date format is y m d.\n");
	oprntf("Commands are CASE SENSITIVE (R is not r)!  Carriage returns\n");
	oprntf("can go almost anywhere. Parameters remain in effect until you\n");
	oprntf("change them, so in this example typing\n");
	oprntf("t 23 32 18 =  \n");
	oprntf("would give circumstances for the same object two hours later.\n\n");

	oprntf("Typing 'a' prints an almanac for the currently specified date -- be\n");
	oprntf("careful you have the correct night if you've used 'n' or 'g'. (below).\n\n");

	oprntf("(Hit return to continue listing...)");
	scanf("%c",&cdum);
	scanf("%c",&cdum);  /* first one eats fossil input.. */

	oprntf("\n\nTwo commands invoke options for the interpretation of times and dates.\n");
        oprntf("[N. B. -- This issue is a little confusing, but important .... ]\n");
	oprntf("By default, the program interprets dates as 'evening', so late PM and\n");
	oprntf("early AM times refer to the same night.  Typing 'n' (for 'night date')\n");
	oprntf("TOGGLES bewteen this and literal dates.  Also by default, input\n");
	oprntf("times are in local (zone) time, but input times can be interpreted as UT;\n");
	oprntf("typing 'g' TOGGLES between these.  Invoking UT mode disables night dates.\n");
	oprntf("Note that 'g', 'n' (and the epoch option 'e') do not cause calculation, but\n");
	oprntf("only affect interpretation of the relevant parameters at compute time.\n\n");

	oprntf("Typing 'l' (look) causes all the input parameters to be typed out without\n");
        oprntf("any computation, which is especially helpful if you're lost by 'g' or 'n'.\n\n");

	oprntf("Typing 'h' creates a table of airmass, etc. at hourly intervals through\n");
	oprntf("the night.  You may wish to redirect output (using system) to print.\n\n");

	oprntf("Typing 'o' computes observability of an object for each dark\n");
	oprntf("and bright run through a range of dates.\n\n");

	oprntf("(Hit return to continue listing...)");
	scanf("%c",&cdum);

	oprntf("\nTyping 'm' displays rough (see 'w' option) positions of major planets.\n\n");

	oprntf("To change observing sites, type 's' and answer the prompts.\n\n");

#if SYS_CLOCK_OK == 1
	oprntf("Typing 'T' sets BOTH the TIME and DATE to 'now' using the system clock.\n");
	oprntf("You can also specify a number of minutes into the future to set.  The status\n");
	oprntf("of the 'n' and 'g' options are properly taken into account.\n\n");
#endif
	oprntf("To enable an approximate proper motion correction, type 'p' and\n");
	oprntf("answer the prompts *carefully*.  Re-enter as zero to turn off.\n\n");
	oprntf("Typing 'x' invokes many useful commands -- log file, object files, etc!\n\n");
	oprntf("Type command, Q (upper case) to stop, or ? for a menu:");
}

void print_accuracy()

{

	char cdum;

        if(sclogfl != NULL) fprintf(sclogfl,"\n\n");
	oprntf("ACCURACY INFO:\n\n");
	oprntf("The distinctions between UTC, UT1, TDT (etc.) are ignored\n");
	oprntf("except that a rough correction to TDT is used for the moon.\n");
	oprntf("The solar ephemeris used is good to a few arcsec.  Moon positions\n");
	oprntf("positions are topocentric and +- about 30 arcsec, hence solar\n");
	oprntf("eclipse paths are +- 50 km and +- 1 min. All rise/set times are\n");
	oprntf("computed to about +-1 min; non-level horizon, site elevation,\n");
	oprntf("and refraction uncertainties are often larger than this.\n\n");
	oprntf("The lunar sky brightness model assumes ideal atmospheric conditions;\n");
	oprntf("true lunar contributions to sky will vary widely.  To compare\n");
	oprntf("a dark site has about V=21.5 mag/sq.arcsec (variable)! Twilight\n");
	oprntf("brightness prediction is for blue, and only very approximate.\n\n");
	oprntf("The planetary calculations are truncated, but the error should\n");
	oprntf("seldom exceed 0.1 degree; MV are best(1'), MJSU ok, Pluto worst.\n");
	oprntf("\nNote that the local sidereal time given is Mean, not true,\n");
	oprntf("and that it assumes the input is true UT, not UTC (< 1 second)\n");
	oprntf("(Hit return to continue listing...)");
	scanf("%c",&cdum);
	scanf("%c",&cdum);  /* first one usually eats fossil input.. */
	oprntf("\nDaylight savings time, if selected, is established using a\n");
	oprntf("site-specific convention (e.g., USA).  Beware of ambiguities\n");
	oprntf("and nonexistent times when the clock is reset.  If necessary,\n");
	oprntf("use the 'g' option and enter times and dates as Greenwich (UT),\n");
	oprntf("or disable DST in site params (see discussion under 'i').\n\n");
	oprntf("The *precession* routine used is a 'rigorous'");
	oprntf(" rotation matrix routine,\ntaken from L. Taff's Computational");
	oprntf(" Spherical Astronomy.\nIt uses IAU1976 constants, is good to < 1 arcsec in 50 years,\n");
	oprntf("and has no troubles near the pole.  Proper motion corrections\n");
	oprntf("are done crudely as x = x0 + mu * dt; this is inaccurate near\n");
	oprntf("the poles.  Use another routine if sub-arcsec accuracy is critical.\n");
	oprntf("Apparent place (with nutation, aberration) is NOT computed.\n");
	oprntf("(Hit return to continue listing...)");
	scanf("%c",&cdum);
	oprntf("\n\nThe parallactic angle follows Filippenko (1982, PASP 94, 715).\n");
	oprntf("\nThe barycentric ('heliocentric') corrections are computed using\n");
	oprntf("an elliptical earth orbit with a few periodic perturbations\n");
	oprntf("including lunar recoil.  The helio-to-barycentric transformation\n");
	oprntf("uses the same algorithms as the planetary postions.  Overall max error:\n");
	oprntf("< 0.2 sec and < 0.005 km/s.  Velocity corrn. includes earth rotation.\n");
	oprntf("\nThe galactic coordinate routine is rigorously accurate, and\n");
	oprntf("precesses to 1950 before transforming.  The ecliptic coord.\n");
	oprntf("routine is for coordinates of date and is good to < 1 arcsec.\n\n");
	oprntf("These routines are **not necessarily correct** at times very far from\n");
	oprntf("the present (1990s).  The program rejects input outside 1900-2100.\n");
	oprntf("\nWhen porting to a new machine, run tests to ensure\n");
	oprntf("correctness and accuracy.  Experience shows that compiler\npeculiarities arise ");
	oprntf("surprisingly often.\n");
	oprntf("(Hit return to continue listing...)");
	scanf("%c",&cdum);
	oprntf("CAUTIONS, LEGALITIES:\n");
	oprntf("Many routines take a time argument which is a double-precision\n");
	oprntf("floating-point julian date; on most workstations, this gives\n");
	oprntf("time resolution of < 0.1 second.  When porting\n");
	oprntf("to another machine or compiler, test that the accuracy is\n");
	oprntf("sufficient.  \n\n");
	oprntf("I cannot guarantee that this program is bug-free, and caution\n");
   /* although at this time I really don't know of any bugs ... ! */
	oprntf("that not all routines are thoroughly precise and rigorous.\n");
	oprntf("The user of this program is responsible for interpreting \n");
	oprntf("results correctly.  I disavow any legal liability for damages\n");
	oprntf("caused by use of this program.\n\n");
	oprntf("Program copyright 1993, John Thorstensen, Dartmouth College\n");
	oprntf("Permission hereby granted for scientific/educational use.\n");
	oprntf("Commercial users must license.  Please communicate problems or\n");
	oprntf("suggestions to the author, John Thorstensen, Dept. Physics\n");
	oprntf("and Astronomy, Dartmouth College, Hanover NH 03755\n");
	oprntf("John.Thorstensen@Dartmouth.edu\n");
	oprntf("\nType command, or ? to see menu again.:");
}

void print_legalities()

{
	printf("\nThis has been superseded -- this shouldn't print out.\n");
}


void ephemgen(ra, dec, ep, lat, longit)
	double ra, dec, ep, lat, longit;

{

/* Prompts for elements of an ephemeris, then
   generates a printout of the *geocentric*
   ephemeris of an object.  Uses current values of ra and dec;
   ignores observatory parameters, which are much more important
   for velocity than for time.  Not *strictly* correct in that
   earth's position in helio-to-geo conversion is computed (strictly
   speaking) for heliocentric jd, while ephemeris is for (presumably)
   geocentric jd.  This makes a difference of at most 30 km/s  over c,
   or about 1 in 10**4, for the heliocentric correction.  Pretty trivial.

   Observatory paramaters are used in determining observability of
   phenomenon.
*/

  double t0=0., per; /* ephemeris -- t0 + per * cycle count */
  int ibeg, iend, cycles;
  double curra, curdec, curep;
  double max_secz, min_alt, max_alt, min_ok_alt, sun_alt, max_ok_sun, secz;
  double sid, ha, altitude, az, rasun, decsun, hasun;
  double jd, jdstart, jdend, tcor, vcor;
  struct date_time tempdate;
  int valid_date = 0;
  short dow;

  printf("Prints geocentric times of repeating phenom, e.g. stellar eclipses.\n");
  printf("Not suitable for very short periods (seconds) -- too many events.\n");
  printf("You may optionally specify that only events observable from your site\n");
  printf("be printed.\n\n");
  printf("Give heliocentric period, days; negative exits:");
  scanf("%lf",&per);
  if(per < 0.) return;
  printf("Give heliocentric jd of time zero, *all* the digits!:");
  scanf("%lf",&t0);
  if(t0 < 2000000.) {
	printf("Exiting -- JD *has* to be greater than 2 million!\n");
	return;
  }
  printf("Give starting UT date for listing, yyyy mm dd, neg year exits:");
  if((valid_date = get_date(&tempdate)) < 0) return;
/*  printf("Give starting ut, h m s:");
  get_time(&tempdate);   Too much of pain to enter .. force to ut = 0 */
  tempdate.h = 0.;
  tempdate.mn = 0.;
  tempdate.s = 0.;
  jdstart = date_to_jd(tempdate);
  printf("Give ending date for listing:");
  get_date(&tempdate);
/*  printf("Give ending time:");
  get_time(&tempdate);  */
  jdend = date_to_jd(tempdate);
  printf("Give maximum airmass for listing, negative to print all:");
  scanf("%lf",&max_secz);
  if(max_secz > 0.) min_ok_alt = 90. - DEG_IN_RADIAN * acos(1/max_secz);
  else min_ok_alt = -100.;
  curep = 2000. + (jdstart - J2000)/365.25;    /* precess to check observ. */
  precrot(ra,dec,ep,curep,&curra,&curdec);
  min_max_alt(lat,curdec,&min_alt,&max_alt);
  if(max_alt < min_ok_alt) {
  oprntf("Object never rises this high -- max alt %5.1f deg -- returning!\n",
           max_alt);
	return;
  }

  printf("Give maximum allowable sun altitude to print, 90 to print all:");
  scanf("%lf",&max_ok_sun);

  oprntf("\nObject RA = ");
  put_coords(ra,3);
  oprntf(", dec = ");
  put_coords(dec,2);
  oprntf(" epoch %6.1f\n",ep);
  oprntf("Site long = ");
  put_coords(longit,3);
  oprntf("W (hr.mn.sec), lat = ");
  put_coords(lat,2);
  oprntf("N \n");
  oprntf("\nEphemeris: %lf + %14.8f E (Heliocentric)\n\n",t0, per);
  oprntf("Listing of events in interval from ");
  print_calendar(jdstart,&dow);
  oprntf(" UT to ");
  print_calendar(jdend,&dow);
  oprntf(" UT.\n");
  if(min_ok_alt > 0.) oprntf("Only events at sec(z) < %5.2f are shown.\n",
        max_secz);
  if(max_ok_sun < 89.)
	oprntf("Only events with sun altit. < %6.1f degrees are shown.\n\n",
	max_ok_sun);
  oprntf("First columns show cycle number, GEOCENTRIC jd, date and time (UT);\n");
  oprntf("Last show HA, sec(z), 'ngt' if night, sun alt if twilit, 'day' if day.\n\n");
  ibeg = (jdstart - t0) / per;  /* truncate */
  iend = (jdend - t0) / per;    /* truncate again */

  for(cycles = ibeg; cycles <= iend; cycles++) {
 	jd = t0 + (double) cycles * per;
	curep = 2000. + (jd - J2000)/365.25;    /* precess to current */
        precrot(ra,dec,ep,curep,&curra,&curdec);
	helcor(jd, curra, curdec, 0., 0., 0., &tcor, &vcor);
/*      oprntf("Raw = %12.4f, Correction: %f\n",jd,tcor);   diagnostic */
        jd = jd - tcor / SEC_IN_DAY;  /* subtract to convert to geo .*/
        sid = lst(jd,longit);
        ha = adj_time(sid - curra);
        altitude = altit(curdec,ha,lat,&az);
	secz = secant_z(altitude);
        if(altitude >= min_ok_alt) {
           lpsun(jd,&rasun,&decsun);  /* lpsun plenty good enough */
	   hasun = lst(jd,longit) - rasun;
	   sun_alt = altit(decsun,hasun,lat,&az);
           if(sun_alt < max_ok_sun) {
              oprntf("# %d  JD(geo) %12.4f = ",cycles,jd);
	      print_calendar(jd,&dow);
	      oprntf(", ");
              print_time(jd,3);
	      oprntf(" UT");
	      put_coords(ha,0);
	      if((secz < 10.) && (secz > 0.)) oprntf(" %5.2f",secz);
              else if (secz > 0.) oprntf(" v.low");
	      else oprntf(" down!");
	      if(sun_alt <= -18.) oprntf(" ngt");
	      else if(sun_alt <= -0.53) oprntf(" %3.0f",sun_alt);
	      else oprntf(" day");
	      oprntf("\n");
	   }
       }
  }
  oprntf("Listing done.  Type ? if you need a menu.\n");
}

#define ALT_3  19.47  /* 19.47 degrees altitude => sec z = 3 */
#define ALT_2  30.
#define ALT_15 41.81
#define SID_RATE 1.0027379093  /* sidereal / solar rate */

double hrs_up(jdup, jddown, jdeve, jdmorn)

	double jdup, jddown, jdeve, jdmorn;
{

   /* If an object comes up past a given point at jdup,
      and goes down at jddown, and evening and morning
      are at jdeve and jdmorn, computes how long
      object is up *and* it's dark.  ... Written as
      function 'cause it's done three times below. */

   double jdup2, jddown0;  /* for the next ... or previous ...
                            time around */

   if(jdup < jdeve) {
     if(jddown >= jdmorn)   /* up all night */
	return ((jdmorn - jdeve) * 24.);
     else if(jddown >= jdeve)  {
	/* careful here ... circumpolar objects can come back *up*
	   a second time before morning.  jdup and jddown are
	   the ones immediately preceding and following the upper
	   culmination nearest the center of the night, so "jdup"
	   can be on the previous night rather than the one we want. */
       jdup2 = jdup + 1.0/SID_RATE;
       if(jdup2 > jdmorn)  /* the usual case ... doesn't rise again */
	   return ((jddown - jdeve) * 24.);
       else return(((jddown - jdeve) + (jdmorn - jdup2)) * 24.);
     }
     else return(0.);
   }
   else if(jddown > jdmorn) {
     if(jdup >= jdmorn) return(0.);
     else {
	/* again, a circumpolar object can be up at evening twilight
	    and come 'round again in the morning ... */
        jddown0 = jddown - 1.0/SID_RATE;
	if(jddown0 < jdeve) return((jdmorn - jdup) * 24.);
        else return(((jddown0 - jdeve) + (jdmorn - jdup)) * 24.);
     }
   }
   else return((jddown - jdup)*24.);  /* up & down the same night ...
      might happen a second time in pathological cases, but this will
      be extremely rare except at very high latitudes.  */
}

void print_air(secz,prec)
    	double secz;
	short prec;
{
   if((secz > 0.) && (secz < 100.)) {
	if(prec == 0) oprntf(" %5.1f ",secz);
	         else oprntf(" %5.2f ",secz);
   }
   else if(secz > 0.) oprntf(" v.low ");
   else if(secz < 0.) oprntf("  down ");
}

void print_ha_air(ha,secz,prec1,prec2)
	double ha, secz;
	short prec1, prec2;
{
 	put_coords(ha,prec1);
	oprntf(" ");
	print_air(secz,prec2);
}

void obs_season(ra, dec, epoch, lat, longit)

   double ra, dec, epoch, lat, longit;

/* prints a table of observability through an observing
   season.  The idea is to help the observer come up
   with an accurately computed "range of acceptable
   dates", to quote NOAO proposal forms ... */

{
   int valid_date, nlun, nph, nch;
   char obj_name[40];
   short dow;
   double sun_twi;
   double jdstart, jdend, jd, jdtrunc, jdevedate;
   double jdmid, jdcent, jdeve, jdmorn, midnfrac;
   double hatwi, hasun, hacent, haeve=0.0, hamorn=0.0;
   double seczcent, seczeve=0.0, seczmorn=0.0;
   double dt15,dt2,dt3;
   double jdtrans, jd3_1, jd3_2, jd2_1, jd2_2,
	  jd15_1, jd15_2, rasun, decsun;
   double hrs_3=0.0, hrs_2=0.0, hrs_15=0.0;
   double min_alt, max_alt, altitude, az;
   double curep, curra, curdec;
   struct date_time tempdate;

   printf("This types out a summary of the observability of your object\n");
   printf("through the observing season.  Observability is summarized\n");
   printf("at new & full moon through a range of dates you specify.\n");
   printf("(a 24-line display holds about a 6-month range.)\n\n");
   if(fabs(lat) > 70.) {
	oprntf("NOTE: The site's geographical latitude is so high that there's\n");
	oprntf("a possibility that 'darkness' will last all day (depending\n");
        oprntf("on how you define the end of twilight below.)  In this case\n");
	oprntf("the HA and sec z at 'twilight' will be for +-12 hours from\n");
	oprntf("sun's lower culmination.\n\n");
   }

   printf("Give approx. starting date, yyyy mm dd, neg year exits:");

  if((valid_date = get_date(&tempdate)) < 0) return;
  tempdate.h =  0.;
  tempdate.mn = 0.;
  tempdate.s = 0.;
  jdstart = date_to_jd(tempdate);
  printf("Give ending date for listing:");
  if((valid_date = get_date(&tempdate)) < 0) return;
  jdend = date_to_jd(tempdate);
  printf("You must now specify the altitude of the sun which defines twilight.\n");
  printf("Type -18. , or give a non-standard choice:");
  scanf("%lf",&sun_twi);

  printf("Name of object:");     /* for labeling redirected output */
  nch = get_line(obj_name);

  jd = jdstart - lun_age(jdstart,&nlun);
  nph = 0;  /* jd is adjusted to last previous new moon */

  jdcent = (jd + jdend) / 2.;  /* temporary use of var */
  curep = 2000. + (jdcent - J2000)/365.25;    /* precess to check observ. */
  precrot(ra,dec,epoch,curep,&curra,&curdec);
  min_max_alt(lat,curdec,&min_alt,&max_alt);

  oprntf("\n          *** Seasonal Observability of %s ***\n",obj_name);
  oprntf("\n     RA & dec: ");
  put_coords(ra,3);
  oprntf(", ");
  put_coords(dec,2);
  oprntf(", epoch %6.1f\n", epoch);
  /*  printf("\nObject RA&dec: ");
  put_coords(curra,3);
  oprntf(", ");
  put_coords(curdec,2);
  oprntf(", epoch %6.1f\n", curep);  */

  oprntf("Site long&lat: ");
  put_coords(longit,3);
  oprntf(" (h.m.s) West, ");
  put_coords(lat,2);
  oprntf(" North.\n\n");


  if(max_alt < ALT_15)
    oprntf("NOTE -- This object is always at sec.z > %5.2f\n",
	1./cos((90. - max_alt) / DEG_IN_RADIAN));
  if(min_alt > ALT_3)
    oprntf("NOTE -- This object is always at sec.z < %5.2f\n",
	1./cos((90. - min_alt) / DEG_IN_RADIAN));

  oprntf("Shown: local eve. date, moon phase, hr ang and sec.z at (1) eve. twilight,\n");
  oprntf("(2) natural center of night, and (3) morning twilight; then comes number of\n");
  oprntf("nighttime hours during which object is at sec.z less than 3, 2, and 1.5.\n");
  oprntf("Night (and twilight) is defined by sun altitude < %4.1f degrees.\n\n",sun_twi);

  oprntf(" Date (eve) moon      eve            cent           morn");
  oprntf("     night hrs@sec.z:\n");
  oprntf("                   HA  sec.z      HA  sec.z      HA  sec.z");
  oprntf("     <3   <2   <1.5\n");


  while(jd <= jdend) {
       if(nph == 0) nph = 2;
       else if(nph == 2) {
	  nlun++;
	  nph = 0;
       }
       flmoon(nlun,nph,&jd);

       /* Take care to compute for the date nearest full or new ...
	  people may use this as a lunar calendar, which it sort of
	  isn't. */

       midnfrac = 0.5 + longit/24.;  /* rough fractional part of jd
		       for local midnight ... */
       jdtrunc = (double)((long) jd);
       jdmid = jdtrunc + midnfrac;
       if((jd - jdmid) > 0.5) jdmid = jdmid + 1.;
       else if((jd - jdmid) < -0.5) jdmid = jdmid - 1.;

       lpsun(jdmid,&rasun,&decsun);
       hasun = adj_time(lst(jdmid,longit) - rasun); /* at rough midn. */
       if(hasun > 0.) jdcent = jdmid + (12. - hasun) / 24.;
       else jdcent = jdmid - (hasun + 12.) / 24.;
	  /* jdcent is very close to sun's lower culmination
	      -- natural center of night */

       hatwi = ha_alt(decsun,lat,sun_twi);
       if(hatwi > 100.) {
	      jdeve = -1.;    /* signal -- no twilight */
              jdmorn = -1.;
       }

       else {
	  if(hatwi < -100.) {  /* always night */
	        jdeve = jdcent - 0.5; /* sensible, anyway  */
	        jdmorn = jdcent + 0.5;
          }
          else {
		jdmorn = jd_sun_alt(sun_twi,(jdcent +(12.-hatwi)/24.),
	           lat,longit);
          	jdeve = jd_sun_alt(sun_twi,(jdcent - (12.-hatwi)/24.),
	           lat,longit);
          }
          haeve = adj_time(lst(jdeve,longit) - curra);
          altitude = altit(curdec,haeve,lat,&az);
	  seczeve = secant_z(altitude);

          hamorn = adj_time(lst(jdmorn,longit) - curra);
          altitude = altit(curdec,hamorn,lat,&az);
	  seczmorn = secant_z(altitude);
       }

       hacent = adj_time(lst(jdcent,longit) - curra);
       altitude = altit(curdec,hacent,lat,&az);
       seczcent = secant_z(altitude);

       jdtrans = jdcent - hacent / (SID_RATE * 24.);
	  /* this will be the transit nearest midnight */
       if((min_alt < ALT_3) && (max_alt > ALT_3)) {
	 /* if it makes sense to compute the times when
	    this object passes three airmasses ... */
	  dt3 = ha_alt(curdec,lat,ALT_3) / (SID_RATE * 24.);
	  jd3_1 = jdtrans - dt3;
	  jd3_2 = jdtrans + dt3;
       }
       else {
	  jd3_1 = 0.;  jd3_2 = 0.;
       }
        if((min_alt < ALT_2) && (max_alt > ALT_2)) {
	 /* if it makes sense to compute the times when
	    this object passes two airmasses ... */
	  dt2 = ha_alt(curdec,lat,ALT_2) / (SID_RATE * 24.);
	  jd2_1 = jdtrans - dt2;
	  jd2_2 = jdtrans + dt2;
       }
       else {
	  jd2_1 = 0.;  jd2_2 = 0.;
       }
       if((min_alt < ALT_15) && (max_alt > ALT_15)) {
	 /* if it makes sense to compute the times when
	    this object passes 1.5 airmasses ... */
	  dt15 = ha_alt(curdec,lat,ALT_15) / (SID_RATE * 24.);
	  jd15_1 = jdtrans - dt15;
	  jd15_2 = jdtrans + dt15;
       }
       else {
	  jd15_1 = 0.;  jd15_2 = 0.;
       }

   /* Now based on times of twilight and times at which object
      passes the airmass points, figure out how long the object
      is up at night ... */

      if(jdeve > 0.) {  /* if twilight occurs ... */
         if(jd2_1 != 0.)
	   hrs_2 = hrs_up(jd2_1, jd2_2,jdeve,jdmorn);
         else if(min_alt > ALT_2)
	   hrs_2 = 24. * (jdmorn - jdeve);  /* always up ... */
         else hrs_2 = 0.;                   /* never up ... */

         if(jd3_1 != 0.)
	   hrs_3 = hrs_up(jd3_1, jd3_2,jdeve,jdmorn);
         else if(min_alt > ALT_3)
	   hrs_3 = 24. * (jdmorn - jdeve);
         else hrs_3 = 0.;

         if(jd15_1 != 0.)
           hrs_15 = hrs_up(jd15_1, jd15_2,jdeve,jdmorn);
         else if(min_alt > ALT_15)
	   hrs_15 = 24. * (jdmorn - jdeve);
         else hrs_15 = 0.;
      }
      else if(jdeve > -500.) {  /* twilight all night */
	  hrs_2 = 0.;
	  hrs_3 = 0.;
	  hrs_15 = 0.;
      }


       /* now print the table itself.  Take pains to
	  get the correct *evening* date .... */

       jdevedate = jdcent - longit/24. - 0.25;
       planet_alert(jdcent,curra,curdec,PLANET_TOL);
         /* better know about it ... */

       print_calendar(jdevedate,&dow);

       /* space table correctly -- what a pain ! */
       caldat(jdevedate,&tempdate,&dow);
       if(tempdate.d < 10) oprntf(" ");

       if(nph == 0) oprntf("   N");
       else oprntf("   F");
       if(jdeve > 0.) print_ha_air(haeve,seczeve,0,0);
       else oprntf(" twi.all.nght! ");
       print_ha_air(hacent,seczcent,0,0);
       if(jdmorn > 0.) print_ha_air(hamorn,seczmorn,0,0);
       else oprntf(" twi.all.nght! ");

       oprntf(" %4.1f  %4.1f  %4.1f \n",hrs_3,hrs_2,hrs_15);

   }
   printf("Listing done.  'f' gives tutorial, '?' prints a menu.\n");
}

#if SYS_CLOCK_OK == 1
#include <time.h>
#endif

#if SYS_CLOCK_OK == 1

int get_sys_date(date, use_dst, enter_ut, night_date, stdz, toffset)

	struct date_time *date;
	short use_dst, enter_ut, night_date;
        double stdz, toffset;
{
	/* Reads the system clock; loads up the date structure
           to conform to the prevailing conventions for the interpretation
           of times.  Optionally adds "toffset" minutes to the system
           clock, as in x minutes in the future. */

	time_t t, *tp;
	struct tm *stm;
        double jd, jdb, jde;
        short dow;

	tp = &t;  /* have to initialize pointer variable for it to
                     serve as an argument. */

	t = time(tp);
	if(t == -1) {
		oprntf("#SYSTEM TIME UNAVAILABLE!  Date & time left alone.\n");
		return(-1);
        }
	stm = localtime(&t);
	date->y = (short) (stm->tm_year + 1900);
	date->mo = (short) (stm->tm_mon + 1);
	date->d = (short) (stm->tm_mday);
	date->h = (short) (stm->tm_hour);
	date->mn = (short) (stm->tm_min);
	date->s = (float) (stm->tm_sec);

	if(toffset != 0.) {
	   jd = date_to_jd(*date);
	   jd = jd + toffset / 1440.;
	   caldat(jd,date,&dow);
        }

        if(enter_ut == 1)  { /* adjust if needed */
           find_dst_bounds(date->y,stdz,use_dst,&jdb,&jde);
	   jd = date_to_jd(*date);
	   jd = jd + zone(use_dst,stdz,jd,jdb,jde)/24.;
           caldat(jd,date,&dow);
        }
	oprntf("\n#DATE AND TIME SET AUTOMATICALLY USING SYSTEM CLOCK ... to\n#");
	print_all(date_to_jd(*date));
	if(enter_ut == 0) oprntf(" local time\n#");
        else oprntf(" Universal time\n#");
      	if((night_date == 1) && (date->h < 12)) {
		date->d = date->d - 1;
		oprntf("#(night-date is on, so internal value of date set to last evening)\n#");
	}
/*
	oprntf("#(NOTE: time does NOT automatically update; 'T' reads clock again.)\n#");
*/
        return(0); /* success */
}

#endif     /* previous routine depends on sys clock */

void indexx(n,arrin,indx)

/* Sort routine from Press et al., "Numerical Recipes in C",
  1st edition, Cambridge University Press. */

int n,indx[];
float arrin[];
{
	int l,j,ir,indxt,i;
	float q;

	for (j=1;j<=n;j++) indx[j]=j;
	l=(n >> 1) + 1;
	ir=n;
	for (;;) {
		if (l > 1)
			q=arrin[(indxt=indx[--l])];
		else {
			q=arrin[(indxt=indx[ir])];
			indx[ir]=indx[1];
			if (--ir == 1) {
				indx[1]=indxt;
				return;
			}
		}
		i=l;
		j=l << 1;
		while (j <= ir) {
			if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;
			if (q < arrin[indx[j]]) {
				indx[i]=indx[j];
				j += (i=j);
			}
			else j=ir+1;
		}
		indx[i]=indxt;
	}
}

struct objct {
	char name[20];
	double ra;
	double dec;
	float ep;
	float xtra;  /* mag, whatever */
};

struct objct objs[MAX_OBJECTS];
int nobjects;

int read_obj_list()

/* Reads a list of objects from a file.  Here's the rules:
     -- each line of file  must be formatted as follows:

name   rahr ramn rasec   decdeg decmin decsec   epoch   [optional number]

    -- the name cannot have any spaces in it.
    -- if the declination is negative, the first character of the
         declination degrees must be a minus sign, and there can
         be no blank space after the minus sign (e. g., -0 18 22 is ok;
	 - 12 13 14 is NOT ok).
    -- other than that, it doesn't matter how you space the lines.

    I chose this format because it's the standard format for
    pointing files at my home institution.  If you'd like to code
    another format, please be my guest!  */

{
        FILE *inf;
	char fname[60], resp[10];
	char buf[200];
	char decstr[10];
	double rah, ram, ras, ded, dem, des, ept;
        int i, nitems;
	float xtr;

	printf("\nThis reads from a file of objects.  Format is as follows,\n\n");
	printf("name_no_blanks<20char   hr mn sec  deg mn sec  epoch  [opt._user_float]\n\n");
	printf("with exactly 1 object per line, blanks between fields, otherwise free-form.\n");
        printf("Anything after the optional user-defined floating pt number is ignored.\n");
	printf("Error checking is unsophisticated; maximum of %d objects.\n\n",
		(MAX_OBJECTS-1));
	printf("Give name of file of objects (or QUIT):");
	scanf("%s",fname);

	if(strcmp(fname,"QUIT") == 0) {
		printf("Quitting ... did not attempt to open a file.\n");
		return(-1);
	}

	inf = fopen(fname,"r");

	if(inf == NULL) {
		printf("\nFILE DID NOT OPEN!  ... I quit.  Try again if you want.\n");
		return(-1);
	}
	else printf("\nopened ... \n");

	if(nobjects != 0) {
		printf("\nYou have %d objects already!\n",nobjects);
	        printf("Type a to append, or r to replace:");
		scanf("%s",resp);
		if(resp[0] == 'r') nobjects = 0;
	}
        /*  on first pass be sure xtra's have a value, just in case. */
        else for(i = 1; i < MAX_OBJECTS; i++) objs[i].xtra = 0.0;

	while((fgets(buf,200,inf) != NULL) && (nobjects < MAX_OBJECTS - 1)) {
		nobjects++;   /* this will be 1-indexed */
		nitems = sscanf(buf,"%s %lf %lf %lf  %s %lf %lf  %lf %f",
			objs[nobjects].name,&rah,&ram,&ras,decstr,&dem,&des,
				&ept,&xtr);
		if(nitems >= 8) {  /* a little error checking here ... */
	             objs[nobjects].ra = rah + ram/60. + ras/3600.;
		     sscanf(decstr,"%lf",&ded);   /* careful with "-0" */
		     if(decstr[0] == '-') {
			if(ded <= 0.)
				ded = ded * -1.;
			objs[nobjects].dec =
					-1.* (ded + dem/60. + des/3600.);
		     }
	             else objs[nobjects].dec = ded + dem/60. + des/3600.;
                     objs[nobjects].ep = ept;
		     if(nitems == 9) objs[nobjects].xtra = xtr;
		     else if(nitems == 8) objs[nobjects].xtra = 99.9;
                }
	        else {
		     printf("Ignoring bad line: %s",buf);
		     nobjects--;
	        }
        }
	printf("\n .... %d objects read from file.\n",nobjects);
	if(nobjects == MAX_OBJECTS - 1)
	  printf("** WARNING ** AT MAX NUMBER OF OBJECTS. You may have missed some.\n");
	fclose(inf);
	return(0);  /* success */
}

int find_by_name(ra, dec, epoch, date, use_dst, enter_ut, night_date, stdz,
		lat, longit)
	double *ra, *dec, epoch, stdz, lat, longit;
	struct date_time date;
	short use_dst, enter_ut, night_date;

{

	/* finds object by name in list, and sets ra and dec to
           those coords if found.  Precesses to current value of
           epoch. */

	char objname[20];
	int i, found = 1;
	double jd, curep, curra, curdec, sid, ha, alt, az, secz, precra, precdec;

	if(nobjects == 0) {
		printf("No objects!\n");
		return(0);
	}

	jd = true_jd(date, use_dst, enter_ut, night_date, stdz);

	if(jd < 0.) {
		printf(" ... exiting because of time error!  Try again.\n");
		return(-1);
	}

	sid = lst(jd, longit);
	curep = 2000. + (jd - J2000) / 365.25;

	printf("RA and dec will be set to list object you name.\n");
	printf("Give exact object name:");
	scanf("%s",objname);

	i = 1;
	while((i <= nobjects) &&
		((found = strcmp(objs[i].name,objname)) != 0)) i++;
        if(found == 0) {
		if(objs[i].ep != epoch) {
		        precrot(objs[i].ra,objs[i].dec,objs[i].ep,
                                epoch,&precra,&precdec);
		}
		else {
			precra = objs[i].ra;
			precdec = objs[i].dec;
		}
		*ra = precra;
		*dec = precdec;
		printf("\nObject found -- name, coords, epoch, user#, HA, airmass --- \n\n");
		printf("%s  ",objs[i].name);
		put_coords(objs[i].ra,3);
		printf("  ");
		put_coords(objs[i].dec,2);
		printf("  %6.1f  %5.2f ",objs[i].ep,objs[i].xtra);
               	precrot(objs[i].ra,objs[i].dec,
				objs[i].ep,curep,&curra,&curdec);
    		ha = adj_time(sid - curra);
 		alt=altit(curdec,ha,lat,&az);
		secz = secant_z(alt);
		print_ha_air(ha,secz,0,1);
		printf("\n\n COORDINATES ARE NOW SET TO THIS OBJECT.\n");
		if(objs[i].ep != epoch)
		printf("(RA & dec have been precessed to %6.1f, your standard input epoch.)\n",
			epoch);
		return(0);
	}
	else {
		printf("Not found in %d entries.\n",i);
		return(-1);
	}
}

void type_list(date, use_dst, enter_ut, night_date, stdz,
		lat, longit)
     	double stdz, lat, longit;
	struct date_time date;
	short use_dst, enter_ut, night_date;


{
	int i;
	double jd, curep, curra, curdec, sid, ha, alt, az, secz;
	short nstart = 1, nend, ok;
	char errprompt[40];

	if(nobjects == 0) {
		printf("No objects!\n");
		return;
	}

	jd = true_jd(date, use_dst, enter_ut, night_date, stdz);

	if(jd < 0.) {
		printf(" ... exiting because of time error!  Try again.\n");
		return;
	}

	sid = lst(jd, longit);
	curep = 2000. + (jd - J2000) / 365.25;

	printf("(Listing will show name, coords, epoch, user#, HA and airmass.)\n");

	strcpy(errprompt,"ERROR IN INPUT ... ");
	oprntf("%d objects in list.\n",nobjects);
/*	while((nobjects > 0) && (nstart > 0)) { used to loop -- too tricky. */
	   /* get out the heavy input checking artillery to avoid
                running away here ... */
	  /*      printf("First and last (numbers) to list, -1 exits:"); */
	        printf("First and last (numbers) to list:");
		ok = getshort(&nstart,-1,(short)nobjects,errprompt);
		/* if(nstart < 0) break; */
		ok = getshort(&nend,nstart,(short)nobjects,errprompt);
		if(nend > nobjects) nend = nobjects;
		if(nstart > nend) nstart = nend;
		oprntf("\n\n");
		print_current(date,night_date,enter_ut);
		oprntf("\n");
                for(i = nstart; i <= nend; i++) {
 			oprntf("%20s ",objs[i].name);
			put_coords(objs[i].ra,3);
			oprntf("  ");
			put_coords(objs[i].dec,2);
			oprntf("   %6.1f  %5.3f ",objs[i].ep,objs[i].xtra);
               		precrot(objs[i].ra,objs[i].dec,
				objs[i].ep,curep,&curra,&curdec);
    			ha = adj_time(sid - curra);
			alt=altit(curdec,ha,lat,&az);
			secz = secant_z(alt);
			print_ha_air(ha,secz,0,1);
			oprntf("\n");
		}
		oprntf("\n");
/*	}*/
}

int find_nearest(ra, dec, epoch, date, use_dst, enter_ut, night_date, stdz,
		lat, longit)

	double *ra, *dec, epoch, stdz, lat, longit;
	struct date_time date;
	short use_dst, enter_ut, night_date;

/* given ra,dec, and epoch, sorts items in list with
   respect to arc distance away, and queries user
   whether to accept.  */

{
 	int i, ind[MAX_OBJECTS];
	double precra, precdec, jd, sid, ha, alt, az,
		secz, seczob=0.0, curra, curdec, curep, hacrit, altcrit=0.0, aircrit;
	float arcs[MAX_OBJECTS];
	char resp[10];
	int found = 0;
        short sortopt,nprnt;

	if(nobjects == 0) {
		printf("No objects!\n");
		return(-1);
	}

	jd = true_jd(date, use_dst, enter_ut, night_date, stdz);

	if(jd < 0.) {
		printf(" ... exiting because of time error!  Try again.\n");
		return(-1);
	}

	printf("Select:\n");
	printf("  1 .. to rank objects by arc dist from current coords;\n");
	printf("  2 .. to rank objects by absolute value of hour angle:\n");
	printf("  3 .. sort by proximity in airmass to present coords:\n");
	printf("  4 .. sort by imminence of setting (reaching critical airmass):\n");
        printf("  5 .. sort by user-supplied optional number.\n");
	printf("  Type 1, 2, 3, 4, or 5 ---> ");
	scanf("%hd",&sortopt);

	if(sortopt == 4) {
		printf("Give critical airmass in west:");
	        scanf("%lf",&aircrit);
		if(aircrit < 1.) {
			oprntf("Airmass must be > 1. ... exiting!\n");
			return(-1);
		}
		altcrit = DEG_IN_RADIAN * asin(1.0 / aircrit);
	}
	sid = lst(jd, longit);
	curep = 2000. + (jd - J2000) / 365.25;

	 /* compute present airmass for option 3 */
        if(sortopt == 3) {
	    precrot(*ra,*dec,epoch,
                         curep,&curra,&curdec);
	    ha = adj_time(sid - curra);
	    alt=altit(curdec,ha,lat,&az);
	    seczob = secant_z(alt);
        }

	for(i = 1; i <= nobjects; i++) {
		if(sortopt == 1) {   /* sort by arc distance */
		   if(objs[i].ep != epoch)
 	  		precrot(objs[i].ra,objs[i].dec,objs[i].ep,
                                epoch,&precra,&precdec);
 		   else {
			precra = objs[i].ra;
			precdec = objs[i].dec;
		   }
	           arcs[i] = subtend(*ra,*dec,precra,precdec);
		}
		else if (sortopt == 2) {  /* sort by hour angle */
		   precrot(objs[i].ra,objs[i].dec,objs[i].ep,
				curep,&curra,&curdec);
	           arcs[i] = fabs(sid - curra);
		}
		else if (sortopt == 3) {  /* sort by difference of airmass */
		   precrot(objs[i].ra,objs[i].dec,objs[i].ep,
				curep,&curra,&curdec);
    		   ha = adj_time(sid - curra);
		   alt=altit(curdec,ha,lat,&az);
	    	   arcs[i] = fabs(secant_z(alt) - seczob);
		}
                else if (sortopt == 4) {  /* sort by proximity to critical airmass */
		   precrot(objs[i].ra,objs[i].dec,objs[i].ep,
				curep,&curra,&curdec);
		   hacrit = ha_alt(curdec,lat,altcrit);
                   if(fabs(hacrit) > 24.) arcs[i] = 100.;
    		   else {
	         	ha = adj_time(sid - curra);
                        arcs[i] = hacrit - ha;
		        if(arcs[i] < 0.) arcs[i] = 100.;
		   }
               }
               else   /* sort by user-supplied extra number */
		   arcs[i] = (double) objs[i].xtra;
	}

	indexx(nobjects,arcs,ind);

	printf("If you now select an object, RA & dec will be set to its coords.\n\n");
	if(sclogfl != NULL) fprintf(sclogfl,"\n\n *** Sorted object listing *** \n");
	oprntf("Listed for each: Name, ra, dec, epoch, user-defined #,\n");
	if(sortopt == 1) oprntf("arclength to coords (deg), ");
        if(sortopt == 4) oprntf("minutes til crit secz, ");
	oprntf("HA, secz, computed for the following date & time:\n\n");
	print_current(date,night_date,enter_ut);
	oprntf("\n\n");
	if((sortopt != 2) && (sortopt != 4)) { /* print relevant info */
	    oprntf("Current coords: ");
	    put_coords(*ra,3);
	    oprntf("  ");
            put_coords(*dec,2);
	    oprntf("  %6.1f ",epoch);
	    precrot(*ra,*dec,epoch,
                                curep,&curra,&curdec);
	    ha = adj_time(sid - curra);
	    alt=altit(curdec,ha,lat,&az);
	    seczob = secant_z(alt);
	    print_ha_air(ha,seczob,0,1);
	    oprntf("\n\n");
	}

	i = 1;
	while(found == 0) {
	    for(nprnt=1;nprnt<=10;nprnt++) {
		precrot(objs[ind[i]].ra,objs[ind[i]].dec,
				objs[ind[i]].ep,
                                   curep,&curra,&curdec);
    		ha = adj_time(sid - curra);
		alt=altit(curdec,ha,lat,&az);

 		oprntf("%2d %13s",i,objs[ind[i]].name);
		put_coords(objs[ind[i]].ra,3);
		oprntf(" ");
		put_coords(objs[ind[i]].dec,2);
		oprntf(" %6.1f %6.2f ",objs[ind[i]].ep, objs[ind[i]].xtra);
		if(sortopt == 1) oprntf(" %6.3f",arcs[ind[i]] * DEG_IN_RADIAN);
                if(sortopt == 4) oprntf(" %5.0f",arcs[ind[i]] * 60.);
		secz = secant_z(alt);
		print_ha_air(ha,secz,0,1);
                oprntf("\n");
                if(nprnt == 5) oprntf("\n");
		i++;
	        if(i > nobjects) break;
            }
	    printf("Type number to select an object, m to see more, q to quit:");

 	    scanf("%s",resp);
 	    if(resp[0] == 'q') {
			oprntf("Abandoning search.\n");
			return(found = -1);
	    }
	    else if((resp[0] == 'm') || (resp[0] == 'M')) {
		if(i > nobjects) {
			oprntf("Sorry -- that's all you have!\n");
			oprntf("Search abandoned.\n");
	        	return(found = -1);
                }
	    }
	    else if(isdigit(resp[0]) != 0) {
		sscanf(resp,"%d",&i);
		if((i < 0) || (i > nobjects)) {
		     	oprntf("BAD OBJECT INDEX -- %d -- start over!\n",i);
			return(-1);
		}
 		if(objs[ind[i]].ep != epoch)
			     precrot(objs[ind[i]].ra,objs[ind[i]].dec,
				objs[ind[i]].ep,
                                   epoch,&precra,&precdec);
		else {
			precra = objs[ind[i]].ra;
			precdec = objs[ind[i]].dec;
		}
                *ra = precra;
		*dec = precdec;
		oprntf("\n%s  ",objs[ind[i]].name);
		put_coords(objs[ind[i]].ra,3);
		oprntf("  ");
		put_coords(objs[ind[i]].dec,2);
		oprntf("  %6.1f  %5.2f ",objs[ind[i]].ep,objs[ind[i]].xtra);
               	precrot(objs[ind[i]].ra,objs[ind[i]].dec,
				objs[ind[i]].ep,curep,&curra,&curdec);
    		ha = adj_time(sid - curra);
 		alt=altit(curdec,ha,lat,&az);
		secz = secant_z(alt);
		print_ha_air(ha,secz,0,1);
		oprntf("\n\n COORDINATES ARE NOW SET TO THIS OBJECT.\n");
		if(objs[ind[i]].ep != epoch)
   		oprntf("(RA & dec have been precessed to %6.1f, your current standard epoch.)\n",
				epoch);
		return(found = 1);
	    }
	    else {
		printf("Unrecognized response ... continuing ..\n");
		if(i > nobjects) {
			printf("That's all the objects .. abandoning search.\n");
			return(found = -1);
		}
            }
       }
	return(0);
}

void set_zenith(struct date_time date, short use_dst, short enter_ut, 
		short night_date, double stdz, double lat,
		double longit, double epoch, double *ra, double *dec)

/* sets RA and dec to zenith as defined by present time and date;
   coords are set to actual zenith but in currently used epoch.  */

{
	double jd, curep;

	jd = true_jd(date, use_dst, enter_ut, night_date, stdz);

	if(jd < 0.) return;  /* nonexistent time. */

	*ra = lst(jd, longit);

        *dec = lat;

	curep = 2000. + (jd - J2000) / 365.25;

	precrot(*ra,*dec,curep,epoch,ra,dec);

	oprntf("\n#COORDINATES SET TO ZENITH: ");
	put_coords(*ra,3);
	oprntf(", ");
	put_coords(*dec,2);
	oprntf(", epoch %6.1f\n#",epoch);
}


void printephase(date, use_dst, enter_ut, night_date, stdz, lat,
	  longit, epoch, ra, dec)

/* prints phase of a repeating phenomenon at this instant. */

struct date_time date;
short use_dst, enter_ut, night_date;
double stdz, lat, longit, epoch, ra, dec;

{
 	double P=0., T0=0.;
	double phase, jd, hjd, tcor, vcor, curep, curra, curdec;

	while(P == 0.) {
		printf("Give period (days) of repeating phenom.:");
		getdouble(&P,1.0e-5,1.0e6,"Give period again:");
	}
	while(T0 == 0.) {
		printf("Give epoch T-0 (Julian date) :");
		getdouble(&T0,1500000.,3.0e6,"Give epoch again:");
	}

	jd = true_jd(date, use_dst, enter_ut, night_date, stdz);

	if(jd < 0.) {
		printf("nonexistent time.\n");
	        return;
	}

 	oprntf("\n\nRA & dec: ");
  	put_coords(ra,3);
  	oprntf(", ");
  	put_coords(dec,2);
  	oprntf(", epoch %6.1f\n", epoch);
	oprntf("Ephemeris: %lf + %14.8f E (Heliocentric)\n",T0, P);
	oprntf("Julian date %f --> UT ",jd);
	print_all(jd);
	oprntf("\n\n");

	curep = 2000. + (jd - J2000) / 365.25;

	precrot(ra,dec,epoch,curep,&curra,&curdec);

	helcor(jd,curra,curdec,0.,0.,0.,&tcor,&vcor);
		/* throwing away vcor .... tcor doesn't need ha, lat,
			or elevation. */
	hjd = jd + tcor/86400.;

	phase = (jd - T0) / P;
	oprntf("If input time already helio, phase is %f\n",phase);
	if(phase < 0.) {
		phase = phase - (int) phase + 1;
		oprntf(" ... i.e., %f\n",phase);
	}
	phase = (hjd - T0) / P;
	oprntf("Making helio correction for current coords, phase is %f\n",
		phase);
	if(phase < 0.) {
		phase = phase - (int) phase + 1;
		oprntf(" ... i.e., %f\n",phase);
	}
	oprntf("(Helio correction is %f seconds, helio J.D. is %lf)\n",
		tcor,hjd);

}

int set_to_jd(date, use_dst, enter_ut, night_date, stdz, jd)

	struct date_time *date;
	short use_dst, enter_ut, night_date;
        double stdz, jd;
{
	/* Takes a jd and loads up the date structure
           to conform to the prevailing conventions for the interpretation
           of times. */

        short dow;
        double jdb, jde, jdloc;

	if((jd < FIRSTJD) || (jd > LASTJD)) {
		oprntf("JD out of range .. allowed %f to %f\n",
			FIRSTJD,LASTJD);
		oprntf("(Corresponds to 1901 --> 2099 calendrical limits.\n");
		oprntf("No action taken ... try again if you want.\n");
		return(-1);
	}

	caldat(jd,date,&dow);   /* first get year (at least) */

        if(enter_ut != 1)  { /* adjust if needed */
           find_dst_bounds(date->y,stdz,use_dst,&jdb,&jde);
	   jd = date_to_jd(*date);
	   jdloc = jd - zone(use_dst,stdz,jd,jdb,jde)/24.;
	   if(use_dst > 0) {  /* north ... daylight savings */
	      if((jde - jd) < 0.041666667 && (jde - jd) > 0.) {
 		oprntf("\n\n IN AMBIGUOUS DST/STD HOUR -- TIME WILL NOT BE CORRECT \n");
	        oprntf("\n   ... use 'g' to set UT input, then do over.\n\n");
	      }
	   }
	   else if(use_dst < 0) {  /* south ... daylight savings */
	      if((jdb - jd) < 0.041666667 && (jdb - jd) > 0.) {
 		oprntf("\n\n IN AMBIGUOUS DST/STD HOUR -- TIME WILL NOT BE CORRECT \n");
	        oprntf("\n   ... use 'g' to set UT input, then do over.\n\n");
	      }
	   }
           caldat(jdloc,date,&dow);
        }
	oprntf("\nDATE AND TIME SET FROM JD ... to\n");
	print_all(date_to_jd(*date));
	if(enter_ut == 0) oprntf(" local time\n");
        else oprntf(" Universal time\n");
      	if((night_date == 1) && (date->h < 12)) {
		date->d = date->d - 1;
		oprntf("(night-date is on, so internal value of date set to last evening)\n");
	}
        return(0); /* success */
}


