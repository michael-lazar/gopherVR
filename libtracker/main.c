/* main.c 
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
 */

/* updated for GopherVR by Cameron Kaiser: stricmp -> strcasecmp */

/* $Id: main.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: main.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.22  1995/03/11  23:06:33  espie
 * version changed.
 *
 * Revision 4.21  1995/03/06  22:35:38  espie
 * Color can be default.
 *
 * Revision 4.20  1995/03/01  15:24:51  espie
 * options -half/-double.
 *
 * Revision 4.19  1995/02/24  15:36:39  espie
 * Case forgotten: if file does not open, song is not set.
 *
 * Revision 4.18  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.17  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.13  1995/02/15  14:30:09  espie
 * optarg being NULL not checked -> segmentation.
 *
 * Revision 4.11  1995/02/14  04:02:28  espie
 * Added version number.
 *
 * Revision 4.9  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.8  1995/02/01  16:39:04  espie
 * Loading formats thru rewind.
 *
 * Revision 4.5  1994/08/23  18:19:46  espie
 * Added speed mode option.
 * Added looping option.
 * Nice OPT_CUT/OPT_ADD.
 * Slight input/output changes.
 * No more call to create_notes_table().
 * Some notice to status.
 * Use new pref scheme.
 * Use info facility instead of printf for usage message.
 * Options changes.
 * Changed extended file semantics.
 * Amiga support.
 * Fixed upo previous song bug.
 * Added bg/fg test.
 * Added loads of new options.
 * Added finetune.
 *
 * Revision 2.20  1992/11/17  17:06:25  espie
 * Added PREVIOUS_SONG handling ???
 * Use streamio for new interface (obsolescent signal handlers), and
 * related changes.
 * Cleaned up path reader, and better signal handling.
 * Support for open_file.
 * Added imask.
 * Use transparent decompression/path lookup through open_file/close_file.
 * Added setup_audio().
 * Added some frequency/oversample/stereo change on the fly.
 * Necessitates rightful closing/reopening of audio.
 * Added compression methods. Changed getopt.
 * Separated mix/stereo stuff.
 * Added transpose feature.
 * Added possibility to get back to MONO for the sgi.
 * Added stereo capabilities to the indigo version.
 * Added recovery and reread for automatic recognition
 * of old/new tracker files.
 * Added two level of fault tolerancy.
 * Added more rational options.
 * Moved almost everything to audio and automaton.
 * Structured part of the code, especially replay ``automaton''
 * and setting up of effects.
 *
 * Revision 1.26  1991/11/17  17:09:53  espie
 * Added missing prototypes.
 * Some more info while loading files.
 * Added FAULT env variable, FAULT resistant playing,
 * for playing modules which are not quite correct.
 * Serious bug: dochangespeed was not reset all the time.
 * Check all these parameters, they MUST be reset for
 * each new song.
 * Fixed a stupid bug: when env variable LOOPING was
 * undefined, we got a segv on strcmp.
 * Now we just test for its existence, since this is
 * about all we want...
 * Bug correction: when doing arpeggio, there might not
 * be a new note, so we have to save the old note value
 * and do the arppeggio on that note.
 * Completely added control with OVERSAMPLE and FREQUENCY.
 * Added control flow.
 * Added pipe decompression, so that now you can do
 * str32 file.Z directly.
 * stdin may go away.
 * Added arpeggio.
 * Added vibslide and portaslide.
 * Added speed command.
 * Added signal control.
 * Error checking: there shouldn't be that many
 * segv signals any more.
 * Moved every command to commands.c.
 * Added some debug code for showing the full
 * sequence for a file.
 * Corrected the bug in volume slide: there is
 * no default value, i.e., if it is 0, it is 0,
 * as stupid as it may seem.
 * Added vibrato.
 * Added fastskip/corrected skip.
 * Modified control flow of the player till
 * it looks like something reasonable (i.e.,
 * the structure is more natural and reflects
 * the way stuff is played actually...)
 * Do not restart the sound when we change instruments
 * on the fly. A bit strange, but it works that way.
 * Modified main to use new data structures.
 * The sound player is MUCH cleaner, it uses now
 * a 3-state automaton for each voice.
 * Corrected ruckus with data type of sample.
 */
     

#include "defs.h"

extern char *VERSION;

#include <signal.h>
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
     
ID("$Id: main.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")


LOCAL void print_usage()
   {
   GENERIC handle;
   handle = begin_info("Usage");
   infos(handle, "This is tracker ");
	info(handle, VERSION);
	info(handle, "This program is NOT to be redistributed");
	info(handle, "         without the full documentation");
	info(handle, "");
   info(handle, "Usage: tracker [options] filename [...]");
   info(handle, "-help               Display usage information");
   info(handle, "-quiet              Print no output other than errors");
   info(handle, "-picky              Do not tolerate any faults (default is to ignore most)");
   info(handle, "-tolerant           Ignore all faults");
   info(handle, "-mono               Select single audio channel output");
   info(handle, "-stereo             Select dual audio channel output");
   info(handle, "-verbose            Show text representation of song");
   info(handle, "-repeats <count>    Number of repeats (0 is forever) (default 1)");
	info(handle, "-loop               Loops the song list (plays again and again)");
   info(handle, "-speed <speed>      Song speed.  Some songs want 60 (default 50)");
   info(handle, "-mix <percent>      Percent of channel mixing. (0 = spatial, 100 = mono)");
   info(handle, "-new -old -both     Select default reading type (default is -both)");
   info(handle, "-frequency <freq>   Set playback frequency in Hz");
   info(handle, "-oversample <times> Set oversampling factor");
   info(handle, "-transpose <n>      Transpose all notes up");
   info(handle, "-scroll             Show what's going on");
   info(handle, "-color              Ansi color scrolling");
   info(handle, "-sync               Try to synch audio output with display");
#ifdef VOLUME_CONTROL
	info(handle, "-speaker				 Output audio to internal speaker");
	info(handle, "-volume <n>         Set volume in dB");
#endif
   info(handle, "");
   info(handle, "RunTime:");
   info(handle, "e,x     exit program");
   info(handle, "n       next song");
   info(handle, "p       restart/previous song");
   info(handle, ">       fast forward");
   info(handle, "<       rewind");
   info(handle, "S       NTSC tempo\t s\tPAL tempo");
   end_info(handle);
   }

/* Command-line options. */
LOCAL struct long_option long_options[] =
{
   {"help",                0, 'H', OPT_HELP},
   {"quiet",               0, 'Q', OPT_QUIET}, 
   {"picky",               0, 'P', OPT_PICKY},
   {"tolerant",            0, 'L', OPT_TOLERANT},
   {"new",                 0, 'N', OPT_NEWONLY},
   {"old",                 0, 'O', OPT_OLDONLY},
   {"both",                0, 'B', OPT_BOTH},
   {"mono",                0, 'M', OPT_MONO},
   {"stereo",              0, 'S', OPT_STEREO},
   {"verbose",             0, 'V', OPT_VERBOSE},
   {"frequency",           1, 'f', OPT_FREQUENCY},
   {"oversample",          1, 'o', OPT_OVERSAMPLE},
   {"transpose",           1, 't', OPT_TRANSPOSE},
   {"repeats",             1, 'r', OPT_REPEATS},
   {"speed",               1, 's', OPT_SPEED},
   {"mix",                 1, 'm', OPT_MIX},
   {"start",               1, 'X', OPT_START},
   {"cut",                 1, '-', OPT_CUT},
   {"add",                 1, '+', OPT_ADD},
   {"half",                1, '/', OPT_HALF},
   {"double",              1, '2', OPT_DOUBLE},
   {"scroll",              0, 'v', OPT_SHOW},
	{"color",					0, 0, OPT_COLOR},	
	{"bw",						0, 0, OPT_BW},	
	{"xterm",					0, 0, OPT_XTERM},	
	{"noxterm",					0, 0, OPT_NOXTERM},	
   {"sync",                0, '=', OPT_SYNC},
	{"loop",						0, 'l', OPT_LOOP},
	{"speedmode",			   1, '?', OPT_SPEEDMODE},
#ifdef VOLUME_CONTROL
	{"speaker",					0, '#', OPT_SPEAKER},
	{"volume",					1, 'u', OPT_VOLUME},
#endif
   {0,                     0,  0 , 0}
};


/* global variable to catch various types of errors and achieve the 
 * desired flow of control
 */
int error;

LOCAL unsigned long half_mask = 0;

/* v = optvalue(def):
 * obtain a numeric value for an option, with default being def
 */
LOCAL int optvalue(def)
int def;
   {
   int d;

   if (optarg && sscanf(optarg, "%d", &d) == 1)
      return d;
   else
      {
      optind--;
      return def;
      }
   }

/* some preferences that don't need to be global */


LOCAL int ask_freq;		/* parameters for setup audio */
LOCAL int oversample;
LOCAL int stereo;

LOCAL int start;			/* parameters for st_play */
LOCAL int transpose;

LOCAL int loop = FALSE;	/* main automaton looping at end of argv ? */


LOCAL void parse_options(argc, argv)
int argc;
char *argv[];
   {
   int c;
   
   while ((c = getlongopt(argc, argv, long_options))
      != BAD_OPTION)
      switch(c)
         {
		case OPT_LOOP:
			loop = TRUE;
			break;
		case OPT_CUT:
		case OPT_ADD:
		case OPT_HALF:
			{
			int i;
			unsigned long imask = 0;
      
			for (i = 0; optarg && optarg[i]; i++)
				{
				char c = tolower(optarg[i]);

				if (c >= '1' && c <= '9')
					imask |= 1<< (c-'0');
				else if (c >= 'a' && c <= 'z')
					imask |= 1 << (c-'a'+10);
				}
			switch(c)
				{
			case OPT_ADD:
				set_pref_scalar(PREF_IMASK, ~imask);
				break;
			case OPT_CUT:
				set_pref_scalar(PREF_IMASK, imask);
				break;
			case OPT_HALF:
				half_mask = imask;
				break;
			case OPT_DOUBLE:
				half_mask = ~imask;
				}
			}        
         break;
		case OPT_SPEEDMODE:
			if (optarg)
				{
				if (strcasecmp(optarg, "old") == 0)
					set_pref_scalar(PREF_SPEEDMODE, OLD_SPEEDMODE);
				else if (strcasecmp(optarg, "normal") == 0)
					set_pref_scalar(PREF_SPEEDMODE, NORMAL_SPEEDMODE);
				else if (strcasecmp(optarg, "finefirst") == 0)
					set_pref_scalar(PREF_SPEEDMODE, FINESPEED_ONLY);
				else if (strcasecmp(optarg, "normalfirst") == 0)
					set_pref_scalar(PREF_SPEEDMODE, SPEED_ONLY);
				}
			break;
			
      case OPT_OLDONLY:   /* old tracker type */
         set_pref_scalar(PREF_TYPE, OLD);
         break;
      case OPT_NEWONLY:   /* new tracker type */
         set_pref_scalar(PREF_TYPE, NEW);
         break;
      case OPT_SHOW:
         set_pref_scalar(PREF_SHOW, TRUE);
         break;
		case OPT_XTERM:
			set_pref_scalar(PREF_XTERM, TRUE);
			break;
		case OPT_NOXTERM:
			set_pref_scalar(PREF_XTERM, FALSE);
			break;
		case OPT_COLOR:
			set_pref_scalar(PREF_COLOR, TRUE);
			break;
		case OPT_BW:
			set_pref_scalar(PREF_COLOR, FALSE);
			break;
      case OPT_SYNC:
         set_pref_scalar(PREF_SYNC, TRUE);
         break;
      case OPT_BOTH:   /* both tracker types */
         set_pref_scalar(PREF_TYPE, BOTH);
         break;
      case OPT_REPEATS:   /* number of repeats */
         set_pref_scalar(PREF_REPEATS, optvalue(0));
         break;
      case OPT_SPEED:  
         set_pref_scalar(PREF_SPEED, optvalue(50));
         break;
      case OPT_MONO:  
         stereo = FALSE;
         break;
      case OPT_STEREO:  
         stereo = TRUE;
         break;
      case OPT_OVERSAMPLE:  
         oversample = optvalue(1);
         break;
      case OPT_FREQUENCY:
         ask_freq = optvalue(0) * 1000;
         break;
      case OPT_TRANSPOSE:
         transpose = optvalue(0);
         break;
      case OPT_PICKY:
         set_pref_scalar(PREF_TOLERATE, 0);
         break;
      case OPT_TOLERANT:
         set_pref_scalar(PREF_TOLERATE, 2);
         break;
      case OPT_MIX:     /* % of channel mix. 
                         * 0->full stereo, 100->mono */
         set_mix(optvalue(30));
         break;
      case OPT_START:
         start = optvalue(0);
         break;
      case OPT_HELP:
         print_usage();
         end_all(0);
         /* NOTREACHED */
      case OPT_VERBOSE:
         set_pref_scalar(PREF_DUMP, TRUE);
			break;
#ifdef VOLUME_CONTROL
		case OPT_VOLUME:
			volume = optvalue(-20);
			break;
		case OPT_SPEAKER:
			use_speaker = 1;
			break;
#endif
         }
   }



/* song = load_song(filename):
 * syntactic sugar around read_song
 *	- display the file name after stripping the path
 * - find the actual file
 * - read the song trying several formats
 * - handle errors gracefully
 */
LOCAL struct song *load_song(name)
char *name;
   {
   struct song *song;
   char *buffer;
	struct exfile *file;
   int i, j;
   
		/* display the file name */
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

		/* read the song */
	file = open_file(name, "r", getenv("MODPATH"));
	if (file)
		{
		switch(get_pref_scalar(PREF_TYPE))
			{
		case BOTH:
			song = read_song(file, NEW);
			if (song)
				break;
			else
				rewind_file(file);
			/* FALLTHRU */
		case OLD:
			song = read_song(file, OLD);
			break;
			/* this is explicitly flagged as a new module,
			 * so we don't need to look for a signature.
			 */
		case NEW:
			song = read_song(file, NEW_NO_CHECK);
			break;
		default:
			song = NULL;
			}
		close_file(file);
		}
	else
		song = NULL;

		/* remove the displayed file name */
	if (buffer)
		{
		status(0);
		free(buffer);
		}

	return song;
   }



/* initialize all options to default values */
LOCAL void set_default_prefs()
	{
	char *s;

   start = 0;
   set_pref_scalar(PREF_IMASK, 0);
   set_pref_scalar(PREF_BCDVOL, 0);
   set_pref_scalar(PREF_DUMP, FALSE);
   set_pref_scalar(PREF_SHOW, FALSE);
#ifdef COLOR_IS_DEFAULT
   set_pref_scalar(PREF_COLOR, TRUE);
#else
   set_pref_scalar(PREF_COLOR, FALSE);
#endif
   set_pref_scalar(PREF_SYNC, FALSE);
   set_pref_scalar(PREF_TYPE, BOTH);
   set_pref_scalar(PREF_REPEATS, 1);
   set_pref_scalar(PREF_SPEED, 50);
   set_pref_scalar(PREF_TOLERATE, 1);
	set_pref_scalar(PREF_SPEEDMODE, NORMAL_SPEEDMODE);

	s = getenv("TERM");
	if (strncmp(s, "xterm", 5) == 0 || strncmp(s, "kterm", 5) == 0 
		|| strncmp(s, "cxterm", 6) == 0)
		set_pref_scalar(PREF_XTERM, TRUE);
	else
		set_pref_scalar(PREF_XTERM, FALSE);
   ask_freq = read_env("FREQUENCY", 0);
   oversample = read_env("OVERSAMPLE", 1);
   transpose = read_env("TRANSPOSE", 0);
   stereo = !getenv("MONO");
   set_mix(30);
	}


LOCAL void adjust_song(s, m)
struct song *s;
unsigned long m;
	{
	int i, j ;

	for (i = 1; i <= s->ninstr; i++)
		if ( (1 << i) & ~m)
			{
			for (j = 0; j <= MAX_VOLUME; j++)
				s->samples[i]->volume_lookup[j] *= 2;
			}
	s->side_width++;
	}


int main(argc, argv)
int argc;
char **argv;
   {
   struct song *song;
   int *is_song;
   int i;

   struct tag *result;

	EXPAND_WILDCARDS(argc,argv);

   is_song = (int *)malloc(sizeof(int) * argc);
   if (!is_song)
      end_all("No memory left");

   for (i = 0; i < argc; i++)
      is_song[i] = FALSE;        /* For termination */

	set_default_prefs();
   if (argc == 1)
      {
      print_usage();
      end_all(0);
      }



looping:
   for (optind = 1; optind < argc; optind++)
      {
      parse_options(argc, argv);
      if (optind >= argc)
         end_all(0);
         
   
      song = load_song(argv[optind]);   
      if (song)
         is_song[optind] = TRUE;
      else
         {
         puts("not a song");
         is_song[optind] = FALSE;
         continue;
         }
play_on:
      if (get_pref_scalar(PREF_DUMP))
         dump_song(song); 
		if (half_mask)
			adjust_song(song, half_mask);
      transpose_song(song, transpose);
      setup_audio(ask_freq, stereo, oversample);
      result = play_song(song, start);
      release_song(song);
      status(0);
      while (result = get_tag(result))
         {
         switch (result->type)
            {
         case PLAY_PREVIOUS_SONG:
            optind--;
            while ((optind > 0) && !is_song[optind])
               optind--;
            if (optind == 0)
					{
					if (loop)
						optind = argc - 1;
					else
						end_all(0);
					}
				song = load_song(argv[optind]);
				goto play_on;
            /* NOTREACHED */
         case PLAY_LOAD_SONG:
            song = load_song(result->data.pointer);
            free(result->data.pointer);
            if (song)
               goto play_on;
         default:
            break;
            }
         result++;
         }
            
      }
	if (loop)
		goto looping;
   end_all(0);
   /* NOTREACHED */
   }


