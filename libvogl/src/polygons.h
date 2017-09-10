/********************************************************************
 * $Id: polygons.h,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _POLYGONS_H_
#define _POLYGONS_H_


/* protos for polygons.c */


int F_polySurfaceDesc( P_POLYLIST_LEAF pPll_polygon, Vector arVer_orig[] );
void dopoly(const P_POLYLIST_LEAF pPll_polygon );
int isPointInPoly( int ScreenX, int ScreenY, P_POLYLIST_LEAF pPll_polygon );
void polyobj(int n, Token dp[], int fill);
void bgntmesh(void);
void swaptmesh(void);
void endtmesh( void );
void bgnqstrip( void );
void endqstrip( void );


#endif /* _POLYGONS_H_ */
