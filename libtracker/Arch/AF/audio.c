/* AF/audio.c
	vi:ts=3 sw=3:

This code written by:

Andrew "Alf" Leahy                         email: alf@st.nepean.uws.edu.au
University of Western Sydney - Nepean.
Sydney, Australia.                         phone: (047) 360622 (work)


Modified by Marc Espie to adjust to tracker 4.0 API.

 */

#include "defs.h"
#include "extern.h"
#include "/usr/local/include/AF/AFlib.h"
ID("$Id: audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")

/* 0 external handset, 1 for internal speaker */
#define SPEAKER 0
     
#define DEFAULT_SET_MIX
#define SEPARATE_BUFFERS

#include "Arch/common.c"

LOCAL int ssize;

LOCAL ATime t, t_r, act, act_r;
LOCAL AC ac, ac_r;
LOCAL AFAudioConn *aud, *aud_r;

int sample_sizes[] = {
	1,	/* MU255 */
	1,	/* ALAW */
	2,	/* Linear PCM, 16 bits, -1.0 <= x < 1.0 */
	2,	/* Linear PCM, 32 bits, -1.0 <= x < 1.0 */
	1,	/* G.721, 64Kbps to/from 32Kbps. */
	1,	/* G.723, 64Kbps to/from 32Kbps. */
	0
};

int open_audio(int f, int s)
{
	AFSetACAttributes attributes;
	int srate, device;
	unsigned int channels;
	AEncodeType type;
	char *server;

	device = SPEAKER;
	attributes.preempt = Mix;
	attributes.start_timeout = 0;
	attributes.end_silence = 0;
	attributes.play_gain = 0;
	attributes.rec_gain =  0;

	if ((server = (char *) getenv("AUDIOFILE")) == NULL)
		end_all("Error: AUDIOFILE unset");
	else
	{
		server = (char *) getenv("AUDIOFILE");
		if ((aud = AFOpenAudioConn( server )) == NULL)
			end_all("Error: can't open connection");
		ac = AFCreateAC(aud, device, ACPlayGain, &attributes);
		srate = ac->device->playSampleFreq;
		type = ac->device->playBufType;
		channels = ac->device->playNchannels;
		ssize = sample_sizes[type] * channels;

		if ((buffer = (char *)malloc(ssize * srate)) == NULL)
			end_all("Couldn't allocate play buffer");

		t = AFGetTime(ac);
	}

	stereo=s;

	if (stereo)
	{
		server = (char *) getenv("AUDIORIGHT");
		if ((aud = AFOpenAudioConn(server)) == NULL)
			end_all("Error: can't open connection");
		ac_r = AFCreateAC(aud, device, ACPlayGain, &attributes);
		srate = ac->device->playSampleFreq;
		type = ac->device->playBufType;
		channels = ac->device->playNchannels;
		ssize = sample_sizes[type] * channels;

		buffer_l = buffer;
		if ((buffer_r = (char *)malloc(ssize * srate)) == NULL)
			end_all("Couldn't allocate play buffer");
		t_r = AFGetTime(ac_r);
	}

	return srate;
}

void set_synchro(int s)
{
}

int update_frequency()
{
	return 0;
}

void output_samples(int left, int right)
	{
	if (stereo)
		{
		if (pms == 256)
			{
			buffer_l[idx] = cvt(left >>10);
			buffer_r[idx++] = cvt(right >>10);
			}
		else
			{
			int s1, s2;

			s1 = (left+right)*pps;
			s2 = (left-right)*pms;
			buffer_l[idx] = cvt((s1 + s2) >>18);
			buffer_r[idx++] = cvt((s1 - s2) >>18);
			}
		}
	else /* mono */
		buffer[idx++] = cvt((left + right) >>10);
	}

void flush_buffer()
{
	act = AFPlaySamples(ac, t, idx, buffer);
	t += idx/ssize;

	if (stereo) 
	{
		act_r = AFPlaySamples(ac_r, t_r, idx, buffer_r);
		t_r += idx/ssize;
	}
	idx = 0;
}

void discard_buffer()
{
}

void close_audio()
{
	free(buffer);

/* Alf: I'm not sure whether these functions are needed
        I think these are Seg Faulting... */

 	(void) AFCloseAudioConn(aud);

	if (stereo)
		(void) AFCloseAudioConn(aud_r);
}
