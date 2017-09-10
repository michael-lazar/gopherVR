/********************************************************************
 * $Id: shading_d.c,v 1.1.1.1 2002/01/18 16:34:27 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "vogl.h"
#include "shading_d.h"
#include "bsp.h"

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif





#define d_CLOSE		14000.0
#define d_FAR		9999999999999999999999999.0
#define d_INFINITY	999999999999999999999999999999999.0

/*
 * Flat shading engine
 */

int
N_DFlatShading( 
	       P_POLYLIST_LEAF	pPll_polygon,
	       Vector  		*arVer_orig,
	       int				cVer_clip,
	       int				prN_x_points[],
	       int				prN_y_points[],
	       int				F_called_from )
{

     double  rD_light_pos [3] = { d_FAR, d_INFINITY, d_INFINITY };
     double  rD_eye_pos [3] = { -d_FAR, -d_INFINITY, -d_INFINITY }; 

     double  rD_point_pos [3] = {0.0, 0.0, 0.0};
     double	rD_reflect [3] = {-0.50, -0.75, 0.25};
     double	rD_surf_norm [3];
     double	rD_ra [3];
     double	rD_rd [3];
     int		rN_intensity [3];
     int		N_color;
     int		N_poly_color;
     
     
     /*
      * When color of the polygon is some number in the range
      *  (d_COLMAP_OFF thru (d_TOTAL_COLS - 1))
      *
      * If it is less than d_COLMAP_OFF then we are going to assume
      * that the user did not give us an INDEX, instead they specified
      * one of the default colors by name (ie: RED, MAGENTA, BLUE etc etc)
      * What we do in this case is to set the value of the color into 
      * a valid "map" color. 
      * We do this so that we can be compatible with GL
      * What happens in the user changed the default values for the
      * (d_COLMAP_OFF thru (d_COLMAP_OFF + 6) elements? 
      */
     N_poly_color = pPll_polygon->F_color;
     if ( N_poly_color < d_COLMAP_OFF )
     {
	  N_poly_color = N_poly_color + (d_COLMAP_OFF - 2);
     }
     
     /*
      * Make it be between 0 thru (d_COLORRAMPS - 1)
      */
     N_poly_color = N_poly_color - d_COLMAP_OFF;
     
     
     /*
      * we already have the normalized surface normal computed
      */
     rD_surf_norm [V_X] = (double) (pPll_polygon->Surface_norm_x);
     rD_surf_norm [V_Y] = (double) (pPll_polygon->Surface_norm_y);
     rD_surf_norm [V_Z] = (double) (pPll_polygon->Surface_norm_z);
     
     /*
      * Find the point in the center of the polygon
      */
     V_DFindCenterOfPolygon( arVer_orig, (pPll_polygon->cVer_orig_numv - 1), 
							rD_point_pos );
     
     /*
      * determine and set rD_ra and rD_rd 
      */
     V_DDetermineRaRd( N_poly_color, rD_ra, rD_rd );
     
     
     V_DCalculateIntensities( rD_point_pos, rD_light_pos, rD_eye_pos, 
			     rD_reflect, rD_surf_norm, rD_ra, rD_rd, rN_intensity );
     
     N_color = N_DBestFitColor( vdevice.rrrN_colormap [N_poly_color], 
			       rN_intensity [d_INDEX_RED], rN_intensity [d_INDEX_GREEN], 
			       rN_intensity [d_INDEX_BLUE] );
     
     /*
      * Fill
      */
     N_color = N_color + d_COLMAP_OFF + (N_poly_color * d_LEN_COLRAMP);
     
     if( d_FROM_POLYGON == F_called_from )
     {
	  color( N_color );
/*	  (*vdevice.dev.Vfill)( cVer_clip, prN_x_points, prN_y_points );*/
	  REALFILLPOLYGON( cVer_clip, prN_x_points, prN_y_points );
     }
     
     return( N_color );
}






	/*
	 *  The intesity function is the following: 
	 *  I = Ia * ra + Is[ rd( us . un) + rs( ur . uv)^f]
	 *
	 * This function should remain the same no matter the shading model
	 * used (ie: flat, gourad, phong). 
	 */

void
V_DCalculateIntensities( 
	double	rD_point_pos [3],
	double	rD_light_pos [3],
	double	rD_eye_pos [3],
	double	rD_reflect [3],
	double	rD_surf_norm [3],
	double	rD_ra [3],
	double	rD_rd [3],
	int		rN_intensity [3] )
{

	const double	k_rD_Ia [3] = {0.56, 0.56, 0.56};
	const double	k_rD_Is [3] = {0.4, 0.4, 0.4};
	const double	k_D_Rs = 0.56; 						/* contrast control */
	const double	k_D_f = 1.0;
	const double	k_D_max_intensity = 1.0;
	const double	k_D_min_intensity = 0.0;
	
	double	rD_light_norm [3];
	double	rD_eye_norm [3];
	double	rD_reflect_norm [3];
	double	rD_tmp [3];
	double	D_tmp_dot1;
	double	D_tmp_dot2;
	double	rD_intensity [3];


    /*
     * Make the vector from the point to the eye 
     */
    V_DMakeVector( rD_eye_pos, rD_point_pos, rD_tmp );
    V_DNormalize( rD_tmp, rD_eye_norm );

	/*
	 * Make the vector from the point to the light source
	 */
	V_DMakeVector( rD_light_pos, rD_point_pos, rD_tmp );
	V_DNormalize( rD_tmp, rD_light_norm );


	/*
	 * Make the reflection vector
	 */
	V_DCalculateURVector( rD_tmp, rD_surf_norm, rD_reflect_norm );

	/*
	 * Calculate:
	 *   ( Us . Un )
	 *   rs ( Ur . Uv ) ^ f
	 */
	D_tmp_dot1 = D_DDotProduct( rD_eye_norm, rD_surf_norm );
	D_tmp_dot2 = D_DDotProduct( rD_reflect_norm, rD_eye_norm );
	D_tmp_dot2 = pow( D_tmp_dot2, k_D_f );  
	D_tmp_dot2 = k_D_Rs * D_tmp_dot2;


	/*
	 * Calculate the R, G, B intesities	
	 */
	rD_intensity [d_INDEX_RED] = fabs( 
					(k_rD_Ia [d_INDEX_RED] * rD_ra [d_INDEX_RED]) +
					k_rD_Is [d_INDEX_RED] * 
					( (rD_rd [d_INDEX_RED] * D_tmp_dot1) + D_tmp_dot2 ) );

    rD_intensity [d_INDEX_GREEN] = fabs( 
                    (k_rD_Ia [d_INDEX_GREEN] * rD_ra [d_INDEX_GREEN]) +
                    k_rD_Is [d_INDEX_GREEN] * 
                    ( (rD_rd [d_INDEX_GREEN] * D_tmp_dot1) + D_tmp_dot2 ) );

    rD_intensity [d_INDEX_BLUE] = fabs(  
                    (k_rD_Ia [d_INDEX_BLUE] * rD_ra [d_INDEX_BLUE]) +
                    k_rD_Is [d_INDEX_BLUE] * 
                    ( (rD_rd [d_INDEX_BLUE] * D_tmp_dot1) + D_tmp_dot2 ) );


	/*
	 * The intensity equation just produces an intensity, it is
	 * up to us to make sure it is a number we can use.
	 *
	 * Make sure we aren't out of range
	 */
	if (k_D_max_intensity < rD_intensity [d_INDEX_RED])
		rD_intensity [d_INDEX_RED] = k_D_max_intensity; 

	if (k_D_min_intensity > rD_intensity [d_INDEX_RED])
        rD_intensity [d_INDEX_RED] = k_D_min_intensity; 


    if (k_D_max_intensity < rD_intensity [d_INDEX_GREEN])
        rD_intensity [d_INDEX_GREEN] = k_D_max_intensity;

    if (k_D_min_intensity > rD_intensity [d_INDEX_GREEN])
        rD_intensity [d_INDEX_GREEN] = k_D_min_intensity;


    if (k_D_max_intensity < rD_intensity [d_INDEX_BLUE])
        rD_intensity [d_INDEX_BLUE] = k_D_max_intensity;

    if (k_D_min_intensity > rD_intensity [d_INDEX_BLUE])
        rD_intensity [d_INDEX_BLUE] = k_D_min_intensity;


	/*
	 * Put the intensity within range
	 */
	rD_intensity [d_INDEX_RED] = 255.0 * rD_intensity [d_INDEX_RED];
	rD_intensity [d_INDEX_GREEN] = 255.0 * rD_intensity [d_INDEX_GREEN];
	rD_intensity [d_INDEX_BLUE] = 255.0 * rD_intensity [d_INDEX_BLUE];

	rN_intensity [d_INDEX_RED] = (int) (rD_intensity [d_INDEX_RED] + .5);
	rN_intensity [d_INDEX_GREEN] = (int) (rD_intensity [d_INDEX_GREEN] + .5); 
	rN_intensity [d_INDEX_BLUE] = (int) (rD_intensity [d_INDEX_BLUE] + .5); 
}





	/*
	 * This routine calcultes the normalized vector describing the
	 * the direction of reflection of the surface.
	 *
	 * The vector can be described as follows:
	 *    r = -s + 2( s . un)un
	 *
	 */
void
V_DCalculateURVector( double rD_light [3], double rD_surf_norm [3],
					  double rD_reflect_norm [3] )
{
	double	D_dot;
	double	rD_reflect [3];


	/*
	 * 2 ( s . un )
	 */
	D_dot = D_DDotProduct( rD_light, rD_surf_norm ); 
	D_dot = 2.0 * D_dot;

	/*
	 * -s + 2 ( s . un ) un
	 */
    rD_reflect [V_X] = (-rD_light [V_X]) + (D_dot * rD_surf_norm [V_X]);
	rD_reflect [V_Y] = (-rD_light [V_Y]) + (D_dot * rD_surf_norm [V_Y]);
	rD_reflect [V_Z] = (-rD_light [V_Z]) + (D_dot * rD_surf_norm [V_Z]);


	/*
	 * Normalize
	 */
	V_DNormalize( rD_reflect, rD_reflect_norm );

}





	/*
	 * Find the center of a polygon
	 */
void
V_DFindCenterOfPolygon( Vector	*arVer_orig,
					   int		cVer_numv,
					   double   rD_point_pos [3] )
{

	int	i;


	for ( i = 0; i < cVer_numv; i++ )
	{
		rD_point_pos [V_X] = (double) (arVer_orig[i][V_X] + rD_point_pos[V_X]);
		rD_point_pos [V_Y] = (double) (arVer_orig[i][V_Y] + rD_point_pos[V_Y]);
		rD_point_pos [V_Z] = (double) (arVer_orig[i][V_Z] + rD_point_pos[V_Z]);
	}

	rD_point_pos [V_X] = rD_point_pos [V_X] / (double) cVer_numv;
	rD_point_pos [V_Y] = rD_point_pos [V_Y] / (double) cVer_numv;
	rD_point_pos [V_Z] = rD_point_pos [V_Z] / (double) cVer_numv;
}





	/*
	 * Given two points construct a vector
	 */
void
V_DMakeVector( double a [3], double b [3], double c [3] )
{
    c [V_X] = a [V_X] - b [V_X];
    c [V_Y] = a [V_Y] - b [V_Y];
    c [V_Z] = a [V_Z] - b [V_Z];
}





	/*
	 * Given a vector normalize it
	 */
void
V_DNormalize( double a [3], double b [3] ) 
{
	double	D_denom;

	D_denom = (double) sqrt( (double) (SQ(a[V_X]) + SQ(a[V_Y]) + SQ(a[V_Z])) );

	if ( 0.0 != D_denom )
	{
		b [V_X] = a [V_X] / D_denom;
		b [V_Y] = a [V_Y] / D_denom;
		b [V_Z] = a [V_Z] / D_denom;
	}
	else
	{
		 b [V_X] = 0.0;
		 b [V_Y] = 0.0;
		 b [V_Z] = 0.0;
	}
}




	/*
	 * Dot Product using doubles
	 */
double
D_DDotProduct( double a [3], double b [3] )
{
	return( (a [0]*b [0]) + (a [1]*b [1]) + (a [2]*b [2]) );
}






	/*
	 * This is NOT the final function - just a test one
	 */
int
N_DBestFitColor( int rrN_colormap [d_LEN_COLRAMP][3],
				int N_red,
				int N_green, 
				int N_blue )

{

	int		rrN_colormap2 [d_LEN_COLRAMP][3];
	int     rrN_colormap3 [d_LEN_COLRAMP][2];
	float	R_fact_red;
	float	R_fact_green;
	float	R_fact_blue;
	int		N_new_red;
	int		N_new_green;
	int		N_new_blue;
	int		irN_cmap2 = 0;
	int		irN_cmap3 = 0;
	int		i;



	R_fact_red = ((float) N_red) / 255.0; 
	R_fact_green = ((float) N_green) / 255.0;
	R_fact_blue = ((float) N_blue) / 255.0;

	N_new_red = (int) (R_fact_red *
		(rrN_colormap [d_LEN_COLRAMP-1][d_INDEX_RED] - 
			rrN_colormap [0][d_INDEX_RED]) + rrN_colormap [0][d_INDEX_RED]);

	N_new_green = (int) (R_fact_green *
		(rrN_colormap [d_LEN_COLRAMP-1][d_INDEX_GREEN] - 
			rrN_colormap [0][d_INDEX_GREEN]) + rrN_colormap [0][d_INDEX_GREEN]);

	N_new_blue = (int) (R_fact_blue *
		(rrN_colormap [d_LEN_COLRAMP-1][d_INDEX_BLUE] - 
			rrN_colormap [0][d_INDEX_BLUE]) + rrN_colormap [0][d_INDEX_BLUE]);



	/*
	 * In the next 3 for loops we are sort of doing color seperation
	 * in an attempt to find the best color in our colormap matching the
	 * "true" color given to us by the intensity calculations
	 */
	for( i = 0; i < d_LEN_COLRAMP; i ++ )
	{
		if( abs( rrN_colormap [i][d_INDEX_RED] - N_new_red ) < d_SKIP2 )
		{
            rrN_colormap2 [irN_cmap2][0] = rrN_colormap [i][d_INDEX_GREEN];
            rrN_colormap2 [irN_cmap2][1] = rrN_colormap [i][d_INDEX_BLUE];
			rrN_colormap2 [irN_cmap2][2] = i;
			
			irN_cmap2++;
		}
	}
	/* error check */
	if ( 0 == irN_cmap2 )
		return( 0 );

	for( i = 0; i < irN_cmap2; i ++ )
    {
		if( abs( rrN_colormap2 [i][0] - N_new_green ) < d_SKIP2 )
        {
            rrN_colormap3 [irN_cmap3][0] = rrN_colormap2 [i][1];
			rrN_colormap3 [irN_cmap3][1] = rrN_colormap2 [i][2];
         
            irN_cmap3++;
        }
    }
	/* error check */
	if ( 0 == irN_cmap3 )
		return( rrN_colormap2 [0][2] );

    for( i = 0; i < irN_cmap3; i ++ )
    {
		if( abs( rrN_colormap3 [i][0] - N_new_blue ) < d_SKIP2 )
		{
			return( rrN_colormap3 [i][1] );
		}
	}


	/*
	 * If for some reason we are here
	 */
	return( rrN_colormap3 [0][1] );
}




void
V_DDetermineRaRd( int F_base_color,
				 double	rD_ra [3],
				 double	rD_rd [3] )
{
	rD_ra [d_INDEX_RED]   = rD_rd [d_INDEX_RED] =
							vdevice.rrR_rard [F_base_color][d_INDEX_RED];
	rD_ra [d_INDEX_GREEN] = rD_rd [d_INDEX_GREEN] =	
							vdevice.rrR_rard [F_base_color][d_INDEX_GREEN];
	rD_ra [d_INDEX_BLUE]  = rD_rd [d_INDEX_BLUE] =
							vdevice.rrR_rard [F_base_color][d_INDEX_BLUE];
}
