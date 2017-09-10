/********************************************************************
 * $Id: drivers.h,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _DRIVERS_H_
#define _DRIVERS_H_


/* protos for drivers.c */

extern	V_ZeroMatrix( Matrix a );

FILE * _voutfile( void );
void viniterror(char *str);
long getcolor( void );
void reshapeviewport(void);
void winconstraints(void);
void keepaspect(void);
void foreground(void );
void vsetflush(int yn);
void vflush(void );
void getorigin(long *x, long *y);
void getsize(long *x, long *y);

#endif /* _DRIVERS_H_ */
