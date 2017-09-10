/* prefs.h 
	vi:ts=3 sw=3:
 */

/* $Id: prefs.h,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
 * $Log: prefs.h,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.12  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.6  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.2  1994/08/23  18:19:46  espie
 * Added speedmode option.
 */

#define BASE_PREFS      50
#define PREF_TYPE       BASE_PREFS
#define PREF_SPEED      (BASE_PREFS+1)
#define PREF_TOLERATE   (BASE_PREFS+2)
#define PREF_REPEATS    (BASE_PREFS+3)
#define PREF_IMASK      (BASE_PREFS+4)
#define PREF_BCDVOL     (BASE_PREFS+5)
#define PREF_DUMP       (BASE_PREFS+6)
#define PREF_SYNC       (BASE_PREFS+7)
#define PREF_SHOW       (BASE_PREFS+8)
#define PREF_SPEEDMODE 	(BASE_PREFS+9)
#define PREF_COLOR		(BASE_PREFS+10)
#define PREF_XTERM		(BASE_PREFS+11)

/* values for PREF_SPEEDMODE */
#define NORMAL_SPEEDMODE 0
#define FINESPEED_ONLY	1
#define SPEED_ONLY 2
#define OLD_SPEEDMODE 3

#define NUMBER_PREFS    (PREF_XTERM - BASE_PREFS + 1)

XT VALUE get_pref P((int index));
XT void set_pref P((int index, VALUE value));
XT int get_pref_scalar P((int index));
XT void set_pref_scalar P((int index, int value));
XT struct tag *get_prefs P((void));
