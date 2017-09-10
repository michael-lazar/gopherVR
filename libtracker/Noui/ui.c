/* unix/ui.c 
	vi:ts=3 sw=3:
 */

/* Set terminal discipline to non blocking io and such.
 */
/* $Id: ui.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $
 * $Log: ui.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:23  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:45  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.11  1995/03/11  23:06:51  espie
 * Display everything in titlebars for xterms.
 *
 * Revision 4.10  1995/03/06  23:36:30  espie
 * Proper color patch.
 *
 * Revision 4.9  1995/03/03  14:24:40  espie
 * Color fixed.
 *
 * Revision 4.8  1995/02/27  14:26:34  espie
 * Cleaned up by Rolf Grossmann.
 *
 * Revision 4.7  1995/02/26  23:08:55  espie
 * POSIX_CONFORMANT.
 *
 * Revision 4.6  1995/02/24  15:40:38  espie
 * Removed all dependencies from show (cached PREF_SHOW) and
 * fixed a small display bug therefore.
 *
 * Revision 4.5  1995/02/21  17:59:26  espie
 * Internal problem: bad log with rcs.
 *
 * Revision 4.4  1995/02/08  13:47:35  espie
 * *** empty log message ***
 *
 * Revision 4.3  1995/01/28  09:56:53  espie
 * Port to FreeBSD/NeXTstep.
 *
 * Revision 4.2  1995/01/28  09:23:38  espie
 * Support for old bsd io discipline (NeXT)
 *
 *
 * Revision 4.0  1994/01/11  18:02:31  espie
 * Major change: lots of new calls.
 * Stupid termio bug: shouldn't restore term to sanity if we don't
 * know what sanity is. For instance, if we haven't modified anything.
 * cflags interpreted correctly.
 * Hsavolai fix.
 * Added bg/fg test.
 */


#include <sys/ioctl.h>
#include <signal.h>

#include "defs.h"
#include "extern.h"
#include "tags.h"
#include "prefs.h"

extern char *VERSION;

#ifdef USE_TERMIOS
#include <sys/termios.h>	/* this should work on all posix hosts */
#endif
#ifdef USE_SGTTY				/* only NeXt does, currently */
#include <sgtty.h>
#include <fcntl.h>
#endif

#ifdef __hpux
#include <sys/bsdtty.h>
#endif

LOCAL void nonblocking_io P((void));
LOCAL void sane_tty P((void));

LOCAL void (*INIT)P((void)) = nonblocking_io;


/* poor man's timer */
LOCAL int current_pattern;
LOCAL int count_pattern, count_song;
#define SMALL_DELAY 25

/* do not define any stdio routines if it's known not to work */


int run_in_fg()
   {
   return TRUE;
   }

LOCAL void sane_tty()
   {
   }

LOCAL struct tag end_marker;

struct tag *get_ui()
   {
   end_marker.type = TAG_END;
   return &end_marker;
   }

#ifdef NOTUSED

#ifdef USE_SGTTY
LOCAL struct sgttyb sanity;
LOCAL struct sgttyb *psanity = 0;
#endif
#ifdef USE_TERMIOS
LOCAL struct termios sanity;
LOCAL struct termios *psanity = 0;
#endif

LOCAL int is_fg;

/* signal handler */

LOCAL void goodbye(sig)
int sig;
   {
   static char buffer[25];
	char *pter = buffer;

	if (get_pref_scalar(PREF_COLOR))
		pter = write_color(pter, 0);
	sprintf(pter, "Signal %d", sig);
   end_all(pter);
   }

LOCAL void abort_this(sig)
int sig;
   {
   end_all("Abort");
   }

#ifdef SIGTSTP
LOCAL void suspend(sig)
int sig;
   {
	static char buffer[25];
	char *buf = buffer;
	
	if (get_pref_scalar(PREF_COLOR))
		buf = write_color(buf, 0);
	*buf = 0;
	puts(buf);
   fflush(stdout);
   sane_tty();
   signal(SIGTSTP, SIG_DFL);
   kill(0, SIGTSTP);
   }
#endif

int run_in_fg()
   {
   int val;

		/* this should work on every unix */
	if (!isatty(fileno(stdin)) || !isatty(fileno(stdout)))
		return FALSE;

   /* real check for running in foreground */
   if (ioctl(fileno(stdin), TIOCGPGRP, &val))
      return FALSE;
#ifdef IS_POSIX
   if (val == getpgrp())
#else
   if (val == getpgrp(0))
#endif
      return TRUE;
   else
      return FALSE;
   }

/* if_fg_sane_tty():
 * restore tty modes, _only_ if running in foreground
 */
LOCAL void if_fg_sane_tty()
	{
	if (run_in_fg())
		sane_tty();
	}


LOCAL void switch_mode()
   {
#ifdef USE_SGTTY
   struct sgttyb zap;
   int tty;
#endif
#ifdef USE_TERMIOS
   struct termios zap;
#endif

#ifdef SIGTSTP
   signal(SIGTSTP, suspend);
#endif
   signal(SIGCONT, switch_mode);
   signal(SIGINT, goodbye);
   signal(SIGQUIT, goodbye);
   signal(SIGUSR1, abort_this);

   if (run_in_fg())
      {
#ifdef USE_SGTTY
      tty = fileno(stdin);
      fcntl(tty, F_SETFL, fcntl(tty, F_GETFL, 0) | FNDELAY);
      ioctl(tty, TIOCGETP, &zap);
      zap.sg_flags |= CBREAK;
      zap.sg_flags &= ~ECHO;
      ioctl(tty, TIOCSETP, &zap);
#endif
#ifdef USE_TERMIOS
		tcgetattr(fileno(stdin), &zap);
      zap.c_cc[VMIN] = 0;     /* can't work with old */
      zap.c_cc[VTIME] = 0; /* FreeBSD versions    */
      zap.c_lflag &= ~(ICANON|ECHO|ECHONL);
      tcsetattr(fileno(stdin), TCSADRAIN, &zap);
#endif
      is_fg = TRUE;
      }
   else
      is_fg = FALSE;
   }

/* nonblocking_io():
 * try to setup the keyboard to non blocking io
 */
LOCAL void nonblocking_io()
   {


#if 0 /* BROKEN */
   /* try to renice our own process to get more cpu time */
   if (nice(-15) == -1)
      nice(0);
#endif


   if (!psanity)
      {
      psanity = &sanity;
#ifdef USE_SGTTY
      ioctl(fileno(stdin), TIOCGETP, psanity);
#endif
#ifdef USE_TERMIOS
      tcgetattr(fileno(stdin), psanity);
#endif
      }
   switch_mode();
   at_end(if_fg_sane_tty);
   }


/* sane_tty():
 * restores everything to a sane state before returning to shell */
LOCAL void sane_tty()
   {
#ifdef USE_SGTTY
      ioctl(fileno(stdin), TIOCSETP, psanity);
#endif
#ifdef USE_TERMIOS
      tcsetattr(fileno(stdin), TCSADRAIN, psanity);
#endif
   }

LOCAL int may_getchar()
   {
   char buffer;

   INIT_ONCE;

   if (run_in_fg() && !is_fg)
      switch_mode();
   if (run_in_fg() && read(fileno(stdin), &buffer, 1))
      return buffer;
   return EOF;
   }

LOCAL struct tag result[2];

struct tag *get_ui()
   {
   result[0].type = TAG_END;
   result[1].type = TAG_END;
   count_pattern++;
   count_song++;
   switch(may_getchar())
      {
   case 'n':
      result[0].type = UI_NEXT_SONG;
      break;
   case 'p':
      if (count_song > SMALL_DELAY)
         result[0].type = UI_RESTART;
      else
         result[0].type = UI_PREVIOUS_SONG;
      count_song = 0;
      break;
   case 'x':
   case 'e':
   case 'q':
      result[0].type = UI_QUIT;
      break;
   case 's':
      result[0].type = UI_SET_BPM;
      result[0].data.scalar = 50;
      break;
   case 'S':
      result[0].type = UI_SET_BPM;
      result[0].data.scalar = 60;
      break;
   case '>':
      result[0].type = UI_JUMP_TO_PATTERN;
      result[0].data.scalar = current_pattern + 1;
      break;
   case '<':
      result[0].type = UI_JUMP_TO_PATTERN;
      result[0].data.scalar = current_pattern;
      if (count_pattern < SMALL_DELAY)
         result[0].data.scalar--;
      break;
   case '?':
		set_pref_scalar(PREF_SHOW, !get_pref_scalar(PREF_SHOW));
		if (get_pref_scalar(PREF_SHOW))
			putchar('\n');
      break;
   default:
      break;
      }
   return result;
   }
      
         
#endif


void notice(s)
char *s;
   {
   fprintf(stderr, "%s\n", s);
   }

void status(s)
char *s;
   {
   if (run_in_fg())
      {
		if (s)
         {
         puts(s);
         }
      else
         putchar('\n');
      }
   }

LOCAL char title[25];
void song_title(s)
char *s;
   {
	strncpy(title, s, 25);
#ifndef XTERM
	if (run_in_fg() && get_pref_scalar(PREF_SHOW))
		puts(title);
#endif
   count_song = 0;
   }


LOCAL char scroll_buffer[200];

GENERIC begin_info(title)
char *title;
   {
   if (run_in_fg())
      return scroll_buffer;
   else
      return 0;
   }

void infos(handle, s)
GENERIC handle;
char *s;
   {
   if (handle)
      printf(s);
   }

void info(handle, line)
GENERIC handle;
char *line;
   {
   if (handle)
      puts(line);
   }

void end_info(handle)
GENERIC handle;
   {
   if (handle)
      fflush(stdout);
   }

LOCAL char *last_result = 0;

LOCAL int ntracks;

void set_number_tracks(n)
int n;
	{
	ntracks = n;
	}

char *new_scroll()
   {
   if (run_in_fg())
      {
		char *temp;
		int i;

      last_result = scroll_buffer;
		temp = last_result;
		if (get_pref_scalar(PREF_COLOR))
			{
			for (i = 0; i * 2 < ntracks; i++)
				{
				strcpy(temp, "                    ");
				temp = write_color(temp + 20, 0);
				*temp++ = '|';
				}
			*temp++ = '|';
			for (i = 0; i * 2 < ntracks; i++)
				{
				strcpy(temp, "                    ");
				temp = write_color(temp + 20, 0);
				*temp++ = '|';
				}
			}
		else
			{
			for (i = 0; i * 2 < ntracks; i++)
				{
				strcpy(temp, "             |");
				temp += 14;
				}
			*temp++ = '|';
			for (i = 0; i * 2 < ntracks; i++)
				{
				strcpy(temp, "             |");
				temp += 14;
				}
			}
		}
   else
      last_result = 0;
   return last_result;
   }
   
void scroll()
   {
   if (run_in_fg() && last_result)
		{
      puts(scroll_buffer);
		fflush(stdout);
		}
   }

void display_pattern(current, total, real)
int current, total, real;
   {
   if (run_in_fg())
		{
	  	if (get_pref_scalar(PREF_XTERM))
	  		{
			if (get_pref_scalar(PREF_SHOW))
				{
				int i;
				for (i = 0; i < ntracks; i++)
					printf("--------------");
				printf("\033]2;tracker %s %3d/%3d[%3d] %s\007\n", VERSION,
					current, total, real, title);
				}
			else
				{
				printf("\033]2;tracker %s %3d/%3d %s\007", VERSION, 
					current, total, title);
				fflush(stdout);
				}
		  	}
		else
		  	{
			if (get_pref_scalar(PREF_SHOW))
				printf("\n%3d/%3d[%3d] %s\n", current, total, real, title);
			else
				printf("%3d/%3d\b\b\b\b\b\b\b", current, total);
			fflush(stdout); 
		   }
      }
   current_pattern = current;
   count_pattern = 0;
   }

int checkbrk()
   {
   return FALSE;
   }
