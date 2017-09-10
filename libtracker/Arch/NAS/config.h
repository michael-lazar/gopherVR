/* config.h 
	vi:ts=3 sw=3:
 */

/* Configuration for Network Audio, SysVR4 */


#define IS_POSIX
#define USE_TERMIOS

/* Don't use protos, because gcc doesn't like structures in parameters
 * and mixing protos with non-protos style */
/* #define P(args) args */
#define P(x) ()

#define ID(x) 

#define USE_AT_EXIT

/* #define KLUDGE_TAG */

typedef void *GENERIC;

#undef MALLOC_NOT_IN_STDLIB
