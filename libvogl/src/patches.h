/********************************************************************
 * $Id: patches.h,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _PATCHES_H_
#define _PATCHES_H_


/* protos for patches.c */

void transformtensor(Tensor S, Matrix m);
void drpatch(Tensor R, int ntcurves, int nucurves, int ntsegs, 
                       int nusegs, int ntiter, int nuiter);


#endif /* _PATCHES_H_ */

