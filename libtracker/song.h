/* song.h 
	vi:ts=3 sw=3:
 */

/* internal data structures for the soundtracker player routine....
 */

/* $Id: song.h,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: song.h,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.13  1995/03/04  00:16:16  espie
 * *** empty log message ***
 *
 * Revision 4.12  1995/03/01  15:24:51  espie
 * Minor changes.
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.8  1995/02/20  16:49:58  espie
 * Added song type (protracker or old st) for further checks.
 *
 * Revision 4.6  1995/02/06  14:50:47  espie
 * Changed sample_info.
 *
 * Revision 4.5  1995/02/01  17:14:54  espie
 * Scaled volume.
 *
 * Revision 4.0  1994/01/11  17:55:59  espie
 * REAL_MAX_PITCH for better player.
 * REAL_MAX_PITCH != MAX_PITCH.
 * Added samples_start.
 *
 * Revision 2.5  1992/10/31  11:18:00  espie
 * New fields for optimized resampling.
 * Exchanged __ANSI__ to SIGNED #define.
 */

#ifdef SIGNED
typedef signed char SAMPLE;
#else
typedef char SAMPLE;
#endif


#define MAX_NUMBER_SAMPLES 32
#define ST_NUMBER_SAMPLES 15
#define PRO_NUMBER_SAMPLES 31

#define NORMAL_PLENGTH 64
#define NORMAL_NTRACKS 4
#define MAX_TRACKS 8
#define NUMBER_SAMPLES 32

#define BLOCK_LENGTH 64
#define NUMBER_TRACKS 4
#define NUMBER_PATTERNS 128

#define NUMBER_EFFECTS 40

/* some effects names */
#define EFF_ARPEGGIO    0
#define EFF_DOWN        1
#define EFF_UP          2
#define EFF_PORTA       3
#define EFF_VIBRATO     4
#define EFF_PORTASLIDE  5
#define EFF_VIBSLIDE    6
#define EFF_TREMOLO		7
#define EFF_OFFSET      9
#define EFF_VOLSLIDE    10
#define EFF_FF          11
#define EFF_VOLUME      12
#define EFF_SKIP        13
#define EFF_EXTENDED    14
#define EFF_SPEED       15
#define EFF_NONE        16
#define EXT_BASE        16
#define EFF_SMOOTH_UP   (EXT_BASE + 1)
#define EFF_SMOOTH_DOWN (EXT_BASE + 2)
#define EFF_GLISS_CTRL	(EXT_BASE + 3)
#define EFF_VIBRATO_WAVE	(EXT_BASE + 4)
#define EFF_CHG_FTUNE   (EXT_BASE + 5)
#define EFF_LOOP        (EXT_BASE + 6)
#define EFF_TREMOLO_WAVE	(EXT_BASE + 7)
#define EFF_RETRIG      (EXT_BASE + 9)
#define EFF_S_UPVOL     (EXT_BASE + 10)
#define EFF_S_DOWNVOL   (EXT_BASE + 11)
#define EFF_NOTECUT     (EXT_BASE + 12)
#define EFF_LATESTART   (EXT_BASE + 13)
#define EFF_DELAY       (EXT_BASE + 14)
#define EFF_INVERT_LOOP	(EXT_BASE + 15)

#define SAMPLENAME_MAXLENGTH 22
#define TITLE_MAXLENGTH 20

#define MIN_PITCH 113
#define MAX_PITCH 856
#define REAL_MAX_PITCH 1050

#define MIN_VOLUME 0
#define MAX_VOLUME 64

/* the fuzz in note pitch */
#define FUZZ 2

/* we refuse to allocate more than 500000 bytes for one sample */
#define MAX_SAMPLE_LENGTH 500000

struct sample_info
   {
   char *name;
   int  length, rp_offset, rp_length;
   unsigned long  fix_length, fix_rp_length;
   int volume;
	int volume_lookup[MAX_VOLUME+1];
	int color;
	int sample_size;			/* 8 or 16 */
   int finetune;
   SAMPLE *start, *rp_start;
   };

/* the actual parameters may be split in two halves occasionnally */

#define LOW(para) ((para) & 15)
#define HI(para) ((para) >> 4)

struct event
   {
   unsigned char sample_number;
   unsigned char effect;
   unsigned char parameters;
   unsigned char note;
   int pitch;
   };

struct block
   {
   struct event *e[MAX_TRACKS];
   };
    
        
struct song_info
   {
   int length;
   int maxpat;
   int transpose;
	int plength;
   char patnumber[NUMBER_PATTERNS];
   struct block *pblocks;
   };

#define OLD_ST 0
#define PROTRACKER 1

struct song
   {
   char *title;
      /* sample 0 is always a dummy sample */
   struct sample_info *samples[MAX_NUMBER_SAMPLES];
	int type;
	int ntracks;
	int ninstr;
	int side_width;
	int max_sample_width;			
   struct song_info info;
   long samples_start;
   };

#define AMIGA_CLOCKFREQ 3575872
