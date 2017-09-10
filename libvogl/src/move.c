/********************************************************************
 * $Id: move.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"


/*
 * move
 *
 * Move the logical graphics position to the world coordinates x, y, z.
 *
 */
void move(register const Coord x, register const Coord y,
	  register const Coord z)
{
     register Mstack *ms;

     vdevice.cpW[V_X] = x;
     vdevice.cpW[V_Y] = y;
     vdevice.cpW[V_Z] = z;
     vdevice.cpW[V_W] = 1.0;
     vdevice.cpVvalid = 0;

     if (vdevice.inobject) {
	  Token	*p = newtokens(4);
	  
	  p[0].i = MOVE;
	  p[1].f = x;
	  p[2].f = y;
	  p[3].f = z;
	  
	  return;
     }

     
     ms = vdevice.transmat;
     QMULTVECTOR_AFFINE(vdevice.cpW, vdevice.cpW, ms->m);
/*     COPYVECTOR( vdevice.cpW, vdevice.cpWtrans );*/

     ms = vdevice.transmatbottom;
     QMULTVECTOR_BOTTOM(vdevice.cpWtrans, vdevice.cpW, ms->m);

/*	vdevice.cpVx = WTOVX(vdevice.cpWtrans);
	vdevice.cpVy = WTOVY(vdevice.cpWtrans);
*/
}




/*
 * moves
 *
 * Move the logical graphics position to the world coordinates x, y, z.
 * expressed as a short integer data type.
 *
 */
void moves(Scoord x, Scoord y, Scoord z)
{
	move((Coord)x, (Coord)y, (Coord)z);
}


/*
 * movei
 *
 * Move the logical graphics position to the world coordinates x, y, z.
 * expressed as an integer data type.
 *
 */
void movei(Icoord x, Icoord y, Icoord z)
{
	move((Coord)x, (Coord)y, (Coord)z);
}


/*
 * move2
 *
 * Move the logical graphics position to the world coords x, y, 0.0
 * (I.e. a 2D move is defined as a 3D move with the Z-coord set to zero)
 *
 */
void move2(Coord x, Coord y)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised) 
		verror("move2: vogl not initialised");
#endif


	move(x, y, 0.0);
}

/*
 * move2s
 *
 * Move the logical graphics position to the world coordinates x, y.
 * expressed as a short integer data type.
 *
 */
void move2s(Scoord x, Scoord y)
{
	move2((Coord)x, (Coord)y);
}


/*
 * move2i
 *
 * Move the logical graphics position to the world coordinates x, y.
 * expressed as an integer data type.
 *
 */
void move2i(Icoord x, Icoord y)
{
	move2((Coord)x, (Coord)y);
}


/*
 * rmv
 *
 * move the logical graphics position from the current world 
 * coordinates by dx, dy, dz 
 *
 */
void rmv(Coord dx, Coord dy, Coord dz)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised) 
		verror("rmv: vogl not initialised");
#endif


	move((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), (vdevice.cpW[V_Z] + dz));
}

/*
 * rmvs
 *
 * move the logical graphics position from the current world 
 * coordinates by dx, dy, dz expressed as a short integer data type.
 *
 */
void rmvs(Scoord dx, Scoord dy, Scoord dz)
{
	rmv((Coord)dx, (Coord)dy, (Coord)dz);
}

/*
 * rmvi
 *
 * move the logical graphics position from the current world 
 * coordinates by dx, dy, dz expressed as an integer data type.
 *
 */
void rmvi(Icoord dx, Icoord dy, Icoord dz)
{
	rmv((Coord)dx, (Coord)dy, (Coord)dz);
}

/*
 * rmv2
 *
 * Move Relative in 2D.
 *
 */
void rmv2(float dx, float dy)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised) 
		verror("rmv2: vogl not initialised");
#endif


	move((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), 0.0);
}

/*
 * rmv2s
 *
 * move the logical graphics position from the current world 
 * coordinates by dx, dy expressed as a short integer data type.
 *
 */
void rmv2s(Scoord dx, Scoord dy)
{
	rmv2((Coord)dx, (Coord)dy);
}

/*
 * rmv2i
 *
 * move the logical graphics position from the current world 
 * coordinates by dx, dy expressed as an integer data type.
 *
 */
void rmv2i(Icoord dx, Icoord dy)
{
	rmv2((Coord)dx, (Coord)dy);
}

