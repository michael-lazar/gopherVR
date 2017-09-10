/* st_read.c 
	vi:ts=3 sw=3:
 */

/* $Id: st_read.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: st_read.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.17  1995/03/03  14:24:28  espie
 * 15 colors.
 *
 * Revision 4.16  1995/03/01  15:24:51  espie
 * Stuff with scaling a bit cleaned up (though not perfect yet).
 *
 * Revision 4.15  1995/02/24  15:36:39  espie
 * Need to set all pblock events to 0 for correct clean up.
 *
 * Revision 4.14  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.13  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.11  1995/02/20  22:53:31  espie
 * Added missing parameters everywhere.
 *
 * Revision 4.10  1995/02/20  22:28:50  espie
 * 8 channels.
 *
 * Revision 4.9  1995/02/20  16:49:58  espie
 * Added song type.
 *
 * Revision 4.7  1995/02/06  14:50:47  espie
 * Changed sample_info.
 *
 * Revision 4.6  1995/02/01  17:14:54  espie
 * Scaled volume.
 *
 * Revision 4.0  1994/01/11  17:56:51  espie
 * Use new open.
 * Added lots of checks for malloc, plus count of bytes read.
 * Amiga support.
 * Added other commands (numerous).
 * Checks for finetune ?
 *
 * Revision 2.16  1992/11/17  17:06:25  espie
 * fix_xxx for better speed.
 * Added some sample information in the dump.
 * Added transpose feature.
 * Feature fix: length 1 sample should be empty.
 * Corrected repeat length problems concerning badly formed files,
 * added signature checking for new tracker files.
 * Corrected small problem with repeat being too short.
 * Coded error types. More amiga specific stuff.
 *
 * Revision 1.17  1991/11/17  16:30:48  espie
 * Rationnalized error recovery.
 * There was a bug: you could try to deallocate
 * stuff in no-noland. Also, strings never got
 * to be freed.
 * Centralized error control to error_song.
 * Added a new test on length, aborts most modules now.
 * Maybe should say it as well.
 * Added checkpoints for early return if file too short.
 * Added memory recovery and error control.
 * Suppressed ! warning for bad note.
 * Added note support.
 * Corrected length and rep_length/rep_offset
 * which are given in words and should be converted to
 * bytes.
 */

#include "defs.h"

#include <ctype.h>
#include <assert.h>

#include "extern.h"
#include "song.h"
#include "channel.h"


LOCAL int color;
ID("$Id: st_read.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")

/***
 ***	Low level st-file access routines 
 ***/

#define MAX_LEN 50
/* s = getstring(f, len):
 * gets a soundtracker string from file f.
 * I.e, it is a fixed length string terminated
 * by a 0 if too short. Length should be
 * smaller than MAX_LEN.
 */
LOCAL char *getstring(f, len)
struct exfile *f;
int len;
   {
   static char s[MAX_LEN];
   char *new;
   int i;
        
   assert(len < MAX_LEN);
   for (i = 0; i < len; i++)
      s[MIN(i, MAX_LEN - 1)] = getc_file(f);
   s[MIN(len, MAX_LEN - 1)] = '\0';
   new = malloc(strlen(s)+1);
   if (!new) 
      return NULL;

   return strcpy(new, s);
   }

/* byteskip(f, len)
 * same as fseek, xcpt it works on stdin
 */
LOCAL void byteskip(f, len)
struct exfile *f;
int len;
   {
   int i;

   for (i = 0; i < len; i++)
      getc_file(f);
   }

/* v = getushort(f)
 * reads an unsigned short from f
 */
LOCAL int getushort(f)
struct exfile *f;
   {
   int i;

      /* order dependent !!! */
   i = getc_file(f) << 8;
   return i | getc_file(f);
   }


/* info = fill_sample_info(f):
 * allocate and fill sample info with the information at current position of
 * file f. Allocate memory for storing the sample, also.  fill_sample_info 
 * is guaranteed to give you an accurate snapshot of what sample should 
 * be like. In particular, length, rp_length, start, rp_start, fix_length, 
 * fix_rp_length will have the values you can expect if part of the sample 
 * is missing.
 */
LOCAL struct sample_info *fill_sample_info(f)
struct exfile *f;
   {
	struct sample_info *info;
	int j;

	info = malloc(sizeof(struct sample_info));
	if (!info)
		{
		error = OUT_OF_MEM;
		return NULL;
		}
	info->finetune = 0;
	info->name = NULL;
	info->length = 0;
	info->start = NULL;
	info->sample_size = 8;
	info->rp_start = NULL;
	info->fix_length = 0;
	info->fix_rp_length = 0;

   info->name = getstring(f, SAMPLENAME_MAXLENGTH);
   if (!info->name)
      {
      error = OUT_OF_MEM;
      return NULL;
      }
   info->length = getushort(f);
   info->finetune = getc_file(f);
   if (info->finetune > 15)
      info->finetune = 0;
   info->volume = getc_file(f);
   info->volume = MIN(info->volume, MAX_VOLUME);
   info->rp_offset = getushort(f);
   info->rp_length = getushort(f);

   /* the next check is for old modules for which
    * the sample data types are a bit confused, so
    * that what we were expecting to be #words is #bytes.
    */
      /* not sure I understand the -1 myself, though it's
       * necessary to play kawai-k1 correctly 
       */
   if (info->rp_length + info->rp_offset - 1 > info->length)
      info->rp_offset /= 2;
    
   if (info->rp_length + info->rp_offset > info->length)
      info->rp_length = info->length - info->rp_offset;

   info->length *= 2;
   info->rp_offset *= 2;
   info->rp_length *= 2;
      /* in all logic, a 2-sized sample could exist,
       * but this is not the case, and even so, some
       * trackers output empty instruments as being 2-sized.
       */
   if (info->length <= 2)
      {
		free(info);
		return empty_sample();
      }
   else
      {
			/* add one byte for resampling */
      info->start = (SAMPLE *)alloc_sample(info->length + 1);
      if (!info->start)
         {
         error = OUT_OF_MEM;
         return NULL;
         }

      if (info->rp_length > 2)
         info->rp_start = info->start + info->rp_offset;
      else
         {
         info->rp_start = NULL;
         info->rp_length = 0;
         }
		info->color = color++;
		if (color > 15) color = 1;
      }

   if (info->length > MAX_SAMPLE_LENGTH)
      error = CORRUPT_FILE;
   info->fix_length = int_to_fix(info->length);
   info->fix_rp_length = int_to_fix(info->rp_length);
	return info;
   }

LOCAL void fill_song_info(info, f)
struct song_info *info;
struct exfile *f;
   {
   int i;
   int p;

   info->length = getc_file(f);
   getc_file(f);
   info->maxpat = -1;
   for (i = 0; i < NUMBER_PATTERNS; i++)
      {
      p = getc_file(f);
      if (p >= NUMBER_PATTERNS)
         p = 0;
      if (p > info->maxpat)
         info->maxpat = p;
      info->patnumber[i] = p;
      }
   info->maxpat++;
   if (info->maxpat == 0 || info->length == 0)
      error = CORRUPT_FILE;
   }

LOCAL void fill_event(e, f)
struct event *e;
struct exfile *f;
   {
   int a, b, c, d;

   a = getc_file(f);
   b = getc_file(f);
   c = getc_file(f);
   d = getc_file(f);
   e->sample_number = (a & 0x10) | (c >> 4);
   e->effect = c & 0xf;
   e->parameters = d;
   if (e->effect == EFF_EXTENDED)
      {
      e->effect = EXT_BASE + HI(e->parameters);
      e->parameters = LOW(e->parameters);
      }
   if (e->effect == 0)
      e->effect = e->parameters ? EFF_ARPEGGIO : EFF_NONE;
   if (e->effect == EFF_SKIP)
      e->parameters = HI(e->parameters) * 10 + LOW(e->parameters);
   e->pitch = ( (a & 15) << 8 ) | b;
   e->note = find_note(e->pitch);
   }

LOCAL void fill_pattern(pattern, f, song)
struct block *pattern;
struct exfile *f;
struct song *song;
   {
   int i, j;
	struct event *e;

	e = malloc(sizeof(struct event) * song->info.plength * song->ntracks);
	if (!e)
		{
		error = OUT_OF_MEM;
		return;
		}
	for (j = 0; j < song->ntracks; j++)
		{
		pattern->e[j] = e;
		e += song->info.plength;
		}
   for (i = 0; i < song->info.plength; i++)
      for (j = 0; j < song->ntracks; j++)
         fill_event(&(pattern->e[j][i]), f);
   }


LOCAL void read_sample(info, f)
struct sample_info *info;
struct exfile *f;
   {
   if (info)
      obtain_sample(info->start, info->length, f);
   }




/* new_song: allocate a new structure for a song.
 *  clear each and every field as appropriate.
 */
LOCAL struct song *new_song()
   {
   struct song *new;
	int i;

   new = (struct song *)malloc(sizeof(struct song));
   if (!new) 
      {
      error = OUT_OF_MEM;
      return NULL;
      }
   new->title = NULL;
   new->info.length = 0;
   new->info.maxpat = -1;
   new->info.transpose = 0;
   new->info.pblocks = NULL;
	for (i = 1; i < MAX_NUMBER_SAMPLES; i++)
		new->samples[i] = empty_sample();
   return new;
   }

/* release_song(song): give back all memory occupied by song. Assume 
 * that each structure has been correctly allocated by a call to the
 * corresponding new_XXX function.
 */
void release_song(song)
struct song *song;
   {
   int i;

   if (!song)
      return;
   for (i = 1; i < MAX_NUMBER_SAMPLES; i++)
      {
		if (song->samples[i] != empty_sample())
			{
			if (song->samples[i]->start)
				free_sample(song->samples[i]->start);
			if (song->samples[i]->name)
				free(song->samples[i]->name);
			free(song->samples[i]);
			}
      }
   if (song->info.pblocks)
		{
		for (i = 0; i < song->info.maxpat; i++)
			if (song->info.pblocks[i].e[0])
				free(song->info.pblocks[i].e[0]);
      free(song->info.pblocks);
		}
   if (song->title)
      free(song->title);
   free(song);
   }

/* error_song(song): what we should return if there was an error. 
 * Actually, is mostly useful for its side effects.
 */
LOCAL struct song *error_song(song)
struct song *song;
   {
   release_song(song);
   return NULL;
   }

/* bad_sig(f, song): read the signature on file f and returns TRUE if 
 * it is not a known sig. Set some parameters of song as a side effect
 */
LOCAL int bad_sig(f, song)
struct exfile *f;
struct song *song;
   {
   char a, b, c, d;

   a = getc_file(f);
   b = getc_file(f);
   c = getc_file(f);
   d = getc_file(f);
   if (a == 'M' && b == '.' && c == 'K' && d == '.')
      return FALSE;
   if (a == 'M' && b == '&' && c == 'K' && d == '!')
      return FALSE;
   if (a == 'F' && b == 'L' && c == 'T' && d == '4')
      return FALSE;
	if (a == '6' && b == 'C' && c == 'H' && d == 'N')
		{
		song->ntracks = 6;
		return FALSE;
		}
	if (a == '8' && b == 'C' && c == 'H' && d == 'N')
		{
		song->ntracks= 8;
		return FALSE;
		}
   return TRUE;
   }

LOCAL void adjust_volumes(s)
struct song *s;
	{
	int i, j;
	for (i = 1; i <= s->ninstr; i++)
		for (j = 0; j <= MAX_VOLUME; j++)		/* note <= not a bug */
			s->samples[i]->volume_lookup[j] = (s->ntracks == 6) ? (4 * j) / 3 : j;
	}

	
/* s = read_song(f, type): try to read a song s of type NEW/OLD/NEW_NOCHECK
 * from file f. Might fail, i.e., return NULL if file is not a mod file of 
 * the correct type.
 */
struct song *read_song(f, type)
struct exfile *f;
int type;
   {
   struct song *song;
   int i;

   error = NONE;

   song = new_song();
   if (!song)
      return error_song(song);

	song->ntracks = 4;
   if (type == NEW || type == NEW_NO_CHECK)
		{
		song->type = PROTRACKER;
      song->ninstr = PRO_NUMBER_SAMPLES;
		}
   else
		{
		song->type = OLD_ST;
      song->ninstr = ST_NUMBER_SAMPLES;
		}

	song->ntracks = NORMAL_NTRACKS;
	song->info.plength = NORMAL_PLENGTH;

   song->title = getstring(f, TITLE_MAXLENGTH);
   if (error != NONE)
      return error_song(song);

	color = 1;		/* reset sample color to 1 for every song read */
   for (i = 1; i <= song->ninstr; i++)
      {
		song->samples[i] = fill_sample_info(f);
      if (error != NONE)
         return error_song(song);
      }

   fill_song_info(&song->info, f);

   if (error != NONE)
      return error_song(song);

   if (type == NEW && bad_sig(f, song))
      return error_song(song);

   if (type == NEW_NO_CHECK)
      byteskip(f, 4);
        
  	song->max_sample_width = 8;			/* temporary */

  	if (song->ntracks == 4)
  		song->side_width = song->max_sample_width + 6 + 1;
	else
		song->side_width = song->max_sample_width + 6 + 2;

   song->info.pblocks = (struct block *)
      malloc(sizeof(struct block) * song->info.maxpat);
   if (!song->info.pblocks)
      {
      error = OUT_OF_MEM;
      return error_song(song);
      }
	for (i = 0; i < song->info.maxpat; i++)
		song->info.pblocks[i].e[0] = NULL;
   for (i = 0; i < song->info.maxpat; i++)
      {
      fill_pattern(song->info.pblocks + i, f, song);
      if (error != NONE)
         return error_song(song);
      }
         /* future code... */
   song->samples_start = tell_file(f);

#if 0
   if (feof(f))
      for (i = 1; i <= song->ninstr; i++)
         find_sample(song->samples[i]);
   else
#endif
      for (i = 1; i <= song->ninstr; i++)
         read_sample(song->samples[i], f);
    
 	adjust_volumes(song);
   if (error != NONE)
      return error_song(song);
   return song;
   }

