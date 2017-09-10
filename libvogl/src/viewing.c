/********************************************************************
 * $Id: viewing.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
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


/*
 * NOTE: the words hither and yon are used in this file instead of near and far
 * as they are keywords on some PC compilers (groan). Change them back at your 
 * on peril.
 */

/*#define COT(a)	((float)(cos((double)(a)) / sin((double)(a))))*/
#define COT(a)	((float)(cos((a)) / sin((a))))


static void normallookat(float vx, float vy, float vz, float px, float py, float pz);


/*
 * polarview
 *
 * Specify the viewer's position in polar coordinates by giving
 * the distance from the viewpoint to the world origin, 
 * the azimuthal angle in the x-y plane, measured from the y-axis,
 * the incidence angle in the y-z plane, measured from the z-axis,
 * and the twist angle about the line of sight. Note: that unlike
 * in VOGLE we are back to tenths of degrees.
 *
 */
void polarview(Coord dist, Angle azim, Angle inc, Angle twist)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised) 
		verror("polarview: vogl not initialised");
#endif


	translate(0.0, 0.0, -dist);
	rotate(-twist, 'z');
	rotate(-inc, 'x');
	rotate(-azim, 'z');
}

/*
 * normallookat
 *
 *	do the standard lookat transformation.
 */
static void normallookat(float vx, float vy, float vz, float px, float py, float pz)
{
	float	l2, l3, sintheta, sinphi, costheta, cosphi;
	Matrix	tmp;

	l2 = sqrt((double)(SQ((px - vx)) + SQ((pz - vz))));
	l3 = sqrt((double)(SQ((px - vx)) + SQ((py - vy)) + SQ((pz - vz))));

	if (l3 != 0.0) {
		sinphi = (vy - py) / l3;
		cosphi = l2 / l3;

		/*
		 * Rotate about X by phi
		 */
		identmatrix(tmp);
		tmp[1][1] = tmp[2][2] = cosphi;
		tmp[1][2] = sinphi;
		tmp[2][1] = -sinphi;
		multmatrix(tmp);
	}

	if (l2 != 0.0) {
		sintheta = (px - vx) / l2;
		costheta = (vz - pz) / l2;

		/*
		 * Rotate about Y by theta
		 */
		identmatrix(tmp);
		tmp[0][0] = tmp[2][2] = costheta;
		tmp[0][2] = -sintheta;
		tmp[2][0] = sintheta;
		multmatrix(tmp);
	}
}

/*
 * lookat
 *
 * Specify the viewer's position by giving a viewpoint and a 
 * reference point in world coordinates. A twist about the line
 * of sight may also be given.  Note that unlike in VOGLE we are
 * back to tenths of degrees.
 */
void lookat(Coord vx, Coord vy, Coord vz, Coord px, Coord py, Coord pz, Angle twist)
{

#ifdef VDEVICECHECK
	if (!vdevice.initialised) 
		verror("lookat: vogl not initialised");
#endif


	rotate(-twist, 'z');

	normallookat(vx, vy, vz, px, py, pz);

	translate(-vx, -vy, -vz);

	
}

/*
 * perspective
 *
 * Specify a perspective viewing pyramid in world coordinates by
 * giving a field of view, aspect ratio, and the locations of the 
 * near(hither) and far(yon) clipping planes in the z direction. Note
 * that unlike in VOGLE we are back to tenths of degrees.
 */
void perspective(Angle ifov, float aspect, Coord hither, Coord yon)
{
	Matrix		mat;
	float		fov;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("perspective: vogl not initialised");
#endif


	if (aspect == 0.0)
		verror("perspective: can't have zero aspect ratio!");

	if ((yon - hither) == 0.0)
		verror("perspective: near clipping plane same as far one.");

	if (ifov == 0 || ifov == 1800)
		verror("perspective: bad field of view passed.");

	fov = ifov / 10.0;
	identmatrix(mat);

	mat[0][0] = COT((D2R * fov / 2.0)) / aspect;
	mat[1][1] = COT((D2R * fov / 2.0));

	mat[2][2] = -(yon + hither) / (yon - hither);
	mat[2][3] = -1;
	mat[3][2] = -2.0 * yon * hither / (yon - hither);
	mat[3][3] = 0;

	/* we aren't going to want to apply this transform right away  
	so push it on the bottom of the stack*/
	COPYMATRIX( vdevice.transmatbottom->m, mat );

	identmatrix(mat);
	loadmatrix(mat);
}

/*
 * window
 *
 * Specify a perspective viewing pyramid in world coordinates by
 * giving a rectangle at the near clipping plane and the location
 * of the far clipping plane.
 *
 */
void window(Coord left, Coord right, Coord bottom, Coord top, Coord hither, Coord yon)
{
	Matrix		mat;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("window: vogl not initialised");
#endif


	if ((right - left) == 0.0)
		verror("window: left clipping plane same as right one.");

	if ((top - bottom) == 0.0)
		verror("window: bottom clipping plane same as top one.");

	if ((yon - hither) == 0.0)
		verror("window: near clipping plane same as far one.");

	identmatrix(mat);

	mat[0][0] = 2.0 * hither / (right - left);
	mat[1][1] = 2.0 * hither / (top - bottom);
	mat[2][0] = (right + left) / (right - left);
	mat[2][1] = (top + bottom) / (top - bottom);
	mat[2][2] = -(yon + hither) / (yon - hither);
	mat[2][3] = -1.0;
	mat[3][2] = -2.0 * yon * hither / (yon - hither);
	mat[3][3] = 0.0;

	/* we aren't going to want to apply this transform right away  
	so push it on the bottom of the stack*/
	COPYMATRIX( vdevice.transmatbottom->m, mat );

	identmatrix(mat);
	loadmatrix(mat);	
}

/*
 * ortho
 *
 * Define a three dimensional viewing box by giving the left,
 * right, bottom and top clipping plane locations and the distances
 * along the line of sight to the near and far clipping planes.
 *
 */
void ortho(Coord left, Coord right, Coord bottom, Coord top, Coord hither, Coord yon)
{
	Matrix		mat;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("ortho: vogl not initialised");
#endif


	if ((right - left) == 0.0)
		verror("ortho: left clipping plane same as right one.");

	if ((top - bottom) == 0.0)
		verror("ortho: bottom clipping plane same as top one.");

	if ((yon - hither) == 0.0)
		verror("ortho: near clipping plane same as far one.");

	identmatrix(mat);

	mat[0][0] = 2.0 / (right - left);
	mat[1][1] = 2.0 / (top - bottom);
	mat[2][2] = -2.0 / (yon - hither);
	mat[3][0] = -(right + left) / (right - left);
	mat[3][1] = -(top + bottom) / (top - bottom);
	mat[3][2] = -(yon + hither) / (yon - hither);

	/* we aren't going to want to apply this transform right away  
	so push it on the bottom of the stack*/
	COPYMATRIX( vdevice.transmatbottom->m, mat );

	identmatrix(mat);
	loadmatrix(mat);	
}

/*
 * ortho2
 *
 * Specify a two dimensional viewing rectangle. 
 *
 */
void ortho2(Coord left, Coord right, Coord bottom, Coord top)
{
	Matrix	mat;

#ifdef VDEVICECHECK
	if (!vdevice.initialised) 
		verror("ortho2: vogl not initialised");
#endif


	identmatrix(mat);

	if ((right - left) == 0.0)
		verror("ortho2: left clipping plane same as right one.");

	if ((top - bottom) == 0.0)
		verror("ortho2: bottom clipping plane same as top one.");

	mat[0][0] = 2.0 / (right - left);
	mat[1][1] = 2.0 / (top - bottom);
	mat[2][2] = -1.0;
	mat[3][0] = -(right + left) / (right - left);
	mat[3][1] = -(top + bottom) / (top - bottom);

	loadmatrix(mat);
}
