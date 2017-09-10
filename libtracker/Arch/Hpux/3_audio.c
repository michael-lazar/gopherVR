/* -*-C-*-
	vi:ts=3 sw=3:
*******************************************************************************
*
* File:		hpux_audio.c
* RCS:		$Header: /var/cvs/clients/gophervr/libtracker/Arch/Hpux/3_audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
* Description:	HP-UX audio hardware interface for tracker
* Author:	Darryl Okahata (darrylo@sr.hp.com)
* Created:	Thu Dec 16 12:09:53 1993
* Modified:     Mon Dec 20 12:17:02 1993 (Darryl Okahata) darrylo@mina
* Language:	C
* Package:	N/A
* Status:	Experimental
*
* (C) Copyright 1993, Hewlett-Packard, all rights reserved.
*
*******************************************************************************

 */

#include "defs.h"
#include <errno.h>
#include <sys/socket.h>

#include <audio/Alib.h>

#include "extern.h"


#define DEFAULT_SET_MIX
#define DEFAULT_BUFFERS
#include "common.c"


#define STEREO_CHANNELS		(ALeftOutputChMask | ARightOutputChMask)

extern AGainDB		volume;		/* volume in dB -- in main.c */
extern char		use_speaker;	/* in main.c */

LOCAL Audio		*audio;
LOCAL ADataFormat	*available_formats;
LOCAL long		available_formats_len;
LOCAL AOutputChMask	available_channels;
LOCAL AByteOrder	byte_order;
LOCAL AudioAttributes	audio_attributes;
LOCAL AGainEntry	gain_entry[4];
LOCAL SSPlayParams	stream_parameters;
LOCAL ATransID		xid;
LOCAL SStream		audio_stream;
LOCAL int		stream_socket;


LOCAL ADataFormat	audio_format;
LOCAL int		max_index;
LOCAL long		buffer_size;


LOCAL int round_sampling_rate(audio, f)
Audio	*audio;
int	f;
{
    int best = 0;
    int i;

    for (i = 0; i < audio->n_sampling_rate; i++) {
	if (abs(audio->sampling_rate_list[i] - f) < abs(best - f)) {
	    best = audio->sampling_rate_list[i];
	}
    }
    return best;
}


LOCAL format_available(fmt)
ADataFormat	fmt;
{
    int		i;

    for (i = 0; i < available_formats_len; ++i) {
	if (available_formats[i] == fmt) {
	    break;
	}
    }
    if (i >= available_formats_len) {
	return (0);
    }
    return (1);
}


/*
 * It would be nice if we could use AGetErrorText(), but this requires that
 * an audio pointer be passed to it, and this pointer could be NULL.
 */
LOCAL void die(msg, error)
char		*msg;
AError		error;
{
    char	*errbuf, buf[1000];	/* memory is cheap, life is short */

    if (msg == NULL) {
	msg = "";
    }
    switch (error) {
    case AENoError:
	errbuf = "(No audio error -- huh?  This should never happen!)";
	break;
    case AESystemCall:
	errbuf = "System call error";
	break;
    case AEBadAudio:
	errbuf = "Bad audio";
	break;
    case AEBadValue:
	errbuf = "Bad audio value";
	break;
    case AEHostNotFound:
	errbuf = "Audio host not found";
	break;
    case AENoSuchAudioNumber:
	errbuf = "No such audio number";
	break;
    case AEBadFileFormat:
	errbuf = "Bad audio file format";
	break;
    case AEBadDataFormat:
	errbuf = "Bad audio data format";
	break;
    case AEFileNotFound:
	errbuf = "Audio file not found";
	break;
    case AEBadLinkID:
	errbuf = "Audio bad link ID";
	break;
    case AEBadGainMatrix:
	errbuf = "Bad audio gain matrix";
	break;
    case AEBadFileHdr:
	errbuf = "Bad audio file header";
	break;
    case AEUnrecognizableFormat:
	errbuf = "Unrecognizable audio format";
	break;
    case AEBadAttribute:
	errbuf = "Bad audio attribute";
	break;
    case AEBadOffset:
	errbuf = "Audio bad offset";
	break;
    case AEBadTransactionID:
	errbuf = "Bad audio transaction ID";
	break;
    case AECantDetermineFormat:
	errbuf = "Audio can't determine format";
	break;
    case AEOutOfMemory:
	errbuf = "out of memory (audio)";
	break;
    case AEOpenFailed:
	errbuf = "Audio open failed";
	break;
    case AEBadSamplingRate:
	errbuf = "Bad audio sampling rate";
	break;
    case AEBadSoundBucket:
	errbuf = "Audio bad sound bucket";
	break;
    case AEBadSoundStream:
	errbuf = "Audio bad sound stream";
	break;
    case AETransactionBusy:
	errbuf = "Audio transaction busy";
	break;
    case AEllbdNotStarted:
	errbuf = "llbd daemon not running";
	break;
    case AERPCFailed:
	errbuf = "Audio RPC failed";
	break;
    case AELibraryMismatch:
	errbuf = "Audio library mismatch";
	break;
    default:
	sprintf(buf, "(unknown audio error %d)", (int) error);
	errbuf = buf;
	break;
    }
	 notice(msg);
	 end_all(errbuf);
}


int open_audio(freq, stereo_flag)
int freq;
int stereo_flag;
{
    long		status_return;
    AudioAttrMask	valid_attributes;
    char		*speaker;
    int		i;

    stereo = stereo_flag;

    if ((audio = AOpenAudio(NULL, &status_return)) == NULL) {
	die("could not open audio: ", status_return);
    }

    available_formats = ADataFormats(audio);
    available_formats_len = ANumDataFormats(audio);
    if (format_available(ADFLin16)) {
	audio_format = ADFLin16;
	dsize = 2;		/* for ADFLin16 */
    } else if (format_available(ADFLin8)) {
	audio_format = ADFLin8;
	dsize = 1;		/* for ADFLin8 */
    } else {
	 end_all("Neither ADFLin16/ADFLin8 supported by the audio device");
    }

    available_channels = AOutputChannels(audio);

    valid_attributes = ASDataFormatMask | ASBitsPerSampleMask |
	ASSamplingRateMask | ASChannelsMask;
    audio_attributes = *ABestAudioAttributes(audio);
    audio_attributes.type = ATSampled;
    audio_attributes.attr.sampled_attr.data_format = ADFLin16;
    audio_attributes.attr.sampled_attr.bits_per_sample = 16;

    if (freq <= 0) {
	/*
	 * Use value closest to CD-quality audio
	 */
	freq = 44100;
    }
    freq = round_sampling_rate(audio, freq);
    audio_attributes.attr.sampled_attr.sampling_rate = freq;

    audio_attributes.attr.sampled_attr.duration.type = ATTFullLength;
    valid_attributes |= ASDurationMask;

    if (stereo &&
	((available_channels & (STEREO_CHANNELS)) == STEREO_CHANNELS)) {
	audio_attributes.attr.sampled_attr.channels = 2;
	audio_attributes.attr.sampled_attr.interleave = 1;
	valid_attributes |= ASInterleaveMask;
    } else {
	audio_attributes.attr.sampled_attr.channels = 1;
    }

    /*
     * Select attributes for playback
     */
    AChoosePlayAttributes(audio, ABestAudioAttributes(audio),
			  valid_attributes, &audio_attributes,
			  &byte_order, &status_return);
    if (byte_order != AMSBFirst) {
	 end_all("Little endian byte ordering not supported");
    }

    /*
     * Use the external jack, unless the user overrides this on the
     * command-line, or SPEAKER is set.	 (The command line stuff is done in
     * main.c.)
     */
    if ((speaker = getenv( "SPEAKER" )) != NULL) {
	if ((*speaker == 'i') || (*speaker == 'I')) {
	    use_speaker = 1;
	}
    }

    stream_parameters.priority = APriorityNormal;

    switch(audio_attributes.attr.sampled_attr.channels) {
    case 1:
	gain_entry[0].u.o.out_ch = AOCTMono;
	gain_entry[0].gain = volume;
	gain_entry[0].u.o.out_dst =
	    (use_speaker) ? AODTMonoIntSpeaker : AODTMonoJack;
	break;
    case 2:
    default:	/* assume no more than 2 channels */
	gain_entry[0].u.o.out_ch = AOCTLeft;
	gain_entry[0].gain = volume;
	gain_entry[0].u.o.out_dst =
	    (use_speaker) ? AODTLeftIntSpeaker : AODTLeftJack;
	gain_entry[1].u.o.out_ch = AOCTRight;
	gain_entry[1].gain = volume;
	gain_entry[1].u.o.out_dst =
	    (use_speaker) ? AODTRightIntSpeaker : AODTRightJack;
	break;
    }

    stream_parameters.gain_matrix.type = AGMTOutput;	   /* gain matrix */
    stream_parameters.gain_matrix.num_entries =
	audio_attributes.attr.sampled_attr.channels;
    stream_parameters.gain_matrix.gain_entries = gain_entry;
    stream_parameters.play_volume = AUnityGain;		   /* play volume */
    stream_parameters.event_mask = 0;			   /* don't solicit any
							      events */

    /*
     * create an audio stream
     */
    xid = APlaySStream(audio, valid_attributes, &audio_attributes,
		       &stream_parameters, &audio_stream, NULL);

    /*
     * create a stream socket
     */
    stream_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(stream_socket < 0) {
	 end_all("Socket creation failed");
    }

    /*
     * connect the stream socket to the audio stream port
     */
    status_return = connect(stream_socket,
			    (struct sockaddr *)&audio_stream.tcp_sockaddr,
			    sizeof(struct sockaddr_in));
    if(status_return < 0) {
	 end_all("Connect failed");
    }

    buffer_size = dsize * audio_attributes.attr.sampled_attr.channels *
	audio_attributes.attr.sampled_attr.sampling_rate;
    buffer = (char *) malloc(buffer_size);
    sbuffer = (short *) buffer;
    if (!buffer) {
	 end_all("Unable to allocate memory for buffer");
    }
    switch (audio_format) {
    case ADFLin16:
	samples_max = buffer_size / 2 - 1;
	break;
    case ADFLin8:
	samples_max = buffer_size - 1;
	break;
    default:
		end_all("Bad");
	break;
    }

    return (audio_attributes.attr.sampled_attr.sampling_rate);
}


void set_synchro(s)
int s;
{
    /* not implemented */
}


int update_frequency()
{
    /* not implemented */
    return 0;
}


void flush_buffer()
{
    int		len_written, len_left;
    char	*buf;

    len_left = idx * 2;
    buf = buffer;
    while (len_left > 0) {
	if((len_written = write(stream_socket, buf, len_left)) < 0) {
	    end_all("Write failed");
	}
	len_left -= len_written;
	buf += len_written;
    }
    idx = 0;
}


void output_samples(left, right)
int left, right;
	{
   if (idx >= max_index) 
		flush_buffer();
	switch(audio_format)
		{
	case ADFLin16:
		if (stereo)
			add_samples16_stereo(left, right);
		else
			buffer16[idx++] = left + right;
			
		break;
	case ADFLin8:
		if (stereo) 
			add_samples8_stereo(left, right)
		else
			buffer[idx++] = left + right;
		break;
	default:
		}
   }


void discard_buffer()
{
    /* not implemented -- is this needed??? */
}


void close_audio()
{
    close(stream_socket);

    /*
     * set close mode to prevent playback from stopping
     *	when we close audio connection
     */
    ASetCloseDownMode(audio, AKeepTransactions, NULL);

    /*
     *	That's all, folks!
     */
    ACloseAudio(audio, NULL);
    audio = NULL;
}


/*
 * Local Variables:
 * c-indent-level: 4
 * c-continued-statement-offset: 4
 * c-brace-offset: -4
 * c-argdecl-indent: 0
 * c-label-offset: -4
 * End:
 */
