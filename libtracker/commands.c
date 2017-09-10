/* commands.c 
	vi:ts=3 sw=3:
 */

/* $Id: commands.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
 * $Log: commands.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.18  1995/03/11  21:40:25  espie
 * Added jump_pattern, invert_loop.
 *
 * Revision 4.17  1995/03/04  00:15:48  espie
 * Implemented vibrato control.
 *
 * Revision 4.16  1995/03/01  15:24:51  espie
 * tone portamento and set offset more conformant to protracker.
 *
 * Revision 4.15  1995/02/25  15:43:11  espie
 * Bug in tremolo FIXED !!!
 *
 * Revision 4.14  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.13  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.11  1995/02/20  22:28:50  espie
 * tremolo
 *
 * Revision 4.10  1995/02/20  16:49:58  espie
 * Checked in new commands, fixed bug in set_finetune:
 * need to adjust the frequency of the current note according to
 * the new finetune.
 *
 * Revision 4.8  1995/02/06  14:50:47  espie
 * Changed sample_info.
 *
 * Revision 4.6  1995/02/01  16:39:04  espie
 * Includes moved to defs.h
 *
 * Revision 4.2  1994/08/23  18:19:46  espie
 * Added speedmode option
 * Abstracted IO.
 * Some notice to status.
 * play_note instead of ch->mode.
 * Fixed bug with bad loops.
 * Modified the way set_speed works.
 * Very small bug with volume (Lawrence).
 * Added bg/fg test.
 * More precise vibrato table.
 *
 * Revision 2.12  1992/11/13  13:24:24  espie
 * Added some extended commands: E12AB, and some.
 * now use set_volume in audio.c. All the device-dependent operation
 * is there.
 * Defensive programming: check the range of each note
 * for arpeggio setup.
 * Structured part of the code, especially replay ``automaton''
 * and setting up of effects.
 *
 * Revision 1.9  1991/11/17  17:09:53  espie
 * Added missing prototypes.
 * Dynamic oversample and frequency.
 * Added arpeggio.
 * Fixed up vibrato depth.
 * Added vibslide and portaslide.
 * Added command 9.
 */


#include "defs.h"
#include "channel.h"
#include "song.h"
#include "extern.h"
#include "prefs.h"
     
ID("$Id: commands.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")

/* sine table for the vibrato effect (obtained through build_vibrato.c) */

int vibrato_table[3][64] = 
	{
   {
   0,50,100,149,196,241,284,325,362,396,426,452,473,490,502,510,512,
   510,502,490,473,452,426,396,362,325,284,241,196,149,100,50,0,-49,
   -99,-148,-195,-240,-283,-324,-361,-395,-425,-451,-472,-489,-501,
   -509,-511,-509,-501,-489,-472,-451,-425,-395,-361,-324,-283,-240,
   -195,-148,-99,-49
   },
	{
	512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,
	512,512,512,512,512,512,512,512,512,512,512,512,512,512,512,-512,-512,
	-512,-512,-512,-512,-512,-512,-512,-512,-512,-512,-512,-512,-512,-512, 
	-512,-512,-512,-512,-512,-512,-512,-512,-512,-512,-512,-512,-512,-512,
	-512,-512
	},
	{
	0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240,256,272,288,
	304,320,336,352,368,384,400,416,432,448,464,480,496,-512,-496,-480,
	-464,-448,-432,-416,-400,-384,-368,-352,-336,-320,-304,-288,-272,-256,
	-240,-224,-208,-192,-176,-160,-144,-128,-112,-96,-80,-64,-48,-32,-16
	}
	};

/***	setting up effects/doing effects :
 ***		set_xxx gets called while parsing the effect,
 *** 		do_xxx gets called each tick, and update the
 ***		sound parameters while playing.
 ***/




/***
 ***	base effects
 ***/

void do_nothing(ch)
struct channel *ch;
   {
   }

LOCAL void not_implemented(a, ch)
struct automaton *a;
struct channel *ch;
	{
	notice("Not implemented");
	}

LOCAL void set_nothing(a, ch)
struct automaton *a;
struct channel *ch;
   {
   }

/* slide pitch (up or down) */
LOCAL void do_slide(ch)
struct channel *ch;
   {
   ch->pitch += ch->slide;
   ch->pitch = MIN(ch->pitch, MAX_PITCH);
   ch->pitch = MAX(ch->pitch, MIN_PITCH);
   set_current_pitch(ch, ch->pitch);
   }

LOCAL void set_upslide(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->adjust = do_slide;
   if (a->para)
      ch->slide = a->para;
   }

LOCAL void set_downslide(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->adjust = do_slide;
   if (a->para)
      ch->slide = -a->para;
   }

/* modulate the pitch with vibrato */
LOCAL void do_vibrato(ch)
struct channel *ch;
   {
   int offset;

      /* this is no longer a literal transcription of the pt
       * code. I have rescaled the vibrato table.
       */
   ch->viboffset += ch->vibrate;
   ch->viboffset &= 63;
      /* please don't use logical shift on signed values */
   offset = (ch->vibtable[ch->viboffset] * ch->vibdepth)/256;
      /* temporary update of only the step value,
       * note that we do not change the saved pitch.
       */
   set_current_pitch(ch, ch->pitch + offset);
   }

LOCAL void set_vibrato(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->adjust = do_vibrato;
   if (HI(a->para))
      ch->vibrate = HI(a->para);
   if (LOW(a->para))
      ch->vibdepth = LOW(a->para);
	if (ch->resetvib)
		ch->viboffset = 0;
   }

LOCAL void do_tremolo(ch)
struct channel *ch;
	{
	int offset;

	ch->tremoffset += ch->tremrate;
	ch->tremoffset &= 63;

	offset = (ch->tremtable[ch->tremoffset] * ch->tremdepth)/128;

	set_temp_volume(ch, ch->volume + offset);
	}

LOCAL void set_tremolo(a, ch)
struct automaton *a;
struct channel *ch;
	{
	ch->adjust = do_tremolo;
	if (HI(a->para))
		ch->tremrate = HI(a->para);
	if (LOW(a->para))
		ch->tremdepth = LOW(a->para);
	if (ch->resettrem)
		ch->tremoffset = 0;
	}

/* arpeggio looks a bit like chords: we alternate between two
 * or three notes very fast.
 */
LOCAL void do_arpeggio(ch)
struct channel *ch;
   {
   if (++ch->arpindex >= MAX_ARP)
      ch->arpindex =0;
   set_current_pitch(ch, ch->arp[ch->arpindex]);
   }

LOCAL void set_arpeggio(a, ch)
struct automaton *a;
struct channel *ch;
   {
      /* arpeggio can be installed relative to the
       * previous note, so we have to check that there
       * actually is a current(previous) note
       */
   if (ch->note == NO_NOTE)
      {
      status("No note present for arpeggio");
      error = FAULT;
      }
   else
      {
      int note;

      ch->arp[0] = pitch_table[ch->note][ch->finetune];
      note = ch->note + HI(a->para);
      if (note < NUMBER_NOTES)
         ch->arp[1] = pitch_table[note][ch->finetune];
      else
         {
         status("Arpeggio note out of range");
         error = FAULT;
         }
      note = ch->note + LOW(a->para);
      if (note < NUMBER_NOTES)
         ch->arp[2] = pitch_table[note][ch->finetune];
      else
         {
         status("Arpeggio note out of range");
         error = FAULT;
         }
      ch->arpindex = 0;
      ch->adjust = do_arpeggio;
      }
   }

/* volume slide. Mostly used to simulate waveform control.
 * (attack/decay/sustain).
 */
LOCAL void do_slidevol(ch)
struct channel *ch;
   {
   set_current_volume(ch, ch->volume + ch->volumerate);
   }

/* note that volumeslide does not have a ``take default''
 * behavior. If para is 0, this is truly a 0 volumeslide.
 * Issue: is the test really necessary ? Can't we do
 * a HI(para) - LOW(para). Answer: protracker does not.
 */
LOCAL void parse_slidevol(ch, para)
struct channel *ch;
int para;
   {
   if (LOW(para))
      ch->volumerate = -LOW(para);
   else
      ch->volumerate = HI(para);
   }

LOCAL void set_slidevol(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->adjust = do_slidevol;
   parse_slidevol(ch, a->para);
   }

/* portamento goes from a given pitch to another.  We could optimize 
 * that effect by splitting the routine into a pitch up/pitch down 
 * part while setting up the effect.
 */
LOCAL void do_portamento(ch)
struct channel *ch;
   {
   if (ch->pitch < ch->pitchgoal)
      {
      ch->pitch += ch->pitchrate;
		if (ch->pitch >= ch->pitchgoal)
			{
			ch->pitch = ch->pitchgoal;
			ch->pitchgoal = 0;		/* ch->pitchgoal reset in protracker */
			ch->adjust = do_nothing;
			}
      }
   else if (ch->pitch > ch->pitchgoal)
      {
      ch->pitch -= ch->pitchrate;
		if (ch->pitch <= ch->pitchgoal)
			{
			ch->pitch = ch->pitchgoal;
			ch->pitchgoal = 0;		/* ch->pitchgoal reset in protracker */
			ch->adjust = do_nothing;
			}
      }
		/* funk glissando: round to the nearest note each time */
	if (ch->funk_glissando)
		set_current_pitch(ch, nearest_note(ch->pitch, ch->finetune));
	else
		set_current_pitch(ch, ch->pitch);
   }

/* if para and pitch are 0, this is obviously a continuation
 * of the previous portamento.
 */
LOCAL void set_portamento(a, ch)
struct automaton *a;
struct channel *ch;
   {
   if (a->para)
      ch->pitchrate = a->para;
   if (a->pitch)
      ch->pitchgoal = a->pitch;
	if (ch->pitchgoal)
		ch->adjust = do_portamento;
   }




/***
 ***	combined commands
 ***/

LOCAL void do_portaslide(ch)
struct channel *ch;
   {
   do_portamento(ch);
   do_slidevol(ch);
   }

LOCAL void set_portaslide(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->adjust = do_portaslide;
   if (a->pitch)
      ch->pitchgoal = a->pitch;
   parse_slidevol(ch, a->para);
   }

LOCAL void do_vibratoslide(ch)
struct channel *ch;
   {
   do_vibrato(ch);
   do_slidevol(ch);
   }

LOCAL void set_vibratoslide(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->adjust = do_vibratoslide;
   parse_slidevol(ch, a->para);
	if (ch->resetvib)
		ch->viboffset = 0;
   }




/***
 ***	effects that just need a setup part
 ***/

/* IMPORTANT: because of the special nature of the player, we can't 
 * process each effect independently, we have to merge effects from 
 * the four channel before doing anything about it. For instance, 
 * there can be several speed change in the same note.
 */
LOCAL void set_speed(a, ch)
struct automaton *a;
struct channel *ch;
   {
   if (a->para >= 32 && get_pref_scalar(PREF_SPEEDMODE) != OLD_SPEEDMODE)
      {
      a->new_finespeed = a->para;
      a->do_stuff |= SET_FINESPEED;
      }
   else if (a->para)
      {
      a->new_speed = a->para;
      a->do_stuff |= SET_SPEED;
      }
   }

LOCAL void set_skip(a, ch)
struct automaton *a;
struct channel *ch;
   {
      /* BCD decoding in read.c */
   a->new_note = a->para;
   a->do_stuff |= SET_SKIP;
   }

LOCAL void set_fastskip(a, ch)
struct automaton *a;
struct channel *ch;
   {
   a->new_pattern = a->para;
   a->do_stuff |= SET_FASTSKIP;
   }

/* immediate effect: starts the sample somewhere
 * off the start.
 */
LOCAL void set_offset(a, ch)
struct automaton *a;
struct channel *ch;
   {
	if (a->para)
		ch->start_offset = a->para * 256;
   set_position(ch, ch->start_offset);
   }

/* change the volume of the current channel. Is effective until there 
 * is a new set_volume, slide_volume, or an instrument is reloaded 
 * explicitly by giving its number. Obviously, if you load an instrument 
 * and do a set_volume in the same note, the set_volume will take precedence.
 */
LOCAL void set_volume(a, ch)
struct automaton *a;
struct channel *ch;
   {
   set_current_volume(ch, a->para);
   }




/***
 ***	extended commands
 ***/

/* retrig note at a fast pace
 */
LOCAL void do_retrig(ch)
struct channel *ch;
   {
   if (--ch->current <= 0)
      {
      reset_note(ch, ch->note, ch->pitch);
      ch->current = ch->retrig;
      }
   }

LOCAL void set_retrig(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->retrig = a->para;
   ch->current = ch->retrig;
   ch->adjust = do_retrig;
   }

/* start note after a small delay
 */
LOCAL void do_latestart(ch)
struct channel *ch;
   {
   if (--ch->current <= 0)
      {
      reset_note(ch, ch->note, ch->pitch);
      ch->adjust = do_nothing;
      }
   }

LOCAL void set_late_start(a, ch)
struct automaton *a;
struct channel *ch;
   {
   play_note(ch->audio, empty_sample() , 0);
   ch->current = a->para;
   ch->adjust = do_latestart;
   }

/* cut note after some time. Note we only kill the volume, 
 * as protracker does (compatibility...)
 */
LOCAL void do_cut(ch)
struct channel *ch;
   {
   if (ch->retrig)
      {
      if (--ch->retrig == 0)
         set_current_volume(ch, 0);
      }
   }

LOCAL void set_note_cut(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->retrig = a->para;
   ch->adjust = do_cut;
   }


LOCAL void set_smooth_up(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->pitch += a->para;
   ch->pitch = MIN(ch->pitch, MAX_PITCH);
   ch->pitch = MAX(ch->pitch, MIN_PITCH);
   set_current_pitch(ch, ch->pitch);
   }

LOCAL void set_smooth_down(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->pitch -= a->para;
   ch->pitch = MIN(ch->pitch, MAX_PITCH);
   ch->pitch = MAX(ch->pitch, MIN_PITCH);
   set_current_pitch(ch, ch->pitch);
   }

LOCAL void set_change_finetune(a, ch)
struct automaton *a;
struct channel *ch;
   {
   ch->finetune = a->para;
	if (a->note != NO_NOTE)
		{
		a->pitch = pitch_table[a->note][ch->finetune];
		reset_note(ch, a->note, a->pitch);
		}
   }


LOCAL void set_loop(a, ch)
struct automaton *a;
struct channel *ch;
   {
      /* Note: the current implementation of protracker
       * does not allow for a jump from pattern to pattern,
       * even though it looks like a logical extension to the current 
       * format.
       */
   if (a->para == 0) 
      ch->loop_note_num = a->note_num;
   else
      {
      if (ch->loop_counter == -1)
         ch->loop_counter = a->para + 1;
      /* We have to defer the actual note jump
       * to automaton.c, because some modules include several
       * loops on the same measure, which is a bit confusing
       * (see don't you want me for a good example)
       */
	 	else
			{
			ch->loop_counter--;
			if (ch->loop_counter > 0)
				{
				a->do_stuff |= JUMP_PATTERN;
				a->loop_note_num = ch->loop_note_num;
				}
			else
				ch->loop_counter = -1;
			}
      }
   }

LOCAL void set_smooth_upvolume(a, ch)
struct automaton *a;
struct channel *ch;
   {
   set_current_volume(ch, ch->volume + a->para);
   }

LOCAL void set_smooth_downvolume(a, ch)
struct automaton *a;
struct channel *ch;
   {
   set_current_volume(ch, ch->volume - a->para);
   }


LOCAL void set_delay_pattern(a, ch)
struct automaton *a;
struct channel *ch;
   {
   a->counter -= (a->para + 1) * a->speed;
   a->do_stuff |= DELAY_PATTERN;
   }



void set_gliss_ctrl(a, ch)
struct automaton *a;
struct channel *ch;
	{
	if (a->para)
		{
		ch->funk_glissando = TRUE;
/*		notice("Funk true");		*/
		}
	else
		{
		ch->funk_glissando = FALSE;
/*		notice("Funk false");	*/
		}
	}

void set_vibrato_wave(a, ch)
struct automaton *a;
struct channel *ch;
	{
	ch->vibtable = vibrato_table[a->para & 3];
	if (a->para & 4)
		ch->resetvib = FALSE;
	else
		ch->resetvib = TRUE;
	}

void set_tremolo_wave(a, ch)
struct automaton *a;
struct channel *ch;
	{
	ch->tremtable = vibrato_table[a->para & 3];
	if (a->para & 4)
		ch->resettrem = FALSE;
	else
		ch->resettrem = TRUE;
	}

LOCAL void do_invert(ch)
struct channel *ch;
	{
	ch->invert_offset += ch->invert_speed;
	if (ch->invert_offset >= 128)
		{
		ch->invert_offset = 0;
		if (ch->samp->rp_length)
			{
			if (++ch->invert_position >= ch->samp->rp_length)
				ch->invert_position = 0;
			ch->samp->rp_start[ch->invert_position] = -1 
				- ch->samp->rp_start[ch->invert_position];
			}
		}
	}

LOCAL void set_invert_loop(a, ch)
struct automaton *a;
struct channel *ch;
	{
	LOCAL unsigned char funk_table[] =
		{0, 5, 6, 7, 8, 10, 11, 13, 16, 19, 22, 26, 32, 43, 64, 128};
	if (a->para)
		{
		ch->invert_speed = funk_table[a->para];
		ch->special = do_invert;
		}
	else
		ch->special = do_nothing;
	}


/* initialize the whole effect table */
void init_effects(table)
void (*table[]) P((struct automaton *a, struct channel *ch));
   {
   int i;

   for (i = 0; i < NUMBER_EFFECTS; i++)
      table[i] = set_nothing;
   table[EFF_ARPEGGIO] = set_arpeggio;
   table[EFF_SPEED] = set_speed;
   table[EFF_SKIP] = set_skip;
   table[EFF_FF] = set_fastskip;
   table[EFF_VOLUME] = set_volume;
   table[EFF_VOLSLIDE] = set_slidevol;
   table[EFF_OFFSET] = set_offset;
   table[EFF_PORTA] = set_portamento;
   table[EFF_PORTASLIDE] = set_portaslide;
   table[EFF_UP] = set_upslide;
   table[EFF_DOWN] = set_downslide;
   table[EFF_VIBRATO] = set_vibrato;
   table[EFF_VIBSLIDE] = set_vibratoslide;
   table[EFF_SMOOTH_UP] = set_smooth_up;
   table[EFF_SMOOTH_DOWN] = set_smooth_down;
   table[EFF_CHG_FTUNE] = set_change_finetune;
   table[EFF_LOOP] = set_loop;
   table[EFF_RETRIG] = set_retrig;
   table[EFF_S_UPVOL] = set_smooth_upvolume;
   table[EFF_S_DOWNVOL] = set_smooth_downvolume;
   table[EFF_NOTECUT] = set_note_cut;
   table[EFF_LATESTART] = set_late_start;
   table[EFF_DELAY] = set_delay_pattern;
	table[EFF_TREMOLO] = set_tremolo;
	table[EFF_GLISS_CTRL] = set_gliss_ctrl;
	table[EFF_VIBRATO_WAVE] = set_vibrato_wave;
	table[EFF_TREMOLO_WAVE] = set_tremolo_wave;
	table[EFF_INVERT_LOOP] = set_invert_loop;
   }

