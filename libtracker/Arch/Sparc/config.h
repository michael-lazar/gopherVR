/* config.h 
	vi:ts=3 sw=3:
 */

/* Configuration for Sparc */

#ifdef __svr4__
#define SOLARIS
#define IS_POSIX
#endif

#define COLOR_IS_DEFAULT
#define USE_TERMIOS
#define XTERM
#undef USE_SGTTY		/* You can use this, but it's discouraged */
#include <stdio.h>


/* #define SOLARIS */
#define P(args) args
/* #define P(x) ()	*/

#define ID(x)
/* #define ID(x)  LOCAL char id[]= x ; */
/* #define USE_AT_EXIT */

#define KLUDGE_TAG 		/* not implemented yet */
#define GZIP 		
/* #define FORKING	*/
/*	#define void		*/

#define stricmp	strcasecmp

typedef void *GENERIC;
/* typedef char *GENERIC; */

#define MALLOC_NOT_IN_STDLIB
#if defined(SOLARIS) && defined(USE_SGTTY)
#define BSD_COMP
#endif
