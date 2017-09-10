/********************************************************************
 * $Id: points.c,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"

/*
 * pnt
 *
 * plot a point in x, y, z.
 * This is just the old VOGLE routine point.
 *
 */
void pnt(float x, float y, float z)
{

#ifdef VDEVICECHECK
	if(!vdevice.initialised)
		verror("pnt: vogl not initialised");
#endif


	move(x, y, z);  
	draw(x, y, z);	
}


/*
 * pnts
 *
 * plot a point (short integer version)
 *
 */
void pnts(Scoord x, Scoord y, Scoord z)
{
	pnt((Coord)x, (Coord)y, (Coord)z);
}

/*
 * pnti
 *
 * plot a point (short integer version)
 *
 */
void pnti(Icoord x, Icoord y, Icoord z)
{
	pnt((Coord)x, (Coord)y, (Coord)z);
}

/*
 * pnt2
 *
 * plot a point in x, y.
 *
 */
void pnt2(Coord x, Coord y)
{
	move(x, y, 0.0);
	draw(x, y, 0.0);
}

/*
 * pnt2s in x, y
 *
 * plot a point (short integer version)
 *
 */
void pnt2s(Scoord x, Scoord y)
{
	pnt2((Coord)x, (Coord)y);
}

/*
 * pnt2i in x, y
 *
 * plot a point (short integer version)
 *
 */
void pnt2i(Icoord x, Icoord y)
{
	pnt2((Coord)x, (Coord)y);
}

/*
 * bgnpoint
 *
 * 	Flags that all v*() routine points will be building up a point list.
 */
void bgnpoint(void)
{
	if (vdevice.bgnmode != NONE)
		verror("vogl: bgnpoint mode already belongs to some other bgn routine");

	vdevice.bgnmode = VPNT;
	vdevice.save = 0;
}

/*
 * endpoint
 *
 * 	Flags that all v*() routine points will be simply setting the 
 * 	current position.
 */
void endpoint(void)
{
	vdevice.bgnmode = NONE;
	vdevice.save = 0;
}
