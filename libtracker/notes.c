/* notes.c 
	vi:ts=3 sw=3:
 */

/* $Id: notes.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: notes.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.13  1995/03/01  15:24:51  espie
 * *** empty log message ***
 *
 * Revision 4.12  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.9  1995/02/20  22:28:50  espie
 * bug in nearest_note.
 *
 * Revision 4.8  1995/02/20  16:49:58  espie
 * Added nearest_note: round pitch to nearest entire note.
 *
 * Revision 4.5  1995/02/01  16:39:04  espie
 * Moved includes to defs.h
 *
 * Revision 4.0  1994/01/11  17:50:04  espie
 * Makes use of autoinit. Uses less memory, starts up faster.
 * auto_init'd create_notes_table(),
 * suppressed note_name static table,
 * use name_of_note() instead (about 120 * 8 bytes gain).
 * Amiga support.
 * Added finetune.
 */

#include "defs.h"

#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "song.h"
#include "channel.h"
#include "extern.h"

ID("$Id: notes.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")


/* we can put it autoinit since find_note is ALWAYS called
 * prior to using pitch_table !
 */
LOCAL void create_notes_table P((void));
LOCAL void (*INIT)P((void)) = create_notes_table;


/* the musical notes correspond to some specific pitch.
 * It's useful to be able to find them back, at least for
 * arpeggii.
 */
short pitch_table[NUMBER_NOTES][NUMBER_FINETUNES];

LOCAL char *note_template = "C-C#D-D#E-F-F#G-G#A-A#B-";

/* note = find_note(pitch): 
 * find note corresponding to the stated pitch 
 */
int find_note(pitch)
int pitch;
   {
   int a, b, i;
   
   INIT_ONCE;

   if (pitch == 0)
      return -1;
   a = 0;
   b = NUMBER_NOTES-1;
   while(b-a > 1)
      {
      i = (a+b)/2;
      if (pitch_table[i][0] == pitch)
         return i;
      if (pitch_table[i][0] > pitch)
         a = i;
      else
         b = i;
      }
   if (pitch_table[a][0] - FUZZ <= pitch)
      return a;
   if (pitch_table[b][0] + FUZZ >= pitch)
      return b;
   return NO_NOTE;
   }

/* pitch = nearest_note(pitch, finetune):
 * return the pitch corresponding to the nearest note for the given
 * finetune
 */
int nearest_note(pitch, finetune)
int pitch;
int finetune;
	{
   int a, b, i;
   
   INIT_ONCE;

   if (pitch == 0)
      return -1;
   a = 0;
   b = NUMBER_NOTES-1;
   while(b-a > 1)
      {
      i = (a+b)/2;
      if (pitch_table[i][finetune] == pitch)
         return pitch;
      if (pitch_table[i][finetune] > pitch)
         a = i;
      else
         b = i;
      }
			/* need some check for the actual nearest note ? */
	return pitch_table[i][finetune];
   }


LOCAL void create_notes_table()
   {
   double base, pitch;
   int i, j, k;

   for (j = -8; j < 8; j++)
      {
      k = j < 0 ? j + 16 : j;
      base = AMIGA_CLOCKFREQ/440.0/4.0 / pow(2.0, j/96.0);

      for (i = 0; i < NUMBER_NOTES; i++)
         {
         pitch = base / pow(2.0, i/12.0);
         pitch_table[i][k] = floor(pitch + 0.5);
         }
      }
    }

char *name_of_note(i)
int i;
   {
   static char name[4];

   if (i == NO_NOTE)
      return "   ";
   else 
      {
      name[0] = note_template[(i+9)%12 * 2];
      name[1] = note_template[(i+9)%12 * 2 +1];
      name[2] = '0' + (i-3)/12;
      name[3] = 0;
      return name;
      }
   }
   
int transpose_song(s, transpose)
struct song *s;
int transpose;
   {
   int oldt;
   int i, j, n;

   if (!s)
      return 0;
   oldt = s->info.transpose;
   for (n = 0; n < s->info.maxpat; n++)
      for (i = 0; i < s->info.plength; i++)
         for (j = 0; j < s->ntracks; j++)
            if (s->info.pblocks[n].e[j][i].note != NO_NOTE)
               s->info.pblocks[n].e[j][i].note += transpose - oldt;
   s->info.transpose = transpose;
   return oldt;
   }
