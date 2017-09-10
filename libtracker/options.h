/* options.h 
	vi:ts=3 sw=3:
 */
/* $Id: options.h,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $ */
/* $Log: options.h,v $
/* Revision 1.1.1.1  2002/01/18 16:34:21  lindner
/* Slightly modded sources for gophervr...
/*
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.14  1995/03/06  22:35:47  espie
 * Colour can be default.
 *
 * Revision 4.13  1995/03/01  15:24:51  espie
 * options half/double.
 *
 * Revision 4.12  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.7  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.6  1995/02/01  16:39:04  espie
 * Moved includes.
 *
 * Revision 4.3  1994/08/23  18:19:46  espie
 * Added speedmode option.
 * */

#define BAD_OPTION (-1)
#define OPT_HELP 0
#define OPT_QUIET 1
#define OPT_PICKY 2
#define OPT_TOLERANT 3
#define OPT_NEWONLY 4
#define OPT_OLDONLY 5
#define OPT_BOTH 6
#define OPT_MONO 7
#define OPT_STEREO 8
#define OPT_VERBOSE 9
#define OPT_FREQUENCY 10
#define OPT_OVERSAMPLE 11
#define OPT_TRANSPOSE 12
#define OPT_REPEATS 13
#define OPT_SPEED 14
#define OPT_MIX 15
#define OPT_START 16
#define OPT_CUT 17
#define OPT_ADD 18
#define OPT_SHOW 19
#define OPT_SYNC 20
#define OPT_SPEAKER 21
#define OPT_VOLUME 22
#define OPT_LOOP 23
#define OPT_SPEEDMODE 24
#define OPT_COLOR 25
#define OPT_HALF 26
#define OPT_DOUBLE 27
#define OPT_BW 28
#define OPT_XTERM 29
#define OPT_NOXTERM 30
