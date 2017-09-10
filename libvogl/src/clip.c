/********************************************************************
 * $Id: clip.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"
#include "clip.h"


#define OUTCODEVAR \
     register int     ix, iy, iz, iw;\
     register int     diff, abs_w, oct

#define FINDOUTCODE(outcode,p) \
     iw = FLOATtoLONG(p[V_W]); abs_w = FASTABS(iw);\
     outcode = FASTSIGN(iw);\
     \
     ix = FLOATtoLONG(p[V_X]); diff = FASTSIGN(abs_w - FASTABS(ix));\
     oct = FASTSIGN(ix) + 1; \
     outcode |= diff << oct;\
     \
     iy = FLOATtoLONG(p[V_Y]); diff = FASTSIGN(abs_w - FASTABS(iy));\
     oct = FASTSIGN(iy) + 3;\
     outcode |= diff << oct;\
     \
     iz = FLOATtoLONG(p[V_Z]); diff = FASTSIGN(abs_w - FASTABS(iz));\
     oct = FASTSIGN(iz) + 5; outcode |= diff << oct;
     


/*
 * FindOutcode
 *
 * Return code if on outside of any clipping plane
 *
 */
static int FindOutcode(register Vector p)
{
     register int     ix, iy, iz, iw;
     register int     diff, abs_w, outcode, t;


     iw = FLOATtoLONG(p[V_W]);
     abs_w = FASTABS(iw);
     outcode = FASTSIGN(iw);   /* 0 or 1 per w's sign */
     
     ix = FLOATtoLONG(p[V_X]);
     diff = FASTSIGN(abs_w - FASTABS(ix));
     t = FASTSIGN(ix) + 1;
     outcode |= diff << t;    /* 0, 2 or 4 or'd with outcode */
     
     iy = FLOATtoLONG(p[V_Y]);
     diff = FASTSIGN(abs_w - FASTABS(iy));
     t = FASTSIGN(iy) + 3;
     outcode |= diff << t;	  /* 0, 8, or 16 or'd with outcode */
     
     iz = FLOATtoLONG(p[V_Z]);
     diff = FASTSIGN(abs_w - FASTABS(iz));
     t = FASTSIGN(iz) + 5;
     outcode |= diff << t;	  /* 0, 32, or 64 or'd with outcode */
     
     
     return(outcode);
}

#define MAKEDGECOORDS(k, i, p) k=MakeEdgeCoords(i,p)

/*
 * clip
 *
 * Clips a 3D line using Homogeneous clipping.
 * Reference: Newman and Sproull
 *
 */
void clip(register Vector p0, register Vector p1)
{
     int c1, c2, vx, vy;
     OUTCODEVAR;
     /*
      * this changed slightly to make it easy to create an inline macro
      *
      *	c1 = MakeEdgeCoords(0, p0);
      *	c2 = MakeEdgeCoords(1, p1);
      */
     c1 = FindOutcode(p0);
     c2 = FindOutcode(p1);

     
/*     FINDOUTCODE(c1, p0);
     FINDOUTCODE(c2, p1);
*/
     if (c1 & c2)
	  /** line isn't on the screen **/
	  return; 

     if (c1 || c2) {
	  /* We need to clip */
	  register float tmp;
	  register float t, t1=0.0, t2=1.0, dx, dy, dz, dw;
	  register int	i;
	  float      wc[2][6];

	  tmp = p0[V_W];
	  wc[0][0] = tmp + p0[V_X]; wc[0][1] = tmp - p0[V_X];
	  wc[0][2] = tmp + p0[V_Y]; wc[0][3] = tmp - p0[V_Y];
	  wc[0][4] = tmp + p0[V_Z]; wc[0][5] = tmp - p0[V_Z];

	  tmp = p1[V_W];
	  wc[1][0] = tmp + p1[V_X]; wc[1][1] = tmp - p1[V_X];
	  wc[1][2] = tmp + p1[V_Y]; wc[1][3] = tmp - p1[V_Y];
	  wc[1][4] = tmp + p1[V_Z]; wc[1][5] = tmp - p1[V_Z];

	  for (i = 0; i < 6; i++)
	       if (wc[0][i] < 0.0 || wc[1][i] < 0.0) {
		    t = wc[0][i] / (wc[0][i] - wc[1][i]);

		    if (wc[0][i] < 0.0 && t > t1) {
			 t1 = t;
		    } else if (t < t2) 
			 t2 = t;
	       }
	  
	  if (t2 >= t1) {
	       vdevice.cpVvalid = 1;
	       dx = p1[V_X] - p0[V_X];
	       dy = p1[V_Y] - p0[V_Y];
	       dz = p1[V_Z] - p0[V_Z];
	       dw = p1[V_W] - p0[V_W];

	       if (c2) {
		    /* clip line on p1s side */
		    p1[V_X] = p0[V_X] + t2 * dx;
		    p1[V_Y] = p0[V_Y] + t2 * dy;
		    p1[V_Z] = p0[V_Z] + t2 * dz;
		    p1[V_W] = p0[V_W] + t2 * dw;
		    vdevice.cpVvalid = 0;
	       }

	       if (c1) {
		    /* clip line on p0s side */
		    p0[V_X] = p0[V_X] + t1 * dx;
		    p0[V_Y] = p0[V_Y] + t1 * dy;
		    p0[V_Z] = p0[V_Z] + t1 * dz;
		    p0[V_W] = p0[V_W] + t1 * dw;
	       }

	  }

     } else
	  vdevice.cpVvalid = 1;

     vdevice.cpVx = WTOVX(p0);
     vdevice.cpVy = WTOVY(p0);
     
     vx = WTOVX(p1);
     vy = WTOVY(p1);
     REALDRAW(vx,vy);

     vdevice.cpVx = vx;
     vdevice.cpVy = vy;

}




/*
 * quickclip
 *
 * A variation on the above that assumes p0 is a valid position in device coords
 *
 */
void quickclip(register const Vector p0, register Vector p1)
{
     register int    outcode;

     
{
     OUTCODEVAR;

     FINDOUTCODE(outcode, p1);
}
     vdevice.cpVvalid = 1;

     if (outcode) {
	  register float	t, t1 = 1.0;
	  register float tmp = p0[V_W]; /* compiler optimization hack */
	  register int i;
	  float  wc[2][6];

	  wc[0][0] = tmp + p0[V_X];
	  wc[0][1] = tmp - p0[V_X];
	  wc[0][2] = tmp + p0[V_Y];
	  wc[0][3] = tmp - p0[V_Y];
	  wc[0][4] = tmp + p0[V_Z];
	  wc[0][5] = tmp - p0[V_Z];

	  tmp = p1[V_W];
	  wc[1][0] = tmp + p1[V_X];
	  wc[1][1] = tmp - p1[V_X];
	  wc[1][2] = tmp + p1[V_Y];
	  wc[1][3] = tmp - p1[V_Y];
	  wc[1][4] = tmp + p1[V_Z];
	  wc[1][5] = tmp - p1[V_Z];
     
     
	  for (i = 0; i < 6; i++)
	       if (wc[0][i] >= 0.0 && wc[1][i] < 0.0) {
		    t = wc[0][i] / (wc[0][i] - wc[1][i]);
		    if (t < t1)
			 t1 = t;
	       }
	  p1[V_X] = p0[V_X] + t1 * (p1[V_X] - p0[V_X]);
	  p1[V_Y] = p0[V_Y] + t1 * (p1[V_Y] - p0[V_Y]);
	  p1[V_Z] = p0[V_Z] + t1 * (p1[V_Z] - p0[V_Z]);
	  p1[V_W] = p0[V_W] + t1 * (p1[V_W] - p0[V_W]);
	  vdevice.cpVvalid = 0;
     }
{
     register int	vx, vy;

     vx = WTOVX(p1);
     vy = WTOVY(p1);
     REALDRAW(vx,vy);
     vdevice.cpVx = vx;
     vdevice.cpVy = vy;
}
}

