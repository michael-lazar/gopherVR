/********************************************************************
 * $Id: trans.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
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
 * translate
 * 
 * Set up a translation matrix and premultiply it and 
 * the top matrix on the stack.
 *
 */
void translate(register float x, register float y, register float z)
{
	register Mstack *ms;

#ifdef VDEVICECHECK
	if (!vdevice.initialised) 
		verror("translate: vogl not initialised");
#endif


        if (vdevice.inobject) {
	     Token *tok = newtokens(4);

	     tok[0].i = TRANSLATE;
	     tok[1].f = x;
	     tok[2].f = y;
	     tok[3].f = z;
	     
	     return;
        }
	
	/*
	 * Do the operations directly on the top matrix of
	 * the stack to speed things up.
	 */
	ms = vdevice.transmat;
	ms->m[3][0] += x * ms->m[0][0] + y * ms->m[1][0] + z * ms->m[2][0];
	ms->m[3][1] += x * ms->m[0][1] + y * ms->m[1][1] + z * ms->m[2][1];
	ms->m[3][2] += x * ms->m[0][2] + y * ms->m[1][2] + z * ms->m[2][2];
	ms->m[3][3] += x * ms->m[0][3] + y * ms->m[1][3] + z * ms->m[2][3];
}

/*
 * rot
 * 
 * Set up a rotate matrix and premultiply it with 
 * the top matrix on the stack.
 *
 */
void rot(float r, char axis)
{
	register float	costheta, sintheta, tmp;
	register Mstack *ms;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("rot: vogl not initialised");
#endif


        if (vdevice.inobject) {
	     Token *tok = newtokens(3);

	     tok[0].i = ROTATE;
	     tok[1].f = r;
	     tok[2].i = axis;

	     return;
        }

	/*
	 * Do the operations directly on the top matrix of
	 * the stack to speed things up.
	 */
	costheta = cos(D2R * r);
	sintheta = sin(D2R * r);
	ms = vdevice.transmat;

	switch(axis) {
	case 'x':
	case 'X':
		tmp = ms->m[1][0];
		ms->m[1][0] = costheta * tmp
					  + sintheta * ms->m[2][0];
		ms->m[2][0] = costheta * ms->m[2][0]
					  - sintheta * tmp;

		tmp = ms->m[1][1];
		ms->m[1][1] = costheta * tmp
					  + sintheta * ms->m[2][1];
		ms->m[2][1] = costheta * ms->m[2][1]
					  - sintheta * tmp;
		tmp = ms->m[1][2];
		ms->m[1][2] = costheta * tmp
					  + sintheta * ms->m[2][2];
		ms->m[2][2] = costheta * ms->m[2][2]
					  - sintheta * tmp;

		tmp = ms->m[1][3];
		ms->m[1][3] = costheta * tmp
					  + sintheta * ms->m[2][3];
		ms->m[2][3] = costheta * ms->m[2][3]
					  - sintheta * tmp;
		break;
	case 'y':
	case 'Y':
		tmp = ms->m[0][0];
		ms->m[0][0] = costheta * tmp
					  - sintheta * ms->m[2][0];
		ms->m[2][0] = sintheta * tmp
					  + costheta * ms->m[2][0];
		tmp = ms->m[0][1];
		ms->m[0][1] = costheta * tmp
					  - sintheta * ms->m[2][1];
		ms->m[2][1] = sintheta * tmp
					  + costheta * ms->m[2][1];
		tmp = ms->m[0][2];
		ms->m[0][2] = costheta * tmp
					  - sintheta * ms->m[2][2];
		ms->m[2][2] = sintheta * tmp
					  + costheta * ms->m[2][2];
		tmp = ms->m[0][3];
		ms->m[0][3] = costheta * tmp
					  - sintheta * ms->m[2][3];
		ms->m[2][3] = sintheta * tmp
					  + costheta * ms->m[2][3];
		break;
	case 'z':
	case 'Z':
		tmp = ms->m[0][0];
		ms->m[0][0] = costheta * tmp
					  + sintheta * ms->m[1][0];
		ms->m[1][0] = costheta * ms->m[1][0]
					  - sintheta * tmp;

		tmp = ms->m[0][1];
		ms->m[0][1] = costheta * tmp
					  + sintheta * ms->m[1][1];
		ms->m[1][1] = costheta * ms->m[1][1]
					  - sintheta * tmp;

		tmp = ms->m[0][2];
		ms->m[0][2] = costheta * tmp
					  + sintheta * ms->m[1][2];
		ms->m[1][2] = costheta * ms->m[1][2]
					  - sintheta * tmp;

		tmp = ms->m[0][3];
		ms->m[0][3] = costheta * tmp
					  + sintheta * ms->m[1][3];
		ms->m[1][3] = costheta * ms->m[1][3]
					  - sintheta * tmp;
		break;
	default:
		verror("rot: illegal axis of rotation");
	}
}




/*
 * grot
 * 
 * Set up a rotate matrix and premultiply it with 
 * a matrix given to us by the user 
 *
 *           WARNING: DOES NOT GENERATE TOKENS!!!!
 *
 *
 * One way to make this routine go faster it to write 3
 * seperate routines: 
 *      GRotX( Angle, Mat_new, Mat_old )
 *		GRotY( Angle, Mat_new, Mat_old ) 
 *		GRotZ( Angle, Mat_new, Mat_old ) 
 * 
 *   this removes: 1 level of indirection
 *                 the switch statement 
 *                 while we are at it get rid of the "tmp =" (???)
 *
 *  -- Gopher Team at a time too late in the work day to do any extra
 *       typing / thinking / possible debugging / We're Men! Jim, Not Gophers!
 *
 */
void GRotate( float r, char axis, Matrix Mat_new, Matrix Mat_old )
{
    register float  costheta, sintheta, tmp;


	/*
	 * Copy Mat_old into Mat_new
	 */
    COPYMATRIX( Mat_new, Mat_old );

    /*
     * Do the operations directly on the top matrix of
     * the stack to speed things up.
     */
/*    costheta = cos((double)(D2R * r));
 *   sintheta = sin((double)(D2R * r));
 */
    costheta = cos(D2R * r);
    sintheta = sin(D2R * r);

    switch(axis) {
    case 'x':
    case 'X':
        tmp = Mat_old[1][0];
        Mat_new [1][0] = costheta * tmp + sintheta * Mat_old [2][0];
        Mat_new [2][0] = costheta * Mat_old [2][0] - sintheta * tmp;

        tmp = Mat_old [1][1];
        Mat_new [1][1] = costheta * tmp + sintheta * Mat_old [2][1];
        Mat_new [2][1] = costheta * Mat_old [2][1] - sintheta * tmp;

        tmp = Mat_old [1][2];
        Mat_new [1][2] = costheta * tmp + sintheta * Mat_old [2][2];
        Mat_new [2][2] = costheta * Mat_old [2][2] - sintheta * tmp;

        tmp = Mat_old [1][3];
        Mat_new [1][3] = costheta * tmp + sintheta * Mat_old [2][3];
        Mat_new [2][3] = costheta * Mat_old [2][3] - sintheta * tmp;
        break;

    case 'y':
    case 'Y':
        tmp = Mat_old [0][0];
        Mat_new [0][0] = costheta * tmp - sintheta * Mat_old [2][0];
        Mat_new [2][0] = sintheta * tmp + costheta * Mat_old [2][0];

        tmp = Mat_old [0][1];
        Mat_new [0][1] = costheta * tmp - sintheta * Mat_old [2][1];
        Mat_new [2][1] = sintheta * tmp + costheta * Mat_old [2][1];

        tmp = Mat_old [0][2];
        Mat_new [0][2] = costheta * tmp - sintheta * Mat_old [2][2];
        Mat_new [2][2] = sintheta * tmp + costheta * Mat_old [2][2];

        tmp = Mat_old [0][3];
        Mat_new [0][3] = costheta * tmp - sintheta * Mat_old [2][3];
        Mat_new [2][3] = sintheta * tmp + costheta * Mat_old [2][3];
        break;

    case 'z':
    case 'Z':
        tmp = Mat_old [0][0];
        Mat_new [0][0] = costheta * tmp + sintheta * Mat_old [1][0];
        Mat_new [1][0] = costheta * Mat_old [1][0] - sintheta * tmp;

        tmp = Mat_old [0][1];
        Mat_new [0][1] = costheta * tmp + sintheta * Mat_old [1][1];
        Mat_new [1][1] = costheta * Mat_old [1][1] - sintheta * tmp;

        tmp = Mat_old [0][2];
        Mat_new [0][2] = costheta * tmp + sintheta * Mat_old [1][2];
        Mat_new [1][2] = costheta * Mat_old [1][2] - sintheta * tmp;

        tmp = Mat_old [0][3];
        Mat_new [0][3] = costheta * tmp + sintheta * Mat_old [1][3];
        Mat_new [1][3] = costheta * Mat_old [1][3] - sintheta * tmp;
        break;

    default:
        verror("rot: illegal axis of rotation");
    }
}





/*
 * rotate
 * 
 * Set up an old style, I've got this real fast way of doing
 * it providing I use ints, rotate.
 *
 */
/*void rotate(Angle r, char axis)
{
	rot(r / (float)10, axis);
}
*/
