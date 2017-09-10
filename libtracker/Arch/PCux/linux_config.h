/* config.h 
	vi:ts=3 sw=3:
 */

/* Configuration for linux */

#undef IS_POSIX			/* I have no idea */
#define USE_TERMIOS
#define USE_AT_EXIT

typedef void *GENERIC;

/* Don't use protos, because gcc doesn't like structures in parameters
 * and mixing protos with non-protos style */
/* #define P(args) args */
#define P(x) ()

/* #define ID(x) */
#define ID(x)  LOCAL char id[] = x ;

#define stricmp strcasecmp
