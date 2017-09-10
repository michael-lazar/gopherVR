/********************************************************************
 * $Id: rect.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"

/*
 * rect
 *
 * draw a rectangle given two opposite corners
 *
 */
void rect(Coord x1, Coord y1, Coord x2, Coord y2)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("rect: vogl not initialised");
#endif


	move2(x1, y1);
	draw2(x2, y1);
	draw2(x2, y2);
	draw2(x1, y2);
	draw2(x1, y1);
}

/*
 * recti
 *
 * draw a rectangle given two opposite corners (expressed as integers)
 */
void recti(Icoord x1, Icoord y1, Icoord x2, Icoord y2)
{
	rect((Coord)x1, (Coord)y1, (Coord)x2, (Coord)y2);
}

/*
 * rects
 *
 * draw a rectangle given two opposite corners (expressed as short integers)
 */
void rects(Scoord x1, Scoord y1, Scoord x2, Scoord y2)
{
	rect((Coord)x1, (Coord)y1, (Coord)x2, (Coord)y2);
}

/*
 * rectf
 *
 * draw a filled rectangle given two opposite corners
 *
 */
void rectf(Coord x1, Coord y1, Coord x2, Coord y2)
{
#ifdef VDEVICECHECK
     if (!vdevice.initialised)
	  verror("rect: vogl not initialised");
#endif
     

     if (vdevice.inobject) {
	  Token *tok = newtokens(5);
	  tok[0].i = RECTF;
	  tok[1].f = x1;
	  tok[2].f = y1;
	  tok[3].f = x2;
	  tok[4].f = y2;
	  return;
     }
     
     pmv2(x1, y1);
     pdr2(x2, y1);
     pdr2(x2, y2);
     pdr2(x1, y2);
     pdr2(x1, y1);
     pclos();
}


/*
 * rectfi
 *
 * draw a filled rectangle given two opposite corners (expressed as integers)
 */
void rectfi(Icoord x1, Icoord y1, Icoord x2, Icoord y2)
{
     rectf((Coord)x1, (Coord)y1, (Coord)x2, (Coord)y2);
}

/*
 * rectfs
 *
 * draw a filled rectangle given two opposite corners (expressed as short
 * integers)
 */
void rectfs(Scoord x1, Scoord y1, Scoord x2, Scoord y2)
{
	rectf((Coord)x1, (Coord)y1, (Coord)x2, (Coord)y2);
}

