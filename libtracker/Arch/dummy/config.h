/* config.h 
	vi:ts=3 sw=3:
 */

/* Configuration for AIX platforms */

#define USE_SGTTY
#define SIGNED			/* This is really needed! */

typedef void *GENERIC;

/* Don't use protos, because gcc doesn't like structures in parameters
 * and mixing protos with non-protos style */
#define P(x) ()

/* #define ID(x) */
#define ID(x)  LOCAL char id[] = x ;
