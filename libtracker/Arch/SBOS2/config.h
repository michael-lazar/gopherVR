/* config.h 
	vi:ts=3 sw=3:
 */

/* Configuration for OS/2 */

//#define MWDEBUG 1
//#include "memwatch.h"
//#include "stdlib.h"


#ifndef __OS2__			/* now how can that be? */
extern FILE *popen(char *command, char *mode);
extern void pclose(FILE *f);
#endif

#define EXPAND_WILDCARDS(x,y)   _wildcard(&x,&y);

/* use if "rb" has an import for the program */
#define BINARY_HEEDED 

#define USE_TERMIOS
#define MALLOC_NOT_IN_STDLIB
#define KLUDGE_TAG

typedef void *GENERIC;

#define P(args) args
/* #define P(x) ()	*/

#define ID(x)
/* #define ID(x)  LOCAL char *id= x ; */
