/* Aix/audio.c 
	vi:ts=3 sw=3:
 */
/* Ported from Linux/audio.c by Sam Hartman <hartmans@mit.edu>*/
/* minor mods for pl14 by Mike Battersby */
/* Modified from soundblaster_audio.c by Hannu Savolainen */
/* hsavolai@cs.helsinki.fi */

#include "defs.h"
#include <unistd.h>
#include <fcntl.h>
#include "extern.h"

#undef SIGNED /*redefined in system include file*/
#include <sys/audio.h>
#include <sys/acpa.h>


ID("$Id: audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")

#define UNSIGNED_BUFFERS
#define DEFAULT_SET_MIX
#define DEFAULT_BUFFERS
#define NEW_OUTPUT_SAMPLES_AWARE

#include "Arch/common.c"

LOCAL int buf_max;
LOCAL int audio;           	/* /dev/acpa0/1 */

LOCAL int dsp_samplesize = 16; /* must be 8 or 16 */

int open_audio(f, s)
int f;
int s;
  	{
  	audio_init init;
	audio_control control;
	audio_change change;
	audio = open("/dev/acpa0/1", O_WRONLY, 0);
   if (audio == -1)
		end_all("Error opening audio device");

	if (f==0) f = 44100;

   init.srate = f;
   init.bits_per_sample = 16;
   init.mode = PCM;
	init.channels = s?2:1;
   init.flags = BIG_ENDIAN;
	init.operation = PLAY;
   if (ioctl(audio, AUDIO_INIT, &init)!= 0)
		end_all("Error initializing ACPA");

	stereo = (init.channels == 2)?1:0;
	buf_max = init.bsize*20;	/* This is set by the ioctl. */

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
		end_all("Error: unknown sample size");
		}
	
	samples_max = buf_max/dsize;

   control.ioctl_request = AUDIO_CHANGE;
	control.request_info = &change;
	control.position = 0;

	change.dev_info = 0;
	change.input = AUDIO_IGNORE;
	change.output = OUTPUT_1;
	change.monitor = AUDIO_IGNORE;
	change.volume = 0x7fff0000;
	change.volume_delay = 0;
	change.balance = 0x3fff0000;
	change.balance_delay = 0;
	change.treble = AUDIO_IGNORE;
	change.bass = AUDIO_IGNORE;
	change.pitch = AUDIO_IGNORE;
	
	if (ioctl(audio, AUDIO_CONTROL, &control) != 0)
		end_all( "Error changing ACPA parameters");

	control.ioctl_request = AUDIO_START;

  	if (ioctl(audio, AUDIO_CONTROL, &control) != 0)
		end_all("Error starting ACPA");

  	return init.srate;

   }

LOCAL void actually_flush_buffer()
   {
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
	case 16: 	/* Cool! 16 bits/sample */
		add_samples16(left, right, n);
		break;
	case 8:
		add_samples8(left, right, n);
		break;
	default:
	   }
	}

void flush_buffer()
   {	/* Dummy version */
   }

/* We must wait for all samples to be played before closing.*/
void close_audio()
   {
   audio_control control;
   if (idx != 0)
		actually_flush_buffer();
   ioctl(audio, AUDIO_WAIT, 0);
   control.position = 0;
	control.ioctl_request = AUDIO_STOP;
	control.request_info = 0;
	ioctl(audio, AUDIO_CONTROL, &control);

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
   audio_control control;
   control.ioctl_request = AUDIO_STOP;
	control.request_info = 0;
	control.position = 0;
	ioctl(audio, AUDIO_CONTROL, &control);
   usleep(150000);
   control.ioctl_request = AUDIO_START;
   if (ioctl(audio, AUDIO_CONTROL, &control) == -1)
		end_all ("Unable to restart AACPA");
	idx = 0;
	}


