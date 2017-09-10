/* amiga/main.c 
	vi:ts=3 sw=3:
 */

/* plays sound/noisetracker files on Sparc, silicon graphics.
 * Authors  : Liam Corner - zenith@dcs.warwick.ac.uk
 *            Marc Espie - espie@ens.fr
 *            Steve Haehnichen - shaehnic@ucsd.edu
 *            Andrew Leahy - alf@st.nepean.uws.edu.au
 *
 * Usage    : tracker <filename> 
 *  this version plays compressed files as well.
 *    Modified version of the standard main.c
 */

/* $Id: main.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: main.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.12  1995/02/14  16:51:22  espie
 * *** empty log message ***
 *
 * Revision 1.11  1995/02/13  22:05:42  espie
 * Added new speedmode option.
 *
 * Revision 1.10  1995/01/13  13:31:35  espie
 * *** empty log message ***
 *
 * Revision 1.12  1994/06/22  00:24:51  Espie
 * Added wb2cli, workbench support !
 * File requester !
 * Uncentralized event handling using event management functions.
 * Suppressed calls to run_in_fg().
 * Added check for / in song names to display something sensible
 * in the title bar...
 * Corrected minor previous song bug.
 * Suppressed spurious code.
 * Added CUT/ADD keywords.
 * Branching to amiga version from unix version 3.20
 */
     

#include <exec/types.h>
#include <exec/memory.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <dos/dosasl.h>
#include <dos/dosextens.h>
#include <exec/libraries.h>
     
#include "defs.h"
#include "version.h"

#include <signal.h>

#include "song.h"
#include "extern.h"

#include "tags.h"
#include "prefs.h"
#include "amiga/amiga.h"
#include "amiga/wb2cli.h"
     
ID("$Id: main.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $")


LOCAL char *amiga_version = "$VER: tracker "VERSION"$";

LOCAL struct MinList temp;

LOCAL void print_usage()
   {
   GENERIC handle;
   
   handle = begin_info("Usage");
   info(handle, "Usage: tracker files OPTIONS");
   info(handle, "HELP               Display usage information");
   info(handle, "QUIET              Print no output other than errors");
   info(handle, "PICKY              Do not tolerate any faults (default is to ignore most)");
   info(handle, "TOLERANT           Ignore all faults");
   info(handle, "VERBOSE            Show text representation of song");
   info(handle, "REPEATS count      Number of repeats (0 is forever) (default 1)");
   info(handle, "SPEED speed        Song speed.  Some songs want 60 (default 50)");
   info(handle, "SPEEDMODE old | normal | finefirst | normalfirst");
   info(handle, "NEW/OLD/BOTH       Select default reading type (default is BOTH)");
   info(handle, "TRANSPOSE n        Transpose notes up by n halftones");
   info(handle, "SCROLL             Show notes scrolling by");
   info(handle, "CUT ab1            Play all instruments except number 1 a and b");
   info(handle, "ADD ab1            Play only instruments except number 1 a and b");
   info(handle, "GUI                open GUI directly");
   end_info(handle);
   }

/* arg parser for command-line options. */

LOCAL char *template = 
"FILES/M,HELP/S,QUIET/S,TOLERANT/S,PICKY/S,NEW/S,OLD/S,BOTH/S,V=VERBOSE/S,\
TRANS=TRANSPOSE/K/N,R=REPEATS/K,SPEED/K/N,START/K,\
CUT/K,ADD/K,SCROLL/S,GUI/S,SPEEDMODE/K";

#define OPT_HELP 1
#define OPT_QUIET 2
#define OPT_TOLERANT 3
#define OPT_PICKY 4
#define OPT_NEW 5
#define OPT_OLD 6
#define OPT_BOTH 7
#define OPT_VERBOSE 8
#define OPT_TRANSPOSE 9
#define OPT_REPEATS 10
#define OPT_SPEED 11
#define OPT_START 12
#define OPT_CUT 13
#define OPT_ADD 14
#define OPT_SHOW 15
#define OPT_GUI 16
#define OPT_SPEEDMODE 17

LOCAL LONG args[19];

LOCAL struct RDArgs *rdargs = 0;

LOCAL void free_args()
   {
   if (rdargs)
      FreeArgs(rdargs);
   }

/* global variable to catch various types of errors
 * and achieve the desired flow of control
 */
int error;

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

LOCAL int start;
LOCAL int transpose;


LOCAL void use_options()
   {
   char *s;
   
   if ((s = args[OPT_CUT]) || (s = args[OPT_ADD]))
      {
      char c;
      ULONG imask = 0;
      
      while (c = *s++)
         {
         if (c >= '1' && c <= '9')
            imask |= 1<< (c-'1');
         else if (c >= 'A' && c <= 'Z')
            imask |= 1 << (c-'A'+9);
         else if (c >= 'a' && c <= 'z')
            imask |= 1 << (c-'a'+9);
         }
      if (args[OPT_CUT])
         set_pref_scalar(PREF_IMASK, imask);
      else
         set_pref_scalar(PREF_IMASK, ~imask);
      }        
   if (args[OPT_OLD])
      set_pref_scalar(PREF_TYPE, OLD);
   if (args[OPT_NEW])
      set_pref_scalar(PREF_TYPE, NEW);
   if (args[OPT_SPEEDMODE])
      {
		if (stricmp(args[OPT_SPEEDMODE], "old") == 0)
			set_pref_scalar(PREF_SPEEDMODE, OLD_SPEEDMODE);
		else if (stricmp(args[OPT_SPEEDMODE], "normal") == 0)
			set_pref_scalar(PREF_SPEEDMODE, NORMAL_SPEEDMODE);
		else if (stricmp(args[OPT_SPEEDMODE], "finefirst") == 0)
			set_pref_scalar(PREF_SPEEDMODE, FINESPEED_ONLY);
		else if (stricmp(args[OPT_SPEEDMODE], "normalfirst") == 0)
			set_pref_scalar(PREF_SPEEDMODE, SPEED_ONLY);
	   }
   if (args[OPT_SHOW])
      set_pref_scalar(PREF_SHOW, TRUE);
   if (args[OPT_BOTH])
      set_pref_scalar(PREF_TYPE, BOTH);
   if (args[OPT_REPEATS])
      set_pref_scalar(PREF_REPEATS, *((ULONG *)args[OPT_REPEATS]));
   if (args[OPT_SPEED])
      set_pref_scalar(PREF_SPEED, *((ULONG *)args[OPT_SPEED]));
   if (args[OPT_TRANSPOSE])
      transpose = *((LONG *)args[OPT_TRANSPOSE]);
   if (args[OPT_PICKY])
      set_pref_scalar(PREF_TOLERATE, 0);
   else if (args[OPT_TOLERANT])
      set_pref_scalar(PREF_TOLERATE, 0);
   if (args[OPT_START])
      start = *((ULONG *)args[OPT_START]);
   if (args[OPT_HELP])
      {
      print_usage();
      end_all(0);
      }
   if (args[OPT_VERBOSE])
      set_pref_scalar(PREF_DUMP, TRUE);
   }

LOCAL struct song *load_song(name)
char *name;
   {
   struct song *song;
   char *buffer;
   int i, j;
   
   i = strlen(name);
   
   for (j = i; j > 0; j--)
      if (name[j] == '/' || name[j] == ':')
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

   switch(get_pref_scalar(PREF_TYPE))
      {
   case BOTH:
      song = do_read_song(name, NEW);
      if (song)
         break;
      /* FALLTHRU */
   case OLD:
      song = do_read_song(name, OLD);
      break;
      /* this is explicitly flagged as a new module,
       * so we don't need to look for a signature.
       */
   case NEW:
      song = do_read_song(name, NEW_NO_CHECK);
      break;
      }
   if (buffer)
      {
      status(0);
      free(buffer);
      }
   return song;
   }


#define PATHSIZE 250

struct MinList *expand_names(char *pat[])
   {
   int i;
   struct AnchorPath *ap;
   int error;
   int total;
   struct amiganame *new;
   
   NewList(&temp);
   ap = AllocVec(sizeof(struct AnchorPath) + PATHSIZE, MEMF_CLEAR);
   if (!ap)
      end_all(0);
   ap->ap_Strlen = PATHSIZE;
   for (i = 0; pat && pat[i]; i++)
      {
      for (error = MatchFirst(pat[i], ap); !error; error = MatchNext(ap))
         {
         total = strlen(ap->ap_Buf) +1 ;
         if (strcmp(ap->ap_Buf + total -6, ".info") == 0)
            continue;
         new = malloc(sizeof(struct amiganame) + total);
         if (!new)
            continue;
         strcpy(new->s, ap->ap_Buf);
         new->i = TRUE;
            AddTail(&temp, new);
         }
      MatchEnd(ap);
      }
   FreeVec(ap);
   return &temp;
   }
      
/* add test for >=37 */

XT struct DosLibrary *DOSBase;      
XT struct WBStartup *_WBenchMsg;

int main(argc, argv)
int argc;
char **argv;
   {
   struct song *song;
   struct MinList *list;
   struct amiganame *element, *i;

   struct tag *result;

   if (DOSBase->dl_lib.lib_Version < 37)
      end_all("Need OS >= 2.04");

   start = 0;
   set_pref_scalar(PREF_IMASK, 0);
   set_pref_scalar(PREF_BCDVOL, 0);
   set_pref_scalar(PREF_DUMP, FALSE);
   set_pref_scalar(PREF_SHOW, FALSE);
   set_pref_scalar(PREF_SYNC, FALSE);
   set_pref_scalar(PREF_TYPE, BOTH);
   set_pref_scalar(PREF_REPEATS, 1);
   set_pref_scalar(PREF_SPEED, 50);
   set_pref_scalar(PREF_TOLERATE, 1);

   if (argc == 1)
      {
      print_usage();
      end_all(0);
      }

   transpose = read_env("TRANSPOSE", 0);


   if (argc > 0)     /* CLI */
      {
      /* check the command name for default reading type */

      rdargs = ReadArgs(template, args, 0);
      if (rdargs)
         at_end(free_args);
      else
         end_all(0);
   
      use_options();
   
      list = expand_names(args[0]);
      }
   else
      (void)WB2CLI(_WBenchMsg, 4000, DOSBase);

again:      
   if (argc == 0 || args[OPT_GUI])
      {
         /* force the interface to appear BEFORE the requester */
      status(0);
      launch_requester();
      list = 0;
      forever
         {
         result = get_ui();
         while (result = get_tag(result))
            {
               /* forgotten from previous versions ! */
            if (result->type == UI_QUIT)
               end_all(0);
            if (result->type == UI_LOAD_SONG)
               {
               i = (struct amiganame *)result->data.pointer;
               if (!i)
                  end_all(0);
               temp.mlh_Tail = 0;
               temp.mlh_Head = i;
               temp.mlh_TailPred = i->n.mln_Pred;
               i->n.mln_Pred->mln_Succ = &(temp.mlh_Tail);
               i->n.mln_Pred = &temp;
               list = &temp;
               break;
               }
            }
         if (list)
            break;
         await_events();
         }
      }
      
   for (element = list->mlh_Head; element->n.mln_Succ; element = element->n.mln_Succ)
      {
      if (!element->i)
         continue;
      song = load_song(element->s);   
      if (song)
         element->i = TRUE;
      else
         {
         char buffer[150];

         sprintf(buffer, "%s is not a song", element->s);
         notice(buffer);
         element->i = FALSE;
         continue;
         }
play_on:
      if (get_pref_scalar(PREF_DUMP))
         dump_song(song); 
      transpose_song(song, transpose);
      setup_audio(0, 1, 1);   /* doesn't really matter on the amiga */
      set_busy_pointer(FALSE);
      result = play_song(song, start);
      set_busy_pointer(TRUE);
      release_song(song);
      while (result = get_tag(result))
         {
         switch (result->type)
            {
         case PLAY_PREVIOUS_SONG:
            for (element = element->n.mln_Pred; element->n.mln_Pred; 
               element = element->n.mln_Pred)
               if (element->i)
                  {
                  song = load_song(element->s);
                  goto play_on;
                  }
            break;
         case PLAY_LOAD_SONG:
               /* splice play load song result into list */
            i = (struct amiganame *)result->data.pointer;
            element->n.mln_Succ->mln_Pred = i->n.mln_Pred;
            i->n.mln_Pred->mln_Succ = element->n.mln_Succ;
            element->n.mln_Succ = i;
            i->n.mln_Pred = element;
         default:
            break;
            }
         result++;
         }
            
      }
   if (argc == 0 || args[OPT_GUI])
      goto again;
   end_all(0);
   /* NOTREACHED */
   }

