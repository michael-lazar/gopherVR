/* soundblaster/audio.c 
	vi:ts=3 sw=3:
 */
/* IMPORTANT NOTE: I can't check that this file works.
 */

/* $Id: audio.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $
 * $Log: audio.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:23  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:41  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.4  1995/02/23  13:52:30  espie
 * primary, secondary -> primary+secondary, primary-secondary
 * strike out 2 multiplications out of 4 !
 *
 * Revision 4.3  1995/02/21  17:57:55  espie
 * Internal problem: RCS not working.
 *
 * Revision 4.2  1995/02/01  16:43:47  espie
 * 23 bit samples.
 *
 * Revision 1.2  1993/12/26  18:54:21  Espie
 * Handle errors better.
 *
 * Revision 3.3  1992/12/03  15:00:50  espie
 * restore stty.
 *
 * Revision 3.1  1992/11/19  20:44:47  espie
 * Protracker commands.
 *
 * Revision 3.0  1992/11/18  16:08:05  espie
 * New release.
 *
 * Revision 1.5  1992/11/17  15:38:00  espie
 * Dummy discard_buffer()
 * Added stereo option (kind of).
 */

#include "defs.h"
#include "extern.h"
#include <i386/isa/sblast.h>

ID("$Id: audio.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $")

#define DEFAULT_BUFFERS
#define DEFAULT_SET_MIX
#define NEW_OUTPUT_SAMPLES_AWARE
#define UNSIGNED_BUFFERS

#include "Arch/common.c"

FILE *audio;            /* /dev/sb_dsp */

int open_audio(f, s)
int f;
int s;
    {
	sbFLAG sb_stereo = s;
	
	audio = fopen("/dev/sb_dsp", "w");
    if (!audio)
		end_all("Error opening audio device");

    stereo = s;
    if (ioctl(fileno(audio), DSP_IOCTL_STEREO, &sb_stereo) == -1)
    	end_all("Error setting stereo/mono");

    if (stereo)
        f *= 2;     /* XXX Stereo takes twice the speed */

    if (f == 0)
        f = -1;     /* read current frequency from driver */

    if (ioctl(fileno(audio), DSP_IOCTL_SPEED, &f) == -1)
    	end_all("Error setting frequency");

    buffer = malloc(sizeof(char) * f);    /* Stereo makes x2 */

    if (stereo)         /* tacky, I know.. */
        return f/ 2;
    else
        return f;
    }

void output_samples(left, right, n)
int left, right, n;
	{
   add_samples8(left, right, n);
    }

void discard_buffer()
	{
	/* not implemented */
	}

void flush_buffer()
    {
    if (fwrite(buffer, sizeof(*buffer), idx, audio) != idx)
		notice("fwrite didn't write all the bytes ?");
    idx = 0;
    }

/*
 * Closing the BSD SBlast sound device waits for all pending samples to play.
 * I think SysV aborts, so you might have to flush manually with ioctl()
 */
void close_audio()
    {
    fclose(audio);
    free(buffer);
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
