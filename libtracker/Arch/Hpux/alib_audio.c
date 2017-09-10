/* hpalib_audio.c 
	vi:ts=3 sw=3:
 */

/* port to hp using the Alib library */
/* $Id: alib_audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
 * $Log: alib_audio.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:42  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.4  1995/02/23  16:42:27  espie
 * Began conversion to `common' model.
 *
 * Revision 1.3  1995/02/23  14:04:53  espie
 * primary/secondary -> pps/pms.
 *
 * Revision 1.2  1995/02/21  17:57:55  espie
 * Internal problem: RCS not working.
 *
 * Revision 1.1  1995/02/01  16:43:47  espie
 * Initial revision
 *
 * Revision 1.1  1995/02/01  16:43:47  espie
 * Initial revision
 *
 * Revision 1.2  1993/12/04  16:12:50  espie
 * BOOL -> boolean.
 *
 * Revision 1.1  1993/07/14  16:33:41  espie
 * Initial revision
 *
 */

#include "defs.h"
#include "extern.h"
#include <audio/Alib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

ID("$Id: alib_audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")


#define DEFAULT_BUFFERRRS
#define DEFAULT_SEX_MIX
#include "common.c"

Audio	*audio;		/* AUDIO connection for Alib, like DISPLAY for Xlib */

LOCAL int freq;			/* which frequency do we want? */

AErrorHandler	prevHandler;	/* pointer to previous handler */    

LOCAL int	audioPaused = True;
LOCAL int	streamSocket = 0;
LOCAL int	pauseCount;

LOCAL short	*bufBase = NULL;
LOCAL long	inLen;

LOCAL ATransID	xid;

LOCAL AConvertParams	*convert_params;

LOCAL AudioAttrMask	AttribsMask = 0, PlayAttribsMask = 0, ignoredMask = 0;
LOCAL AudioAttributes	Attribs, PlayAttribs;
LOCAL AGainEntry	gainEntry[4];	/* Need to be global since */
					/* PlayAttribs is global and it */
					/* contains a pointer to gainEntry */
LOCAL SSPlayParams	streamParams;



long myErrorHandler(audio, err_event)
	Audio		*audio;
	AErrorEvent	*err_event;
{
    char	errorbuf[132];

    AGetErrorText(audio, err_event->error_code, errorbuf, 131);
	 end_all(errorbuf);
}


void create_playstream()
{
    LOCAL SStream	audioStream;

    /*
     * Initiate transaction.
     */
    xid = APlaySStream(audio, ~0, &PlayAttribs, &streamParams,
		       &audioStream, NULL);

    /*
     * Create a stream socket.
     */
    streamSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(streamSocket < 0) {
	 end_all("Socket creation failed");
    }
  
    /*
     * Connect the stream socket to the audio stream port.
     */
    if (connect(streamSocket,
		(struct sockaddr *)&audioStream.tcp_sockaddr,
		sizeof(struct sockaddr_in)) < 0) {
		end_all("Connect failed");
    }
}  


int open_audio(f, s)
int f;
int s;
{
    char		*pSpeaker;
    int			useIntSpeaker;
    int			seekOffset, data_length;
    AByteOrder		play_byte_order, byte_order;


    audio = AOpenAudio(NULL, NULL);

    if (!audio) {
		end_all("Error opening audio device");
    }

    /* replace default error handler */
    prevHandler = ASetErrorHandler(myErrorHandler);

    stereo = s;
    if (stereo) {
        Attribs.attr.sampled_attr.channels = 2;
        AttribsMask |= (AttribsMask | ASChannelsMask);
    }

    /*
     * Get the best attributes from the server?
     */
    if (f <= 0) {
	AudioAttributes	*bestAttr;

	bestAttr = ABestAudioAttributes(audio);
	freq = bestAttr->attr.sampled_attr.sampling_rate;
    } else
	freq = f;

    PlayAttribs.attr.sampled_attr.sampling_rate = freq;
    PlayAttribsMask |= ASSamplingRateMask;

    Attribs.attr.sampled_attr.sampling_rate = freq;
    AttribsMask |= ASSamplingRateMask;

    AChooseSourceAttributes(audio, NULL, NULL, AFFRawLin16,
			    AttribsMask, &Attribs, &seekOffset,
			    &data_length, &byte_order, NULL);

    AChoosePlayAttributes(audio, &Attribs, PlayAttribsMask,
			  &PlayAttribs, &play_byte_order, NULL);

    /*
     * Prepare for conversion.
     * Must remember to free convert_params by calling AEndConversion.
     */
    convert_params = ASetupConversion(audio, &Attribs, &byte_order,
				      &PlayAttribs, &play_byte_order, NULL);

    pSpeaker = getenv("SPEAKER");	/* get user speaker preference */
    if (pSpeaker) {
	useIntSpeaker = (*pSpeaker == 'i' || *pSpeaker == 'I');
    } else {
	/*
	 * SPEAKER env.var not found - use internal speaker.
	 */  
	useIntSpeaker = 1;
    }

    switch (PlayAttribs.attr.sampled_attr.channels) {

    case 1:	/* Mono */
	gainEntry[0].u.o.out_ch = AOCTMono;
	gainEntry[0].gain = AUnityGain;
	gainEntry[0].u.o.out_dst
	    = (useIntSpeaker) ? AODTMonoIntSpeaker : AODTMonoJack;
	break;

    case 2:	/* Stereo */
    default:
	gainEntry[0].u.o.out_ch = AOCTLeft;
	gainEntry[0].gain = AUnityGain;
	gainEntry[0].u.o.out_dst
	    = (useIntSpeaker) ? AODTLeftIntSpeaker : AODTLeftJack;
	gainEntry[1].u.o.out_ch = AOCTRight;
	gainEntry[1].gain = AUnityGain;
	gainEntry[1].u.o.out_dst
	    = (useIntSpeaker) ? AODTRightIntSpeaker : AODTRightJack;
	break;
    }
    streamParams.gain_matrix.type = AGMTOutput;	/* gain matrix */
    streamParams.gain_matrix.num_entries
	= PlayAttribs.attr.sampled_attr.channels;
    streamParams.gain_matrix.gain_entries = gainEntry;
    streamParams.play_volume = AUnityGain;	/* play volume */
    streamParams.priority = APriorityNormal;	/* normal priority */
    streamParams.event_mask = 0;		/* don't solicit any events */

    /*
     * Create an audio stream.
     */
    create_playstream();

    /*
     * Calculate the required buffer size for the data prior to conversion
     * and allocate memory for the pre-converted data.
     */
    inLen = ACalculateLength(audio, audio->block_size,
			     &PlayAttribs, &Attribs, NULL); 
    buffer16 = malloc(inLen);
    idx = 0;

    /*
     * Allocate a buffer for the converted data.
     */
    bufBase = malloc(inLen);

    /*
     * Start stream paused so we can transfer enough data (3 seconds worth)
     * before playing starts to prevent stream from running out.
     */
    APauseAudio(audio, xid, NULL, NULL);
    pauseCount = 3
		* PlayAttribs.attr.sampled_attr.channels
		* PlayAttribs.attr.sampled_attr.sampling_rate
		* (PlayAttribs.attr.sampled_attr.bits_per_sample >> 3);
    audioPaused = True;

    freq = PlayAttribs.attr.sampled_attr.sampling_rate;

    if (freq != f)
		{
		static char buf[50];
		sprintf(buf, "Frequency used is %d\n", freq);
		notice(buf);
		}

    set_mix(30);

    return freq;
}


void output_samples(left, right)
int left, right;
	{
   if (idx > inLen - 2)
		flush_buffer();

   if (stereo) 
		add_samples16_stereo(left, right);
   else 
	  	buffer16[idx++] = left + right;
	}


void discard_buffer()
{
    /*
     * Destroy old playstream.
     */
    AStopAudio(audio, xid, ASMThisTrans, NULL, NULL);
    close(streamSocket);
    streamSocket = 0;

    /*
     * Recreate new playstream.
     */
    create_playstream();
    idx = 0;
}


void flush_buffer()
{
    int		len_written = 0, len, bytes_written, bytes_read;
    short	*buf = buffer16;

    /*
     * Convert buffer
     */
    AConvertBuffer(audio, convert_params, buffer16, buffer16 * sizeof(short),
		   bufBase, inLen, &bytes_read, &bytes_written, NULL);
    len = bytes_written;
    buf = bufBase;    

    /*
     * Write the converted data to the stream socket
     */
    while (len) {
        /*
	 * write converted data to stream socket until we have emptied buffer
	 */
	if ((len_written = write(streamSocket, buf, len)) < 0) {
		end_all("Write failed");
        }
	buf += len_written;
	len -= len_written;

	if (audioPaused) {
	    pauseCount -= len_written;
	    if (len_written == 0 || pauseCount <= 0) {
		AResumeAudio(audio, xid, NULL, NULL);
		audioPaused = False;
	    }
	}
    }
    idx = 0;
}


void close_audio()
{
    int		bytes_written;


    if (audioPaused) {
	AResumeAudio(audio, xid, NULL, NULL);
    }
    /*
     * Free the convert_params structure and flush out
     * the conversion pipeline
     */
    AEndConversion(audio, convert_params, bufBase,
		   audio->block_size, &bytes_written, NULL);

    ASetCloseDownMode(audio, AKeepTransactions, NULL);
    ASetErrorHandler(prevHandler);
    ACloseAudio(audio, NULL);

    if (streamSocket)	close(streamSocket);
    if (buffer16)		free(buffer16);
    if (bufBase)	free(bufBase);

	end_all(0);
}


int update_frequency()
{
    /* not implemented */
    return 0;
}


void set_synchro(sync)
int sync;
{
    if (streamSocket) {
	if (sync)
	    setsockopt(streamSocket, SOL_SOCKET, TCP_NODELAY, 1);
	else
	    setsockopt(streamSocket, SOL_SOCKET, TCP_NODELAY, 0);
    }
}
