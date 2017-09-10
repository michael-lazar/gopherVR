/********************************************************************
 * $Id: valloc.c,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "vogl.h"
#include "valloc.h"


/* this function is named in honor of Llyod B. Free.
 * Mr. Free was an NBA basketball player for the 
 * Philadephia 76ers (and other) who changed his first name 
 * to "World" because he had an "All World" 3-point short  
 * and thought it was cool when the announcers referred to 
 * him as "World B Free"
 *
 * no hungarian naming convention here, please 
 * -mpm
 */
void WorldBFree( char *p )
{
#ifdef MACINTOSH
	DisposePtr( (Ptr) p );
#else
	free( p );
#endif
}


/*
 * vallocate
 *
 *	Allocate some memory, barfing if malloc returns NULL.
 */
char * vallocate(int size)
{
	char	*p;

	if (size == 0) {
	     char buf[60];
	     sprintf(buf,"vallocate: can't calloc 0 bytes\n");
	     verror(buf);
   	}

#ifdef MACINTOSH
	p = (char *)NewPtr( size );
	if ( MemError() != noErr ) {
#else
	if ((p = (char *)malloc(size)) == (char *)0) {
#endif
		char buf[60];
		
		sprintf(buf,"vallocate: request for %d bytes returned NULL", size);
		verror(buf);
	}
	
	return (p);
}



/*
 * Cvallocate
 *
 *	Allocate some memory, barfing if calloc returns NULL.
 */
char * Cvallocate(unsigned n, int size)
{
	char	*p;

	if (n==0 || size == 0) {
		char buf[60];
		sprintf(buf,"Cvallocate: can't calloc 0 bytes\n");
		verror(buf);
	   }
	     
#ifdef MACINTOSH
	p = (char *)NewPtr( (n * size) );
	if ( MemError() != noErr ) {
#else	
	if ((p = (char *)calloc(n,size)) == (char *)0) {
#endif
		char buf[60];
		sprintf(buf,"Cvallocate: request for %d bytes returned NULL", (size * n ));
		verror(buf);
	}

	return (p);
}
