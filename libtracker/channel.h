/* channel.h 
	vi:ts=3 sw=3:
 */

/* $Id: channel.h,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: channel.h,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.14  1995/03/11  21:40:13  espie
 * Added better jump pattern, invert_loop.
 *
 * Revision 4.13  1995/03/04  00:15:28  espie
 * Implemented vibrato control.
 *
 * Revision 4.12  1995/03/01  15:24:51  espie
 * Added start_offset.
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.9  1995/02/20  22:28:50  espie
 * Tremolo
 *
 * Revision 4.8  1995/02/20  16:49:58  espie
 * Added funk_glissando for command 3.
 *
 * Revision 4.5  1995/02/01  16:39:04  espie
 * Includes moved to defs.h
 *
 * Revision 3.9  1993/11/17  15:31:16  espie
 * audio_channel private.
 * Amiga support.
 * Added finetune.
 *
 * Revision 2.7  1992/11/13  13:24:24  espie
 * Added parameters for extended Retriger command.
 * Added transpose feature.
 * Structured part of the code, especially replay ``automaton''
 * and setting up of effects.
 *
 * Revision 1.5  1991/11/16  16:54:19  espie
 * Bug correction: when doing arpeggio, there might not
 * be a new note, so we have to save the old note value
 * and do the arppeggio on that note.
 * Added fields for arpeggio.
 */

     
#ifndef NUMBER_PATTERNS
#define NUMBER_PATTERNS 128
#endif

#define MAX_ARP 3
     
/* there is no note in each channel initially.
 * This is defensive programming, because some
 * commands rely on the previous note. Checking
 * that there was no previous note is a way to
 * detect faulty modules.
 */
#define NO_NOTE 255

struct channel
   {
   struct sample_info *samp;
   struct audio_channel *audio;
   int finetune;
   int volume;             /* current volume of the sample (0-64) */
   int pitch;              /* current pitch of the sample */
   int note;               /* we have to save the note cause */
                           /* we can do an arpeggio without a new note */
    
   int arp[MAX_ARP];       /* the three pitch values for an arpeggio */
   int arpindex;           /* an index to know which note the arpeggio is doing */

   int viboffset;          /* current offset for vibrato (if any) */
   int vibdepth;           /* depth of vibrato (if any) */
   int vibrate;            /* step rate for vibrato */
	int *vibtable;
	int resetvib;

   int slide;              /* step size of pitch slide */

   int pitchgoal;          /* pitch to slide to */
   int pitchrate;          /* step rate for portamento */

   int volumerate;         /* step rate for volume slide */


	int tremoffset;
	int tremdepth;
	int tremrate;
	int *tremtable;
	int resettrem;

	int start_offset;

   int retrig;             /* delay for extended retrig command */
   int current;

	int funk_glissando;	
                           /* current command to adjust parameters */
   void (*adjust) P((struct channel *ch));
	int loop_counter;
	int loop_note_num;

	int invert_speed;
	int invert_offset;
	int invert_position;
   void (*special) P((struct channel *ch));
   };

#define DO_NOTHING 0 
#define SET_SPEED 1
#define SET_SKIP 2
#define SET_FASTSKIP 4
#define SET_FINESPEED 32

#define JUMP_PATTERN 8
#define DELAY_PATTERN 16

#define NORMAL_SPEED 6
#define NORMAL_FINESPEED 125

struct automaton
   {
   int pattern_num;           /* the pattern in the song */
   int note_num;              /* the note in the pattern */
   struct block *pattern;     /* the physical pattern */
   struct song_info *info;    /* we need the song_info */

   char gonethrough[NUMBER_PATTERNS + 1];  /* to check for repeats */

   int counter;               /* the fine position inside the effect */
   int speed;                 /* the `speed', number of effect repeats */
   int finespeed;             /* the finespeed, base is 100 */

   int do_stuff;              /* keeping some stuff to do */
                              /* ... and parameters for it: */
   int new_speed, new_note, new_pattern, new_finespeed;

   int pitch, note, para;     /* some extra parameters effects need */

   int loop_note_num, loop_counter;
                              /* for command E6 */
   };
