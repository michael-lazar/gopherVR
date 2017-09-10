/*======================================================================

MSF - This is user header for SBOS2.SYS versions 0.91
MSF - this is part of SBOS2, original header below.

   Header file for Sound Blaster user programs.
   [ This file is a part of SBlast-BSD-1.4 ]

   Steve Haehnichen <shaehnic@ucsd.edu>

   $Id: sbos2_user.h,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $
 

   Copyright (C) 1992 Steve Haehnichen.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 * $Log: sbos2_user.h,v $
 * Revision 1.1.1.1  2002/01/18 16:34:23  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:41  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.1  1995/02/08  13:16:22  espie
 * Initial revision
 *
 * Revision 1.5  1992/06/13  01:48:15  steve
 * Released in SBlast-BSD-1.4

======================================================================*/

#ifndef SBLAST_H
#define SBLAST_H


typedef unsigned char FLAG;

/*
 * Available compression modes
 * (These are actually DSP DMA-start commands, but you don't care)
 */
enum
{
  PCM_8 	= 0x14,		/* Straight 8-bit PCM */
  ADPCM_4 	= 0x74,		/* 4-bit ADPCM compression */
  ADPCM_2_6	= 0x76,		/* 2.5-bit ADPCM compression */
  ADPCM_2 	= 0x16,		/* 2-bit ADPCM compression */
};

enum { 
  CH_LEFT, CH_RIGHT, CH_BOTH
};	/* Stereo channel choices */

/*   IOCTLs for FM, Mixer, and DSP control.   */

/* MSF - Define category for SBDSP ioctl requests (DSP and MIXER actually) */
#define DSP_CAT   0x80

/*
 * FM: Reset chips to silence
 *     Play a note as defined by struct sb_fm_note
 *     Set global FM parameters as defined by struct sb_fm_params
 */
/* MSF - use same functions codes as original driver */
#define FM_IOCTL_RESET      	10
#define FM_IOCTL_PLAY_NOTE  	11
#define FM_IOCTL_SET_PARAMS 	13

/* Mixer: Set mixer volume levels
 *        Set mixer control parameters (non-volume stuff)
 *	  Read current mixer volume levels
 *	  Read current mixer parameters
 *	  Reset the mixer to a default state
 */	  
#define MIXER_IOCTL_SET_LEVELS 	20
#define MIXER_IOCTL_SET_PARAMS 	21
#define MIXER_IOCTL_READ_LEVELS	22
#define MIXER_IOCTL_READ_PARAMS 23
#define MIXER_IOCTL_RESET 	24

/* DSP: Reset the DSP and terminate any transfers.
 *      Set the speed (in Hz) of DSP playback/record.
 *      (Note that the speed parameter is modified to be the actual speed.)
 *      Turn the DSP voice output on (non-zero) or off (0)
 *      Flush any pending written data.
 *      Set the DSP decompression mode to one of the above modes.
 *      Set Stereo playback/record on (non-zero) or off (0)
 *      Set the DMA transfer buffer size
 *      Set error when there is recording overrun (ESUCCESS for ignore)
 */
#define DSP_IOCTL_RESET 	0
#define DSP_IOCTL_SPEED 	1
#define DSP_IOCTL_VOICE 	2
#define DSP_IOCTL_FLUSH 	3
#define DSP_IOCTL_COMPRESS	4
#define DSP_IOCTL_STEREO	5
#define DSP_IOCTL_BUFSIZE       6
#define DSP_IOCTL_OVERRUN_ERRNO 7

/* DSP legal speed range (really!) */
#define		DSP_MAX_SPEED_PRO	43478
#define         DSP_MAX_SPEED_REG       22222
#define		DSP_MIN_SPEED	3906

/* define largest, smallest sizes for transfer buffer size */
#define DSP_MIN_BUFSIZE 256
#define DSP_MAX_BUFSIZE (64*1024-2)

struct stereo_vol
{
  BYTE l;			/* Left volume */
  BYTE r;			/* Right volume */
};


/*
 * Mixer volume levels for MIXER_IOCTL_SET_VOL & MIXER_IOCTL_READ_VOL
 */
struct sb_mixer_levels
{
  struct stereo_vol master;	/* Master volume */
  struct stereo_vol voc;	/* DSP Voice volume */
  struct stereo_vol fm;		/* FM volume */
  struct stereo_vol line;	/* Line-in volume */
  struct stereo_vol cd;		/* CD audio */
  BYTE mic;			/* Microphone level */
};

/*
 * Mixer parameters for MIXER_IOCTL_SET_PARAMS & MIXER_IOCTL_READ_PARAMS
 */
struct sb_mixer_params
{
  BYTE record_source;		/* Recording source (See SRC_xxx below) */
  FLAG hifreq_filter;		/* Filter frequency (hi/low) */
  FLAG filter_input;		/* ANFI input filter */
  FLAG filter_output;		/* DNFI output filter */
  FLAG dsp_stereo;		/* 1 if DSP is in Stereo mode */
};
#define SRC_MIC         1	/* Select Microphone recording source */
#define SRC_CD          3	/* Select CD recording source */
#define SRC_LINE        7	/* Use Line-in for recording source */


/*
 * Data structure composing an FM "note" or sound event.
 */
struct sb_fm_note
{
  BYTE channel;			/* LEFT, RIGHT, or BOTH */
  BYTE operator;		/* Operator cell (0 or 1) */
  BYTE voice;			/* FM voice (0 to 8) */

  /* Per operator: */
  BYTE waveform;		/* 2 bits: Select wave shape (see WAVEFORM) */
  FLAG am;			/* Amplitude modulation */
  FLAG vibrato;			/* Vibrato effect */
  FLAG do_sustain;		/* Do sustain phase, or skip it */
  FLAG kbd_scale;		/* Keyboard scaling? */

  BYTE harmonic;		/* 4 bits: Which harmonic to generate */
  BYTE scale_level;		/* 2 bits: Decrease output as freq rises*/
  BYTE volume;			/* 6 bits: Intuitive volume */

  BYTE attack;			/* 4 bits: Attack rate */
  BYTE decay;			/* 4 bits: Decay rate */
  BYTE sustain;			/* 4 bits: Sustain level */
  BYTE release;			/* 4 bits: Release rate */

  /* Apply to both operators of one voice: */
  BYTE feedback;		/* 3 bits: How much feedback for op0 */
  FLAG key_on;			/* Set for active, Clear for silent */
  FLAG indep_op;		/* Clear for op0 to modulate op1,
				   Set for parallel tones. */

  /* Freq = 50000 * Fnumber * 2^(octave - 20) */
  BYTE octave;			/* 3 bits: What octave to play (0 = low) */
  unsigned int fnum;		/* 10 bits: Frequency "number" */
};

/*
 * FM parameters that apply globally to all voices, and thus are not "notes"
 */
struct sb_fm_params
{
  BYTE channel;			/* LEFT, RIGHT, or BOTH, as defined above */

  FLAG am_depth;		/* Amplitude Modulation depth (1=hi) */
  FLAG vib_depth;		/* Vibrato depth (1=hi) */
  FLAG wave_ctl;		/* Let voices select their waveform */
  FLAG speech;			/* Composite "Speech" mode (?) */
  FLAG kbd_split;		/* Keyboard split */
  FLAG rhythm;			/* Percussion mode select */

  /* Percussion instruments */
  FLAG bass;
  FLAG snare;
  FLAG tomtom;
  FLAG cymbal;
  FLAG hihat;
};

#endif				/* !def SBLAST_H */
