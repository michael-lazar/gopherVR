/********************************************************************
 * $Id: mapping.c,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"

float	Vcx, Vcy, Vsx, Vsy;

/*
 * calcW2Vcoeffs
 *
 *	Calculate the linear coeffs defining the mapping of world
 *	space to actual device space
 *
 * Add .5 to it since it gets added all the time for rounding in WtoVx and
 * WtoVy
 */
void CalcW2Vcoeffs(void)
{
	Vcx = (float)(vdevice.maxVx + vdevice.minVx) * 0.5 + 0.5;
	Vcy = (float)(vdevice.maxVy + vdevice.minVy) * 0.5 + 0.5;

	Vsx = (float)(vdevice.maxVx - vdevice.minVx) * 0.5;
	Vsy = (float)(vdevice.maxVy - vdevice.minVy) * 0.5;
}

/*
 * WtoVx
 *
 * return the Screen X coordinate corresponding to world point 'p' 
 * (does the perspective division as well)
 */
int WtoVx(float p[])
{
	return((int)(p[0] * Vsx / p[3] + Vcx)); 
}

/*
 * WtoVy
 *
 * return the Screen Y coordinate corresponding to world point 'p' 
 * (does the perspective division as well)
 */
int WtoVy(float p[])
{
	return((int)(p[1] * Vsy / p[3] + Vcy)); 
}

