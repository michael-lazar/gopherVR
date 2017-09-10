#include <stdlib.h>
#include <vogl.h>
#include <math.h>
#include "eyemotion.h"
#include "vogltools.h"
#include <sys/time.h>

extern int usec_render;

/* normalize to approximately 100fps */
#define TIMESLICE 20000
#define NTIME ((usec_render > TIMESLICE) ? 0 : usleep(TIMESLICE - usec_render))
#define DODISP displayfunc(); NTIME;

/**********************************************************************
 * Rotate the scene about the axis
 */

void RotateAboutAxis(float val)
{
     rot(val, 'y');
}


/**********************************************************************
 * Set the initial view in a scene
 */
void EyeInitialPoint( void )
{
     perspective(600, 16.0/9.0, 0.0, 800.0);
     lookat(0.0, 0.0, 0.0, 0.0, 0.0, 800.0, 0);
     EyeTranslate( 600.0, -550.0, -1075.0 ); 
     rot( -80.0, 'y' ); 
}



/**********************************************************************
 * Jump up into the air in many variations
 */

/* does this ever call anything other than drawscene()? I think not -- CK */

void
EyeAirJordan(void (*displayfunc)(), float lookDownAmount, float Moveahead,
	     float Moveup, float seconds)
{
     float y, t, v0, y0, oldy, g, deltat;
     float yAccumulated;
        
     /* bend your knees in preparation for jumping */
     EyeTranslate(0.0, 10.0 , 0.0);  DODISP;
     

     EyeTranslate(0.0, 6.0 , 0.0);  DODISP;

     y0 = 0.0;
     v0 = 8.0;
     y  = 0.0;
     deltat = (1.0/seconds);
     t  = deltat;
     g  = 9.8 / 3;  /* only 1/3 the usual gravity! */
     yAccumulated = 0.0;
     
     /** Simulate a gravitational force **/
     /** y(t) = -.5 gt^2 + v0t + y0 **/
     do {
	  oldy = y;
	  y = -.5 *  g * t * t + v0 * t + y0;
	  if (y < 0.0) {
	       y = 0.0;  /* Hit the ground... */
	  }
	  EyeTranslate(0.0, Moveup * (oldy - y), Moveahead * (usec_render/(float)TIMESLICE));
	  yAccumulated += (oldy - y);
	  /* tilt where the eye is looking down as we jump */
	  EyeRotateX( - yAccumulated * lookDownAmount );  displayfunc();
	  /* undo the tilt so our jump trajectory is straight */
	  EyeRotateX( yAccumulated * lookDownAmount ); 
	  t += deltat * (usec_render/(float)TIMESLICE);
     }  while (y > 0.0);
        
     /* rebound once */
     EyeTranslate(0.0, 30.0 , 0.0); DODISP;

     EyeTranslate(0.0, -30, 0.0); DODISP;

     /*straighten out your legs */
     EyeTranslate(0.0, -10.0 , 0.0); DODISP;

     EyeTranslate(0.0, -6.0 , 0.0); DODISP;

}

/**********************************************************************
 * make a not-user-selectable groundplane; it is not selectable because
 * it doesn't have a gopherobject ID. All non-selectable stuff has to be 
 * created before we place the gopher objects in the scene.
 */
#include "hungarian.h"
void makegroundplane(void)
{
     long my_ID;

     /* 
      * before we start modelling the scene do two rotations to the
      * coordinate system so that things look rightside up. Note that
      * while doing a 180 degree rotation about the x axis looks like
      * the same thing this hoses up the code in BSP that checks to
      * see if the user clicked on a gopher object; at least on the
      * Mac its bogus unless we do both a y and z axis
      * rotate. Probably simplest to remodel the 3d icons so we don't
      * have to twirl the coordinate systems... -mpm 
      */

     rotate( 1800, 'y' );  
     rotate( 1800, 'z' );  
     
/*     my_ID = genobj();
     makeobj(my_ID);*/
     pushmatrix();

{
     L_NUMBER    rlN_i1 [3] = { -20000,  2,      0 };
     L_NUMBER    rlN_i2 [3] = { -20000,  2,      -20000 };
     L_NUMBER    rlN_i3 [3] = {  0,      2,      -20000 };
     L_NUMBER    rlN_i4 [3] = {  0,      2,      0 };
     
     L_NUMBER    rlN_i5 [3] = { 0,               2,      20000 };
     L_NUMBER    rlN_i6 [3] = { 0,               2,      0 };
     L_NUMBER    rlN_i7 [3] = { 20000,   2,      0 };
     L_NUMBER    rlN_i8 [3] = { 20000,   2,      20000 };
     
     L_NUMBER    rlN_i9  [3] = { -20000, 3,      20000 };
     L_NUMBER    rlN_i10 [3] = { -20000, 3,      0 };
     L_NUMBER    rlN_i11 [3] = {      0, 3,      0 };
     L_NUMBER    rlN_i12 [3] = {      0, 3,      20000 };
     
     L_NUMBER    rlN_i13 [3] = { 20000,  3,      -20000 };
     L_NUMBER    rlN_i14 [3] = { 20000,  3,      0 };
     L_NUMBER    rlN_i15 [3] = {     0,  3,      0 };
     L_NUMBER    rlN_i16 [3] = {     0,  3,      -20000 };
     
     color(OBJECT_GROUND1_COLOR);
     bgnpolygon();
     v3i( rlN_i9 );
     v3i( rlN_i10 );
     v3i( rlN_i11 );
     v3i( rlN_i12 );
     endpolygon();
     
     color(OBJECT_GROUND1_COLOR);
     bgnpolygon();
     v3i( rlN_i13 );
     v3i( rlN_i14 );
     v3i( rlN_i15 );
     v3i( rlN_i16 );
     endpolygon();
     
     color(OBJECT_GROUND2_COLOR);
     bgnpolygon();
     v3i( rlN_i1 );
     v3i( rlN_i2 );
     v3i( rlN_i3 );
     v3i( rlN_i4 );
     endpolygon();
     
     color(OBJECT_GROUND2_COLOR);
     bgnpolygon();
     v3i( rlN_i5 );
     v3i( rlN_i6 );
     v3i( rlN_i7 );
     v3i( rlN_i8 );
     endpolygon();
}                 
popmatrix();
/*closeobj();
callobj(my_ID );
delobj( my_ID );*/
}


/**********************************************************************
 *
 */

void
JumpIntoView(void (*displayfunc)())
{
     const lookDownAmount = 7.0;                     
     float y, t, v0, y0, oldy, g, deltat;
     float yAccumulated;
     
     float AccumY;
     
     /* start from where we want to end up */
     EyeInitialPoint();
     
     y0 = 0.0;
     v0 = 8.0;
     y  = 0.0;
     deltat = (1.0/12.0); 
     t  = deltat;
     g  = 9.8 / 3;  /* only 1/3 the usual gravity! */
     yAccumulated = 0.0;
     
     /** Simulate a gravitational force **/
     
     
     /** Simulate a gravitational force **/
     /** y(t) = -.5 gt^2 + v0t + y0 **/
     do {
	  oldy = y;
	  y = -.5 *  g * t * t + v0 * t + y0;
	  if (y < 0.0) {
	       y = 0.0;  /* Hit the ground... */
	  }
	  if (oldy > y) 
	       EyeTranslate(0.0, 700.0 * (oldy - y), 20.0 * (oldy - y));
	  else
	       EyeTranslate(0.0, 700.0 * (oldy - y), -20.0 * (oldy - y));
	  yAccumulated += (oldy - y);
	  
	  if ( oldy > y ) {
	  	/* tilt where the eye is looking down as we jump */
	  	EyeRotateX( -yAccumulated * lookDownAmount );
		displayfunc();
	  	/* undo the tilt so our jump trajectory is straight */
	  	EyeRotateX( yAccumulated * lookDownAmount );

		t += deltat * (usec_render / (float)TIMESLICE);
	  } else {
		t += deltat;
	  }

     }  while (y > 0.0);

     /* rebound once */
     EyeTranslate(0.0, 30.0 , 0.0);  DODISP;
     EyeTranslate(0.0, -30, 0.0);  DODISP;
     
     /*straighten out your legs */
     EyeTranslate(0.0, -10.0 , 0.0); DODISP;
     EyeTranslate(0.0, -6.0 , 0.0); DODISP;

     
}

/**********************************************************************
 *
 */

void
JumpOutofView(void (*displayfunc)())
{
     float InitialX = 0.0;
     float InitialY = 0.0;
     float deltatheta = PI/20.0;
     float oldx, oldy, x, y;
     float theta;
     float oldangle;
     float angle;

     oldx = InitialX; oldy = InitialY;

     for (theta = 0; theta < (4 * PI); theta += deltatheta) {
	  x = theta *theta *  cos(theta);
	  y = theta *theta *  sin(theta);

	  EyeRotateY(-oldangle);
	  EyeTranslate(100 * (x-oldx), -10.0, 100 * (y-oldy));

	  angle = 90.0 + theta * R2D;
	  EyeRotateY(angle);
	  oldangle = angle;

	  oldy = y;
	  oldx = x;

	  DODISP;
     }
}


void
JumpIntoView_notdone(void (*displayfunc)())
{
     float InitialX = 152.094788;
     float InitialY = -24.088957 ;
     float deltatheta = PI/20.0;
     float oldx, oldy, x, y;
     float theta;
     float oldangle = 0.0;
     float angle;

     EyeInitialPoint();
     EyeTranslate(-100 * InitialX, -600.0, -100 * InitialY);
     oldx = InitialX;
     oldy = InitialY;

     for (theta = (4*PI)-deltatheta; theta > 0.0; theta -= deltatheta) {
	  x = theta * theta *  cos(theta);
	  y = theta * theta *  sin(theta);


	  EyeTranslate(- 100 * (x-oldx), 10.0, -100 * (y-oldy));

	  angle = 90.0 + theta * R2D;
	  oldy = y;
	  oldx = x;

	  EyeRotateY(angle); DODISP;
	  EyeRotateY(-angle);
     }
}
/**********************************************************************
 *
 */

void
MotionSickness(void (*displayfunc)())
{
     float InitialX = 0.0;
     float InitialY = 0.0;
     float deltatheta = PI/10.0;
     float oldx, oldy, x, y;
     float theta;

     oldx = InitialX; oldy = InitialY;

     for (theta = 0; theta < 10 * PI; theta += deltatheta) {
	  x = theta * cos(theta);
	  y = theta * sin(theta);

	  EyeTranslate(100 * (x-oldx), 0.0, 100 * (y-oldy));

	  oldy = y;
	  oldx = x;

	  DODISP;
     }
     
}

#define UODISP drawscene(); NTIME;

void EyeLocationUp( int GoUp )
{
        int i;
        
        /* bend your knees in preparation for jumping */
        EyeTranslate(0.0, 10.0 , 0.0); UODISP;
        EyeTranslate(0.0, 6.0 , 0.0); UODISP;

        if ( GoUp ) {
	     for (i = 0; i < 4; i++ ) {
		  EyeTranslate( 0.0, -200.0, 0.0 );       UODISP;
	     }
        } else {
	     for (i = 0; i < 4; i++ ) {
		  EyeTranslate( 0.0, 200.0, 0.0 );        UODISP;
	     }
        }
        /* rebound once */
        EyeTranslate(0.0, 30.0 , 0.0); UODISP;
        EyeTranslate(0.0, -30, 0.0); UODISP;
        /*straighten out your legs */
        EyeTranslate(0.0, -10.0 , 0.0); UODISP;
        EyeTranslate(0.0, -6.0 , 0.0); UODISP;
}
