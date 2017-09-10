/********************************************************************
 * $Id: minv4.h,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _MINV4_H_
#define _MINV4_H_



/* proto for minv4.c */


void minv4(Matrix a, Matrix b);
void minv4_affine(Matrix out, Matrix in);

#endif /* _MINV4_H_ */
