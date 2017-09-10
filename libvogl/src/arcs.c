/********************************************************************
 * $Id: arcs.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif

static int	nsegs = 32;



/*
 * arcprecision
 *
 *	sets the number of segments in an arc or circle.
 *	- obsolete function.
 */
 
void arcprecision(int noseg)
{
	nsegs = noseg;
}

/*
 * circleprecision
 *
 *	sets the number of segments in an arc or circle.
 */
void circleprecision(int noseg)
{
	nsegs = noseg;
}

/*
 * arc
 *
 * draw an arc at a given location.  Precision of arc (# line segments)
 * is calculated from the value given to circleprecision.
 *
 */
void arc(Coord x, Coord y, Coord radius, Angle sang, Angle eang)
{
     float	cx, cy, dx, dy;
     float	startang, endang, deltang, cosine, sine, angle;
     int	i, numsegs;
#ifdef UNIX
	int sync;
#endif

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("arc: vogl not initialised");
#endif

	startang = (float)sang / 10.0;
	endang = (float)eang / 10.0;

	angle = startang * D2R;
	numsegs = (endang - startang) / 360.0 * nsegs + 0.5;
	deltang = (endang - startang) * D2R / numsegs;
	cosine = cos((double)deltang);
	sine = sin((double)deltang);

	if (vdevice.inobject) {
	     Token *t = newtokens(8);
	     t[0].i = ARC;
	     t[1].f = x;
	     t[2].f = y;
	     t[3].f = radius * cos((double)angle);
	     t[4].f = radius * sin((double)angle);
	     t[5].f = cosine;
	     t[6].f = sine;
	     t[7].i = numsegs;
	     return;
	}

#ifdef UNIX
	if ((sync = vdevice.sync))
		vdevice.sync = 0;
#endif

	/* calculates initial point on arc */

	cx = x + radius * cos((double)angle);
	cy = y + radius * sin((double)angle);
	move2(cx, cy);

	for (i = 0; i < numsegs; i++)  {
		dx = cx - x; 
		dy = cy - y;
		cx = x + dx * cosine - dy * sine;
		cy = y + dx * sine + dy * cosine;
		draw2(cx, cy);
	}

#ifdef UNIX
	if (sync) {
		vdevice.sync = 1;
		(*vdevice.dev.Vsync)();
	}
#endif

}

/*
 * arcs
 *
 * draw an arc at a given location.  (Expressed as short integers) 
 * Precision of arc (# line segments) is calculated from the value
 * given to circleprecision.
 *
 */
void arcs(Scoord x, Scoord y, Scoord radius, Angle sang, Angle eang)
{
	arc((Coord)x, (Coord)y, (Coord)radius, sang, eang);
}

/*
 * arci
 *
 * draw an arc at a given location.  (Expressed as integers) 
 * Precision of arc (# line segments) is calculated from the value
 * given to circleprecision.
 *
 */
void arci(Icoord x, Icoord y, Icoord radius, Angle sang, Angle eang)
{
	arc((Coord)x, (Coord)y, (Coord)radius, sang, eang);
}

/*
 * arcf
 *
 *	draw a filled sector in a given location. The number of line
 * segments in the arc of the segment is the same as in arc.
 */
void arcf( Coord x, Coord y, Coord radius, Angle sang, Angle eang)
{
	float	cx, cy, dx, dy;
	float	deltang, cosine, sine, angle;
	int	i, numsegs;
	float	startang, endang;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("arcf: vogl not initialised");
#endif


	startang = sang / 10.0;
	endang = eang / 10.0;

	angle = startang * D2R;
	numsegs = (endang - startang) / 360.0 * nsegs + 0.5;
	deltang = (endang - startang) * D2R / numsegs;
	cosine = cos((double)deltang);
	sine = sin((double)deltang);

	if (vdevice.inobject) {
	     Token *t = newtokens(8);
	     t[0].i = ARCF;
	     t[1].f = x;
	     t[2].f = y;
	     t[3].f = radius * cos((double)angle);
	     t[4].f = radius * sin((double)angle);
	     t[5].f = cosine;
	     t[6].f = sine;
	     t[7].i = numsegs;
	     return;
	}
	
	pmv2(x, y);
			/* calculates initial point on arc */

	cx = x + radius * cos((double)angle);
	cy = y + radius * sin((double)angle);

	pdr2(cx, cy);

	for (i = 0; i < numsegs; i++)  {
		dx = cx - x; 
		dy = cy - y;
		cx = x + dx * cosine - dy * sine;
		cy = y + dx * sine + dy * cosine;
		pdr2(cx, cy);
	}

	pclos();
}

/*
 * arcfs
 *
 * draw a filled sector at a given location.  (Expressed as short integers) 
 * Precision of arc (# line segments) is calculated from the value
 * given to circleprecision.
 *
 */
void arcfs(Scoord x, Scoord y, Scoord radius, Angle sang, Angle eang)
{
	arcf((Coord)x, (Coord)y, (Coord)radius, sang, eang);
}

/*
 * arcfi
 *
 * draw a filled sector at a given location.  (Expressed as integers) 
 * Precision of arc (# line segments) is calculated from the value
 * given to circleprecision.
 *
 */
void arcfi( Icoord x, Icoord y, Icoord radius, Angle sang, Angle  eang)
{
	arcf((Coord)x, (Coord)y, (Coord)radius, sang, eang);
}


/*
 * circ
 *
 * Draw a circle of given radius at given world coordinates. The number of
 * segments in the circle is the same as that of an arc.
 *
 */
void circ( Coord x, Coord y, Coord radius)
{
	float	cx, cy, dx, dy;
	float	angle, cosine, sine;
	int	i;
	
#ifdef UNIX
	int sync;
#endif

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("circ: vogl not initialised");
#endif


	angle = 2.0 * PI / nsegs;
	cosine = cos((double)angle);
	sine = sin((double)angle);

	if (vdevice.inobject) {
	     Token *t = newtokens(7);
	     t[0].i = CIRCLE;
	     t[1].f = x;
	     t[2].f = y;
	     t[3].f = radius;
	     t[4].f = cosine;
	     t[5].f = sine;
	     t[6].i = nsegs;
	     return;
	}

	cx = x + radius;
	cy = y;

#ifdef UNIX
	if ((sync = vdevice.sync))
		vdevice.sync = 0;
#endif

	move2(cx, cy);
	for (i = 0; i < nsegs; i++) {
		dx = cx - x; 
		dy = cy - y;
		cx = x + dx * cosine - dy * sine;
		cy = y + dx * sine + dy * cosine;
		draw2(cx, cy);
	}

#ifdef UNIX
	if (sync) {
		vdevice.sync = 1;
		draw2(x + radius, y);
	}
#endif
}

/*
 * circs
 *
 * Draw a circle of given radius at given world coordinates expressed as
 * short integers. The number of segments in the circle is the same as that
 * of an arc.
 *
 */
void circs( Scoord x, Scoord y, Scoord radius)
{
	circ((Coord)x, (Coord)y, (Coord)radius);
}


/*
 * circi
 *
 * Draw a circle of given radius at given world coordinates expressed as
 * integers. The number of segments in the circle is the same as that
 * of an arc.
 *
 */
void circi( Icoord x, Icoord y, Icoord radius)
{
	circ((Coord)x, (Coord)y, (Coord)radius);
}

/*
 * circf
 *
 * Draw a filled circle of given radius at given world coordinates.
 * The number of segments in the circle is the same as that of an arc.
 *
 */
void circf( Coord x, Coord y, Coord radius)
{
	float	cx, cy, dx, dy;
	float	angle, cosine, sine;
	int	i;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("circf: vogl not initialised");
#endif


	angle = 2.0 * PI / nsegs;
	cosine = cos((double)angle);
	sine = sin((double)angle);

	if (vdevice.inobject) {
	     Token *t = newtokens(7);
	     t[0].i = CIRCF;
	     t[1].f = x;
	     t[2].f = y;
	     t[3].f = radius;
	     t[4].f = cosine;
	     t[5].f = sine;
	     t[6].i = nsegs;
	     return;
	}

	cx = x + radius;
	cy = y;

	pmv2(cx, cy);
	for (i = 0; i < nsegs; i++) {
		dx = cx - x; 
		dy = cy - y;
		cx = x + dx * cosine - dy * sine;
		cy = y + dx * sine + dy * cosine;
		pdr2(cx, cy);
	}

	pclos();
}

/*
 * circfs
 *
 * Draw a circle of given radius at given world coordinates expressed as
 * short integers. The number of segments in the circle is the same as that
 * of an arc.
 *
 */
void circfs( Scoord x, Scoord y, Scoord radius)
{
	circf((Coord)x, (Coord)y, (Coord)radius);
}

/*
 * circfi
 *
 * Draw a circle of given radius at given world coordinates expressed as
 * integers. The number of segments in the circle is the same as that
 * of an arc.
 *
 */
void circfi(Icoord x, Icoord y, Icoord radius)
{
	circf((Coord)x, (Coord)y, (Coord)radius);
}

