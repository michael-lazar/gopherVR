/* config.h 
	vi:ts=3 sw=3:
 */

/* Configuration for the current platform */

//#define MWDEBUG 1
//#include "memwatch.h"
//#include "stdlib.h"


#include <stdio.h>

extern FILE *popen(char *command, char *mode);
extern void pclose(FILE *f);
#define SPECIAL_SAMPLE_MEMORY

#define P(args) args
/* #define P(x) ()	*/

#define ID(x)
/* #define ID(x)  LOCAL char *id= x ; */
/* #define USE_AT_EXIT */

#define KLUDGE_TAG
/* #define GZIP 		*/
/* #define FORKING	*/
/*	#define void		*/

typedef void *GENERIC;
/* typedef char *GENERIC; */

#ifdef AMIGA
#else
#ifdef __386BSD__
#else
#define MALLOC_NOT_IN_STDLIB
#endif
#endif
