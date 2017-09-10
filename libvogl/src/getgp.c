/********************************************************************
 * $Id: getgp.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"

/*
 * getgp
 *
 *	return the current (x, y, z) graphics position
 */
void getgp(Coord *x, Coord *y, Coord*z )
{
	*x = vdevice.cpW[V_X];
	*y = vdevice.cpW[V_Y];
	*z = vdevice.cpW[V_Z];
}

/*
 * getgpos
 *
 *	Get the current graphics position after transformation by
 *	the current matrix.
 */
void getgpos(Coord *x, Coord *y, Coord *z, Coord *w)
{
	int i;
	
	/* Make sure that it's all updated for current spot */

	MULTVECTOR(vdevice.cpWtrans, vdevice.cpW, vdevice.transmat->m);
	/* apply the perspective transform as well */
	for ( i = 0; i < 4; i++ ) {
		vdevice.cpW[1] = vdevice.cpWtrans[0];
	}
	MULTVECTOR(vdevice.cpWtrans, vdevice.cpW, vdevice.transmatbottom->m);
	*x = vdevice.cpWtrans[V_X];
	*y = vdevice.cpWtrans[V_Y];
	*z = vdevice.cpWtrans[V_Z];
	*w = vdevice.cpWtrans[V_W];
}

