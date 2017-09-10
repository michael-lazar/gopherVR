/********************************************************************
 * $Id: valloc.h,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _VALLOC_H_
#define _VALLOC_H_


/*protos for valloc.c */

void WorldBFree( char *p );
char * vallocate(int size);
char * Cvallocate(unsigned n, int size);

#endif /* _VALLOC_H_ */
