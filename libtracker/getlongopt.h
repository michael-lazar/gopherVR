/* getopt.h 
	vi:ts=3 sw=3:
 */
/* $Id: getlongopt.h,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
 * $Log: getlongopt.h,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.11  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.10  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.5  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.4  1995/02/01  16:39:04  espie
 * Moved includes to defs.h
 *
 */

struct long_option
	{
	char *fulltext;
	int argn;
	char abbrev;
	int code;
	};

/* n = getlongopt(argc, argv, options):
 * try to parse options out of argv, using ways similar to standard getopt.
 * Named getlongopt to avoid conflicts with getopt
 */
XT int getlongopt P((int argc, char *argv[], struct long_option *options));

XT int optind;

XT char *optarg;

