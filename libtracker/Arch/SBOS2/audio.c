/* soundblaster_audio.c */

/* modified by David Nichols for this PM MOD player */

/* MODIFIED BY Michael Fulbright (MSF) to work with os/2 device driver */

/* $Author: lindner $
 * $Id: audio.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $
 * $Log: audio.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:23  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:41  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.5  1995/02/23  22:41:45  espie
 * Added # of bits.
 *
 * Revision 1.4  1995/02/23  17:03:14  espie
 * Continuing changes for a standard file.
 *
 * Revision 1.3  1995/02/23  13:52:30  espie
 * primary, secondary -> primary+secondary, primary-secondary
 * strike out 2 multiplications out of 4 !
 *
 * Revision 1.2  1995/02/21  17:57:55  espie
 * Internal problem: RCS not working.
 *
 */

#define INCL_DOS

#include "defs.h"
#include <os2.h>
#include "sbos2_user.h"
#include "extern.h"
#include "song.h"

#define DEFAULT_SET_MIX
#define DEFAULT_BUFFERS
#define UNSIGNED8
#define NEW_OUTPUT_SAMPLES_AWARE
#include "Arch/common.c"

struct sb_mixer_levels sbLevels;
struct sb_mixer_params sbParams;

ID("$Id: audio.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $");


HFILE hAudio;      /* audio handle */

int fixparams = 0;
int filterout;
int filterin;
int filterhi;

void restoreparams()
{
   ULONG parlen, datlen;

   if (fixparams)
   {
      parlen = 0;
      datlen = sizeof(struct sb_mixer_params);
      DosDevIOCtl(hAudio, DSP_CAT, MIXER_IOCTL_READ_PARAMS,
             NULL, 0, &parlen, &sbParams, datlen, &datlen);
      sbParams.hifreq_filter = filterhi;
      sbParams.filter_output = filterout;
      sbParams.filter_input = filterin;
      parlen = 0;
      datlen = sizeof(struct sb_mixer_params);
      DosDevIOCtl(hAudio, DSP_CAT, MIXER_IOCTL_SET_PARAMS,
              NULL, 0, &parlen, &sbParams, datlen, &datlen);
   }
}

int open_audio(int frequency, int DMAbuffersize)
{
  USHORT status, freq;
  USHORT   flag;
  ULONG  datlen, parlen, action, temp;
 
  /* MSF - open SBDSP for output */
  status = DosOpen( "SBDSP$", &hAudio, &action, 0, FILE_NORMAL, FILE_OPEN,
   OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYREADWRITE |
   OPEN_FLAGS_WRITE_THROUGH | OPEN_FLAGS_NOINHERIT |
   OPEN_FLAGS_NO_CACHE | OPEN_FLAGS_FAIL_ON_ERROR, NULL );

  if (status != 0) 
    end_all("Error opening audio device SBDSP$");

  /* see if we are on a SBREG or SBPRO */
  status = DosOpen( "SBMIX$", &temp, &action, 0, FILE_NORMAL, FILE_OPEN,
   OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE |
   OPEN_FLAGS_WRITE_THROUGH | OPEN_FLAGS_NOINHERIT | 
   OPEN_FLAGS_NO_CACHE, NULL );

  if (status !=0) stereo=FALSE;
  else
   {
      fixparams = TRUE;
      parlen = 0;
      datlen = sizeof(struct sb_mixer_params);
      DosDevIOCtl(hAudio, DSP_CAT, MIXER_IOCTL_READ_PARAMS,
             NULL, 0, &parlen, &sbParams, datlen, &datlen);
      filterhi = sbParams.hifreq_filter;
      filterout = sbParams.filter_output;
      filterin = sbParams.filter_input;
      sbParams.hifreq_filter = TRUE;
      sbParams.filter_output = FALSE;
      sbParams.filter_input = TRUE;
      parlen = 0;
      datlen = sizeof(struct sb_mixer_params);
      DosDevIOCtl(hAudio, DSP_CAT, MIXER_IOCTL_SET_PARAMS,
               NULL, 0, &parlen, &sbParams, datlen, &datlen);
      datlen=1;
      parlen=0;
      flag=stereo;
      status=DosDevIOCtl(hAudio, DSP_CAT, DSP_IOCTLstereo,
			 NULL, 0, &parlen, &flag, 1, &datlen);
      if (status != 0)
         end_all("Error setting stereo/mono");
      datlen = 1;
      flag = DMAbuffersize * 1024;
      DMAbuffersize = flag;
      status=DosDevIOCtl(hAudio, DSP_CAT, DSP_IOCTL_BUFSIZE,
                    NULL, 0, &parlen, &DMAbuffersize, datlen, &datlen);
      if (status != 0)
          end_all("Error setting DMA buffer size");
   }

  if (stereo) frequency *= 2;  /* XXX Stereo takes twice the speed */

  if (frequency == 0) frequency = -1;  /* read current frequency from driver */

  /* set speed */
  datlen=2;
  parlen=0;
  freq = (USHORT) frequency;
  status=DosDevIOCtl(hAudio, DSP_CAT, DSP_IOCTL_SPEED,
		     NULL, 0, &parlen, &freq, 2, &datlen);
  frequency=freq;
  if (status!=0)
      end_all("Error setting frequency");

  buffer = malloc (sizeof(SAMPLE) * frequency);	/* Stereo makes x2 */
  idx = 0;

  if (stereo) return (frequency / 2);
  else return (frequency);
}

void output_samples (int left, int right, int n)
	{
	add_samples8(left, right, n);
	}

void discard_buffer()
	{
	/* not implemented */
	}

void flush_buffer ()
{
  ULONG numread, status;

  status = DosWrite(hAudio, buffer, idx, &numread);
   if (status != 0)
   {
      char buf[80];
      sprintf(buf, "Error writing to audio device: %d, tried to write: %d, wrote: %d", status, idx, numread);
      end_all(buf);
   }
  if (numread != idx)
   {
      char buf[80];

      sprintf(buf, "DosWrite mismatch, idx: %d, numread: %d", idx, numread);
      notice(buf);
   }      
  idx = 0;
}

void flush_DMA_buffers()
{
  ULONG status, datlen, parlen;

  /* now tell device driver to flush out internal buffers */
  parlen=0;
  datlen=0;
  status=DosDevIOCtl(hAudio, DSP_CAT, DSP_IOCTL_FLUSH,
                    NULL, 0, &parlen, NULL, 0, &datlen);
   if (status != 0)
   {
      char buf[80];

      sprintf(buf, "Error flushing DMA buffers: %d", status);
      notice(buf);
   }
}

void close_audio ()
{
   DosClose(hAudio);
}

int update_frequency()
	{
	/* not implemented */
	return 0;
	}

void set_synchro(int s)
	{
	/* not implemented */
	}
