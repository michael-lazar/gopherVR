/* dump_song.c 
	vi:ts=3 sw=3:
 */

/* $Id: dump_song.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: dump_song.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.16  1995/03/06  23:36:13  espie
 * Proper color patch.
 *
 * Revision 4.15  1995/03/03  14:23:51  espie
 * Color fixed (mostly).
 *
 * Revision 4.14  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.13  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.11  1995/02/20  22:28:50  espie
 * color
 *
 * Revision 4.8  1995/02/06  14:50:47  espie
 * Changed sample_info.
 *
 * Revision 4.7  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.6  1995/02/01  16:39:04  espie
 * Includes moved to defs.h
 *
 * Revision 4.0  1994/01/11  17:46:01  espie
 * Use virtual windows.
 * No more call to run_in_fg(), use begin_info result instead.
 * Added instrument name as shown per display.c.
 * Use info facility.
 * Amiga support.
 * Very small bug with volume (Lawrence).
 * Added finetune display.
 * Added bg/fg test.
 */

#include "defs.h"

#include <ctype.h>

#include "song.h"
#include "extern.h"
#include "channel.h"
#include "prefs.h"

ID("$Id: dump_song.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")

LOCAL void *handle = 0;
LOCAL char buffer[80];

extern char instname[];	/* from display.c */



/***
 ***	dump_block/dump_song:
 ***		show most of the readable info concerning a module on the screen
 ***/

/* THIS NEED SOME UPDATING (FIXED VALUE BLOCK_LENGTH/NUMBER_TRACKS) */
LOCAL void dump_block(b)
struct block *b;
   {
   int i, j;

   for (i = 0; i < BLOCK_LENGTH; i++)
      {
      for (j = 0; j < NUMBER_TRACKS; j++)
         {
         sprintf(buffer,"%8d%5d%2d%4d", b->e[j][i].sample_number,
            b->e[j][i].pitch, b->e[j][i].effect,
            b->e[j][i].parameters);
         infos(handle, buffer);
         }
      info(handle, "");
      }
   }

/* make_readable(s):
 * transform s into a readable string 
 */
LOCAL void make_readable(s)
char *s;
   {
   char *t, *orig;

   if (!s)
      return;

   orig = s;
   t = s;

      /* get rid of the st-xx: junk */
   if (strncmp(s, "st-", 3) == 0 || strncmp(s, "ST-", 3) == 0)
      {
      if (isdigit(s[3]) && isdigit(s[4]) && s[5] == ':')
         s += 6;
      }
   while (*s)
      {
      if (isprint(*s))
         *t++ = *s;
      s++;
      }
   *t = '\0';
   while (t != orig && isspace(t[-1]))
      *--t = '\0';
   }

void dump_song(song)
struct song *song;
   {
   int i, j;
   int maxlen;
   static char dummy[1];

   
   handle = begin_info(song->title);
   if (!handle)
      return;

   dummy[0] = '\0';
   maxlen = 0;
   for (i = 1; i < song->ninstr; i++)
      {
		if (song->samples[i])
			{
			if (!song->samples[i]->name)
				song->samples[i]->name = dummy;
			make_readable(song->samples[i]->name);
			if (maxlen < strlen(song->samples[i]->name))
				maxlen = strlen(song->samples[i]->name);
			}
      }
   for (i = 1; i < song->ninstr; i++)
      {
		if (song->samples[i]) 
			{
			if (song->samples[i]->start || strlen(song->samples[i]->name) > 2)
				{
				static char s[15];
				char *base = s;
				
				if (get_pref_scalar(PREF_COLOR))
					{		
					base = write_color(base, song->samples[i]->color);
					}
				*base++ = instname[i];
				*base++ = ' ';
				*base++ = 0;
				infos(handle, s);
				infos(handle, song->samples[i]->name);
				for (j = strlen(song->samples[i]->name); j < maxlen + 2; j++)
					infos(handle, " ");
				if (song->samples[i]->start)
					{
					sprintf(buffer, "%5d", song->samples[i]->length);
					infos(handle, buffer);
					if (song->samples[i]->rp_length > 2)
						{
						sprintf(buffer, "(%5d %5d)", 
							song->samples[i]->rp_offset, 
							song->samples[i]->rp_length);
						infos(handle, buffer);
						}
					else
						infos(handle, "             ");
					if (song->samples[i]->volume != MAX_VOLUME)
						{
						sprintf(buffer, "%3d", song->samples[i]->volume);
						infos(handle, buffer);
						}
					else 
						infos(handle, "   ");
					if (song->samples[i]->finetune)
						{
						sprintf(buffer, "%3d", song->samples[i]->finetune);
						infos(handle, buffer);
						}
					}
				base = s;
				if (get_pref_scalar(PREF_COLOR))
					base = write_color(base, 0);
				*base = 0;
				info(handle, s);
				}
			}
      }
   end_info(handle);
   handle = 0;
   }
