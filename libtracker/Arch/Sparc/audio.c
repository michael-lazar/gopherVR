/* sparc/audio.c 
	vi:ts=3 sw=3:
 */

/* $Id: audio.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $
 * $Log: audio.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:23  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:41  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.13  1995/03/03  14:22:55  espie
 * Fixed audio info bug.
 *
 * Revision 4.12  1995/02/26  23:07:14  espie
 * solaris.
 *
 * Revision 4.11  1995/02/25  15:44:15  espie
 * discard_buffer incorrect.
 *
 * Revision 4.10  1995/02/24  15:36:39  espie
 * Finally fixed speed/sync/late start.
 *
 * Revision 4.9  1995/02/24  13:48:39  espie
 * Fixed minor bug (interaction between pause and -sync).
 *
 * Revision 4.8  1995/02/24  13:43:52  espie
 * Added -sync feature.
 * In the absence of -sync, pause half a second at start to allow for
 * data to accumulate in buffer first.
 * Suppressed update freq on the fly since audioctl does not allow it.
 *
 * Revision 4.7  1995/02/23  22:41:45  espie
 * Added # of bits.
 *
 * Revision 4.6  1995/02/23  16:42:27  espie
 * Began conversion to `common' model.
 *
 * Revision 4.5  1995/02/23  13:52:30  espie
 * primary, secondary -> primary+secondary, primary-secondary
 * strike out 2 multiplications out of 4 !
 *
 * Revision 4.4  1995/02/21  17:57:55  espie
 * Internal problem: RCS not working.
 *
 * Revision 4.3  1995/02/01  16:43:47  espie
 * 23 bit samples.
 *
 * Revision 4.2  1994/01/13  09:19:08  espie
 * Forgotten something.
 *
 * Revision 4.0  1994/01/11  18:16:36  espie
 * New release.
 *
 * Revision 3.14  1993/12/04  16:12:50  espie
 * BOOL -> boolean.
 * Merged ss10/solaris.
 * Merged support for solaris together.
 * Fixed /16 bug.
 * Corrected mix problem.
 * restore stty.
 * Sync pseudo call.
 * Added update_frequency call, mostly unchecked
 * Added finetune.
 * Protracker commands.
 *
 * Revision 1.3  1992/11/17  15:38:00  espie
 * discard_buffer() call for snappier interface calls.
 * - Unified support for all sparcs.
 * - moved down to level 2 io.
 */

#include "defs.h"
#include "extern.h"
#ifdef SOLARIS
#include <sys/audioio.h>
#else
#include <sun/audioio.h>
#endif
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stropts.h>
#include <signal.h>
     
#define DEFAULT_SET_MIX
#define DEFAULT_BUFFERS
#define NEW_OUTPUT_SAMPLES_AWARE

#include "Arch/common.c"

/* things that aren't defined in all sun/audioio.h */

#ifndef AUDIO_ENCODING_LINEAR
#define AUDIO_ENCODING_LINEAR (3)
#endif
#ifndef AUDIO_GETDEV
#define AUDIO_GETDEV	_IOR(A, 4, int)
#endif
#ifndef AUDIO_DEV_UNKNOWN
#define AUDIO_DEV_UNKNOWN (0)
#endif
#ifndef AUDIO_DEV_AMD
#define AUDIO_DEV_AMD (1)
#endif

ID("$Id: audio.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $")

LOCAL int audio;
LOCAL int written;
LOCAL int wait_samples;

LOCAL struct audio_info ainfo, ainfo2;

LOCAL void set_notify()
	{
	if (audio && tsync)
		{
      static int flags = S_MSG;
      ioctl(audio, I_SETSIG, &flags);
      }
	}


LOCAL void set_synchronize()
	{
	written = ainfo.play.eof;
	set_notify();
	if (!ainfo2.play.pause && !tsync)
		{
		ainfo2.play.pause = TRUE;
		ioctl(audio, AUDIO_SETINFO, &ainfo2);
		wait_samples = ainfo.play.sample_rate / 2;
		}
	else
		wait_samples = 0;
	}

int open_audio(f, s)
int f;
int s;
   {
	int type;
	LOCAL int possible[] = { 8000, 9600, 11025, 16000, 18900, 22050, 32000,
	37800, 44100, 48000, 0};

	int basic;
#ifdef SOLARIS
	audio_device_t dev;

	audio = open("/dev/audio", O_WRONLY);
#else
   audio = open("/dev/audio", O_WRONLY|O_NDELAY);
#endif


	basic = 0;
   if (audio == -1)
		end_all("Error: could not open audio");

		/* round frequency to acceptable value */
	f = best_frequency(f, possible, 22050);

		/* check whether we know about AUDIO_ENCODING_LINEAR */
	AUDIO_INITINFO(&ainfo);
	AUDIO_INITINFO(&ainfo2);
#ifdef SOLARIS
	ioctl(audio, AUDIO_GETDEV, &dev);
	if (strcmp(dev.name, "SUNW,dbri") != 0)
#else
	if (ioctl(audio, AUDIO_GETDEV, &type) ||
	type == AUDIO_DEV_UNKNOWN || type == AUDIO_DEV_AMD || basic)
#endif
		{
			/* not a new ss5/10/20 -> revert to base quality audio */
		stereo = 0;
		dsize = 1;
		ainfo.play.sample_rate = 8000;
		ainfo.play.encoding = AUDIO_ENCODING_ULAW;
		ainfo.play.channels = 1;
		}
	else
		{
			/* tentative set up */
		stereo = s;
		ainfo.play.sample_rate = f;
		ainfo.play.precision = 16;
		dsize = 2;
		if (stereo)
			ainfo.play.channels = 2;
		else
			ainfo.play.channels = 1;
			/* try it */
		ainfo.play.encoding = AUDIO_ENCODING_LINEAR;
		if (ioctl(audio, AUDIO_SETINFO, &ainfo) != 0)
			/* didn't work: fatal problem */
			end_all("Error: AUDIO_SETINFO");
		}
	idx = 0;
	samples_max = ainfo.play.channels*ainfo.play.sample_rate;
	buffer = (char *)malloc(dsize*samples_max);
	buffer16 = (short *)buffer;
	if (!buffer)
		end_all("Error: could not allocate buffer");

	set_synchronize();
	return ainfo.play.sample_rate;
	}

void set_synchro(s)
int s;
	{
	tsync = s;
	set_notify();
	}

int update_frequency()
	{
	return 0;

	/* the current implementation of the audio device does not allow
	 * output frequency change through /dev/audioctl
	 *	-> this code is not needed 
	int oldfreq;

	oldfreq = ainfo.play.sample_rate;
	if (ioctl(audio, AUDIO_GETINFO, &ainfo) == 0)
		{
		if (oldfreq != ainfo.play.sample_rate)
			{
			samples_max = ainfo.play.channels * ainfo.play.sample_rate;
			buffer = realloc(buffer, dsize * samples_max);
			buffer16 = (short *)buffer;
			return ainfo.play.sample_rate;
			}
		}
	return 0;
	 */
	}


void output_samples(left, right, n)
int left, right, n;
   {

	switch(ainfo.play.encoding)
		{
	case AUDIO_ENCODING_LINEAR:
		add_samples16(left, right, n);
		break;
	case AUDIO_ENCODING_ULAW:
		buffer[idx++] = cvt((left + right) >> (n-13));
		break;
	default:
		end_all("Error:Unknown audio encoding");
		}
	}

void flush_buffer()
   {
	int actual;
	int number;

	actual = write(audio, buffer, dsize * idx);
	if (actual == -1)
		notice("Write to audio failed");
	else if (actual != dsize * idx)
		notice("Short write to audio");
	if (wait_samples)
		{
		wait_samples -= actual;
		if (wait_samples <= 0)
			{
			wait_samples = 0;
			ainfo2.play.pause = FALSE;
			ioctl(audio, AUDIO_SETINFO, &ainfo2);
			}
		}
	if (tsync)
		{
		write(audio, buffer, 0); 
		written++;
			/* theorically, we should maintain a precise count of the
			 * samples output vs the samples waiting. In practice, doing
			 * a rapid estimate of the number of buffers needed for half
			 * a second with the current value of idx is quite enough
			 */
		number = ainfo.play.sample_rate/2/idx;
		if (written - ainfo.play.eof >= number)
			{
			while (ioctl(audio, AUDIO_GETINFO, &ainfo), 
				written - ainfo.play.eof >= number)
					wait(SIGPOLL);
			}
		}
	idx = 0;
   }

void discard_buffer()
	{
	ioctl(audio, I_FLUSH, FLUSHW);
	if (wait_samples)
		{
		ainfo2.play.pause = FALSE;
		ioctl(audio, AUDIO_SETINFO, &ainfo2);
		wait_samples = 0;
		}
	set_synchronize();
	}

void close_audio()
   {
	free(buffer);
   close(audio);
   }

int output_resolution()
	{
	return 16;
	}
