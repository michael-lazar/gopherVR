/* unix/ui.c 
	vi:ts=3 sw=3:
 */

/* special termio discipline for sun/sgi,
 * for non blocking io and such.
 * These functions should not be too difficult
 * to write for a PC.
 */
/* $Id: ui.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
 * $Log: ui.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:41  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.2  1995/02/21  17:57:55  espie
 * Internal problem: RCS not working.
 *
 * Revision 4.0  1994/01/11  18:02:31  espie
 * Major change: lots of new calls.
 *
 * Revision 3.15  1993/12/04  16:12:50  espie
 * BOOL -> boolean.
 *
 * Revision 3.13  1993/10/06  17:17:45  espie
 * Stupid termio bug: shouldn't restore term to sanity if we don't
 * know what sanity is. For instance, if we haven't modified anything.
 *
 * Revision 3.11  1993/07/17  12:00:30  espie
 * Added other commands (numerous).
 *
 * Revision 3.10  1993/07/14  16:33:41  espie
 * Added partial code for hpux.
 *
 * Revision 3.9  1993/04/28  20:14:41  espie
 * My error...
 *
 * Revision 3.8  1993/04/25  14:50:17  espie
 * cflags interpreted correctly.
 *
 * Revision 3.7  1993/01/16  16:23:33  espie
 * Hsavolai fix.
 *
 * Revision 3.6  1993/01/15  14:00:28  espie
 * Added bg/fg test.
 *
 * Revision 3.5  1993/01/06  17:58:39  espie
 * Added changes for linux.
 *
 * Revision 3.4  1992/12/03  15:00:50  espie
 * restore stty.
 *
 * Revision 3.3  1992/11/27  10:29:00  espie
 * General cleanup
 *
 * Revision 3.2  1992/11/22  17:20:01  espie
 * Added update_frequency call, mostly unchecked
 *
 * Revision 3.1  1992/11/19  20:44:47  espie
 * Protracker commands.
 *
 */

#ifdef dec
#define stub_only
#endif

#if defined(linux) || defined(__386BSD__)
#include <termios.h>
#else
#ifdef __hpux
#include <sys/bsdtty.h>
#endif
#include <sys/termio.h>
#endif
#ifdef __386BSD__
#include <sys/ioctl.h>
#endif
#ifdef __OS2__
#include <os2.h>
#include <sys/termio.h>
#endif
#include <signal.h>
#include "defs.h"
#include "extern.h"
#include "tags.h"
#include "prefs.h"


LOCAL void nonblocking_io P((void));
LOCAL void sane_tty P((void));

LOCAL void (*INIT)P((void)) = nonblocking_io;


LOCAL int show;
/* poor man's timer */
LOCAL int current_pattern;
LOCAL int count_pattern, count_song;
#define SMALL_DELAY 25

/* do not define any stdio routines if it's known not to work */

#ifdef stub_only

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

#else

LOCAL struct termio sanity;
LOCAL struct termio *psanity = 0;

LOCAL int is_fg;

/* signal handler */

LOCAL void goodbye(sig)
int sig;
    {
    static char buffer[25];

    sprintf(buffer, "Signal %d", sig);
    end_all(buffer);
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
   fflush(stdout);
   sane_tty();
   signal(SIGTSTP, SIG_DFL);
   kill(0, SIGTSTP);
   }
#endif

int run_in_fg()
   {
   USHORT          Indicator;
   return(VioGetAnsi(&Indicator,0)==0);
   }

LOCAL void switch_mode()
   {
   struct termio zap;

#ifdef SIGTSTP
   signal(SIGTSTP, suspend);
#endif
   signal(SIGINT, goodbye);
   signal(SIGQUIT, goodbye);
   signal(SIGUSR1, abort_this);

   if (run_in_fg())
      {
      ioctl(fileno(stdin), TCGETA, &zap);
      zap.c_cc[VEOL] = 0;
      zap.c_cc[VEOF] = 0;
      zap.c_lflag &= ~(ICANON | ECHO);
      ioctl(fileno(stdin), TCSETA, &zap);
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
   show = get_pref_scalar(PREF_SHOW);
   /* try to renice our own process to get more cpu time */
   if (!psanity)
      {
      psanity = &sanity;
      ioctl(fileno(stdin), TCGETA, psanity);
      }
   switch_mode();
   at_end(sane_tty);
   }


/* sane_tty():
 * restores everything to a sane state before returning to shell */
LOCAL void sane_tty()
   {
      ioctl(fileno(stdin), TCSETA, psanity);
   }

LOCAL int may_getchar()
   {
   int ch;

   if((ch = _read_kbd(0,0,0)) == 0);
       { 
       ch = _read_kbd(0,0,0);
       ch = -1;
       }
   return((ch<0?EOF:ch));
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
      show = !show;
		set_pref_scalar(PREF_SHOW, show);
		if (show)
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
	if (run_in_fg() && !show)
		puts(title);
   count_song = 0;
   }


LOCAL char scroll_buffer[80];

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

char *new_scroll(void)
   {
   if (run_in_fg())
      {
      last_result = scroll_buffer;
      strcpy(scroll_buffer, "             |             |             |             ");
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

void display_pattern(current, total)
int current, total;
   {
   if (run_in_fg())
      {
      if (show)
         printf("\n%3d/%3d %s\n", current, total, title);
      else
         printf("%3d/%3d\b\b\b\b\b\b\b", current, total);
      fflush(stdout); 
      }
   current_pattern = current;
   count_pattern = 0;
   }

int checkbrk()
   {
   return FALSE;
   }
