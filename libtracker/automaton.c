/* automaton.c 
	vi:ts=3 sw=3:
 */

/* $Id: automaton.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: automaton.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.16  1995/03/11  21:40:02  espie
 * Modified jump pattern to work...
 *
 * Revision 4.15  1995/03/03  14:23:28  espie
 * Spurious bug fixed.
 *
 * Revision 4.14  1995/03/01  15:24:51  espie
 * Block length was hardcoded.
 *
 * Revision 4.13  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.12  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.6  1995/02/01  16:39:04  espie
 * Includes moved to defs.h
 *
 * Revision 4.2  1994/08/23  18:19:46  espie
 * Added speedmode option
 * Abstracted IO calls.
 * Use display_pattern.
 * Fixed up repeat code, should work better now.
 * Fixed bug with bad loops.
 * Modified the way set_speed works.
 * Corrected stupid bug (run_in_fg)
 * Added bg/fg test.
 * General cleanup
 * Added finetune.
 * Protracker commands.
 *
 * Revision 2.16  1992/11/17  17:15:37  espie
 * New output for new interface
 * Modified repeat logic: now works irregardless of repeat points.
 * start
 *
 * Revision 2.8  1992/07/14  14:23:41  espie
 * Changed fine speed command and comments.
 * Added two level of fault tolerancy.
 */
     

     
#include "defs.h"
#include "song.h"
#include "channel.h"
#include "extern.h"
#include "prefs.h"
     
ID("$Id: automaton.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")
     

/* set up the automaton so that I haven't got through patterns 
 * #from to #to
 */
LOCAL void clear_repeats(a, from, upto)
struct automaton *a;
int from, upto;
   {
   int i;

   for (i = from; i <= upto; i++)
      a->gonethrough[i] = FALSE;
   }

/* set up the automaton so that I haven't got through any patterns
 */
LOCAL void reset_repeats(a)
struct automaton *a;
   {
   clear_repeats(a, 0, a->info->length);
   a->gonethrough[a->info->length] = TRUE;
   }

/* update the pattern to play in the automaton. Checks that the pattern 
 * actually exists. Handle repetitions as well.
 */
LOCAL void set_pattern(a)
struct automaton *a;
   {
   int p;


   if (a->pattern_num >= a->info->length)
      {
      error = UNRECOVERABLE;
      return;
      }

   if (a->gonethrough[a->pattern_num])
      {
      error = ENDED;
      reset_repeats(a);
      }
   else
      a->gonethrough[a->pattern_num] = TRUE;

      /* there is a level of indirection in the format,
       * i.e., patterns can be repeated.
       */
   p = a->info->patnumber[a->pattern_num];
   if (p >= a->info->maxpat)
      {
      error = UNRECOVERABLE;
      return;
      }

   display_pattern(a->pattern_num, a->info->length, p);

   a->pattern = a->info->pblocks + p;
   }

/* initialize all the fields of the automaton necessary
 * to play a given song.
 */
void init_automaton(a, song, start)
struct automaton *a;
struct song *song;
int start;
   {
   a->info = &song->info;
   a->pattern_num = start;    /* first pattern */

   a->loop_note_num = 0;
   a->loop_counter = 0;

   reset_repeats(a);

   a->note_num = 0;           /* first note in pattern */
   a->counter = 0;            /* counter for the effect tempo */
   a->speed = NORMAL_SPEED;   /* this is the default effect tempo */
   a->finespeed = NORMAL_FINESPEED;    /* this is the fine speed 
										 * (100%=NORMAL_FINESPEED) */
   a->do_stuff = DO_NOTHING;  /* some effects affect the automaton,
                               * we keep them here.  */
   error = NONE;              /* Maybe we should not reset errors at
                               * this point ?  */
   set_pattern(a);
   }

/* get to the next pattern, and display stuff 
 */
LOCAL void advance_pattern(a)
struct automaton *a;
   {
   if (++a->pattern_num >= a->info->length)
		{
		error = ENDED;
		reset_repeats(a);
      a->pattern_num = 0;
		}
   set_pattern(a);
   a->note_num = 0;
   }

        

/* process all the stuff which we need to advance in the song,
 * including set_speed, set_skip, set_fastskip, and set_loop.
 */
void next_tick(a)
struct automaton *a;
   {
      /* there are three classes of speed changes:
       * 0 does nothing. (should stop for genuine protracker)
       * <32 is the effect speed (resets the fine speed).
       * >=32 changes the finespeed, default 125
       */
	if (a->do_stuff & (SET_SPEED | SET_FINESPEED) == SET_SPEED | SET_FINESPEED)
		switch(get_pref_scalar(PREF_SPEEDMODE))
			{
		case FINESPEED_ONLY:
			a->do_stuff &= ~SET_SPEED;
			break;
		case SPEED_ONLY:
			a->do_stuff &= ~SET_FINESPEED;
		default:
			break;
			}
		
   if ((a->do_stuff & SET_SPEED) && (a->do_stuff & SET_FINESPEED))
      {
      a->speed = a->new_speed;
      a->finespeed = a->new_finespeed; 
      }
   else if (a->do_stuff & SET_FINESPEED)
      {
      a->finespeed = a->new_finespeed;
      }
   else if (a->do_stuff & SET_SPEED)
      {
      a->speed = a->new_speed;
      a->finespeed = NORMAL_FINESPEED;
      }

   if (++a->counter >= a->speed)
      {
      a->counter = 0;
         /* loop: may change note in pattern right away */
      if (a->do_stuff & JUMP_PATTERN)
         a->note_num = a->loop_note_num;
      else if (a->do_stuff & SET_FASTSKIP)
         {
         a->pattern_num = a->new_pattern;
         set_pattern(a);
         a->note_num = 0;
         }
      else if (a->do_stuff & SET_SKIP)
         {
         advance_pattern(a);
         a->note_num = a->new_note;
         }
      else
         {
         if (++a->note_num >= a->info->plength)
            advance_pattern(a);
         }
      a->do_stuff = DO_NOTHING;
      }
   }


