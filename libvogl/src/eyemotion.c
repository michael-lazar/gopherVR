/********************************************************************
 * $Id: eyemotion.c,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"
#include <stdio.h>

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif

#include "eyemotion.h"


static Vector Rotation;

void EyeInit(void)
{
     register Mstack *ms;

     ms = vdevice.transmat;
     identmatrix( ms->m );
     Rotation[V_X] = Rotation[V_Y] = Rotation[V_Z] = 0.0;

}     


/* 
 * translations and rotations relative to the eyepoint coordinate system
 * rather than relative to the world coordinate system... useful for being
 * able to turn your head as you fly through a scene.
 *
 */
 
void EyeTranslate(register const float x, 
		  register const float y, 
		  register const float z)
{
     register Mstack *ms;
     register float  f;
     /*
      * Do the operations directly on the top matrix of
      * the stack to speed things up.
      */

     ms = vdevice.transmat;


     f = ms->m[0][3];
     ms->m[0][0] += f * x;
     ms->m[0][1] += f * y;
     ms->m[0][2] += f * z;
     
     f = ms->m[1][3];
     ms->m[1][0] += f * x;
     ms->m[1][1] += f * y;
     ms->m[1][2] += f * z;

     f = ms->m[2][3];     
     ms->m[2][0] += f * x;
     ms->m[2][1] += f * y;
     ms->m[2][2] += f * z;

     f = ms->m[3][3];
     ms->m[3][0] += f * x;
     ms->m[3][1] += f * y;
     ms->m[3][2] += f * z;
     
}

void EyeRotateX(register float angle )
{
     int i;
     register Mstack *ms = vdevice.transmat;
     float cosine_theta, sine_theta, t;

     angle *= D2R;

     Rotation[V_X] += angle;
	
     sine_theta = sin(angle);
     cosine_theta = cos(angle);
     for (i=0; i < 4; i++) {
	  t = ms->m[i][1];
	  ms->m[i][1] = t * cosine_theta - ms->m[i][2] * sine_theta;
	  ms->m[i][2] = t * sine_theta + ms->m[i][2] * cosine_theta;
     }

}

void EyeRotateY(register float angle )
{
     register Mstack *ms;
     register float  t;
     register float sintheta, costheta;

     angle *= D2R;
     
     Rotation[V_Y] += angle;

     sintheta = sin(angle);
     costheta = cos(angle);

     ms = vdevice.transmat;

     t = ms->m[0][0];
     ms->m[0][0] = t * costheta + ms->m[0][2] * sintheta;
     ms->m[0][2] = ms->m[0][2] * costheta - t * sintheta;

     t = ms->m[1][0];
     ms->m[1][0] = t * costheta + ms->m[1][2] * sintheta;
     ms->m[1][2] = ms->m[1][2] * costheta - t * sintheta;

     t = ms->m[2][0];
     ms->m[2][0] = t * costheta + ms->m[2][2] * sintheta;
     ms->m[2][2] = ms->m[2][2] * costheta - t * sintheta;

     t = ms->m[3][0];
     ms->m[3][0] = t * costheta + ms->m[3][2] * sintheta;
     ms->m[3][2] = ms->m[3][2] * costheta - t * sintheta;

}


void EyeRotateZ(register float angle )
{
     register int i;
     register Mstack *ms = vdevice.transmat;
     register float cosine_theta, sine_theta, t;
     
     angle *= D2R;
     Rotation[V_Z] += angle;

     sine_theta = sin(angle);
     cosine_theta = cos(angle);

     for (i = 0; i < 4; i++) {
	  t = ms->m[i][0];
	  ms->m[i][0] = t * cosine_theta - ms->m[i][1] * sine_theta;
	  ms->m[i][1] = t * sine_theta + ms->m[i][1] * cosine_theta;
     }
}

void
EyeGetLocation(Vector *loc)
{
     register Mstack *ms = vdevice.transmat;
     
     (*loc)[0] = ms->m[3][0];
     (*loc)[1] = ms->m[3][1];
     (*loc)[2] = ms->m[3][2];
}

#define VecLen(V1, V2) sqrt(SQ(V2[V_X]-V1[V_X]) + SQ(V2[V_Y]-V1[V_Y]) + SQ(V2[V_Z]-V1[V_Z]))


void
EyeGoto(float x, float y, float z,
	float rx, float ry, float rz,
	float speed, void *(displayfunc)(void))
{
     float oldlen, newlen;
     float rxdiff, rydiff, rzdiff;
     Vector Eyepoint, NewEyepoint;
     Vector Destination;
     Vector Diff;
     float len;
     float deltat;
     float movement;
     
     Destination[V_X] = x;
     Destination[V_Y] = y;
     Destination[V_Z] = z;

     newlen = oldlen = 0.0;
     while ((newlen - oldlen) > -.95) {

	  EyeGetLocation(&Eyepoint);
	  oldlen = VecLen(Eyepoint, Destination);

	  EyeTranslate(0.0, 0.0, 1.0);
	  displayfunc();

	  EyeGetLocation(&NewEyepoint);
	  newlen = VecLen(NewEyepoint, Destination);
	  
	  /**************************************************************
	   * Are we getting closer? 
	   *  A differene of +1 means we're heading away (180 degrees)
	   *  A different of -1 means we're heading correctly.
	   *  Even numbers mean we're at a right angle
	    **/
	  EyeRotateY(1.0);

	  displayfunc();
	  printf ("New: %f, Old: %f Diff: %f\n", newlen, oldlen, newlen - oldlen);
     }
     
     /* 
      * Calculate the vector and distance to the new point
      */

	  Diff[V_X] = x - Eyepoint[V_X];
	  Diff[V_Y] = y - Eyepoint[V_Y];
	  Diff[V_Z] = z - Eyepoint[V_Z];
     
	  printf("Diff is %f, %f, %f\n", Diff[V_X], Diff[V_Y], Diff[V_Z]);
	  len = SQ(Diff[V_X]) + SQ(Diff[V_Y]) + SQ(Diff[V_Z]);
	  len = sqrt(len);



/*     rydiff = (atan2(Diff[V_Z], Diff[V_X]) - Rotation[V_Y]) * R2D ;
     printf("Angle to Rotate Y is %f, current Rotation %f\n", rydiff, Rotation[V_Y] * R2D);
*/
/*     deltat = rydiff/speed;
     for (movement = 0.0; abs(movement - rydiff) > .001; movement += deltat) {
	  EyeRotateY(deltat);
	  displayfunc();
     }*/


/*     deltat = len / speed;
     for (movement = 0.0; movement < len; movement += deltat) {
	  EyeTranslate((Diff[V_X]/speed), (Diff[V_Y]/speed),
		       (Diff[V_Z]/speed));
	  EyeTranslate(0.0, 0.0, deltat);
	  
	  displayfunc();
     }
     
     */

}

