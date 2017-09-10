/* randomize.c 
	vi:ts=3 sw=3:
 */

/* $Id: randomize.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $ 
 * $Log: randomize.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.13  1995/03/01  15:24:51  espie
 * cleanup.
 *
 * Revision 4.12  1995/02/24  13:47:56  espie
 * Minor change for FreeBSD
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.6  1995/02/01  20:41:45  espie
 * Added color.
 *
 */

/* input: a series of names (as argv[1:argc - 1])
 * output: the same names, in a random order.
 * with the new database lookup facility, very useful for e.g.,
 * tracker `randomize *` (jukebox)
 */

#include "defs.h"
#include <sys/types.h>
#ifdef IS_POSIX
#include <time.h>
#else
#include <sys/time.h>
#endif

ID("$Id: randomize.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")

/* n = random_range(max): output a number in the range 0:max - 1.
 * For our purpose, we don't have to get a very random number,
 * so the standard generator is alright.
 */
int random_range(max)
int max;
    {
    static init = 0;

        /* initialize the generator to an appropriate seed eventually */
    if (!init)
        {
        srand(time(0));
        init = 1;
        }
    return rand()%max;
    }

/* output(s): output s in a suitable format. Ideally, output() should use
 * the shell quoting conventions for difficult names. Right now, it doesn't
 */
void output(s)
char *s;
    {
    for(; *s; s++)
        switch(*s)
            {
    /*    case ' ':
        case '(':
        case ')':
        case '\\':
            putchar('\\');
            */
        default:
            putchar(*s);
            }
    putchar(' ');
    }

int main(argc, argv)
int argc;
char *argv[];
    {
    int i, k;

        /* set up everything so that our names are in argv[0 : argc - 2] */
    for (i = argc - 1, argv++; i; i--)
        {
            /* invariant: the remaining names are in argv[0: i - 1] */
        k = random_range(i);
        output(argv[k]);
        argv[k] = argv[i - 1];
        }
   exit(0);
    }
