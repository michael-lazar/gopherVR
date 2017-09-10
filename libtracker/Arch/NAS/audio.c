/* NAS/audio.c 
	vi:ts=3 sw=3:
 */

/* $Id: audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
 * $Log: audio.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:43  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.2  1995/03/11  23:06:18  espie
 * *** empty log message ***
 *
 * Revision 1.1  1995/03/08  13:48:24  espie
 * Initial revision
 *
 */
/* 

    Stephen Hocking (sysseh@devetir.qld.gov.au)

    The event oriented nature of NAS programming dictated a rather unusual
    solution. Because NAS is built around an event loop, it demands to be in
    control of the application. We can either massively patch the tracker 
    sources to allow this, or simulate co-routines with occasional snoops
    on the event queue so as not to miss events. Either of these are too much
    work for me.
    What I then did was to use fork and have the processes squirting data down
    a pipe. The back end handles all the netaudio stuff, processing events 
    and requesting data from the pipe when necessary. The front end waits
    until the backend returns an int saying what the max frequency is, 0 if
    it fails to make a connection. Then it runs along producing data and
    stuffing it down the pipe as normal.
*/
 

#include "defs.h"
#include "extern.h"
#include <unistd.h>
#include <fcntl.h>
#include <audio/audiolib.h>
#include <audio/soundlib.h>

#define DEFAULT_SET_MIX
#define DEFAULT_BUFFERS
#define NEW_OUTPUT_SAMPLES_AWARE

#include "Arch/common.c"

#define BUF_MAX 1024

LOCAL int audio;           	        /* pipe fd to netaudio process */


LOCAL int dsp_samplesize = 16; /* must be 8 or 16 */

/* communication with child */

LOCAL int pipe1[2], pipe2[2];

LOCAL int num_tracks = 2;
LOCAL int data_format;
LOCAL int sample_rate = 44100;




#define BUF_SAMPLES	(2 * sample_rate)
#define LOW_WATER	(BUF_SAMPLES / 3)



/*-------------------------------------------------------------------------*/
/***
 ***	 Child part... This code never communicates with the rest of tracker
 ***	 except through pipes 
 ***/

LOCAL int endian_data = 1;
#define little_endian ((*(char *)(&endian_data)) == 1)
typedef struct
	{
   AuServer *aud;
   AuFlowID flow;
   int fd;
   char *buf;
	} InfoRec, *InfoPtr;

LOCAL InfoRec infor;
LOCAL char *server = NULL;

LOCAL void fatalError(char *message)
	{
   fprintf(stderr, message);
   fprintf(stderr, "\n");
   exit(1);
	}

LOCAL void sendFile(AuServer *aud, InfoPtr i, AuUint32 numBytes)
	{
   int n;

   while (numBytes) 
		{
		if (n = read(i->fd, i->buf, numBytes))
			{
			AuWriteElement(aud, i->flow, 0, n, i->buf, AuFalse, NULL);
			numBytes -= n;
			}
		else
			exit(0);
		}
	}

LOCAL AuBool eventHandler(AuServer *aud, AuEvent *ev,
	AuEventHandlerRec *handler)
	{
	InfoPtr i = (InfoPtr) handler->data;

	switch (ev->type)
		{
   case AuEventTypeElementNotify:
      {
		AuElementNotifyEvent *event = (AuElementNotifyEvent *) ev;
	
		switch (event->kind)
			{
	   case AuElementNotifyKindLowWater:
	   	sendFile(aud, i, event->num_bytes);
			break;
		case AuElementNotifyKindState:
			switch (event->cur_state)
				{
	      case AuStatePause:
				if (event->reason != AuReasonUser)
				sendFile(aud, i, event->num_bytes);
				break;
				}
			}
      }
	   }
	return AuTrue;
	}

LOCAL int doNetAudioConn()
	{
  	AuDeviceID device = AuNone;
  	AuElement  elements[3];
  	char *filer;
  	int i;
  
  
  	if (!(infor.aud = AuOpenServer(server, 0, NULL, 0, NULL, NULL))) 
		{
    	fprintf(stderr, "Can't open audio server\n");
		return 0;
		}

	data_format = (little_endian) ? 
		AuFormatLinearSigned16LSB : AuFormatLinearSigned16MSB;

  	infor.fd = pipe1[0];

	if (sample_rate > AuServerMaxSampleRate(infor.aud))
		sample_rate = AuServerMaxSampleRate(infor.aud);

	if (sample_rate < AuServerMinSampleRate(infor.aud))
		sample_rate = AuServerMinSampleRate(infor.aud);

	  	/* look for an output device */
   for (i = 0; i < AuServerNumDevices(infor.aud); i++)
   	if ((AuDeviceKind(AuServerDevice(infor.aud, i)) ==
			AuComponentKindPhysicalOutput) &&
			AuDeviceNumTracks(AuServerDevice(infor.aud, i)) == num_tracks)
			{
			device = AuDeviceIdentifier(AuServerDevice(infor.aud, i));
			break;
			}

	if (device == AuNone)
		fatalError("Couldn't find an output device");

  	if (!(infor.flow = AuCreateFlow(infor.aud, NULL)))
   	fatalError("Couldn't create flow");

  	AuMakeElementImportClient(&elements[0], sample_rate, data_format, 
		num_tracks, AuTrue, BUF_SAMPLES, LOW_WATER, 0, NULL);
	AuMakeElementExportDevice(&elements[1], 0, device, sample_rate,
	   AuUnlimitedSamples, 0, NULL);
	AuSetElements(infor.aud, infor.flow, AuTrue, 2, elements, NULL);

	AuRegisterEventHandler(infor.aud, AuEventHandlerIDMask, 0, infor.flow,
		eventHandler, (AuPointer) &infor);

	infor.buf = (char *) malloc(BUF_SAMPLES * num_tracks *
		AuSizeofFormat(data_format));

	return sample_rate;
	}

LOCAL void HandleEvents()
	{
	AuStartFlow(infor.aud, infor.flow, NULL);
	while (1)
		AuHandleEvents(infor.aud);
	}


LOCAL void start_child()
	{
	sample_rate = doNetAudioConn();
	write(pipe2[1], &sample_rate, sizeof(int));
	HandleEvents();        /* Never returns */
	}

/***
 ***	End of child part
 ***/
/*-------------------------------------------------------------------------*/



int open_audio(f, s)
int f;
int s;
	{
  	int childpid;
  
  	stereo = s;
  	sample_rate = f;

  	if (stereo)
   	num_tracks = 2;
	else
   	num_tracks = 1;

  	if (pipe(pipe1) < 0 || pipe(pipe2) < 0)
		end_all("Can't create pipes");

  	if ((childpid = fork()) < 0)
		end_all("Can't fork");
  	else if (childpid > 0)      /* parent - must wait for sample rate */
    	{
      close(pipe1[0]);
      close(pipe2[1]);

      read(pipe2[0], &sample_rate, sizeof(int));  /* sample for parent */
      
      audio = pipe1[1];
		dsize = 2;
		samples_max = BUF_MAX * num_tracks;
      buffer16 = (short *)malloc(samples_max * dsize);
      idx = 0;

      return sample_rate;
		}
  else                       	/* Child */
  		start_child();				/* never returns */
	}

LOCAL void actually_flush_buffer()
	{
	write(audio, buffer16, dsize * idx);
  	idx = 0;
	}

void output_samples(left, right, n)
int left, right, n;
	{
	if (idx >= samples_max - 1)
		actually_flush_buffer();
	add_samples16(left, right, n);
	}

void flush_buffer()
	{	/* Dummy version */
	}

void close_audio()
	{
  	actually_flush_buffer();
	close(audio);
	free(buffer16);
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

