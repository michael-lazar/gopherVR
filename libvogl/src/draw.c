/********************************************************************
 * $Id: draw.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include "vogl.h"
#include "clip.h"

/*
 * draw
 *
 * draw a line form the logical graphics position to the
 * the world coordinates x, y, z.
 *
 */
void draw(register float x, register float y, 
	  register float z)
{
     register Mstack *ms;
     register float w;
     Vector	res;

     if (vdevice.inobject) 
     {
	  Token	*tok = newtokens(4);
	  
	  vdevice.cpW[V_X] = x;
	  vdevice.cpW[V_Y] = y;
	  vdevice.cpW[V_Z] = z;

	  tok[0].i = DRAW;
	  tok[1].f = x;
	  tok[2].f = y;
	  tok[3].f = z;
	  
	  vdevice.cpVvalid = 0;
	  
	  return;
     }


/* Multiply through by the top transformation matrix.
   Note that the matrix is affine and w = 1.0 */

     ms = vdevice.transmat;
     vdevice.cpW[V_X] = x * ms->m[0][0] + y * ms->m[1][0] +
	  z * ms->m[2][0] + ms->m[3][0];
     vdevice.cpW[V_Y] = x * ms->m[0][1] + y * ms->m[1][1] +
	  z * ms->m[2][1] + ms->m[3][1];
     vdevice.cpW[V_Z] = x * ms->m[0][2] + y * ms->m[1][2] +
	  z * ms->m[2][2] + ms->m[3][2];
     vdevice.cpW[V_W] =  ms->m[3][3];


     ms = vdevice.transmatbottom;
     x = vdevice.cpW[V_X];
     y = vdevice.cpW[V_Y];
     z = vdevice.cpW[V_Z];
     w = vdevice.cpW[V_W];
     /** We remove some mults because of the way the bottom matrix is
       constructed  (See MULTVECTOR_BOTTOM() in vogl.h)**/

     res[0] = x * ms->m[0][0] + z * ms->m[2][0] + w * ms->m[3][0];
     res[1] = y * ms->m[1][1] +	z * ms->m[2][1] + w * ms->m[3][1];
     res[2] = z * ms->m[2][2] + w * ms->m[3][2];
     res[3] = z * ms->m[2][3] + w * ms->m[3][3];


     if (vdevice.cpVvalid)
	  quickclip(vdevice.cpWtrans, res);
     else
	  clip(vdevice.cpWtrans, res);
     
     COPYVECTOR( vdevice.cpWtrans, res);
     
}





/*
 * draws
 *
 * draw a line form the logical graphics position to the
 * the world coordinates x, y, z expressed as a short integer data type.
 *
 */
void draws(Scoord x, Scoord y, Scoord z)
{
	draw((Coord)x, (Coord)y, (Coord)z);
}


/*
 * drawi
 *
 * draw a line form the logical graphics position to the
 * the world coordinates x, y, z expressed as an integer data type.
 *
 */
void drawi(Icoord x, Icoord y, Icoord z)
{
	draw((Coord)x, (Coord)y, (Coord)z);
}



/*
 * draw2
 *
 * draw a line from the logical graphics position  to the
 * the world coordinates x, y.
 *
 */
void draw2(float x, float y)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("draw2: vogl not initialised");
#endif


	draw(x, y, 0.0);
}

/*
 * draw2s
 *
 * draw a line from the logical graphics position  to the
 * the world coordinates x, y expressed as a short integer data type.
 *
 */
void draw2s(Scoord x, Scoord y)
{
	draw2((Coord)x, (Coord)y);
}


/*
 * draw2i
 *
 * draw a line from the logical graphics position  to the
 * the world coordinates x, y expressed as an integer data type.
 *
 */
void draw2i(Icoord x, Icoord y)
{
	draw2((Coord)x, (Coord)y);
}

/*
 * rdr
 *
 * 3D relative draw from the logical graphics position by dx, dy, dz.
 * This is the same as the VOGLE routine rdraw.
 *
 */
void rdr(float dx, float dy, float dz)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised) 
		verror("rdr: vogl not initialised");
#endif


	draw((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), (vdevice.cpW[V_Z] + dz));
}

/*
 * rdrs
 *
 * 3D relative draw from the logical graphics position by dx, dy, dz
 * expressed as a short integer data type.
 *
 */
void rdrs(Scoord dx, Scoord dy, Scoord dz)
{
	rdr((Coord)dx, (Coord)dy, (Coord)dz);
}

/*
 * rdri
 *
 * 3D relative draw from the logical graphics position by dx, dy, dz
 * expressed as an integer data type.
 *
 */
void rdri(Icoord dx, Icoord dy, Icoord dz)
{
	rdr((Coord)dx, (Coord)dy, (Coord)dz);
}


/*
 * rdr2
 *
 * 2D relative draw from the logical graphics position by dx, dy.
 * This is the same as the VOGLE routine rdraw2.
 *
 */
void rdr2(float dx, float dy)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised) 
		verror("rdr2: vogl not initialised");
#endif


	draw((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), 0.0);
}


/*
 * rdr2s
 *
 * 3D relative draw from the logical graphics position by dx, dy
 * expressed as a short integer data type.
 *
 */
void rdr2s(Scoord dx, Scoord dy)
{
	rdr2((Coord)dx, (Coord)dy);
}

/*
 * rdr2i
 *
 * 3D relative draw from the logical graphics position by dx, dy
 * expressed as an integer data type.
 *
 */
void rdr2i(Icoord dx, Icoord dy)
{
	rdr2((Coord)dx, (Coord)dy);
}


/*
 * bgnline
 *
 * 	Flags that all v*() routine points will be building up a line list.
 */
void bgnline( void )
{
	if (vdevice.bgnmode != NONE)
		verror("vogl: bgnline mode already belongs to some other bgn routine");

	vdevice.bgnmode = VLINE;
	vdevice.save = 1;
}

/*
 * endline
 *
 * 	Flags that all v*() routine points will be simply setting the 
 * 	current position.
 */
void endline( void )
{
	vdevice.bgnmode = NONE;
	vdevice.save = 0;
}

/*
 * bgnclosedline
 *
 * 	Flags that all v*() routine points will be building up a line list.
 */
void bgnclosedline(void)
{
	if (vdevice.bgnmode != NONE)
		verror("vogl: bgncloseline mode already belongs to some other bgn routine");

	vdevice.bgnmode = VCLINE;
	vdevice.save = 1;
}

/*
 * endclosedline
 *
 * 	Flags that all v*() routine points will be simply setting the 
 * 	current position.
 */
void endclosedline(void)
{
	vdevice.bgnmode = NONE;

	draw(vdevice.savex, vdevice.savey, vdevice.savez);
}
