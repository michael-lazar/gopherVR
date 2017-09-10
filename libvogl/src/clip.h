/********************************************************************
 * $Id: clip.h,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _CLIP_H_
#define _CLIP_H_


/* prototypes for clip.c */

void clip(register Vector p0, register Vector p1);

int MakeEdgeCoords(int i,Vector p);

void quickclip(register const Vector p0, register Vector p1);


#endif /* _CLIP_H_ */
