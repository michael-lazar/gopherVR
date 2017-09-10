/* pcux/audio.c 
	vi:ts=3 sw=3:
 */
/* minor mods for pl14 by Mike Battersby */
/* Modified from soundblaster_audio.c by Hannu Savolainen */
/* hsavolai@cs.helsinki.fi */

#include "defs.h"
#include <unistd.h>
#include <fcntl.h>
#include "extern.h"

#define DEFAULT_BUFFERS
#define UNSIGNED8
#define DEFAULT_SET_MIX
#define NEW_OUTPUT_SAMPLES_AWARE

#include "Arch/common.c"


#ifdef PL_14
/* For some reason my pl14 kernel had no sys/soundcard.h (???) */
#include "/usr/src/linux/drivers/sound/soundcard.h"
#else
#ifdef __OpenBSD__
/* OpenBSD soundcard.h is in /usr/include */
#include <soundcard.h>
#else
#ifndef __FreeBSD__
/*	This should be sys/soundcard.h	*/
#include <sys/soundcard.h>
#else
#include <machine/soundcard.h>
#endif
#endif
#endif




ID("$Id: audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")

LOCAL int samples_max;
LOCAL int audio;           	/* /dev/dsp */


LOCAL int dsp_samplesize = 16; /* must be 8 or 16 */

int open_audio(f, s)
int f;
int s;
   {
	int buf_max;

   audio = open("/dev/dsp", O_WRONLY, 0);
   if (audio == -1)
		end_all("Error opening audio device");

   if (ioctl(audio, SNDCTL_DSP_SAMPLESIZE, &dsp_samplesize) == -1)
		end_all("Error setting sample size");

   stereo = s;

   if (ioctl(audio, SNDCTL_DSP_STEREO, &stereo) == -1)
    	end_all("Error setting stereo/mono");

   if (f==0) 
		f = 44100;

   if (ioctl(audio, SNDCTL_DSP_SPEED, &f) == -1)
		end_all("Error setting frequency");

   if (ioctl (audio, SNDCTL_DSP_GETBLKSIZE, &buf_max) == -1)
		end_all("Error getting buffsize");

   buffer = malloc(buf_max);
   buffer16 = (short *)buffer;
   idx = 0;
	switch(dsp_samplesize)
		{
	case 16:
		dsize = 2;
		break;
	case 8:
		dsize = 1;
		break;
	default:
		end_all("Error: unknown dsp_samplesize");
		}
	samples_max = buf_max / dsize;

	return f;
   }

LOCAL void actually_flush_buffer()
   {
   int l,i;

   write(audio, buffer, dsize * idx);
   idx = 0;
   }

void output_samples(left, right, n)
int left, right, n;
	{
	if (idx >= samples_max - 1)
		actually_flush_buffer();
	switch(dsp_samplesize)
		{
	case 16:				/*   Cool! 16 bits samples */
		add_samples16(left, right, n);
		break;
	case 8:
		add_samples8(left, right, n);
		break;
	default:	/* should not happen */
	break;
	   }
   }

void flush_buffer()
    {	/* Dummy version */
    }

/*
 * Closing the Linux sound device waits for all pending samples to play.
 */
void close_audio()
    {
    actually_flush_buffer();
    close(audio);
    free(buffer);
    }

/* dummy system calls, to patch ? */
void set_synchro(s)
	{
	}

int update_frequency()
	{
	return 0;
	}

void discard_buffer()
	{
	}

