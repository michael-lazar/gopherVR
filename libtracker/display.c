/* display.c 
	vi:ts=3 sw=3:
 */

/* $Id: display.c,v 1.1.1.1 2002/01/18 16:34:20 lindner Exp $
 * $Log: display.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:20  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.20  1995/03/11  21:40:38  espie
 * Added jump_pattern, invert_loop.
 *
 * Revision 4.19  1995/03/04  00:16:00  espie
 * Implemented vibrato control.
 *
 * Revision 4.18  1995/03/03  14:23:41  espie
 * Color fixed (mostly).
 *
 * Revision 4.17  1995/02/27  14:24:23  espie
 * Minor bug in dump_delimiter.
 *
 * Revision 4.16  1995/02/25  15:43:11  espie
 * Display MUCH better.
 *
 * Revision 4.15  1995/02/24  15:36:39  espie
 * Stupid bug: missing sample name in table (8)
 *
 * Revision 4.14  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.13  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.11  1995/02/20  22:28:50  espie
 * tremolo.
 *
 * Revision 4.10  1995/02/20  16:49:58  espie
 * Bug: need to check sample length against 0 before dividing.
 *
 * Revision 4.9  1995/02/14  04:02:28  espie
 * Kludge for the amiga.
 *
 * Revision 4.7  1995/02/06  14:50:47  espie
 * Changed sample_info.
 *
 * Revision 4.6  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.5  1995/02/01  16:39:04  espie
 * Includes moved to defs.h
 *
 * Revision 4.0  1994/01/11  17:45:22  espie
 * Major change: does not use sprintf heavily.
 *
 * Generalized open.c.
 * Use name_of_note(), no need for run_in_fg().
 * Small bug: strcpy -> stringcopy.
 * Cond code to make show/not show robust.
 * Added instrument name as shown per display.c.
 * Major change: use scroller interface.
 * Lots of LOCAL added + minor changes.
 * Try to get rid of %d format in printf.
 */
     
#include "defs.h"
#include "song.h"
#include "channel.h"
#include "extern.h"
#include "tags.h"
#include "prefs.h"

ID("$Id: display.c,v 1.1.1.1 2002/01/18 16:34:20 lindner Exp $")
LOCAL void init_display P((void));
LOCAL void (*INIT)P((void)) = init_display;
     
#define ENTRY_SIZE 14
LOCAL char *base;

/* lookup tables for speed */
LOCAL char *num[] = {
" 0", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9",
"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
"30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
"40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
"50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
"60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
"70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
"80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
"90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
"00", "01", "02", "03", "04", "05", "06", "07", "08", "09"};

char instname[] = { ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

LOCAL void reset_buffer()
   {
   base = new_scroll();
   }

LOCAL void next_entry()
   {
	if (get_pref_scalar(PREF_COLOR))
		base += 7;
   base += ENTRY_SIZE;
   }

/* utility functions to avoid the overhead of strncpy */
LOCAL void copy1(to, from)
char *to, *from;
   {
   *to = *from;
   }
   
LOCAL void copy2(to, from)
char *to, *from;
   {
   *to++ = *from++;
   *to = *from;
   }

LOCAL void copy3(to, from)
char *to, *from;
   {
   *to++ = *from++;
   *to++ = *from++;
   *to = *from;
   }

LOCAL void copy4(to, from)
char *to, *from;
	{
   *to++ = *from++;
   *to++ = *from++;
   *to++ = *from++;
   *to = *from;
	}

LOCAL void stringcopy(to, from)
char *to, *from;
   {
   while (*from)
      *to++ = *from++;
   }

LOCAL void num2(to, n)
char *to;
int n;
   {
   char *v = num[n];
   *to++ = *v++;
   *to = *v;
   }

LOCAL void num3(to, n)
char *to;
int n;
   {
   char *v;

   if (n >= 100)
      *to = "0123456789"[n/100];
   while (n > 109)
      n -= 100;
   v = num[n];
   to++;
   *to++ = *v++;
   *to = *v;
   }

LOCAL void (*table[NUMBER_EFFECTS]) P((int samp, int para, int note, \
	struct channel *ch));

/* all the various dump for the effects */
LOCAL void disp_default(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
   }

LOCAL void disp_speed(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
   if (para < 32)
      {
      stringcopy(base+6, "SPD");
      num2(base+10, para);
      }
   else
      {
      stringcopy(base+6, "spd%");
      num3(base+10, para * 100/NORMAL_FINESPEED);
      }
   }

LOCAL void disp_nothing(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
   }

LOCAL void disp_portamento(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		stringcopy(base+2, "-->");
		copy3(base+5, name_of_note(note));
		if (para)
			{
			base[8] = '(';
			num3(base+9, para);
			base[12] = ')';
			}
		}
   }

LOCAL void disp_portaslide(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		stringcopy(base+2, "-->");
		copy3(base+5, name_of_note(note));
		if (LOW(para))
			{
			base[9] = '-';
			num2(base+10, LOW(para));
			}
		else
			{
			base[9] = '+';
			num2(base+10, HI(para));
			}
		}
   }

LOCAL void disp_upslide(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		base[8] = '-';
		if (para)
			num3(base+9, para);
		}
   }

LOCAL void disp_downslide(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		base[8] = '+';
		if (para)
			num3(base+9, para);
		}
   }

LOCAL void disp_vibrato(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
	if (para || ch->samp->start)
		copy2(base+6, "vb");
   if (para)
      {
      num2(base+8, LOW(para));
      base[10] = '/';
      num2(base+11, HI(para));
      }
   }

LOCAL void disp_vibrato_wave(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
	{
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
	copy2(base+6, "vb");
	switch(para)
		{
	case 0:
		copy4(base+9, "sine");
		break;
	case 1:
		copy4(base+9, "sqre");
		break;
	case 2:
		copy4(base+9, "ramp");
		break;
	case 4:
		copy4(base+9, "SINE");
		break;
	case 5:
		copy4(base+9, "SQRE");
		break;
	case 6:
		copy4(base+9, "RAMP");
		break;
	default:
		copy4(base+9, "????");
		}
	}

LOCAL void disp_tremolo_wave(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
	{
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
	copy2(base+6, "tr");
	switch(para)
		{
	case 0:
		copy4(base+9, "sine");
		break;
	case 1:
		copy4(base+9, "sqre");
		break;
	case 2:
		copy4(base+9, "ramp");
		break;
	case 4:
		copy4(base+9, "SINE");
		break;
	case 5:
		copy4(base+9, "SQRE");
		break;
	case 6:
		copy4(base+9, "RAMP");
		break;
	default:
		copy4(base+9, "????");
		}
	}

LOCAL void disp_tremolo(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
	if (para || ch->samp->start)
		copy2(base+6, "tr");
   if (para)
      {
      num2(base+8, LOW(para));
      base[10] = '/';
      num2(base+11, HI(para));
      }
   }

LOCAL void disp_vibratoslide(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+6, "vibs");
		if (LOW(para))
			{
			base[10] = '-';
			num2(base+11, LOW(para));
			}
		else
			{
			base[10] = '+';
			num2(base+11, HI(para));
			}
		}
   }

LOCAL void disp_slidevol(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+6, "vol");
		if (LOW(para))
			{
			base[10] = '-';
			num2(base+11, LOW(para));
			}
		else
			if (HI(para))
				{
				base[10] = '+';
				num2(base+11, HI(para));
				}
		}
   }

LOCAL void disp_volume(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		if (para)
			{
			stringcopy(base+6, "vol");
			num3(base+10, para);
			}
		else
			stringcopy(base+6, "silent");
		}
   }

LOCAL void disp_arpeggio(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		if (note != NO_NOTE)
			{
			copy3(base+2, name_of_note(note));
			copy3(base+6, name_of_note(note + LOW(para)));
			copy3(base+10, name_of_note(note + HI(para)));
			}
		else
			if (ch->note == NO_NOTE)
				stringcopy(base, "Arp error");
			else
				{
				copy3(base+6, name_of_note(ch->note + LOW(para)));
				copy3(base+10, name_of_note(ch->note + HI(para)));
				}  
		}
   }

LOCAL void disp_retrig(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base + 6, "rtg");
		num3(base+9, para);
		}
   }


LOCAL void disp_note_cut(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+6, "cut");
		num3(base+9, para);
		}
   }

LOCAL void disp_late_start(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+6, "lte");
		num3(base+9, para);
		}
   }

LOCAL void disp_offset(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+6, "off   %");
		if (ch->samp->length)
			num3(base+9, para * 25600/ch->samp->length);
		}
   }

LOCAL void disp_smooth_up(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+6, "sth-");
		num3(base+10, para);
		}
   }

LOCAL void disp_smooth_down(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+6, "sth+");
		num3(base+10, para);
		}
   }

LOCAL void disp_smooth_upvolume(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+8, "++");
		num3(base+10, para);
		}
   }

LOCAL void disp_smooth_downvolume(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+8, "--");
		num3(base+10, para);
		}
   }

LOCAL void disp_change_finetune(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		{
		copy3(base+2, name_of_note(note));
		stringcopy(base+6, "fine");
		num2(base+11, para);
		}
   }

LOCAL void disp_skip(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
   if (para)
      {
      stringcopy(base+6, "skp");
      num3(base+10, para);
      }
   else
      stringcopy(base+6, "next");
   }

LOCAL void disp_fastskip(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
   stringcopy(base+6, "ff");
   num3(base+10, para);
   }

LOCAL void disp_loop(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
   if (para == 0)
      stringcopy(base+6, "SETLOOP");
   else
      {
      stringcopy(base+6, "LOOP");
      num3(base+10, para);
      }
   }

LOCAL void disp_delay_pattern(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
   {
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
   stringcopy(base+6, "DLAY");
   num3(base+10, para);
   }

LOCAL void disp_gliss_ctrl(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
	{
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
	if (para)
		stringcopy(base+6, "gls on");
	else
		stringcopy(base+6, "gls off");
	}

LOCAL void disp_invert_loop(samp, para, note, ch)
int samp, para, note;
struct channel *ch;
	{
	if (ch->samp->start)
		copy3(base+2, name_of_note(note));
	if (para)
		{
      stringcopy(base+6, "inv");
      num3(base+10, para);
		}
	else
		stringcopy(base+6, "inv off");
	}

#define disp_nothing disp_default

LOCAL void init_display()
   {
   int i;

   for (i = 0; i < NUMBER_EFFECTS; i++)
      table[i] = disp_nothing;
	table[EFF_VIBRATO_WAVE] = disp_vibrato_wave;
	table[EFF_TREMOLO_WAVE] = disp_tremolo_wave;
	table[EFF_GLISS_CTRL] = disp_gliss_ctrl;
   table[EFF_ARPEGGIO] = disp_arpeggio;
   table[EFF_SPEED] = disp_speed;
   table[EFF_SKIP] = disp_skip;
   table[EFF_FF] = disp_fastskip;
   table[EFF_VOLUME] = disp_volume;
   table[EFF_VOLSLIDE] = disp_slidevol;
   table[EFF_OFFSET] = disp_offset;
   table[EFF_PORTA] = disp_portamento;
   table[EFF_PORTASLIDE] = disp_portaslide;
   table[EFF_UP] = disp_upslide;
   table[EFF_DOWN] = disp_downslide;
   table[EFF_VIBRATO] = disp_vibrato;
	table[EFF_TREMOLO] = disp_tremolo;
   table[EFF_VIBSLIDE] = disp_vibratoslide;
   table[EFF_SMOOTH_UP] = disp_smooth_up;
   table[EFF_SMOOTH_DOWN] = disp_smooth_down;
   table[EFF_CHG_FTUNE] = disp_change_finetune;
   table[EFF_LOOP] = disp_loop;
   table[EFF_RETRIG] = disp_retrig;
   table[EFF_S_UPVOL] = disp_smooth_upvolume;
   table[EFF_S_DOWNVOL] = disp_smooth_downvolume;
   table[EFF_NOTECUT] = disp_note_cut;
   table[EFF_LATESTART] = disp_late_start;
   table[EFF_DELAY] = disp_delay_pattern;
	table[EFF_INVERT_LOOP] = disp_invert_loop;
   reset_buffer();
   }

void dump_event(ch, e)
struct channel *ch;
struct event *e;
   {
   INIT_ONCE;
   
	if (ch && base)	/* do we have a scroll line AND are we not finished */
		{
		if (get_pref_scalar(PREF_COLOR))
			base = write_color(base, ch->samp->color);
		if (ch->samp != empty_sample())
			*base = instname[e->sample_number];
		(*table[e->effect])(e->sample_number, e->parameters, e->note, ch);
		next_entry();
		}
	else
		{
		scroll();
		reset_buffer();
		}
	}

void dump_delimiter()
	{
	INIT_ONCE;
	if (base)
#ifdef AMIGA
		*base++;
#else
		*base++= '|';
#endif
	}
