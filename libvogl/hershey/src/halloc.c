#include <stdio.h>
#include <stdlib.h>
#include "halloc.h"
#include "vogl.h"

/*
 * hallocate
 *
 *	Allocate some memory, barfing if malloc returns NULL.
 */
char * hallocate(int size)
{
	char	*p;

#ifdef MACINTOSH
	p = (char *)NewPtr( size );
	if ( MemError() != noErr ) {
#else
	if ((p = (char *)malloc(size)) == (char *)0) {
#endif
 		char buf[60];
 		
		sprintf(buf,"hallocate: request for %d bytes returned NULL", size);
		verror(buf);
	}
	
	return (p);
}
