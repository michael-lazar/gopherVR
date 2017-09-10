/* st_play.c 
	vi:ts=3 sw=3:
 */

/* $Id: st_play.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: st_play.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.19  1995/03/11  21:41:01  espie
 * Added invert_loop.
 *
 * Revision 4.18  1995/03/04  00:16:21  espie
 * implemented vibrato wave.
 *
 * Revision 4.17  1995/03/03  14:24:21  espie
 * *** empty log message ***
 *
 * Revision 4.16  1995/03/01  15:24:51  espie
 * data width.
 *
 * Revision 4.15  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.14  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.12  1995/02/20  22:53:31  espie
 * Some display updated.
 *
 * Revision 4.11  1995/02/20  22:28:50  espie
 * 8 channels.
 *
 * Revision 4.10  1995/02/20  16:49:58  espie
 * Added funk_glissando
 *
 * Revision 4.9  1995/02/14  04:02:28  espie
 * Suppressed comment.
 *
 * Revision 4.8  1995/02/06  14:50:47  espie
 * Changed sample_info.
 *
 * Revision 4.7  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.6  1995/02/01  16:39:04  espie
 * Left/Right channels.
 *
 * Revision 4.0  1994/01/11  17:51:40  espie
 * Use the new UI calls.
 * Use the new pref settings.
 *
 * Revision 1.14  1994/01/09  23:24:37  Espie
 * Last bug fix.
 * Suppressed really outdated code.
 * New names: new_channel_tag_list, release_audio_channel.
 * Some notice to status.
 * Use new pref scheme.
 * Use get_ui
 * Use autoinit feature of display.c
 * discard_buffer forgotten...
 * Handle errors better.
 *
 * Revision 3.18  1993/12/04  16:12:50  espie
 * Lots of changes.
 * New high-level functions.
 * Amiga support.
 * Bug with delay_pattern: can't factorize the check for effect thingy.
 * Reniced verbose output display.
 * Bug fix: now use correct finetune when loading samples/starting notes.
 * Added bg/fg test.
 * General cleanup
 * Added <> operators.
 * Added update frequency on the fly.
 * Added finetune.
 * Protracker commands.
 *
 * Revision 2.19  1992/11/17  17:15:37  espie
 * Added interface using may_getchar(). Still primitive, though.
 * imask, start.
 * Added transpose feature.
 * Added possibility to get back to MONO for the sgi.
 * Added stereo capabilities to the indigo version.
 * Added two level of fault tolerancy.
 * Added some control on the number of replays,
 * and better error recovery.
 */
     
#include "defs.h"
#include "song.h"
#include "channel.h"
#include "extern.h"
#include "tags.h"
#include "prefs.h"
     

extern int vibrato_table[3][64];

ID("$Id: st_play.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")
     

/* setting up a given note */
void reset_note(ch, note, pitch)
struct channel *ch;
int note;
int pitch;
   {
   ch->pitch = pitch;
   ch->note = note;
   ch->viboffset = 0;
	ch->tremoffset = 0;
   play_note(ch->audio, ch->samp, pitch);
   }

/* changing the current pitch (value may be temporary, and so is not stored
 * in channel pitch, for instance vibratos)
 */
void set_current_pitch(ch, pitch)
struct channel *ch;
int pitch;
   {
   set_play_pitch(ch->audio, pitch);
   }

/* changing the current volume, storing it in ch->volume
 */
void set_current_volume(ch, volume)
struct channel *ch;
int volume;
   {
   ch->volume = MAX(MIN(volume, MAX_VOLUME), MIN_VOLUME);
   set_play_volume(ch->audio, ch->volume);
   }

/* changing the current volume WITHOUT storing it
 */
void set_temp_volume(ch, volume)
struct channel *ch;
int volume;
	{
	volume = MAX(MIN(volume, MAX_VOLUME), MIN_VOLUME);
	set_play_volume(ch->audio, volume);
	}

void set_position(ch, pos)
struct channel *ch;
int pos;
   {
   set_play_position(ch->audio, pos);
   }

/* init_channel(ch, dummy):
 * setup channel, with initially a dummy sample ready to play, and no note.
 */
LOCAL void init_channel(ch, side)
struct channel *ch;
int side;
   {
	struct tag tags[2];
	tags[0].type = AUDIO_SIDE;
	tags[0].data.scalar = side;
	tags[1].type = TAG_END;
   ch->samp = empty_sample();
   ch->finetune = 0;
   ch->audio = new_channel_tag_list(tags);
   ch->volume = 0; 
   ch->pitch = 0; 
   ch->note = NO_NOTE;

      /* we don't setup arpeggio values. */
   ch->viboffset = 0; 
   ch->vibdepth = 0;
   ch->vibrate = 0;
	ch->vibtable = vibrato_table[0];
	ch->resetvib = FALSE;

	ch->tremoffset = 0;
	ch->tremdepth = 0;
	ch->tremrate = 0;
	ch->tremtable = vibrato_table[0];
	ch->resettrem = FALSE;

   ch->slide = 0; 

   ch->pitchgoal = 0; 
   ch->pitchrate = 0;

   ch->volumerate = 0;

	
	ch->funk_glissando = FALSE;
	ch->start_offset = 0;
   ch->adjust = do_nothing;
	ch->loop_counter = -1;

   ch->special = do_nothing;
	ch->invert_speed = 0;
	ch->invert_offset = 0;
	ch->invert_position = 0;
   }



LOCAL int VSYNC;        /* base number of sample to output */
LOCAL void (*eval[NUMBER_EFFECTS]) P((struct automaton *a, struct channel *ch));
                    		/* the effect table */
LOCAL int oversample;   /* oversample value */
LOCAL int frequency;    /* output frequency */

LOCAL struct channel chan[MAX_TRACKS];
                    /* every channel */

LOCAL int ntracks;		/* number of tracks of the current song */
LOCAL struct sample_info **voices;

LOCAL struct automaton a;


void init_player(o, f)
int o, f;
   {
   oversample = o;
   frequency = f;
   init_tables(o, f);
   init_effects(eval);
   }

LOCAL void setup_effect(ch, a, e)
struct channel *ch;
struct automaton *a;
struct event *e;
   {
   int samp, cmd;

      /* retrieves all the parameters */
   samp = e->sample_number;

      /* load new instrument */
   if (samp)  
      {  /* note that we can change sample in the middle of a note. This 
			 * is a *feature*, not a bug (see made). Precisely: the sample 
			 * change will be taken into account for the next note, BUT the 
			 * volume change takes effect immediately.
          */
      ch->samp = voices[samp];
		ch->finetune = voices[samp]->finetune;
		if ((1L<<samp) & get_pref_scalar(PREF_IMASK))
			ch->samp = empty_sample();
		set_current_volume(ch, voices[samp]->volume);
      }

   a->note = e->note;
   if (a->note != NO_NOTE)
      a->pitch = pitch_table[a->note][ch->finetune];
   else
      a->pitch = e->pitch;
   cmd = e->effect;
   a->para = e->parameters;

   if (a->pitch >= REAL_MAX_PITCH)
      {
      char buffer[60];

      sprintf(buffer,"Pitch out of bounds %d", a->pitch);
      status(buffer);
      a->pitch = 0;
      error = FAULT;
      }

      /* check for a new note: portamento
       * is the special case where we do not restart
       * the note.
       */
   if (a->pitch && cmd != EFF_PORTA && cmd != EFF_PORTASLIDE)
      reset_note(ch, a->note, a->pitch);
   ch->adjust = do_nothing;
      /* do effects */
   (eval[cmd])(a, ch);
   }


LOCAL void adjust_sync(ofreq, tempo)
int ofreq, tempo;
   {
   VSYNC = ofreq * NORMAL_FINESPEED / tempo;
   }

LOCAL void dump_events(a)
struct automaton *a;
	{
		/* display the output in a reasonable order:
		 * LEFT1 LEFT2 || RIGHT1 RIGHT 2
		 */
	dump_event(chan, &(a->pattern->e[0][a->note_num]));
	dump_event(chan+3, &(a->pattern->e[3][a->note_num]));
	if (ntracks > 4)
		dump_event(chan+4, &(a->pattern->e[4][a->note_num]));
	if (ntracks > 7)
		dump_event(chan+7, &(a->pattern->e[7][a->note_num]));
	dump_delimiter();
	dump_event(chan+1, &(a->pattern->e[1][a->note_num]));
	dump_event(chan+2, &(a->pattern->e[2][a->note_num]));
	if (ntracks > 5)
		dump_event(chan+5, &(a->pattern->e[5][a->note_num]));
	if (ntracks > 6)
		dump_event(chan+6, &(a->pattern->e[6][a->note_num]));
	dump_event(0, 0);
	}


LOCAL void play_once(a)
struct automaton *a;
   {
   int channel;

   if (a->do_stuff & DELAY_PATTERN)
      for (channel = 0; channel < ntracks; channel++)
         /* do the effects */
			{
         (chan[channel].special)(chan + channel);
         (chan[channel].adjust)(chan + channel);
			}
   else
      {  
      if (a->counter == 0)
         {
         for (channel = 0; channel < ntracks; channel++)
            /* setup effects */
            setup_effect(chan + channel, a, 
               &(a->pattern->e[channel][a->note_num]));
			if (get_pref_scalar(PREF_SHOW))
				dump_events(a);
         }
      else
         for (channel = 0; channel < ntracks; channel++)
				{
            /* do the effects */
				(chan[channel].special)(chan + channel);
            (chan[channel].adjust)(chan + channel);
				}
      }

      /* advance player for the next tick */
   next_tick(a);
      /* actually output samples */
   resample(oversample, VSYNC / a->finespeed);
   }

LOCAL struct tag pres[2];


LOCAL void init_channels()
	{
   release_audio_channels();

	init_channel(chan, LEFT_SIDE);
	init_channel(chan + 1, RIGHT_SIDE);
	init_channel(chan + 2, RIGHT_SIDE);
	init_channel(chan + 3, LEFT_SIDE);
	if (ntracks > 4)
		{
		init_channel(chan + 4, LEFT_SIDE);
		init_channel(chan + 5, RIGHT_SIDE);
		}
	if (ntracks > 6)
		{
		init_channel(chan + 6, RIGHT_SIDE);
		init_channel(chan + 7, LEFT_SIDE);
		}
	}

struct tag *play_song(song, start)
struct song *song;
int start;
   {
   int tempo;
   int countdown;      /* keep playing the tune or not */

   song_title(song->title);
   pres[1].type = TAG_END;
   
	ntracks = song->ntracks;
	set_number_tracks(ntracks);
   tempo = get_pref_scalar(PREF_SPEED);

   adjust_sync(frequency, tempo);
    /* a repeats of 0 is infinite replays */
   
   countdown = get_pref_scalar(PREF_REPEATS);
   if (countdown == 0)
      countdown = 50000;   /* ridiculously huge number */

   voices = song->samples; 

   init_automaton(&a, song, start);

	init_channels();

	set_data_width(song->side_width, song->max_sample_width);

   while(countdown)
      {
      struct tag *result;
      
      play_once(&a);
      result = get_ui();
      while(result = get_tag(result))
         {
         switch(result->type)
            {  
         case UI_LOAD_SONG:
            if (!result->data.pointer)
               break;
         case UI_NEXT_SONG:
         case UI_PREVIOUS_SONG:
            discard_buffer();
            pres[0].type = result->type;
            pres[0].data = result->data;
            return pres;
         case UI_QUIT:
            discard_buffer();
            end_all(0);
            /* NOTREACHED */
         case UI_SET_BPM:
            tempo = result->data.scalar;
            adjust_sync(frequency, tempo);
            break;
         case UI_RESTART:
            discard_buffer();
            init_automaton(&a, song, start);
				init_channels();
            break;
         case UI_JUMP_TO_PATTERN:
            if (result->data.scalar >= 0 && 
						result->data.scalar < a.info->length)
               {
               discard_buffer();
               init_automaton(&a, song, result->data.scalar);
               }
            break;
            /*
         case ' ':
            while (may_getchar() == EOF)
               ;
            break;
             */
         default:
            break;
            }
         result++;
         }

      {
      int new_freq;
      if (new_freq = update_frequency())
         {
         frequency = new_freq;
         adjust_sync(frequency, tempo);
         init_tables(oversample, frequency);
         }
      }

      switch(error)
         {
      case NONE:
         break;
      case ENDED:
         countdown--;
         break;
      case SAMPLE_FAULT:
      case FAULT:
      case PREVIOUS_SONG:
      case NEXT_SONG:
      case UNRECOVERABLE:
         if ( (error == SAMPLE_FAULT && get_pref_scalar(PREF_TOLERATE))
            ||(error == FAULT && get_pref_scalar(PREF_TOLERATE) > 1) )
            break;
         pres[0].type = PLAY_ERROR;
         pres[0].data.scalar = error;
         return pres;
      default:
         break;
         }
         error = NONE;
      }
   pres[0].type = TAG_IGNORE;      
   return pres;
   }

