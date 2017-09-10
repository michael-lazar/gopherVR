/********************************************************************
 * $Id: pref.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/


#include "vogl.h"

static	long	px = -1, py = -1, pxs = -1, pys = -1;

/*
 * prefposition
 *
 *	Specify a prefered position for a window that is
 *	under control of a window manager.
 *	Position is the location of the upper left corner.
 *	Should be called before ginit.
 */
void prefposition(long x1, long x2, long y1, long y2) 
{
	if (x1 < 0 || x2 < 0)
		verror("prefposition: bad x value");

	if (y1 < 0 || y2 < 0)
		verror("prefposition: bad y value");


	px = x1;
	py = y1;
	pxs = x2 - x1;
	pys = y2 - y1;
	if (pxs <= 0 || pys <= 0)
		verror("prefposition: bad window size");
}

/*
 * prefsize
 *
 *	Specify the prefered size for a window under control of
 *	a window manager.
 *	Should be called before ginit.
 */
void prefsize(long x, long y)
{
	if (x < 0)
		verror("prefsize: bad x value");

	if (y < 0)
		verror("prefsize: bad y value");

	pxs = x;
	pys = y;
}

/*
 * getprefposandsize
 *
 *	Returns the prefered position and size of a window under
 *	control of a window manager. (-1 for unset parameters)
 */
void getprefposandsize(int *x, int *y, int *xs, int *ys)
{
	*x = px;
	*y = py;
	*xs = pxs;
	*ys = pys;
}

