/* hplow_audio.c 
	vi:ts=3 sw=3:
 */

/* Driver for HP9000 series 710/7x5 with HP-UX 9.01 */
/* Using low-level audio calls */
/* Copyright 1993 Marc Espie   (Marc.Espie@ens.fr)  */
/* Copyright 1993 Markus Gyger (mgyger@itr.ch) */

#include "defs.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/audio.h>
#include "extern.h"

#define DEFAULT_BUFFERS
#define DEFAULT_SET_MIX
#include "common.c"

ID("$Id: low_audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")
#ifndef DEFAULT_SAMPLE_RATE
#define DEFAULT_SAMPLE_RATE  22050
#endif

#ifndef AUDIO_NAME
#define AUDIO_NAME           "/dev/audio"
#endif
#ifndef AUDIO_CTL_NAME
#define AUDIO_CTL_NAME       "/dev/audioCtl"
#endif

LOCAL int audio;
LOCAL int audio_ctl;

LOCAL struct audio_describe ainfo;
LOCAL int sample_rate;
LOCAL int channels;
LOCAL int min_samples;


LOCAL int available(f)
int f;
    {
    int best = 0;
    int i;

    for (i = 0; i < ainfo.nrates; i++)
	if (abs(ainfo.sample_rate[i] - f) < abs(best - f))
	    best = ainfo.sample_rate[i];
    return best;
    }

int open_audio(f, s)
int f;
int s;
    {
    int type;

    audio_ctl = open(AUDIO_CTL_NAME, O_WRONLY | O_NDELAY);
    if (audio_ctl == -1)
        {
        end_all("No audio control device");
        }

    if (ioctl(audio_ctl, AUDIO_DESCRIBE, &ainfo) == -1)
        {
	end_all("No audio info");
        }

    if (f == 0)
        f = DEFAULT_SAMPLE_RATE;
        /* round frequency to acceptable value */
    sample_rate = available(f);

        /* check whether we have stereo device */
    if (ainfo.nchannels < 2)
        {
            /* a 710 or 425 -> revert to base quality audio */
        stereo = 0;
        channels = 1;
        }
    else
        {
            /* 7x5 set up */
        stereo = s;
        if (stereo)
            {
            channels = 2;
            set_mix(30);
            }
        else
            channels = 1;
        }

        /* write a minimum of samples to avoid underflows on 715 (???) */
    switch (ainfo.audio_id)
        {
        case AUDIO_ID_CS4215:
            /* empirically found values */
            min_samples = 12288;
            if (channels < 2) min_samples = 8192;
            if (sample_rate <= 11025) min_samples = 6144;
            if (sample_rate <= 8000) min_samples = 4096;
            break;

        case AUDIO_ID_PSB2160:
        default:
            min_samples = 0;
            break;
        }

    if (ioctl(audio_ctl, AUDIO_SET_DATA_FORMAT, AUDIO_FORMAT_LINEAR16BIT) == -1)
        {
		  end_all("Linear format not available");
        }
    if (ioctl(audio_ctl, AUDIO_SET_CHANNELS, channels) == -1)
        {
		  end_all("Could not set # of audio channels");
        }
    if (ioctl(audio_ctl, AUDIO_SET_SAMPLE_RATE, sample_rate) == -1)
        {
		  end_all("Could not set audio sample rate");
        }

    audio = open(AUDIO_NAME, O_WRONLY | O_NDELAY);
    if (audio == -1)
        {
		  end_all("Could not open audio device");
        }

#ifdef SET_OUTPUT
        /* ensure we hear something */
    if (ioctl(audio_ctl, AUDIO_SET_OUTPUT,
        ((SET_OUTPUT & 1) ? AUDIO_OUT_INTERNAL : 0) |   /* speaker */
        ((SET_OUTPUT & 2) ? AUDIO_OUT_EXTERNAL : 0) |   /* phones  */
        ((SET_OUTPUT & 4) ? AUDIO_OUT_LINE : 0) |   /* line    */
        ((SET_OUTPUT & 7) ? 0 : AUDIO_OUT_NONE)) == -1)
		  notice("Warning: could not set audio output");
#endif

    idx = 0;
    buffer16 = (short *)malloc(2 * channels * sample_rate);
    buffer = (char *)buffer16;
    if (!buffer)
        end_all("Could not allocate buffer");
    return sample_rate;
    }

void set_synchro(s)
int s;
    {
    }

int update_frequency()
    {
    int oldfreq;

    oldfreq = sample_rate;
    if (ioctl(audio, AUDIO_GET_SAMPLE_RATE, &sample_rate) != -1)
        {
        if (oldfreq != sample_rate)
            {
            buffer16 = (short *)realloc(buffer16, 2 * channels * sample_rate);
            buffer = (char *)buffer16;
            return sample_rate;
            }
        }
    return 0;
    }

void output_samples(left, right)
int left, right;
	{
   if (stereo)
		add_samples16_stereo(left, right);
	else
		buffer16[idx++] = left + right;
    }

void flush_buffer()
    {
    if (idx >= min_samples)
        {
        write(audio, buffer, 2 * idx);
        idx = 0;
        }
    }

void discard_buffer()
    {
    if (ioctl(audio, AUDIO_RESET, RESET_TX_BUF) == -1)
		notice("Warning: could not discard audio buffer");
    }

void close_audio()
    {
    free(buffer);
    close(audio);
    close(audio_ctl);
    }

