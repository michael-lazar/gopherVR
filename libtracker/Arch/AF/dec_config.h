/* config.h 
	vi:ts=3 sw=3:
 */

/* Configuration for the DEC Alpha platform */

/* outdated port, so -> no working user interface */
#define stub_only		

#define IS_POSIX		/* just a guess! */
#define USE_TERMIOS
#define MALLOC_NOT_IN_STDLIB

typedef void *GENERIC;

#define P(args) args
#define ID(x)
