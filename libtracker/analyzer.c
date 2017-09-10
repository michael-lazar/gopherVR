/* analyzer.c 
	vi:ts=3 sw=3:
*/


/* read module files and output statistics on them */

/* $Id: analyzer.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: analyzer.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.18  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.13  1995/02/20  22:28:50  espie
 * Spurious bug with # of samples.
 *
 * Revision 4.12  1995/02/20  16:49:58  espie
 * Working.
 *
 * Revision 4.9  1995/02/06  14:50:47  espie
 * Changed sample_info.
 *
 * Revision 4.7  1995/02/01  16:39:04  espie
 * Includes moved to defs.h
 *
 * Revision 4.6  1995/01/28  09:23:59  espie
 * Need (?) a return 0 at the end.
 *
 * Revision 4.1  1994/01/12  16:10:20  espie
 * Fixed up last minute problems.
 * Lots of changes.
 * removed create_note_tables(), run_in_fg().
 * Use new pref scheme.
 * New open_file semantics.
 * Added speed check.
 * Added patch for non termio.
 */

#include "defs.h"

#include "extern.h"
#include "song.h"
#include "tags.h"
#include "prefs.h"

ID("$Id: analyzer.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")

int error;


int use_command[16];
int use_extended[16];

void analyze_block(b)
struct block *b;
   {
   int i, j;
   struct event *e;

   for (i = 0; i < BLOCK_LENGTH; i++)
      {
      int special;

      special = 0;
      for (j = 0; j < NUMBER_TRACKS; j++)
         {
         e = &b->e[j][i];
         switch(e->effect)
            {
#if 0
         case 13: /* skip */
            return;
         case 11: /* fastskip */
            return;
#endif
         case 14:
				if (!use_extended[HI(e->parameters)])
					use_extended[HI(e->parameters)] = i+1;
            break;
         case 15:
            if (special != 0 && e->parameters != special)
               putchar('!');
            else
               special = e->parameters;
         default:
				if (!use_command[e->effect])
            use_command[e->effect] = i+1;
            }
         }
      }
   }


void analyze_song(song)
struct song *song;
   {
   int i;

   for (i = 1; i <= song->ninstr ; i++)
      {
      if (song->samples[i])
         {
         if (song->samples[i]->finetune)
            printf("Sample %d: finetune is %d\n", 
               i, song->samples[i]->finetune);
         }
      }
   for (i = 0; i < 16; i++)
      {
      use_command[i] = FALSE;
      use_extended[i] = FALSE;
      }
   for (i = 0; i < song->info.maxpat; i++)
      analyze_block(song->info.pblocks+i);
   for (i = 0; i < 16; i++)
      if (use_command[i])
         printf("%3d", i);
   for (i = 0; i < 16; i++)
      if (use_extended[i])
         printf("%3dE", i);
   printf("\n");
   }

int main(argc, argv)
int argc;
char **argv;
   {
   int i;

	struct exfile *file;
   struct song *song;
   int default_type;

   default_type = BOTH;
   set_pref_scalar(PREF_TOLERATE, 2);

   for (i = 1; i < argc; i++)
      {
		file = open_file(argv[i], "r", getenv("MODPATH"));
		if (file)
			{
			song = read_song(file, NEW);
			if (!song)
				{
				rewind_file(file);
				song = read_song(file, OLD);
				}
			close_file(file);
			if (song)
				{
				puts(argv[i]);
				analyze_song(song);
				release_song(song);
				}
			}
      }
   return 0;
   }



