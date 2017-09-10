/* NeXT/audio.c 
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
 * Revision 1.8  1995/02/27  14:25:37  espie
 * Rolf Grossmann patch.
 *
 * Revision 1.7  1995/02/23  22:41:45  espie
 * Added # of bits.
 *
 * Revision 1.6  1995/02/23  17:03:14  espie
 * Continuing changes for a standard file.
 *
 * Revision 1.5  1995/02/23  16:42:27  espie
 * Began conversion to `common' model.
 *
 * Revision 1.4  1995/02/23  13:52:30  espie
 * primary, secondary -> primary+secondary, primary-secondary
 * strike out 2 multiplications out of 4 !
 *
 * Revision 1.3  1995/02/21  17:57:55  espie
 * Internal problem: RCS not working.
 *
 * Revision 1.2  1995/02/08  13:16:22  espie
 * *** empty log message ***
 *
 * Revision 1.1  1995/02/01  16:43:47  espie
 * Initial revision
 *
 */

#include "defs.h"
#include <sound/sound.h>
#include "extern.h"

ID("$Id: audio.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")

#define DEFAULT_BUFFERS
#define DEFAULT_SET_MIX
#define NEW_OUTPUT_SAMPLES_AWARE

#include "Arch/common.c"


#define SND_PLAY_PRIO   5

LOCAL SNDSoundStruct ainfo;
LOCAL struct {
    SNDSoundStruct snd;
    char data;
} *snd;
LOCAL int tag;
unsigned int play_ahead;

LOCAL int dsize;
#define DATASIZE 40960 /* 176400 */

int
open_audio(int f, int s)
	{
	static int possible[] = { 8012, 22050, 44100, 0};

	f = best_frequency(f, possible, 22050);

   stereo = s;
   ainfo.samplingRate = f;
   if (stereo)
		ainfo.channelCount = 2;
   else
		ainfo.channelCount = 1;
   if(f!=8012)
		{
		ainfo.dataFormat = SND_FORMAT_LINEAR_16;
		dsize = 2;
		}
   else
		{
		ainfo.dataFormat = SND_FORMAT_MULAW_8;
		dsize = 1;
		if (stereo)
			notice("Warning: Your hardware may not be fast enough \
for mulaw-stereo.");
		}
    
   idx = 0;
   if (SNDAlloc((SNDSoundStruct **)&snd, DATASIZE, ainfo.dataFormat,
			ainfo.samplingRate, ainfo.channelCount, 4))
		end_all("Sound allocation error.");
    
   buffer = &snd->data;
   buffer16 = (short *)&snd->data;
	samples_max = DATASIZE/dsize;
   tag = 1;
   play_ahead = 5;
   return f;
	}

void
set_synchro(int s)
	{ /* not implemented */
	}

int
update_frequency()
	{ /* frequency can't change */
   return 0;
	}

LOCAL void
actually_flush_buffer(void)
	{
   if (tag > play_ahead)
		SNDWait(tag-play_ahead);
   if (ainfo.dataFormat == SND_FORMAT_LINEAR_16)
       SNDSwapSoundToHost(&snd->data, &snd->data, samples_max, 1,
			  SND_FORMAT_LINEAR_16);
   if (SNDStartPlaying(&snd->snd, tag++, SND_PLAY_PRIO, 0,
			NULL, (SNDNotificationFun)SNDFree))
		notice("Sound playing error.");	/* ### end_all? */

   idx = 0;
   if (SNDAlloc((SNDSoundStruct **)&snd, DATASIZE, ainfo.dataFormat,
			ainfo.samplingRate, ainfo.channelCount, 4))
		end_all("Sound allocation error.");
   buffer = &snd->data;
   buffer16 = (short *)&snd->data;
	}

void
output_samples(int left, int right, int n)
	{
	switch(ainfo.dataFormat)
		{
	case SND_FORMAT_LINEAR_16:
		add_samples16(left, right, n);
		break;
	case SND_FORMAT_LINEAR_8:
		add_samples8(left, right, n);
		break;
	case SND_FORMAT_MULAW_8:
		if (stereo)
			{				/* stuff to fix (size of data) right there */
			if (pms[n] == pps[n])
				{
				buffer[idx++] = SNDMulaw(left/65536);
				buffer[idx++] = SNDMulaw(right/65536);
				}
			else
				{
				int s1, s2;

				s1 = (left+right)*pps[n];
				s2 = (left-right)*pms[n];

				buffer[idx++] = SNDMulaw((s1+s2)/65536);
				buffer[idx++] = SNDMulaw((s1-s2)/65536);
				}
			}
		else
			buffer[idx++] = SNDMulaw((left + right)/256);
		break;
	default:
		end_all("Error: unknown output format");
		}
	if (idx >= samples_max)
		actually_flush_buffer();
	}

void 
flush_buffer(void)
	{
	/* dummy call on the NeXt, not enough data */
	}

void
discard_buffer(void)
	{
   int i;
    
   for(i=1; i<=play_ahead; i++)
		SNDStop(tag-i);
	}

void
close_audio(void)
	{
   SNDFree(&snd->snd);
   SNDWait(0);
	}
