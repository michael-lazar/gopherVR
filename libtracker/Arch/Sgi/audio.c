/* sgi/audio.c 
	vi:ts=3 sw=3:
 */

/* $Id: audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
 * $Log: audio.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:41  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.7  1995/02/26  23:07:14  espie
 * tsync.
 *
 * Revision 4.6  1995/02/23  22:41:45  espie
 * Added # of bits.
 *
 * Revision 4.5  1995/02/23  17:03:14  espie
 * Continuing changes for a standard file.
 *
 * Revision 4.4  1995/02/23  13:52:30  espie
 * primary, secondary -> primary+secondary, primary-secondary
 * strike out 2 multiplications out of 4 !
 *
 * Revision 4.3  1995/02/21  17:57:55  espie
 * Internal problem: RCS not working.
 *
 * Revision 4.2  1995/02/01  16:43:47  espie
 * 23 bit samples.
 *
 * Revision 4.0  1994/01/11  18:01:04  espie
 * Changed name.
 *
 * Revision 3.8  1993/12/04  16:12:50  espie
 * BOOL -> boolean.
 *
 * Revision 3.7  1993/11/11  20:00:03  espie
 * Amiga support.
 *
 * Revision 3.6  1993/07/14  16:33:41  espie
 * Added stuff.
 *
 * Revision 3.5  1993/05/09  14:06:03  espie
 * Corrected mix problem.
 *
 * Revision 3.4  1992/11/27  10:29:00  espie
 * General cleanup
 *
 * Revision 3.3  1992/11/24  10:51:19  espie
 * Added pseudo discardbuffer.
 *
 * Revision 3.2  1992/11/22  17:20:01  espie
 * Checks for finetune ?
 *
 * Revision 3.1  1992/11/19  20:44:47  espie
 * Protracker commands.
 *
 * Revision 3.0  1992/11/18  16:08:05  espie
 * New release.
 *
 * Revision 2.11  1992/11/17  15:38:00  espie
 * Dummy discard_buffer()
 * Changed sync_audio value again.
 * Added synchro for dump.
 * Bug fix: must ask new frequency after we tried to set it to get it
 * rounded up.
 * Added stereo option (kind of).
 * Separated mix/stereo stuff.
 * Checked buffer size.
 * Added possibility to get back to MONO for the sgi.
 * Added stereo capabilities to the indigo version.
 * Ask the frequency to the audio device.
 * Corrected bug: when closing audio,
 * we now wait for the samples queue to be empty.
 */

#include "defs.h"
#include "extern.h"

#define DEFAULT_BUFFERS
#define DEFAULT_SET_MIX
#define NEW_OUTPUT_SAMPLES_AWARE

#include "Arch/common.c"

XT int sginap(long ticks);
     
ID("$Id: audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")

LOCAL int number;

LOCAL ALport audio;
LOCAL ALconfig config;

LOCAL int donotwait = FALSE;
LOCAL long chpars[] = {AL_OUTPUT_RATE, 0};


int open_audio(f, s)
int f, s;
   {

	donotwait = FALSE;
   chpars[1] = f;
   if (f != 0)
		ALsetparams(AL_DEFAULT_DEVICE, chpars, 2);
   ALgetparams(AL_DEFAULT_DEVICE, chpars, 2);
   config = ALnewconfig();
   stereo = s;
   if (stereo)
		{
      ALsetchannels(config, AL_STEREO);
      number = 2;
      }
   else
      {
      ALsetchannels(config, AL_MONO);
      number = 1;
      }
   ALsetwidth(config, AL_SAMPLE_16);
   audio = ALopenport("soundtracker mono", "w", config);
   idx = 0;
   buffer16 = malloc(sizeof(signed short) * number * chpars[1]);
   return chpars[1];
   }

void set_synchro(s)
int s;
	{
	tsync = s;
	}

int update_frequency()
	{
	int oldfreq;

	oldfreq = chpars[1];
	ALgetparams(AL_DEFAULT_DEVICE, chpars, 2);
	if (chpars[1] != oldfreq)
		{
		buffer16 = realloc(buffer16, sizeof(signed short) * number * chpars[1]);
		return chpars[1];
		}
	else
		return 0;
	}


void output_samples(int left, int right, int n)
	{
	add_samples16(left, right, n);
	}

void flush_buffer(void)
   {
   ALwritesamps(audio, buffer16, idx);
	if (tsync)
		while(ALgetfilled(audio) > idx * 10)
			/* busy wait */
			;
   idx = 0;
   }

void discard_buffer(void)
	{
	donotwait = TRUE;
	/* mostly not implemented, only working when using close_audio
	 * right after
	 */
	}

void close_audio(void)
   {
	if (!donotwait)
		{
		while(ALgetfilled(audio) != 0)
			sginap(1);
		}
   ALcloseport(audio);
   ALfreeconfig(config);
   free(buffer16);
   }
