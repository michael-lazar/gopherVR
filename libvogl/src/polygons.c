/********************************************************************
 * $Id: polygons.c,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include "vogl.h"

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif

#ifdef UNIX 
#include "../hershey/src/htext.h"
#else
#include "htext.h"
#endif

#include "bsp.h"
#include "shading.h"
#include "shading_d.h"
#include "polygons.h"
#include "valloc.h"
/*
 * tmesh, and quadstrips added by
 * marsh@ppdrs1.nrl.navy.mil (Spencer J. Marsh)
 */

#define	SURF_NONE		0
#define	SURF_TEXT		1
#define	SURF_TEXTURE	2


extern int		irPgp_current; /* the index into the painter algorithm array */
extern int		F_curr_type;


/*
 * for use with text drawing routines
 */
extern P_LINETEXT	pLtx_head;
extern int		cLtx_lines;
extern int		g_B_text_waiting;
extern TEXTATTR		g_Txt_curr_text;

/*
 * for use with texture mapping routines
 */
extern	int			g_B_texture_waiting;


/*
 * For poly clipping and such..
 */
static	float	F[6][4], S[6][4], I[4], p[MAXVERTS][4];
static	int	nout, first[6], numv;
static  long	polymodeflag = PYM_FILL;
static	int	ip1[MAXVERTS], ip2[MAXVERTS];


/*
 *  Orientation of backfacing polygons(in screen coords)
 */
static	int	clockwise = 1;



static void polyoutline(int n, int ipx[], int ipy[]);
static void polyclip(register const int n, float p[MAXVERTS][4] );
static void shclip(float Pnt[4], int side);
static void shclose(int side);
static int intersect(int side, register Vector Ip, register Vector Pnt);
static int visible(int side);
static int ObjectIsVisible(const float p [MAXVERTS][4] );

#define VISIBLE(r, side)    switch (side) {\
     case 0: r=((S[0][3] + S[0][0]) >= 0.0);break;\
     case 1: r=((S[1][3] - S[1][0]) >= 0.0);break;\
     case 2: r=((S[2][3] + S[2][1]) >= 0.0);break;\
     case 3: r=((S[3][3] - S[3][1]) >= 0.0);break;\
     case 4: r=((S[4][3] + S[4][2]) >= 0.0);break;\
     case 5: r=((S[5][3] - S[5][2]) >= 0.0);break;}


#define INTERSECTVAR \
     float	wc1; float	wc2;\
     float	a;\
     register   int     iwc1, iwc2;

#define INTERSECT(r,side,Ip,Pnt) {\
     wc2=S[side][3];\
     wc1=Pnt[3];\
     switch (side) {\
     case 0: wc1 += Pnt[0]; wc2 += S[0][0]; break;\
     case 1: wc1 -= Pnt[0]; wc2 -= S[1][0]; break;\
     case 2: wc1 += Pnt[1]; wc2 += S[2][1]; break;\
     case 3: wc1 -= Pnt[1]; wc2 -= S[3][1]; break;\
     case 4: wc1 += Pnt[2]; wc2 += S[4][2]; break;\
     case 5: wc1 -= Pnt[2]; wc2 -= S[5][2]; break;}\
	  r=0;iwc1=FLOATtoLONG(wc1);iwc2=FLOATtoLONG(wc2);\
     if (FASTSIGN(iwc1) !=  FASTSIGN(iwc2)) {\
	  a = wc1 / (wc1 - wc2);\
	  if (!FASTSIGN(FLOATtoLONG(a)) && a <= 1.0){\
	       Ip[0] = Pnt[0] + a * (S[side][0] - Pnt[0]);\
	       Ip[1] = Pnt[1] + a * (S[side][1] - Pnt[1]);\
	       Ip[2] = Pnt[2] + a * (S[side][2] - Pnt[2]);\
	       Ip[3] = Pnt[3] + a * (S[side][3] - Pnt[3]);\
	       r=1;\
	  }\
     }\
 }



/*
 * concave
 *
 *	signal wether or not polygons are concave (not a lot of use at the moment).
 */
void concave(const long yesno)
{
	vdevice.concave = yesno;
}

/*
 * backface
 *
 *	Turns on culling of backfacing polygons. A polygon is
 * backfacing if it's orientation in *screen* coords is clockwise.
 */
void backface(const int onoff)
{
	vdevice.attr->a.backface = onoff;
	clockwise = 1;
}

/*
 * frontface
 *
 *	Turns on culling of frontfacing polygons. A polygon is
 * frontfacing if it's orientation in *screen* coords is anti-clockwise.
 */
void frontface(const int onoff)
{
	vdevice.attr->a.backface = onoff;
	clockwise = 0;
}

/*
 * polymode
 *
 *	Sets the polygon filling mode - only filled or outlined supported
 */
void polymode(const long mode)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("polymode: vogl not initialised");
#endif


	if (vdevice.inobject) {
	     Token *tok = newtokens(2);
	     tok[0].i = POLYMODE;
	     tok[1].i = (int)mode;
	     polymodeflag = mode;
	     return;
	}

	polymodeflag = mode;
/*
 * On older SGI Machines this call used to work... On the newer
 * boxes it doesn't do anything. If you want the old stuff then
 * #define OLD_SGI_BOXES somewhere.
 */
#define OLD_SGI_BOXES 1
#ifdef OLD_SGI_BOXES
	polymodeflag = mode;
#endif
}



int F_polySurfaceDesc( P_POLYLIST_LEAF pPll_polygon, Vector arVer_orig[] )
{
     float    R_distance = 9999999999999999.0;
     float    R_tmp_distance = 0.0;
     int      i;
     int      F_flag = 0;


    /*
     * To determine how far away the polygon is we take the distance of the
     * closest vertex, from the polygon this polygon originaly came from, to 
     * our eyepoint (0,0,0)
     *
     * We do this because all polygons as a result of the 1 original polygon
     * should be calculated to be the same distance from the eye. If we did
     * not do this it is very possible that the parts of the text string
     * (or parts of the texture) associated with different child polygons
     * will be drawn/not drawn and make the object look funny. This was
     * the text/texture is either drawn or its not. 
     */

     for ( i = 0; i < (pPll_polygon->cVer_orig_numv - 1); i++ )
     {
	  /*** Note that there we used to do a sqrt here, but.....
	       in interests of speed....   - pml ***/
	  R_tmp_distance = (arVer_orig[i][V_X] * arVer_orig [i][V_X])+
				(arVer_orig[i][V_Y] * arVer_orig [i][V_Y])+
				(arVer_orig[i][V_Z] * arVer_orig [i][V_Z]);

	  if ( R_tmp_distance < R_distance)
	       R_distance = R_tmp_distance;
     }

     /*
      * Return the proper flag
      *
      * BTW: these values are based on what looks good on the screen
      */
     if ( R_distance <= SQ(5000.0) )
	  F_flag++;
     if ( R_distance <= SQ(2500.0) )
	  F_flag++; 

	
     switch (F_flag)
     {
     case 0: return (SURF_NONE);
     case 1: return (SURF_TEXT);
     case 2: return (SURF_TEXTURE);
     }

     return( SURF_TEXT );
}



/*
 * dopoly
 *
 *	do a transformed polygon with n edges using fill
 */

void dopoly(register const P_POLYLIST_LEAF pPll_polygon )
{
     register const int Numv = pPll_polygon->cVer_numv;
     register int	i;
     register const Mstack    *mst = vdevice.transmat;
     register const Mstack    *msb = vdevice.transmatbottom;
     register int       behindUs = TRUE;
     float	        rR_tmp_p [MAXVERTS][4];
	

     /*
      * Apply topmost matrix on the polygon, test the z value while doing it.
      *
      * if all the object's Z values are all negative then it is behind the eye
      * and we can skip displaying it... 
      */
     for ( i = 0; i < Numv; i++ ) {

	  QMULTVECTOR_AFFINE(rR_tmp_p[i], pPll_polygon->rVer_polygon[i], 
		      mst->m );
	  
	  /* we assume here that we have at most a 60 degree view angle... */
	  if (behindUs) {
	       if ((rR_tmp_p[i][V_Z] <= (rR_tmp_p[i][V_Y]+rR_tmp_p[i][V_Y]))|| 
		    (rR_tmp_p[i][V_Z] <= (rR_tmp_p[i][V_X]+rR_tmp_p[i][V_X])))
		    behindUs = FALSE;
	       /* it only takes one point in front to have to 
		  display the polygon */
	  }

	  /*
	   * If it is facing the eye continue, otherwise break out
	   */
	  if (i == 2)
	       if (!(ObjectIsVisible( rR_tmp_p )))
		    return;
     }

     if ( behindUs == TRUE ) 
	  return;

     /*
      * Apply the perspective matrix on the polygon
      */

     for ( i = 0; i < Numv;  i++ )
     {
	  QMULTVECTOR_BOTTOM(p[i], rR_tmp_p[i], msb->m );
     }

     /*
      * Clip the polygons
      */
     polyclip(pPll_polygon->cVer_numv, p);

     /*
      * Right?, why bother going on
      */

     if (nout < 2)
	  return;


     /*
      * Test whether we need to process the original poly vertex list
      */

     if (!(d_DYNAMIC == vdevice.F_shade_freq || pPll_polygon->B_text ||
	 pPll_polygon->B_texture)) {

	  REALSETCOLOR(pPll_polygon->F_shaded_color);
	  REALFILLPOLYGON(nout,ip1,ip2);
	  return;

     } else {

	  register Vector	*arVer_orig;
	  register float	F_surf_desc = SURF_NONE;

	  /*
	   * Apply topmost matrix on the original polygon this polygon
	   * came from 
	   */
	  arVer_orig = (Vector *) Cvallocate( pPll_polygon->cVer_orig_numv, 
					     sizeof( Vector ) ); 
	
	  for ( i = 0; i < pPll_polygon->cVer_orig_numv; i++ )
	  {
	       QMULTVECTOR_AFFINE(arVer_orig[i], 
				  pPll_polygon->arVer_original[i], 
				  mst->m);
	  }

	
	  /*    if ( FALSE != vdevice.fill ) {
	   *
	   * Since we aren't doing dynamic shading right now, it makes
	   * no sense to test for this repeatedly inside the drawing
	   * loop. If you want to turn on  dynamic shading uncomment
	   * the N_FlatShading function call -mpm 
	   */

/*
 *
 * if ( d_DYNAMIC == vdevice.F_shade_freq ) { * N_FlatShading(
 pPll_polygon, arVer_orig, nout, * ip1, ip2, d_FROM_POLYGON ); * }
 else {
 *
 * 
 *	can save a few cycles here by calling the native set color function directly... -mpm	       
 color( pPll_polygon->F_shaded_color ); */

	  REALSETCOLOR(pPll_polygon->F_shaded_color);
	  REALFILLPOLYGON(nout,ip1,ip2);
/*		  }
 *
 *    } else {
 *		vdevice.cpVx = ip1[0];
 *		vdevice.cpVy = ip2[0];
 *		vdevice.cpVvalid = 0;
 *		polyoutline(nout, ip1, ip2);
 *    }
 * 	
 *     vdevice.fill = 0;
 * 
 * again, we are only doing filled polygons right now so don't mess around with outlines
 */	
	  /*
	   * See if we need to care about what attributes to draw on the face
	   * of this polygon
	   */
	  if ( pPll_polygon->B_text || pPll_polygon->B_texture)
	       F_surf_desc = F_polySurfaceDesc( pPll_polygon, arVer_orig );

	
	  WorldBFree( (char *) arVer_orig );
	
	  /*
	   * Draws any text associated with the polygon
	   */
	  if (pPll_polygon->B_text)
	  {
	       if ( (SURF_TEXT == F_surf_desc) || (SURF_TEXTURE == F_surf_desc) )
	       {
		    register Coord oldx, oldy, oldz;
		    register P_POLYGONLINE theline = pPll_polygon->pPgt_string;
		    register int tmp;

		    if (theline != NULL) {
			 vdevice.attr->a.color = pPll_polygon->F_text_color;
			 REALSETCOLOR(pPll_polygon->F_text_color);
		  
			 move( theline[0].R_draw[V_X],
			      -theline[0].R_draw[V_Y], 
			      theline[0].R_draw[V_Z] );
		    
			 tmp =  (pPll_polygon->cPgt_lines-1);
			 for( i=0; i <tmp;)
			 {
			      oldx = theline[i].R_move [V_X];
			      oldy = theline[i].R_move [V_Y];
			      oldz = theline[i].R_move [V_Z];

			      draw(oldx, -oldy, oldz);
			      
			      i++;
			      
			      if (theline[i].R_draw [V_X] != oldx ||
				  theline[i].R_draw [V_Y] != oldy ||
				  theline[i].R_draw [V_Z] != oldz ||
				  !vdevice.cpVvalid) {
				   
				   move( theline[i].R_draw [V_X],
					-theline[i].R_draw [V_Y], 
					theline[i].R_draw [V_Z] );
			      }
			 }
			 draw( theline[i].R_move [V_X],
			      -theline[i].R_move [V_Y], 
			      theline[i] .R_move [V_Z] );
		    }
		    
	       }
	  }
     }
     
}

/***********************************************************************
 * Do backface culling
 */
static int 
ObjectIsVisible(const float p [MAXVERTS][4] )
{
     register float z;
     register float R_nx, R_ny, R_nz;
     register float R_ax, R_ay, R_az;
     register float R_bx, R_by, R_bz;

      /*
      * calculate Normal vector to the plane
      */

     R_ax = p[0][V_X] - p[1][V_X];
     R_ay = p[0][V_Y] - p[1][V_Y];
     R_az = p[0][V_Z] - p[1][V_Z];
     
     R_bx = p[2][V_X] - p[1][V_X];
     R_by = p[2][V_Y] - p[1][V_Y];
     R_bz = p[2][V_Z] - p[1][V_Z];
     
     R_nx = -(R_ay * R_bz) + (R_az * R_by);
     R_ny = (R_ax * R_bz) - (R_az * R_bx);
     R_nz = -(R_ax * R_by) + (R_ay * R_bx);

     /*
      * Make a dot product of the normal and a line from a point on
      *	the polygon back to to origin of the coordinate system. Since
      *	we want to get the angle between the surface normal and the
      *	view ref pt.  and we have transformed the points to be in view
      *	space, the view ref. pt. must be at 0,0,0. There may be some
      *	problems with this in the case of the ortho projections, but
      *	we only care about perspective anyway.  -mpm */
	 
     z = ( - (R_nx * p[1][V_X] +
	      R_ny * p[1][V_Y] +
	      R_nz * p[1][V_Z]));


     return((clockwise ? ((z) > 0) : ((z) <= 0)));
}



/*
 * isPointInPoly
 *
 *	transform a polygon and tell us if a point is inside it
 */
int isPointInPoly( int ScreenX, int ScreenY, P_POLYLIST_LEAF pPll_polygon )
{

     int		i;
     float	rR_tmp_p [MAXVERTS][4];
     int        behindUs = TRUE;

     /*
      * Apply topmost matrix on the polygon
      */
     for ( i = 0; i < pPll_polygon->cVer_numv; i++ ) 
     {
	  QMULTVECTOR( rR_tmp_p[i], pPll_polygon->rVer_polygon [i], vdevice.transmat->m );
     }


     /* 
      * if all the object's Z values are all negative then it is behind the eye,
      * and we can skip it... 
      */

     for ( i = 0; i < pPll_polygon->cVer_numv; i++ ) {
	  /*	    if ( rR_tmp_p[i][V_Z] < 0.0 ) {  */
	  /* we assume here that we have at most a 60 degree view angle... */
	  if (( rR_tmp_p[i][V_Z] <= (rR_tmp_p[i][V_Y] + rR_tmp_p[i][V_Y]) ) ||
	      ( rR_tmp_p[i][V_Z] <= (rR_tmp_p[i][V_X] + rR_tmp_p[i][V_X]) )) { 
	       behindUs = FALSE;
	       break;		/*it only takes one point in front to have to consider the polygon*/
	  }
     }
     if ( behindUs == TRUE ) return( FALSE );

     /*
      * If it is facing the eye continue, otherwise break out
      */
     if (!(ObjectIsVisible( rR_tmp_p )))
     {
	  return( FALSE );
     }


     /*
      * Apply the perspective matrix on the polygon
      */
     for ( i = 0; i < pPll_polygon->cVer_numv; i++ )
     {
	  QMULTVECTOR( p[i], rR_tmp_p [i], vdevice.transmatbottom->m );
     }


     /*
      * Clip the polygons
      */
     polyclip( pPll_polygon->cVer_numv, p );

     /*
      * Right?, why bother going on
      */
     if ( nout <= 2 )
     {
	  return( FALSE );
     } else {		
	  if ( (*vdevice.dev.VPointInPolygon)( ScreenX, ScreenY, nout, ip1, ip2) ) {
	       /* just like in the fabulous NetScrape and MoJack... 
		* things you have read should be...
		* colored RED... ha.ha.ha.ha! 
		*/
	       pPll_polygon->F_shaded_color = RED;  
	       return( TRUE );
	  } 
	  return( FALSE );
     }
}






/*
 * polyoutline
 *
 *	draws a polygon outline from already transformed points.
 */
static void polyoutline(int n, int ipx[], int ipy[])
{
	int	i;

	if (n > 2) {
		for (i = 1; i < n; i++) {
			REALDRAW(ipx[i],ipy[i]);

			vdevice.cpVx = ipx[i];
			vdevice.cpVy = ipy[i];
		}
		REALDRAW(ipx[0],ipy[0]);

		vdevice.cpVx = ipx[0];
		vdevice.cpVy = ipy[0];
	}
}

/*
 * polyobj
 *
 *	construct a polygon from a object token list.
 */
void polyobj(int n, Token dp[], int fill)
{
	int	i, j;
	float	vect[4];
	POLYGON_ENTRY	Bsp_curr_entry;


	Bsp_curr_entry.rVer_polygon = (Vector *) Cvallocate( n, sizeof( Vector ) ); 
	for (i = 0, j = 0; i < n; i++, j += 3) 
	{
		vect[V_X] = dp[j + V_X].f;
		vect[V_Y] = dp[j + V_Y].f;
		vect[V_Z] = dp[j + V_Z].f;
		vect[V_W] = 1;

		QMULTVECTOR_AFFINE(Bsp_curr_entry.rVer_polygon[i],
			    vect, vdevice.transmat->m);
	}


    /*
     * Right now 99.99% of this code is duplicated in the pclos()
     * routine, this sucks, we should make this 1 function and call
     * it when we need it.
     */
	if (fill)
	{
		vdevice.fill = polymodeflag;
	}
	else
	{
		vdevice.fill = 0;
	}

    Bsp_curr_entry.F_type = F_curr_type;
    Bsp_curr_entry.cVer_numv = n;
    Bsp_curr_entry.R_lastx = dp[V_X].f; 
    Bsp_curr_entry.R_lasty = dp[V_Y].f; 
    Bsp_curr_entry.R_lastz = dp[V_Z].f; 
    Bsp_curr_entry.F_color = vdevice.attr->a.color; 
    Bsp_curr_entry.N_fill = (int) polymodeflag; 
	Bsp_curr_entry.B_split = FALSE;
	Bsp_curr_entry.R_norm_x = 0.0;
	Bsp_curr_entry.R_norm_y = 0.0; 
	Bsp_curr_entry.R_norm_z = 0.0; 
	Bsp_curr_entry.F_Backfacing = FALSE;

    /*
     * If we have text waiting - we (as a polygon) must pick it up
     * and include it as one of our attributes.
     */ 
    if (TRUE == g_B_text_waiting)
    {
        P_LINETEXT  pLtx_travel;
        int         i = 0;
		float		rR_temp [4], rR_result [4];
		Matrix		Mat_new, Mat_ident;


        g_B_text_waiting = FALSE;
		Bsp_curr_entry.F_color = vdevice.attr->a.color;

        Bsp_curr_entry.B_text = TRUE;
        Bsp_curr_entry.cPgt_lines = cLtx_lines;
		Bsp_curr_entry.R_text_vec_x = g_Txt_curr_text.R_curr_text_vec_x;
		Bsp_curr_entry.R_text_vec_y = g_Txt_curr_text.R_curr_text_vec_y;
		Bsp_curr_entry.R_text_vec_z = g_Txt_curr_text.R_curr_text_vec_z;
		Bsp_curr_entry.F_text_color = g_Txt_curr_text.F_curr_text_color;

		/*
		 * Must apply the same transform on the vertex
		 *   ( Bsp_curr_entry.R_text_vec_x, Bsp_curr_entry.R_text_vec_y, 0, 1 )
		 * as we just did above.
		 *
		 * check1
		 */
		rR_temp [V_X] = g_Txt_curr_text.R_curr_text_vec_x; 
		rR_temp [V_Y] = g_Txt_curr_text.R_curr_text_vec_y;
		rR_temp [V_Z] = g_Txt_curr_text.R_curr_text_vec_z; 
		rR_temp [V_W] = 1.0;
		QMULTVECTOR_AFFINE( rR_result, rR_temp, vdevice.transmat->m);
	
		/*
		 * Set up the mat that will rotate the text before we splatter it
		 * on the polygon
		 */
/* 
 * we do NOT want an identity matrix here since this won't take into acount
 * any of the rotations that have accumulated on the transformation
 * matrix. Instead, take the top matrix and assume no scaling has been done
 * and factor out any translation component to the matrix (i.e. set the
 * first three elements in the bottom row to be zero). -mpm
 *
 * 		identmatrix( Mat_ident ); 
 */
		
		getmatrix( Mat_ident );
		Mat_ident[3][0] = 0.0;
		Mat_ident[3][1] = 0.0;
		Mat_ident[3][2] = 0.0;
		
		GRotate( g_Txt_curr_text.R_curr_text_angle, 'y', Mat_new, Mat_ident );


        Bsp_curr_entry.pPgt_string = 
            (P_POLYGONLINE) Cvallocate( cLtx_lines, sizeof(POLYGONLINE) );

        pLtx_travel = pLtx_head; 
        while (pLtx_travel)
        {
			float	rR_tmp_move [4];
			float	rR_tmp_draw [4];
			float	rR_result_move [4];
			float	rR_result_draw [4];


			rR_tmp_move [V_X] = pLtx_travel->R_move [V_X];
			rR_tmp_move [V_Y] = pLtx_travel->R_move [V_Y]; 
			rR_tmp_move [V_Z] = 0.0; 
			rR_tmp_move [V_W] = 1.0; 

			rR_tmp_draw [V_X] = pLtx_travel->R_draw [V_X]; 
			rR_tmp_draw [V_Y] = pLtx_travel->R_draw [V_Y]; 
			rR_tmp_draw [V_Z] = 0.0; 
			rR_tmp_draw [V_W] = 1.0; 

			/*
			 * Apply the rotation matrix on the text
			 */
			QMULTVECTOR( rR_result_move, rR_tmp_move, Mat_new );
			QMULTVECTOR( rR_result_draw, rR_tmp_draw, Mat_new );

			Bsp_curr_entry.pPgt_string [i].R_move [V_X] = 
								rR_result_move [V_X] + rR_result [V_X];
            Bsp_curr_entry.pPgt_string [i].R_move [V_Y] = 
								rR_result_move [V_Y] - rR_result [V_Y];
            Bsp_curr_entry.pPgt_string [i].R_move [V_Z] = 
								rR_result_move [V_Z] + rR_result [V_Z];

            Bsp_curr_entry.pPgt_string [i].R_draw [V_X] = 
								rR_result_draw [V_X] + rR_result [V_X];
            Bsp_curr_entry.pPgt_string [i].R_draw [V_Y] = 
								rR_result_draw [V_Y] - rR_result [V_Y];
            Bsp_curr_entry.pPgt_string [i].R_draw [V_Z] = 
								rR_result_draw [V_Z] + rR_result [V_Z];

			pLtx_travel = pLtx_travel->pLtx_next;
            i++;

        }

	    /*
	     * Clear pLtx_head
	     */
        while (pLtx_head)
        {
            pLtx_travel = pLtx_head->pLtx_next;
            WorldBFree( (char *) pLtx_head );
            pLtx_head = pLtx_travel;
        }

    }
    else
    {
        Bsp_curr_entry.B_text = FALSE;
        Bsp_curr_entry.cPgt_lines = 0;
		Bsp_curr_entry.R_text_vec_x = 0.0;
		Bsp_curr_entry.R_text_vec_y = 0.0;
		Bsp_curr_entry.R_text_vec_z = 0.0;
		Bsp_curr_entry.F_text_color = BLACK;
		Bsp_curr_entry.pPgt_string = (P_POLYGONLINE) NULL;
	}


    /*
	 * Does this polygon have a texture associated with it?
     */
    if (TRUE == g_B_texture_waiting)
	{
		Bsp_curr_entry.B_texture = TRUE;
		g_B_texture_waiting = FALSE;
	}
	else
	{
		Bsp_curr_entry.B_texture = FALSE;	
	}	

	
	/*
	 * Add the current entry to the list of polygons from which
	 * to construct the BSP tree from
	 */
	V_AddPolygonList( &Bsp_curr_entry );


    /*
     * Free some memory
     */
    WorldBFree( (char *) Bsp_curr_entry.rVer_polygon );
    if ( Bsp_curr_entry.B_text == TRUE ) {
    	WorldBFree( (char *)  Bsp_curr_entry.pPgt_string );
    }
}



/*
 * poly2
 *
 *	construct a polygon from an (x, y) array of points provided by the user.
 */
void poly2(const long nv, float dp[][2])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("poly2: vogl not initialised");
#endif


	vdevice.fill = 0;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = 0.0;
	}

	poly(nv, np);
}

/*
 * poly2i
 *
 *	construct a polygon from an (x, y) array of points provided by the user.
 * Icoord version.
 */
void poly2i(const long nv, Icoord dp[][2])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("poly2i: vogl not initialised");
#endif


	vdevice.fill = 0;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = 0.0;
	}

	poly(nv, np);
}

/*
 * poly2s
 *
 *	construct a polygon from an (x, y) array of points provided by the user.
 * Scoord version.
 */
void poly2s(const long nv, Scoord dp[][2])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("poly2s: vogl not initialised");
#endif


	vdevice.fill = 0;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = 0.0;
	}

	poly(nv, np);
}

/*
 * polyi
 *
 *	construct a polygon from an (x, y, z) array of points provided by the user.
 * Icoord version.
 */
void polyi(const long nv, Icoord dp[][3])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("polyi: vogl not initialised");
#endif


	vdevice.fill = 0;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = dp[i][V_Z];
	}

	poly(nv, np);
}

/*
 * polys
 *
 *	construct a polygon from an (x, y, z) array of points provided by the user.
 * Scoord version.
 */
void polys(const long nv, Scoord dp[][3])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("poly2s: vogl not initialised");
#endif


	vdevice.fill = 0;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = dp[i][V_Z];
	}

	poly(nv, np);
}

/*
 * polf2
 *
 *	construct a filled polygon from an (x, y) array of points provided
 * by the user.
 */
void polf2(const long nv, float dp[][2])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("polf2: vogl not initialised");
#endif


	vdevice.fill = polymodeflag;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = 0.0;
	}

	poly(nv, np);

	vdevice.fill = 0;
}

/*
 * polf2i
 *
 *	construct a filled polygon from an (x, y) array of points provided
 * by the user. Icoord version.
 */
void polf2i(const long nv, Icoord dp[][2])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("polf2i: vogl not initialised");
#endif


	vdevice.fill = polymodeflag;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = 0.0;
	}

	poly(nv, np);

	vdevice.fill = 0;
}

/*
 * polf2s
 *
 *	construct a filled polygon from an (x, y) array of points provided
 * by the user. Scoord version.
 */
void polf2s(const long nv, Scoord dp[][2])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("polf2s: vogl not initialised");
#endif


	vdevice.fill = polymodeflag;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = 0.0;
	}

	poly(nv, np);

	vdevice.fill = 0;
}

/*
 * polfi
 *
 *	construct a filled polygon from an (x, y, z) array of points provided
 * by the user. Icoord version.
 */
void polfi(const long nv, Icoord dp[][3])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("polfi: vogl not initialised");
#endif


	vdevice.fill = polymodeflag;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = dp[i][V_Z];
	}

	poly(nv, np);

	vdevice.fill = 0;
}

/*
 * polfs
 *
 *	construct a filled polygon from an (x, y, z) array of points provided
 * by the user. Scoord version.
 */
void polfs(const long nv, Scoord dp[][3])
{
	int	i;
	float	np[MAXVERTS][3];

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("polfs: vogl not initialised");
#endif


	vdevice.fill = polymodeflag;

	for (i = 0; i < (int)nv; i++) {
		np[i][V_X] = dp[i][V_X];
		np[i][V_Y] = dp[i][V_Y];
		np[i][V_Z] = dp[i][V_Z];
	}

	poly(nv, np);

	vdevice.fill = 0;
}

/*
 * poly
 *
 *	construct a polygon from an array of points provided by the user.
 */
void poly(const long nv, float dp[][3])
{
	int	i, j;
	Vector	vect;
	int	n = nv;
	
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("poly: vogl not initialised");
#endif


	if (vdevice.inobject) {
	     Token *tok = newtokens(2 + 3 * n);
	     tok[0].i = POLY;
	     tok[1].i = n;
	     for (i = 0, j = 2; i < n; i++, j += 3) {
		  tok[j + V_X].f = dp[i][V_X];
		  tok[j + V_Y].f = dp[i][V_Y];
		  tok[j + V_Z].f = dp[i][V_Z];
	     }
	     return;
	}

	for (i = 0; i < n; i++) {
		vect[V_X] = dp[i][V_X];
		vect[V_Y] = dp[i][V_Y];
		vect[V_Z] = dp[i][V_Z];
		vect[V_W] = 1;
		QMULTVECTOR_AFFINE(p[i], vect, vdevice.transmat->m);
	}

	/*dopoly(n);*/

	vdevice.cpW[V_X] = dp[0][V_X];
	vdevice.cpW[V_Y] = dp[0][V_Y];
	vdevice.cpW[V_Z] = dp[0][V_Z];
}

/*
 * polf
 *
 *	construct a filled polygon from an array of points provided
 * by the user.
 */
void polf(const long nv, float dp[][3])
{
	int	i, j;
	Vector	vect;
	long	n = nv;
	
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("poly: vogl not initialised");
#endif


	vdevice.fill = polymodeflag;

	if (vdevice.inobject) {
	     Token *tok = newtokens(2 + 3 * n);
	     tok[0].i = POLYF;
	     tok[1].i = n;
	     for (i = 0, j = 2; i < n; i++, j += 3) {
		  tok[j + V_X].f = dp[i][V_X];
		  tok[j + V_Y].f = dp[i][V_Y];
		  tok[j + V_Z].f = dp[i][V_Z];
	     }
	     return;
	}

	for (i = 0; i < n; i++) {
		vect[V_X] = dp[i][V_X];
		vect[V_Y] = dp[i][V_Y];
		vect[V_Z] = dp[i][V_Z];
		vect[V_W] = 1;
		QMULTVECTOR_AFFINE(p[i], vect, vdevice.transmat->m);
	}

	/*dopoly(n);*/

	vdevice.cpW[V_X] = dp[0][V_X];
	vdevice.cpW[V_Y] = dp[0][V_Y];
	vdevice.cpW[V_Z] = dp[0][V_Z];

	vdevice.fill = 0;
}


/*
 * pmv
 *
 *	set the start position of a polygon
 */
void pmv(float x,float  y, float z)
{
	vdevice.inpolygon = 1;
	vdevice.fill = polymodeflag;
	numv = 0;
	p[numv][V_X] = x;
	p[numv][V_Y] = y;
	p[numv][V_Z] = z;
	p[numv][V_W] = 1.0;
}

/*
 * pmvi
 *
 *	The integer argument version of pmv.
 */
void pmvi(Icoord x, Icoord y, Icoord z)
{
	pmv((float)x, (float)y, (float)z);
}

/*
 * pmv2i
 *
 *	The integer argument version of pmv2.
 */
void pmv2i(Icoord x, Icoord y)
{
	pmv((float)x, (float)y, vdevice.cpW[V_Z]);
}

/*
 * pmvs
 *
 *	The integer argument version of pmv.
 */
void pmvs(Scoord x, Scoord y, Scoord z)
{
	pmv((float)x, (float)y, (float)z);
}

/*
 * pmv2s
 *
 *	The integer argument version of pmv2.
 */
void pmv2s(Scoord x, Scoord y)
{
	pmv((float)x, (float)y, vdevice.cpW[V_Z]);
}

/*
 * pmv2
 *
 *	set up a polygon which will be constructed by a series of
 * move draws in x, y.
 */
void pmv2(float x, float y)
{
	pmv(x, y, vdevice.cpW[V_Z]);
}


/*
 * pdr
 *
 *	add another vertex to the polygon array
 */
void pdr(Coord x, Coord y, Coord z)
{

	numv++;

/* don't check for MAXVERTS... we feel the need for speed
	if (numv >= MAXVERTS) {
		char	buf[100];

		sprintf(buf, "pdr: can't draw a polygon with more than %d vertices", MAXVERTS);
		verror(buf);
	}
*/
	p[numv][V_X] = x;
	p[numv][V_Y] = y;
	p[numv][V_Z] = z;
	p[numv][V_W] = 1.0;
}

/*
 * rpdr
 *
 *	relative polygon draw.
 */
void rpdr(Coord dx, Coord dy, Coord dz)
{
	pdr((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), (vdevice.cpW[V_Z] + dz));
}

/*
 * rpdr2
 *
 *	relative polygon draw - only (x, y).
 */
void rpdr2(Coord dx, Coord dy)
{
	pdr((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), vdevice.cpW[V_Z]);
}

/*
 * rpdri
 *
 *	relative polygon draw. Icoord version.
 */
void rpdri(Icoord dx, Icoord dy, Icoord dz)
{
	pdr((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), (vdevice.cpW[V_Z] + dz));
}

/*
 * rpdr2i
 *
 *	relative polygon draw - only (x, y). Icoord version.
 */
void rpdr2i(Icoord dx, Icoord dy)
{
	pdr((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), vdevice.cpW[V_Z]);
}

/*
 * rpdrs
 *
 *	relative polygon draw. Icoord version.
 */
void rpdrs(Scoord dx, Scoord dy, Scoord dz)
{
	pdr((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), (vdevice.cpW[V_Z] + dz));
}

/*
 * rpdr2s
 *
 *	relative polygon draw - only (x, y). Scoord version.
 */
void rpdr2s(Scoord dx, Scoord dy)
{
	pdr((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), vdevice.cpW[V_Z]);
}

/*
 * rpmv
 *
 *	relative polygon move.
 */
void rpmv(Coord dx, Coord dy, Coord dz)
{
	pmv((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), (vdevice.cpW[V_Z] + dz));
}

/*
 * rpmv2
 *
 *	relative polygon move - only (x, y).
 */
void rpmv2(Coord dx, Coord dy)
{
	pmv((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), vdevice.cpW[V_Z]);
}

/*
 * rpmvi
 *
 *	relative polygon move. Icoord version.
 */
void rpmvi(Icoord dx, Icoord dy, Icoord dz)
{
	pmv((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), (vdevice.cpW[V_Z] + dz));
}

/*
 * rpmv2i
 *
 *	relative polygon move - only (x, y). Icoord version.
 */
void rpmv2i(Icoord dx, Icoord dy)
{
	pmv((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), vdevice.cpW[V_Z]);
}

/*
 * rpmvs
 *
 *	relative polygon move. Icoord version.
 */
void rpmvs(Scoord dx, Scoord dy, Scoord dz)
{
	pmv((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), (vdevice.cpW[V_Z] + dz));
}

/*
 * rpmv2s
 *
 *	relative polygon move - only (x, y). Scoord version.
 */
void rpmv2s(Scoord dx, Scoord dy)
{
	pmv((vdevice.cpW[V_X] + dx), (vdevice.cpW[V_Y] + dy), vdevice.cpW[V_Z]);
}

/*
 * pdri
 *
 *	The integer argument version of pdr.
 */
void pdri(Icoord x, Icoord y, Icoord z)
{
	pdr((float)x, (float)y, (float)z);
}

/*
 * pdr2i
 *
 *	The integer argument version of pdr2.
 */
void pdr2i(Icoord x, Icoord y)
{
	pdr((float)x, (float)y, vdevice.cpW[V_Z]);
}

/*
 * pdrs
 *
 *	The short argument version of pdr.
 */
void pdrs(Scoord x, Scoord y)
{
	pdr((float)x, (float)y, vdevice.cpW[V_Z]);
}

/*
 * pdr2s
 *
 *	The short argument version of pdr2.
 */
void pdr2s(Scoord x, Scoord y)
{
	pdr((float)x, (float)y, vdevice.cpW[V_Z]);
}

/*
 * pdr2
 *
 *	add another vertex to the polygon array
 */
void pdr2(float x, float y)
{
	pdr(x, y, vdevice.cpW[V_Z]);
}






/*
 * pclos
 *
 *	draw the polygon started by the above.
 *  used by things like the rectf call
 */
void pclos(void)
{
     float		lstx, lsty, lstz;
     int		i, j;
     POLYGON_ENTRY	Bsp_curr_entry;


#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("pclos: vogl not initialised");
#endif


     vdevice.inpolygon = 0;
     
     if (vdevice.inobject) {
	  Token *tok = newtokens(2 + 3 * (numv + 1));
	  tok[0].i = POLYF;
	  tok[1].i = numv + 1;
	  for (i = 0, j = 2; i <= numv; i++, j += 3) {
	       tok[j + V_X].f = p[i][V_X];
	       tok[j + V_Y].f = p[i][V_Y];
	       tok[j + V_Z].f = p[i][V_Z];
	  }
	  
	  return;
     }
     
     lstx = p[numv][V_X];
     lsty = p[numv][V_Y];
     lstz = p[numv][V_Z];
     
     numv++;
     
     Bsp_curr_entry.rVer_polygon = (Vector *) Cvallocate( numv, sizeof(Vector) );
     for (i = 0; i < numv; i++) {
	  QMULTVECTOR_AFFINE(Bsp_curr_entry.rVer_polygon[i],
			     p[i], vdevice.transmat->m);
     }
     
     
     /*
      * Right now 99.99% of this code is duplicated in the polyobj()
      * routine, this sucks, we should make this 1 function and call
      * it when we need it.
      */
     Bsp_curr_entry.F_type = F_curr_type;
     Bsp_curr_entry.cVer_numv = numv;
     Bsp_curr_entry.R_lastx = lstx;
     Bsp_curr_entry.R_lasty = lsty;
     Bsp_curr_entry.R_lastz = lstz;
     Bsp_curr_entry.F_color = vdevice.attr->a.color; 
     Bsp_curr_entry.N_fill = (int) polymodeflag; 
     Bsp_curr_entry.B_split = FALSE;
     Bsp_curr_entry.R_norm_x = 0.0; 
     Bsp_curr_entry.R_norm_y = 0.0; 
     Bsp_curr_entry.R_norm_z = 0.0; 
     Bsp_curr_entry.F_Backfacing = FALSE;
     
     
     
     /*
      * If we have text waiting - we (as a polygon) must pick it up
      * and include it as one of our attributes.
      */ 
     if (TRUE == g_B_text_waiting)
     {
	  P_LINETEXT  pLtx_travel;
	  int         i = 0;
	  float       rR_temp [4], rR_result [4];
	  Matrix      Mat_new, Mat_ident;

	  
	  g_B_text_waiting = FALSE;
	  Bsp_curr_entry.F_color = vdevice.attr->a.color;
	  
	  Bsp_curr_entry.B_text = TRUE;
	  Bsp_curr_entry.cPgt_lines = cLtx_lines;
	  Bsp_curr_entry.R_text_vec_x = g_Txt_curr_text.R_curr_text_vec_x;
	  Bsp_curr_entry.R_text_vec_y = g_Txt_curr_text.R_curr_text_vec_y;
	  Bsp_curr_entry.R_text_vec_z = g_Txt_curr_text.R_curr_text_vec_z;
	  Bsp_curr_entry.F_text_color = g_Txt_curr_text.F_curr_text_color;
	  
	  /*
	   * Must apply the same transform on the vertex
	   *   ( Bsp_curr_entry.R_text_vec_x, Bsp_curr_entry.R_text_vec_y, 0, 1 )
	   * as we just did above.
	   *
	   * check1
	   */
	  rR_temp [V_X] = g_Txt_curr_text.R_curr_text_vec_x; 
	  rR_temp [V_Y] = g_Txt_curr_text.R_curr_text_vec_y;
	  rR_temp [V_Z] = g_Txt_curr_text.R_curr_text_vec_z; 
	  rR_temp [V_W] = 1.0;
	  QMULTVECTOR_AFFINE( rR_result, rR_temp, vdevice.transmat->m);
	  
	  /*
	   * Set up the mat that will rotate the text before we splatter it
	   * on the polygon
	   */
	  identmatrix( Mat_ident ); 
	  GRotate( g_Txt_curr_text.R_curr_text_angle, 'y', Mat_new, Mat_ident );
	  
	  
	  Bsp_curr_entry.pPgt_string = 
	       (P_POLYGONLINE) Cvallocate( cLtx_lines, sizeof(POLYGONLINE) );
	  
	  pLtx_travel = pLtx_head; 
	  while (pLtx_travel)
	  {
	       float   rR_tmp_move [4];
	       float   rR_tmp_draw [4];
	       float   rR_result_move [4];
	       float   rR_result_draw [4];
	       
	       
	       rR_tmp_move [V_X] = pLtx_travel->R_move [V_X];
	       rR_tmp_move [V_Y] = pLtx_travel->R_move [V_Y]; 
	       rR_tmp_move [V_Z] = 0.0; 
	       rR_tmp_move [V_W] = 1.0; 
	       
	       rR_tmp_draw [V_X] = pLtx_travel->R_draw [V_X]; 
	       rR_tmp_draw [V_Y] = pLtx_travel->R_draw [V_Y]; 
	       rR_tmp_draw [V_Z] = 0.0; 
	       rR_tmp_draw [V_W] = 1.0; 
	       
	       /*
		* Apply the rotation matrix on the text
		*/
	       QMULTVECTOR( rR_result_move, rR_tmp_move, Mat_new );
	       QMULTVECTOR( rR_result_draw, rR_tmp_draw, Mat_new );
	       
	       Bsp_curr_entry.pPgt_string [i].R_move [V_X] = 
		    rR_result_move [V_X] + rR_result [V_X];
	       Bsp_curr_entry.pPgt_string [i].R_move [V_Y] = 
		    rR_result_move [V_Y] - rR_result [V_Y];
	       Bsp_curr_entry.pPgt_string [i].R_move [V_Z] = 
		    rR_result_move [V_Z] + rR_result [V_Z];
	       
	       Bsp_curr_entry.pPgt_string [i].R_draw [V_X] = 
		    rR_result_draw [V_X] + rR_result [V_X];
	       Bsp_curr_entry.pPgt_string [i].R_draw [V_Y] = 
		    rR_result_draw [V_Y] - rR_result [V_Y];
	       Bsp_curr_entry.pPgt_string [i].R_draw [V_Z] = 
		    rR_result_draw [V_Z] + rR_result [V_Z];
	       
	       pLtx_travel = pLtx_travel->pLtx_next;
	       i++;
	  }
	  
	  /*
	   * Clear pLtx_head
	   */
	  while (pLtx_head)
	  {
	       pLtx_travel = pLtx_head->pLtx_next;
	       WorldBFree( (char *)  pLtx_head );
	       pLtx_head = pLtx_travel;
	  }
	  
     }
     else
     {
	  Bsp_curr_entry.B_text = FALSE;
	  Bsp_curr_entry.cPgt_lines = 0;
	  Bsp_curr_entry.R_text_vec_x = 0.0;
	  Bsp_curr_entry.R_text_vec_y = 0.0;
	  Bsp_curr_entry.R_text_vec_z = 0.0;
	  Bsp_curr_entry.F_text_color = BLACK;
	  Bsp_curr_entry.pPgt_string = (P_POLYGONLINE) NULL;
     }
     
     
     /*
      * Does this polygon have a texture associated with it?
      */
     if (TRUE == g_B_texture_waiting)
     {
	  Bsp_curr_entry.B_texture = TRUE;
	  g_B_texture_waiting = FALSE;
     }
     else
     {
	  Bsp_curr_entry.B_texture = FALSE;   
     }   
     
     
     /*
      * Add the current entry to the list of polygons from which
      * to construct the BSP tree from
      */
     V_AddPolygonList( &Bsp_curr_entry );
     
     /*
      * Free some memory
      */
     WorldBFree( (char *)  Bsp_curr_entry.rVer_polygon );
     if ( Bsp_curr_entry.B_text == TRUE ) {
	  WorldBFree( (char *)  Bsp_curr_entry.pPgt_string );
     }
}


/*
 * The following routines are an implementation of the Sutherland - Hodgman
 * polygon clipper, as described in "Reentrant Polygon Clipping"
 * Communications of the ACM Jan 1974, Vol 17 No. 1.
 */
static void
polyclip(register const int n, float p[MAXVERTS][4] )
{
	int	i;

	nout = 0;
	for (i = 0; i < 6; i++)
	     first[i] = 1;

	for (i = 0; i < n; i++) 
	     shclip(p[i], 0);

	shclose(0);

}


static void 
shclip(float Pnt[4], register int side)
{
     register int isVisible;
     register int isIntersect;
     INTERSECTVAR
     float	P[4];

     do {
	  isVisible   = 0;

	  if (side == 6) {
	       ip1[nout] = WTOVX(Pnt);
	       ip2[nout++] = WTOVY(Pnt);
	  } else {
	       COPYVECTOR(P, Pnt);
	       
	       if (first[side]) {
		    first[side] = 0;
		    COPYVECTOR(F[side], P);
	       } else {
		    INTERSECT(isIntersect, side, I, P );
		    if (isIntersect)
			 shclip(I, side + 1);

	       }

	       COPYVECTOR(S[side], P);
	       VISIBLE(isVisible, side);
		
	       if (isVisible) {
		    COPYVECTOR(Pnt, S[side]);
		    side++;
	       }
	  }
     } while (isVisible);  /*** Tail recursion ***/
}

static void shclose(register int side)
{
     register int isIntersect;
     INTERSECTVAR;
     
     do {
	  INTERSECT(isIntersect, side, I, F[side]);
	  if (isIntersect)
	       shclip(I, side + 1);
	  
	  if (side != 0)
	       first[side-1] = 1;

	  side++;
     } while (side < 6);


}

/*static int intersect(int side, register Vector Ip, register Vector Pnt)
{
     register	float	wc1 = 0.0;
     register	float	wc2 = 0.0;
     register	float	a;
     
     switch (side) {
     case 0:
	  wc1 = Pnt[3] + Pnt[0];
	  wc2 = S[0][3] + S[0][0];
	  break;
     case 1:
	  wc1 = Pnt[3] - Pnt[0];
	  wc2 = S[1][3] - S[1][0];
	  break;
     case 2:
	  wc1 = Pnt[3] + Pnt[1];
	  wc2 = S[2][3] + S[2][1];
	  break;
     case 3:
	  wc1 = Pnt[3] - Pnt[1];
	  wc2 = S[3][3] - S[3][1];
	  break;
     case 4:
	  wc1 = Pnt[3] + Pnt[2];
	  wc2 = S[4][3] + S[4][2];
	  break;
     case 5:
	  wc1 = Pnt[3] - Pnt[2];
	  wc2 = S[5][3] - S[5][2];
	  break;
     }
     
     if (FASTSIGN(wc1) !=  FASTSIGN(wc2)) {
	  a = wc1 / (wc1 - wc2);
	  if (a < 0.0 || a > 1.0) {
	       return(0);
	  } else {
	       Ip[0] = Pnt[0] + a * (S[side][0] - Pnt[0]);
	       Ip[1] = Pnt[1] + a * (S[side][1] - Pnt[1]);
	       Ip[2] = Pnt[2] + a * (S[side][2] - Pnt[2]);
	       Ip[3] = Pnt[3] + a * (S[side][3] - Pnt[3]);
	       return(1);
	  }
     }
     return(0);
}

static int visible(int side)
{
     switch (side) {
     case 0:
	  return(S[0][3] + S[0][0] >= 0.0);
	  
     case 1:
	  return((S[1][3] - S[1][0]) >= 0.0);
	  
     case 2:
	  return((S[2][3] + S[2][1]) >= 0.0);
	  
     case 3:
	  return((S[3][3] - S[3][1]) >= 0.0);
	  
     case 4:
	  return((S[4][3] + S[4][2]) >= 0.0);
	  
     case 5:
	  return((S[5][3] - S[5][2]) >= 0.0);

     }
     return(-1);
}
*/

/*
 * bgnpolygon
 *
 *	Set a flag so that we know what to do with v*() calls.
 */
void bgnpolygon(void)
{
	if (vdevice.bgnmode != NONE)
		verror("vogl: bgnpolygon mode already belongs to some other bgn routine");

	vdevice.bgnmode = VPOLY;
	vdevice.fill = polymodeflag;
	vdevice.save = 1;
	vdevice.inpolygon = 1;

}

/*
 * endpolygon
 *
 *	Set a flag so that we know what to do with v*() calls.
 */
void endpolygon(void)
{

	pclos();

	vdevice.bgnmode = NONE;
	vdevice.fill = 0;
	vdevice.inpolygon = 0;
	vdevice.save = 0;
}

/*
 * bgntmesh
 *
 *	Set a flag so that we know what to do with v*() calls.
 */
void bgntmesh(void)
{
	if (vdevice.bgnmode != NONE)
		verror("vogl: bgntmesh mode already belongs to some other bgn routine");

	vdevice.bgnmode = VTMESH;
	vdevice.fill = polymodeflag;
	vdevice.save = 0;
	vdevice.inpolygon = 1;

}

/*
 * swaptmesh
 *
 *    cause v* calls to save in the new register 
 */
void swaptmesh(void)
{
	if (vdevice.bgnmode != VTMESH)
	  verror("vogl: swaptmesh called outside bgntmesh/endtmesh ");
	if( vdevice.save < 2)
	  verror("vogl: swaptmesh called before first triangle was defined ");
	vdevice.save++;
}
/*
 * endtmesh
 *
 *	Set a flag so that we know what to do with v*() calls.
 */
void endtmesh( void )
{
	vdevice.bgnmode = NONE;
	vdevice.fill = 0;
	vdevice.inpolygon = 0;
	vdevice.save = 0;
}

/*
 * bgnqstrip
 *
 *	Set a flag so that we know what to do with v*() calls.
 */
void bgnqstrip( void )
{
	if (vdevice.bgnmode != NONE)
		verror("vogl: bgnqstrip mode already belongs to some other bgn routine");

	vdevice.bgnmode = VQSTRIP;
	vdevice.fill = polymodeflag;
	vdevice.save = 0;
	vdevice.inpolygon = 1;

}

/*
 * endqstrip
 *
 *	Set a flag so that we know what to do with v*() calls.
 */
void endqstrip( void )
{
	vdevice.bgnmode = NONE;
	vdevice.fill = 0;
	vdevice.inpolygon = 0;
	vdevice.save = 0;
}
