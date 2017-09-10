/*
 * API for libtracker calls...
 */

/* global variable to catch various types of errors
 * and achieve the desired flow of control
 */
int error;


#include "defs.h"

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#ifdef MALLOC_NOT_IN_STDLIB
#include <malloc.h>
#endif
#include <ctype.h>
#ifdef VOLUME_CONTROL
#ifdef __hpux
#define true /* kludge to avoid typedef of boolean (name clash with macro) */
#include <audio/Alib.h>
#undef true
AGainDB	volume = -20;
char use_speaker = 0;
#endif
#endif

     
#include "song.h"
#include "extern.h"
#include "options.h"

#include "getlongopt.h"
#include "tags.h"
#include "prefs.h"

LOCAL struct song *do_read_song(name, type)
char *name;
int type;
   {
   struct song *song;
   struct exfile *file;

   file = open_file(name, "r", getenv("MODPATH"));
   if (!file)
      return NULL;
   song = read_song(file, type); 
   close_file(file);
   return song;
   }


LOCAL struct song *load_song(name)
char *name;
{
     struct song *song;
     char *buffer;
     int i, j;
     
     i = strlen(name);
     
     for (j = i; j > 0; j--)
	  if (name[j] == '/' || name[j] == '\\')
	  {
	       j++;
	       break;
	  }
     
     buffer = malloc( i - j + 5);
     if (buffer)
     {
	  sprintf(buffer, "%s...", name + j);
	  status(buffer);
     }
     
     
     song = do_read_song(name, NEW);
     return song;
}


/*
 * Play a song...
 */

void TrackerPlay(char *filename) {
     struct song *song;
     struct tag  *result;


     set_pref_scalar(PREF_IMASK, 0);
     set_pref_scalar(PREF_BCDVOL, 0);
     set_pref_scalar(PREF_DUMP, FALSE);
     set_pref_scalar(PREF_SHOW, FALSE);
     set_pref_scalar(PREF_SYNC, FALSE);
     set_pref_scalar(PREF_TYPE, BOTH);
     set_pref_scalar(PREF_REPEATS, 1);
     set_pref_scalar(PREF_SPEED, 50);
     set_pref_scalar(PREF_TOLERATE, 1);
     
     
     song = load_song(filename);
     setup_audio(0, 0, 1);
     /*     tag = play_song(song, 0);*/
     while (1)
	  (void)  play_song(song, 0);

     release_song(song);
}
