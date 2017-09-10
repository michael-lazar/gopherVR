/* getopt.c 
	vi:ts=3 sw=3:
 */

/* $Id: getopt.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: getopt.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.6  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.5  1995/02/01  16:39:04  espie
 * Moved includes to defs.h
 *
 * Revision 1.5  1993/12/04  16:12:50  espie
 * New getopt semantics.
 */

#include <ctype.h>

#include "defs.h"
#include "getlongopt.h"

ID("$Id: getopt.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")

int optind = 1;
char *optarg = 0;
LOCAL not_an_option = 0;

LOCAL int parse_option(argv, option)
char *argv[];
struct long_option *option;
	{
	optind++;
	if (option->argn)
		optarg = argv[optind++];
	return option->code;
	}

int getlongopt(argc, argv, options)
int argc;
char *argv[];
struct long_option *options;
	{
	if (not_an_option == optind)
		return -1;
	if (optind >= argc)
		return -1;
	if (argv[optind][0] == '-')
		{
		char *match = argv[optind]+1;
		if (strlen(match) == 1)
			{
			if (match[0] == '-')
				{
				not_an_option = ++optind;
				return -1;
				}
			while(options->fulltext)
				{
				if (options->abbrev == match[0])
					return parse_option(argv, options);
				else
					options++;
				}
			return -1;
			}
		else
			{
			int max_match = 0;
			struct long_option *best = 0;

			while (options->fulltext)
				{
				int i;
				for (i = 0; ; i++)
					{
					if (options->fulltext[i] == 0 && match[i] == 0)
						return parse_option(argv, options);
					if (match[i] == 0)
						{
						if (i > max_match)
							{
							max_match = i;
							best = options;
							}
						break;
						}
					if (tolower(options->fulltext[i]) != tolower(match[i]))
						break;
					}
				options++;
				}
			if (max_match < 3)
				{
				fprintf(stderr, "Unrecognized option: %s\n", match);
				return -1;
				}
			return parse_option(argv, best);
			}
		}
	else
		return -1;
	}
