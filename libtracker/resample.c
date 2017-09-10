/* resample.c 
	vi:ts=3 sw=3:
 */

/* $Id: resample.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
 * $Log: resample.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.22  1995/03/01  15:24:51  espie
 * - implemented data width notion.
 * - set what happened is set_position overflowed the sample
 *  -> change behavior to DO_NOTHING from go on repeating.
 *
 * Revision 4.21  1995/02/27  14:24:23  espie
 * Minor bug in dump_delimiter.
 *
 * Revision 4.20  1995/02/23  23:33:01  espie
 * Linear resampling changed.
 *
 * Revision 4.19  1995/02/23  22:40:44  espie
 * Changed call to output_samples, added # of bits.
 *
 * Revision 4.18  1995/02/23  13:49:41  espie
 * Checked optimization -> all the time for oversample == 1 is spent
 * for dividing by oversample in the main loop. Recoded accordingly.
 * Suppressed special case. With the division in leff/right channels
 * from the start, recognized that the inner loop sampling=0..oversample
 * was no longer necessary... just output summed sample once every
 * oversample sample.
 *
 * Revision 4.17  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.16  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.11  1995/02/06  14:50:47  espie
 * Changed sample_info.
 *
 * Revision 4.10  1995/02/01  17:14:54  espie
 * Scaled volume.
 *
 * Revision 4.9  1995/02/01  16:39:04  espie
 * Implemented Left/Right channels, 23 bits output.
 *
 * Revision 4.4  1994/08/23  18:19:46  espie
 * New sampling function.
 * Finally corrected irksome probleme with MAX_PITCH.
 * First changes to augment the number of channels.
 * Added one level of abstraction. Look through player.c for the
 * old high-level functions.
 * Optimized output and fixed up some details.
 * Unrolled code for oversample = 1 to be more efficient at
 * higher frequency (since a high frequency is better than
 * a higher oversample).
 *
 * Revision 2.14  1992/11/06  19:31:53  espie
 * Fixed missing parameter type.
 * fix_xxx for better speed.
 * set_volume.
 * Added possibility to get back to MONO for the sgi.
 * Added stereo capabilities to the indigo version.
 * Minor bug: a SAMPLE_FAULT is a minor error,
 * we should first check that there was no other
 * error before setting it.
 * New resample function coming from the player.
 * Added more notes.
 *
 * Revision 2.1  1991/11/17  23:07:58  espie
 * Just computes some frequency-related parameters.
 *
 *
 */

#include <math.h>

#include "defs.h"
#include "song.h"
#include "channel.h"
#include "tags.h"
#include "extern.h"
     
ID("$Id: resample.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")

/* DO_NOTHING is also used for the automaton */
#define DO_NOTHING 0
#define PLAY 1
#define REPLAY 2


#define MAX_CHANNELS 8

LOCAL struct audio_channel
   {
   struct sample_info *samp;
   int mode;
   unsigned long pointer;
   unsigned long step;
   int volume;
	int scaled_volume;
   int pitch;
	int side;
   } chan[MAX_CHANNELS];

/* left/right */
#define NUMBER_SIDES 2
LOCAL int total[NUMBER_SIDES];
LOCAL int allocated = 0;

struct audio_channel *new_channel_tag_list(prop)
struct tag *prop;
   {
   struct audio_channel *new;

   new = &chan[allocated++];
   new->mode = DO_NOTHING;
   new->pointer = 0;
   new->step = 0;
   new->pitch = 0;
   new->volume = 0;
	new->scaled_volume = 0;
   new->samp = empty_sample();

	while (prop = get_tag(prop))
		{
		switch(prop->type)
			{
		case AUDIO_SIDE:
			new->side = prop->data.scalar;
			break;
		default:
			break;
			}
		prop++;
		}
	total[new->side]++;
   return new;
   }

void release_audio_channels()
   {
   allocated = 0;
   }

/* Have to get some leeway for vibrato (since we don't bound pitch with
 * vibrato). This is conservative.
 */
#define VIB_MAXDEPTH 150


#define C fix_to_int(ch->pointer)

LOCAL unsigned long step_table[REAL_MAX_PITCH + VIB_MAXDEPTH];  
                  /* holds the increment for finding the next sampled
                   * byte at a given pitch (see resample() ).
                   */

/* create a table for converting ``amiga'' pitch
 * to a step rate at a given resampling frequency.
 * For accuracy, we don't use floating point, but
 * instead fixed point ( << ACCURACY).
 * IMPORTANT NOTES:
 * - we need to make it fit within 32 bits (long), which must be enough 
 * for ACCURACY + log2(max sample length)
 * - for linear resampling to work correctly, we need 
 * sample size (8 bit) + volume size (6 bit) + ACCURACY to fit within a
 * long. Appropriate steps will have to be taken when we switch to 16 bit
 * samples...
 * - never forget that samples are SIGNED numbers. If you have unsigned 
 * samples, you have to convert them SOMEWHERE.
 */
LOCAL void create_step_table(oversample, output_fr)
int oversample;     /* we sample oversample i for each byte output */
int output_fr;      /* output frequency */
   {
   double note_fr; /* note frequency (in Hz) */
   double step;
   int pitch;      /* amiga pitch */

		/* special case: oversample of 0 means linear resampling */
	if (oversample == 0)
		oversample = 1;
   step_table[0] = 0;
   for (pitch = 1; pitch < REAL_MAX_PITCH + VIB_MAXDEPTH; pitch++)
      {
      note_fr = AMIGA_CLOCKFREQ / pitch;
         /* int_to_fix(1) is the normalizing factor */
      step = note_fr / output_fr * int_to_fix(1) / oversample;
      step_table[pitch] = (long)step;
      }
   }
         
LOCAL void readjust_pitch()
   {
   int i;
   for (i = 0; i < allocated; i++)
      chan[i].step = step_table[chan[i].pitch];
   }

void init_tables(oversample, frequency)
int oversample, frequency;
   {
   create_step_table(oversample, frequency);
   readjust_pitch();
   }


LOCAL int max_side;		/* number of bits on one side */

LOCAL int max_sample;	/* number of bits for one sample */

void set_data_width(side, sample)
int side, sample;
	{
	max_side = side;
	max_sample = sample;
	}

/* The playing mechanism itself.
 * According to the current channel automaton,
 * we resample the instruments in real time to
 * generate output.
 */
void resample(oversample, number)
int oversample;
int number;
   {
   int i;            /* sample counter */
   int channel;      /* channel counter */
   int sampling;     /* oversample counter */
	int step;         /* fractional part for linear resampling */
   long value[NUMBER_SIDES];
                     /* recombinations of the various data */
   struct audio_channel *ch;

      /* safety check: we can't have a segv there, provided
       * chan points to a valid sample.
       * For `empty' samples, what is needed is fix_length = 0
       * and rp_start = NULL
       */

		/* do the resampling, i.e., actually play sounds */
		/* code unfolding for special cases */
	switch(oversample)
		{
	case 0:	/* linear resampling */
      for (i = 0; i < number; i++) 
         {
			value[LEFT_SIDE] = value[RIGHT_SIDE] = 0;
         for (channel = 0; channel < allocated; channel++)
            {
            ch = chan + channel;
            switch(ch->mode)
               {
            case DO_NOTHING:
               break;
            case PLAY:
                  /* Since we now have fix_length, we can
                   * do that check with improved performance
                   */
               if (ch->pointer < ch->samp->fix_length)
                  {
						step = fractional_part(ch->pointer);
						value[ch->side] += 
							 (ch->samp->start[C] * (total_step - step) +
							   ch->samp->start[C+1] * step)
							 * (ch->scaled_volume);
                  ch->pointer += ch->step;
                  break;
                  }
               else
                  {
                  ch->mode = REPLAY;
                  ch->pointer -= ch->samp->fix_length;
                  /* FALLTHRU */
                  }
            case REPLAY:
                     /* is there a replay ? */
               if (!ch->samp->rp_start)
                  {
                  ch->mode = DO_NOTHING;
                  break;
                  }
               while (ch->pointer >= ch->samp->fix_rp_length)
                  ch->pointer -= ch->samp->fix_rp_length;
					step = fractional_part(ch->pointer);
					value[ch->side] += 
					 	(ch->samp->rp_start[C] * (total_step - step) +
							ch->samp->rp_start[C+1] * step)
						 * ch->scaled_volume ;
               ch->pointer += ch->step;
               break;
               }
            } 
				/* some assembly required... */
         output_samples(value[LEFT_SIDE], value[RIGHT_SIDE], ACCURACY+max_side);
         }
		break;
	default:		/* standard oversampling code */
		value[LEFT_SIDE] = value[RIGHT_SIDE] = 0;
		i = sampling = 0;
		while(TRUE)
         {
         for (channel = 0; channel < allocated; channel++)
				{
				ch = chan + channel;
				switch(ch->mode)
					{
				case DO_NOTHING:
					break;
				case PLAY:
						/* Since we now have fix_length, we can
						 * do that check with improved performance
						 */
					if (ch->pointer < ch->samp->fix_length)
						{
						value[ch->side] += ch->samp->start[C] * ch->scaled_volume;
						ch->pointer += ch->step;
						break;
						}
					else
						{
						ch->mode = REPLAY;
						ch->pointer -= ch->samp->fix_length;
						/* FALLTHRU */
						}
				case REPLAY:
							/* is there a replay ? */
					if (!ch->samp->rp_start)
						{
						ch->mode = DO_NOTHING;
						break;
						}
					while (ch->pointer >= ch->samp->fix_rp_length)
						ch->pointer -= ch->samp->fix_rp_length;
					value[ch->side] += ch->samp->rp_start[C] * ch->scaled_volume;
					ch->pointer += ch->step;
					break;
					}
				}
			if (++sampling >= oversample)
				{
				sampling = 0;
				switch(oversample)
					{
				case 1:
					output_samples(value[LEFT_SIDE],
						value[RIGHT_SIDE], max_side);
					break;
				case 2:
					output_samples(value[LEFT_SIDE], 
						value[RIGHT_SIDE], max_side+1);
					break;
				case 4:
					output_samples(value[LEFT_SIDE],
						value[RIGHT_SIDE], max_side+2);
					break;
				default:
					output_samples(value[LEFT_SIDE]/oversample,
						value[RIGHT_SIDE]/oversample, max_side);
					}
				
				value[LEFT_SIDE] = value[RIGHT_SIDE] = 0;
				if (++i >= number) 
					break;
				}
         }   
      }

   flush_buffer();
   }


/* setting up a given note */
void play_note(au, samp, pitch)
struct audio_channel *au;
struct sample_info *samp;
int pitch;
   {
   au->pointer = 0;
   au->pitch = pitch;
   au->step = step_table[pitch];
	au->samp = samp;
	au->scaled_volume = au->samp->volume_lookup[au->volume];
	au->mode = PLAY;
   }

/* changing the current pitch (value may be temporary, and not stored
 * in channel pitch, for instance for vibratos)
 */
void set_play_pitch(au, pitch)
struct audio_channel *au;
int pitch;
   {
      /* save current pitch in case we want to change
       * the step table on the run
       */
   au->pitch = pitch;
   au->step = step_table[pitch];
   }

/* changing the current volume. You HAVE to get through there so that it 
 * will work on EVERY machine.
 */
void set_play_volume(au, volume)
struct audio_channel *au;
int volume;
   {
   au->volume = volume;
	au->scaled_volume = au->samp->volume_lookup[volume];
   }

void set_play_position(au, pos)
struct audio_channel *au;
int pos;
   {
   au->pointer = int_to_fix(pos);
			/* setting position too far must have this behavior for protracker */
	if (au->pointer >= au->samp->fix_length)
		au->mode = DO_NOTHING;
   }

