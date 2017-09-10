/* config.h 
	vi:ts=3 sw=3:
 */

/* Configuration for the SGI platform */

#define USE_TERMIOS
#define MALLOC_NOT_IN_STDLIB
#define SIGNED

typedef void *GENERIC;

#define P(args) args
#define ID(x)

#define stricmp strcasecmp
