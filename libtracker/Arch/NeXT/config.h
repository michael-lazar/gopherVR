/* config.h 
	vi:ts=3 sw=3:
 */

/* Configuration for the NeXT platform */

#define USE_SGTTY		/* NeXTStep 3.2 is not (well) enough POSIX */

typedef void *GENERIC;

#define P(args) args
#define ID(x)

#define stricmp	strcasecmp
