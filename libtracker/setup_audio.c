/* setup_audio.c 
	vi:ts=3 sw=3:
 */
/* higher level interface to the raw metal */

/* $Id: setup_audio.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: setup_audio.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.12  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.0  1994/01/11  17:55:28  espie
 * Use autoinit.
 * Suppressed multiple at_end.
 * Use new pref scheme.
 * Modified in a more consistent way.
 * Added check before closing for the sgi.
 * Added finetune.
 */



#include "defs.h"
#include "extern.h"
#include "tags.h"
#include "prefs.h"

ID("$Id: setup_audio.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")

LOCAL void init_audio P((void));

LOCAL void (*INIT)P((void)) = init_audio;

LOCAL int opened = FALSE;
LOCAL int ask_freq, real_freq, oversample;
LOCAL int stereo;


/* forward declaration */
LOCAL void do_close_audio P((void));

LOCAL void init_audio()
   {
   at_end(do_close_audio);
   }

/* setup_audio(frequency, stereo, oversample):
 * try to avoid calling open_audio and other things
 * all the time
 */
void setup_audio(f, s, o)
int f;
int s;
int o;
   {
   INIT_ONCE;

   if (!opened)
      {
      ask_freq = f;
      stereo = s;
      oversample = o;
      real_freq = open_audio(f, s);
      init_player(o, real_freq);
      opened = TRUE;
      }
   else
      {
      int new_freq;

      if (s != stereo || f != ask_freq)
         {
         ask_freq = f;
         stereo = s;
         close_audio();
         new_freq = open_audio(f, s);
         }
      else
         new_freq = real_freq;

      if (new_freq != real_freq || oversample != o)
         {
         real_freq = new_freq;
         oversample = o;
         init_player(o, real_freq);
         }
      }
   set_synchro(get_pref_scalar(PREF_SYNC));
   }

void do_close_audio()
   {
   if (opened)
      {
      close_audio();
      }
   opened = FALSE;
   }

