/* extern.h 
	vi:ts=3 sw=3:
 */

/* $Id: extern.h,v 1.1.1.1 2002/01/18 16:34:20 lindner Exp $
 * $Log: extern.h,v $
 * Revision 1.1.1.1  2002/01/18 16:34:20  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.17  1995/03/03  14:23:59  espie
 * Color independence.
 *
 * Revision 4.16  1995/03/01  15:24:51  espie
 * Proto for set_data_width.
 *
 * Revision 4.15  1995/02/23  22:40:44  espie
 * Changed call to output_samples, added # of bits.
 *
 * Revision 4.14  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.13  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.10  1995/02/20  16:49:58  espie
 * Added nearest_note.
 *
 * Revision 4.9  1995/02/14  04:02:28  espie
 * Changed amiga prototype.
 *
 * Revision 4.8  1995/02/06  14:50:47  espie
 * Changed sample_info.
 *
 * Revision 4.7  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.6  1995/02/01  16:39:04  espie
 * Added LEFT_SIDE/RIGHT_SIDE defines (finally !)
 *
 * Revision 4.1  1994/02/04  14:54:08  espie
 * Fixed up ansi C stupid bug.
 * Lots of new proto for all functions.
 * Generalized open.c.
 * Comments.
 * Added checkbrk.
 * General cleanup.
 * Suppressed some unused code.
 * Better portability.
 * Removed init_display.
 * No more reentrency problems with INIT_ONCE.
 * Protos for ui: notice, info, scroller, pattern display.
 * Mostly working.
 * Just dies with a guru.
 * Plus timing problems at start.
 * New prototypes.
 * Initial revision
 *
 * Revision 3.15  1993/12/04  16:12:50  espie
 * Amiga support.
 * Added bg/fg test.
 * stty_sane.
 * New audio functions.
 * Simplified delay_pattern.
 * Protracker commands.
 * New release.
 *
 * Revision 2.19  1992/11/17  17:06:25  espie
 * Lots of new functions to account for new interface.
 * open_file support.
 * Separated mix/stereo stuff.
 * Added possibility to get back to MONO for the sgi.
 * Added stereo capabilities to the indigo version.
 * Added some new song types to automatize the choice process.
 * Moved resampling to audio, added prototype.
 * Added SAMPLE_FAULT, for trying to play
 * a note without a sample (not really an error).
 *
 * Revision 1.7  1991/11/08  14:25:55  espie
 * Modified audio prototype so that you can change
 * frequency.
 * Added prototype for release_song.
 * Added arpeggio effect.
 * Added entries for new effects.
 * Added entries for commands.c.
 */


/*--------------------------- resample.c -------------------------*/
#define ACCURACY 12
#define fix_to_int(x) ((x) >> ACCURACY)
#define int_to_fix(x) ((x) << ACCURACY)
#define fractional_part(x) ((x) & (total_step - 1))
#define total_step	 (1 << ACCURACY)

/* release_audio_channels:
 * free every audio channel previously allocated
 */
XT void release_audio_channels P((void));

#define LEFT_SIDE 0
#define RIGHT_SIDE 1
#define BASE_AUDIO 20
#define AUDIO_SIDE (BASE_AUDIO)

/* chan = new_channel_tag_list(prop):
 * allocates a new channel for the current song
 * properties: AUDIO_SIDE LEFT_SIDE (default)/RIGHT_SIDE
 */
XT struct audio_channel *new_channel_tag_list P((struct tag *prop));

/* init_tables(oversample, frequency):
 * precomputes the step_table and the pitch_table
 * according to the desired oversample and frequency.
 * This is static, you can call it again whenever you want.
 * Adjust the currently used audio channels if needed.
 */
XT void init_tables P((int oversample, int frequency));

/* set_data_width(side_width, sample_width):
 * accumulated data on each side will have width side_width bits,
 * and each sample will never be greater than sample_width
 */
XT void set_data_width P((int side, int sample));

/* resample(oversample, number):
 * send number samples out computed according
 * to the current state of channels
 * and oversample.
 */
XT void resample P((int oversample, int number));

/* play_note(au, samp, pitch)
 * set audio channel au to play samp at pitch
 */
XT void play_note P((struct audio_channel *au, struct sample_info *samp, \
int pitch));

/* set_play_pitch(au, pitch):
 * set channel au to play at pitch pitch
 */
XT void set_play_pitch P((struct audio_channel *au, int pitch));

/* set_play_volume(au, volume):
 * set channel au to play at volume volume
 */
XT void set_play_volume P((struct audio_channel *au, int volume));

/* set_play_position(au, pos):
 * set position in sample for channel au at given offset
 */
XT void set_play_position P((struct audio_channel *au, int pos));


/*--------------------------- empty.c ----------------------------*/
/* empty = empty_sample():
 * return a suitable empty sample
 */
XT struct sample_info *empty_sample P((void));


/*--------------------------- automaton.c ------------------------*/
/* init_automaton(a, song, start):
 * put the automaton a in the right state to play song from pattern #start.
 */
XT void init_automaton P((struct automaton *a, struct song *song, int start));

/* next_tick(a):
 * set up everything for the next tick.
 */
XT void next_tick P((struct automaton *a));


/*--------------------------- commands.c -------------------------*/
/* init_effects(): set up all data for the effects
 * can't be set up as an autoinit
 */
XT void init_effects P((void (*table[])()));

/* do_nothing: this is the default behavior for an effect.
 */
XT void do_nothing P((struct channel *ch));


/*--------------------------- dump_song.c ------------------------*/
/* dump_song(s): 
 * display some information pertinent to the given song s
 */
XT void dump_song P((struct song *song));


/*--------------------------- display.c --------------------------*/
/* dump_event(ch, e): dump event e as occuring on channel ch
 * (some events need the current channel state for a correct dump)
 * special case: ch == 0 means current set of events done
 */
XT void dump_event P((struct channel *ch, struct event *e));

/* dump_delimiter(): add a delimiter to the current dump, to 
 * separate left channels from right channels, for instance
 */
XT void dump_delimiter P((void));


/*--------------------------- main.c -----------------------------*/
#define OLD 0
#define NEW 1
/* special new type: for when we try to read it as both types.
 */
#define BOTH 2
/* special type: does not check the signature */
#define NEW_NO_CHECK 3


/* error types. Everything is centralized,
 * and we check in some places (see st_read, player and main)
 * that there was no error. Additionnally signal traps work
 * that way too.
 */
 
/* normal state */
#define NONE 0  
/* read error */
#define FILE_TOO_SHORT 1
#define CORRUPT_FILE 2
/* trap error: goto next song right now */
#define NEXT_SONG 3
/* run time problem */
#define FAULT 4
/* the song has ended */
#define ENDED 5
/* unrecoverable problem: typically, trying to 
 * jump to nowhere land.
 */
#define UNRECOVERABLE 6
/* Missing sample. Very common error, not too serious. */
#define SAMPLE_FAULT 7
/* New */
#define PREVIOUS_SONG 8
#define OUT_OF_MEM 9
XT int error;


/*--------------------------- notes.c ----------------------------*/
#define NUMBER_NOTES 120
#define NUMBER_FINETUNES 17
/* set of pitch for all notes for all finetunes (used heavily by protracker)
 */
XT short pitch_table[NUMBER_NOTES][NUMBER_FINETUNES];    /* 120 * 17 = big ! */

/* note = find_note(pitch):
 * find note corresponding to a given pitch
 */
XT int find_note P((int pitch));

/* pitch = nearest_note(pitch, finetune):
 * find entire note with the nearest pitch with the given finetune
 */
XT int nearest_note P((int pitch, int finetune));

/* oldtranspose = transpose_song(song, newtranspose):
 * tranpose whole song to a new pitch
 */
XT int transpose_song P((struct song *song, int newtranspose));

/* name = name_of_note(note):
 * name of the note. Warning! This name is only valid until a new call 
 * to name_of_note (static storage)
 */
XT char *name_of_note P((int note));


/*--------------------------- open.c -----------------------------*/
/* handle = open_file(filename, mode, path):
 * transparently open a compressed file.
 */
XT struct exfile *open_file P((char *fname, char *fmode, char *path));

/* close_file(handle):
 * close a file that was opened with open_file.
 */
XT void close_file P((struct exfile *file));

/* analogous of fgetc, ftell, fread, and rewind */
XT int getc_file P((struct exfile *file));
XT int tell_file P((struct exfile *file));
XT int read_file P((void *p, int s, int n, struct exfile *file));
XT void rewind_file P((struct exfile *file));


/*--------------------------- st_play.c --------------------------*/
/* reset_note(ch, note, pitch):
 * set channel ch to play note at pitch pitch
 */
XT void reset_note P((struct channel *ch, int note, int pitch));

/* set_current_pitch(ch, pitch):
 * set ch to play at pitch pitch
 */
XT void set_current_pitch P((struct channel *ch, int pitch));

/* set_current_volume(ch, volume):
 * set channel ch to play at volume volume
 */
XT void set_current_volume P((struct channel *ch, int volume));

/* set_temp_volume(ch, volume):
 * set channel ch to play at volume volume, but without storing it
 * (used only for tremolo)
 */
XT void set_temp_volume P((struct channel *ch, int volume));

/* set_position(ch, pos):
 * set position in sample for current channel at given offset
 */
XT void set_position P((struct channel *ch, int pos));

/* init_player(oversample, frequency):
 * sets up the player for a given oversample and
 * output frequency.
 * Note: we can call init_player again to change oversample and
 * frequency.
 */
XT void init_player P((int o, int f));

/* play_song(song, start):
 * play the song.  return tags as shown in get_ui 
 */
XT struct tag *play_song P((struct song *song, int start));


/*--------------------------- st_read.c --------------------------*/
/* s = read_song(f, type):
 * tries to read f as a song of type NEW/OLD/NEW_NOCHECK
 * returns NULL (and an error) if it doesn't work.
 * Returns a dynamic song structure if successful.
 */
XT struct song *read_song P((struct exfile *f, int type));

/* release_song(s):
 * release all the memory song occupies.
 */
XT void release_song P((struct song *song));


/*--------------------------- setup_audio.c ----------------------*/
/* setup_audio(ask_freq, stereo, oversample):
 * setup the audio output with these values 
 */
XT void setup_audio P((int f, int s, int o));

/*--------------------------- audio.c ----------------------------*/
/* frequency = open_audio(f, s):
 * try to open audio with a sampling rate of f, and eventually stereo.
 * We get the real frequency back. If we ask for 0, we
 * get the ``preferred'' frequency.
 * Note: we have to close_audio() before we can open_audio() again.
 * Note: even if we don't ask for stereo, we still have to give a
 * right and left sample.
 */
XT int open_audio P((int f, int s));
/* close_audio():
 * returns the audio to the system control, doing necessary
 * cleanup
 */
XT void close_audio P((void));
/* set_mix(percent): set mix channels level.
 * 0: spatial stereo. 100: mono.
 */
XT void set_mix P((int percent));

/* output_samples(l, r, n): outputs a pair of stereo samples.
 * Samples are n bits signed.
 * Output routine should be able to face anything from 16 to 25
 */
XT void old_output_samples P((int left, int right));
XT void output_samples P((int left, int right, int n));

/* flush_buffer(): call from time to time, because buffering
 * is done by the program to get better (?) performance.
 */
XT void flush_buffer P((void));

/* discard_buffer(): try to get rid of the buffer contents
 */
XT void discard_buffer P((void));

/* new_freq = update_frequency():
 * if !0, frequency changed and playing should be updated accordingly
 */
XT int update_frequency P((void));

/* bits = output_resolution()
 * returns the number of bits expected for the output.
 * Not necessary to use 16 bit samples if output is to be 8 bits
 * for instance. Return 16 by default
 */
XT int output_resolution P((void));

/* set_synchro(boolean):
 * try to synchronize audio output by using a smaller buffer
 */
XT void set_synchro P((int s));

#ifdef SPECIAL_SAMPLE_MEMORY
XT GENERIC alloc_sample P((int len));
XT void free_sample P((GENERIC s));
XT int obtain_sample P((GENERIC start, int l, struct exfile *f));

#else
#define alloc_sample(len)		calloc(len, 1)
#define free_sample(sample)		free(sample)
#define obtain_sample(start, l, f)	read_file(start, 1, l, f)
#endif


/*--------------------------- tools.c ----------------------------*/
/* v = read_env(name, default):
 * read a scalar value in the environment
 */
XT int read_env P((char *name, int def));


/*--------------------------- autoinit.c -------------------------*/
/* used for decentralizing initialization/termination of various
 * system routines
 */

/* end_all(s): the program must exit now, after displaying s to the user, 
 * usually through notice and calling all stacked at_end() functions. 
 * s may be 0 for normal exit. DO NOT use exit() anywhere in tracker 
 * but end_all() instead.
 */
XT void end_all P((char *s));

/* at_end(cleanup): stack cleanup to be called at program's termination
 */
XT void at_end P((void (*cleanup)(void)));

/* INIT_ONCE: macro for autoinitialization of routines.
 * modules that need an init routine should LOCAL void INIT = init_routine,
 * and add INIT_ONCE; at EVERY possible first entry point for their routine.
 * (I mean every, don't try to second-guess me !)
 */
#define INIT_ONCE	if (INIT){void (*func)P((void)) = INIT; INIT = 0; (*func)();}


/*--------------------------- $(UI)/ui.c ------------------------*/
/* see unix/ui.c for the general unix implementation.
 * The old may_getchar() has been replaced by the tag-based
 * get_ui
 */
/* get_ui(): returns an array of tags that reflect the current user-interface
 * actions. Unknown tags WILL be ignored.
 * Note that get_ui will be called about once every tick, providing a poor man's
 * timer to the interface writer if needed to code multiple actions on the same
 * user-input. See unix/termio.c for a good example.
 * see amiga/ui.c for the correct way to do it when you have a real timer.
 *
 * VERY IMPORTANT: who do the tags belong to ?
 *   as a general rule, result (and their values) MUST only be considered
 *   valid between two calls to get_ui ! Be careful to call get_ui ONLY at
 *   reasonable places.
 *   One exception: structures that are dynamically allocated (like UI_LOAD_SONG
 *   values) will ONLY get freed when you ask for it !
 */
XT struct tag *get_ui P((void));
#define BASE_UI 10
#define UI_NEXT_SONG	(BASE_UI)            /* load next song */
#define UI_PREVIOUS_SONG (BASE_UI + 1)    /* load previous song */
#define UI_LOAD_SONG (BASE_UI + 2)        /* load song. Name as value */
#define UI_SET_BPM (BASE_UI + 3)          /* set beat per minute to value */
#define UI_JUMP_TO_PATTERN (BASE_UI + 4)  /* jump to pattern #value.  Use 
       												 * display_pattern to keep in 
														 * sync with the player */
#define UI_RESTART (BASE_UI + 5)          /* restart current song. Not 
														 * quite jump to 0 */
#define UI_QUIT (BASE_UI + 6)             /* need I say more ? */
#define UI_DISPLAY (BASE_UI + 7)          /* status of scrolling window: 
														 * true or false */


/* st_play.c translates the get_ui() tags in a standard way.
 * Actually it doesn't translate anything right now...
 */
#define BASE_PLAY 20
#define PLAY_NEXT_SONG UI_NEXT_SONG
#define PLAY_PREVIOUS_SONG UI_PREVIOUS_SONG
#define PLAY_LOAD_SONG UI_LOAD_SONG

#define PLAY_ERROR BASE_PLAY

/* Most of these functions are information display function.
 * A correct implementation should heed run_in_fg() if needed
 */

/* notice(s): important message for the user (terminal error maybe).
 * take extra pain to make it apparent even if run in background
 */
XT void notice P((char *s));

/* status(s): some indication of the system current status... 
 * Used for fleeing error messages too. 
 * s = 0 is valid and indicates return to the default status.
 */
XT void status P((char *s));

/* begin_info: open a logical information window.
 * returns 0 if the window couldn't be opened.
 * A NULL window shouldn't be used, but don't count on it !
 */
XT GENERIC begin_info P((char *title));
/* info(handle, line): add a line to the info window,
 * completing the current line if applicable
 */
XT void info P((GENERIC handle, char *line));
/* infos(handle, line): add to the current line of the info window
 */
XT void infos P((GENERIC handle, char *s));
/* end_info(handle): this window is complete...
 */
XT void end_info P((GENERIC handle));

/* Scrolling score display:
 * new_scroll() returns a writable buffer of a suitable length for n tracks
 * in which display.c will write what it needs.
 * It can return 0 if not applicable.
 */
XT char *new_scroll P((void));

/* set_number_tracks(n) sets the number of tracks for new_scroll, in order
 * to allocate room accordingly
 */
XT void set_number_tracks P((int n));

/* scroll: returns this scrolling line to the program. Note that
 * scroll doesn't take any argument, and implies calls to new_scroll/scroll
 * are paired. After a call to scroll, the last pointer returned by new_scroll
 * should be considered invalid !
 */
XT void scroll P((void));

/* display_pattern(current, total, real): we are at current/total(real) 
 * in the current song
 * may be used as a poor man's timer.
 */
XT void display_pattern P((int current, int total, int real));

/* song_title(s): the current song title is s.
 * ui implementors: Don't count on this pointer remaining valid AFTER the call,
 * make a copy if needed
 */
XT void song_title P((char *s));

/* boolean checkbrk():
 * check whether a break occured and we should end right now.
 * Call it often enough (like when loading songs and stuff)
 */
XT int checkbrk P((void));


/*--------------------------- color.c ----------------------------*/
/* s = write_color(base, color):
 * write sequence to switch to color color at base, returning
 * position after the sequence
 */
XT char *write_color P((char *base, int color));

