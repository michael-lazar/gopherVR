/********************************************************************
 * $Id: shading.c,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
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
#include "shading.h"
#include "bsp.h"

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif



#define d_INFINITY	99999999999999999999999999999999.0

	/*
	 * Flat shading engine
	 */

int N_FlatShading( 
		  P_POLYLIST_LEAF	pPll_polygon,
		  Vector  		*arVer_orig,
		  int				cVer_clip,
		  int				prN_x_points[],
		  int				prN_y_points[],
		  int				F_called_from )
{
     float  rR_light_pos [3] = { d_INFINITY, d_INFINITY, d_INFINITY};
     float  rR_eye_pos [3]; 
     
     float  rR_point_pos [3]= {0.0, 0.0, 0.0};
     float  rR_reflect [3] = {-0.50, -0.75, 0.25};
     float  rR_surf_norm [3];
     float  rR_ra [3];
     float  rR_rd [3];
     float  temp1N_intensity [3];
     int    N_color;
     int    N_poly_color;

     /* set the eye position to that passed in to us */
     rR_eye_pos[V_X] = rEyeTransX;
     rR_eye_pos[V_Y] = rEyeTransY;
     rR_eye_pos[V_Z] = rEyeTransZ;
     
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
     rR_surf_norm [V_X] = (float) (pPll_polygon->Surface_norm_x);
     rR_surf_norm [V_Y] = (float) (pPll_polygon->Surface_norm_y);
     rR_surf_norm [V_Z] = (float) (pPll_polygon->Surface_norm_z);
     
     /*
      * Find the point in the center of the polygon
      */
     V_FindCenterOfPolygon( arVer_orig, (pPll_polygon->cVer_orig_numv - 1), 
			   rR_point_pos );
     
     /*
      * determine and set rR_ra and rR_rd 
      */
     V_DetermineRaRd( N_poly_color, rR_ra, rR_rd );
     
     V_CalculateIntensities( rR_point_pos, rR_light_pos, rR_eye_pos, 
			    rR_reflect, rR_surf_norm, rR_ra, rR_rd, temp1N_intensity );
     
     /*
      * Put the intensity within range
      */
     N_color = N_BestFitColor( vdevice.rrrN_colormap [N_poly_color], 
			      temp1N_intensity [d_INDEX_RED], temp1N_intensity [d_INDEX_GREEN], 
			      temp1N_intensity [d_INDEX_BLUE] );
     
     /*
      * Fill
      */
     N_color = N_color + d_COLMAP_OFF + (N_poly_color * d_LEN_COLRAMP);
     
     
     if( d_FROM_POLYGON == F_called_from )
     {
	  color( N_color );
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
V_CalculateIntensities( 
	float	rR_point_pos [3],
	float	rR_light_pos [3],
	float	rR_eye_pos [3],
	float	rR_reflect [3],
	float	rR_surf_norm [3],
	float	rR_ra [3],
	float	rR_rd [3],
	float	rR_intensity [3] )
{

     const float	k_rR_Ia [3] = {0.57, 0.57, 0.57};  /* ambient light */
     const float	k_rR_Is [3] = {0.4, 0.4, 0.4};
     
     const float	k_R_Rs = 2.0; /* contrast control */
     const float	k_R_f = 1.0;
     const float	k_R_max_intensity = 1.0;
     const float	k_R_min_intensity = 0.0;
     
     float	rR_light_norm [3];
     float	rR_eye_norm [3];
     float	rR_reflect_norm [3];
     float	rR_tmp [3];
     float	R_tmp_dot1;
     float	R_tmp_dot2;
     
     
     /*
      * Make the vector from the point to the eye 
      */
     V_MakeVector( rR_eye_pos, rR_point_pos, rR_tmp );
     V_Normalize( rR_tmp, rR_eye_norm );
     
     
     /*
      * Make the vector from the point to the light source
      */
     V_MakeVector( rR_light_pos, rR_point_pos, rR_tmp );
     V_Normalize( rR_tmp, rR_light_norm );
     
     
     
     /*
      * Make the reflection vector
      */
     /*	V_CalculateURVector( rR_tmp, rR_surf_norm, rR_reflect_norm );
	
	printf("ReflectNorm: %f, %f, %f\n", rR_reflect_norm[0], rR_reflect_norm[1], rR_reflect_norm[2]);*/
     rR_reflect_norm[0] = 0.0;
     rR_reflect_norm[1] = 0.0;
     rR_reflect_norm[2] = 0.0;
     
     /*
      * Calculate:
      *   ( Us . Un )
      *   rs ( Ur . Uv ) ^ f
      */
     R_tmp_dot1 = R_DotProduct( rR_eye_norm, rR_surf_norm );
     R_tmp_dot2 = R_DotProduct( rR_reflect_norm, rR_eye_norm );
     R_tmp_dot2 = pow( R_tmp_dot2, k_R_f );  
     R_tmp_dot2 = k_R_Rs * R_tmp_dot2;
     
     /*
      * Calculate the R, G, B intesities	
      */
     rR_intensity [d_INDEX_RED] = fabs( 
				       (k_rR_Ia [d_INDEX_RED] * rR_ra [d_INDEX_RED]) +
				       k_rR_Is [d_INDEX_RED] * 
				       ( (rR_rd [d_INDEX_RED] * R_tmp_dot1) + R_tmp_dot2 ) );
     
     rR_intensity [d_INDEX_GREEN] = fabs( 
					 (k_rR_Ia [d_INDEX_GREEN] * rR_ra [d_INDEX_GREEN]) +
					 k_rR_Is [d_INDEX_GREEN] * 
					 ( (rR_rd [d_INDEX_GREEN] * R_tmp_dot1) + R_tmp_dot2 ) );
     
     rR_intensity [d_INDEX_BLUE] = fabs(  
					(k_rR_Ia [d_INDEX_BLUE] * rR_ra [d_INDEX_BLUE]) +
					k_rR_Is [d_INDEX_BLUE] * 
					( (rR_rd [d_INDEX_BLUE] * R_tmp_dot1) + R_tmp_dot2 ) );
     
     /*
      * The intensity equation just produces an intensity, it is
      * up to us to make sure it is a number we can use.
      *
      * Make sure we aren't out of range
      */
     
     if (k_R_max_intensity < rR_intensity [d_INDEX_RED])
	  rR_intensity [d_INDEX_RED] = k_R_max_intensity; 
     
     if (k_R_min_intensity > rR_intensity [d_INDEX_RED])
	  rR_intensity [d_INDEX_RED] = k_R_min_intensity; 
     
     if (k_R_max_intensity < rR_intensity [d_INDEX_GREEN])
	  rR_intensity [d_INDEX_GREEN] = k_R_max_intensity;
     
     if (k_R_min_intensity > rR_intensity [d_INDEX_GREEN])
	  rR_intensity [d_INDEX_GREEN] = k_R_min_intensity;
     

     if (k_R_max_intensity < rR_intensity [d_INDEX_BLUE])
	  rR_intensity [d_INDEX_BLUE] = k_R_max_intensity;
     
     if (k_R_min_intensity > rR_intensity [d_INDEX_BLUE])
	  rR_intensity [d_INDEX_BLUE] = k_R_min_intensity;
     

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
V_CalculateURVector(  float rR_light [3], float rR_surf_norm [3],
		    float rR_reflect_norm [3] )
{
     float	R_dot;
     float	rR_reflect [3];
     
     
     /*
      * 2 ( s . un )
      */
     R_dot = R_DotProduct( rR_light, rR_surf_norm ); 
     R_dot = 2.0 * R_dot;
     
     /*
      * -s + 2 ( s . un ) un
      */
     rR_reflect [V_X] = (-rR_light [V_X]) + (R_dot * rR_surf_norm [V_X]);
     rR_reflect [V_Y] = (-rR_light [V_Y]) + (R_dot * rR_surf_norm [V_Y]);
     rR_reflect [V_Z] = (-rR_light [V_Z]) + (R_dot * rR_surf_norm [V_Z]);
     
     
     /*
      * Normalize
      */
     V_Normalize( rR_reflect, rR_reflect_norm );
     
}





/*
 * Find the center of a polygon
 */
void
V_FindCenterOfPolygon( Vector *arVer_orig, int	cVer_numv,
		      float rR_point_pos [3] )
{
     
     int	i;
     
     
     for ( i = 0; i < cVer_numv; i++ )
     {
	  rR_point_pos [V_X] = (float) (arVer_orig[i][V_X] + rR_point_pos[V_X]);
	  rR_point_pos [V_Y] = (float) (arVer_orig[i][V_Y] + rR_point_pos[V_Y]);
	  rR_point_pos [V_Z] = (float) (arVer_orig[i][V_Z] + rR_point_pos[V_Z]);
     }
     
     rR_point_pos [V_X] = rR_point_pos [V_X] / (float) cVer_numv;
     rR_point_pos [V_Y] = rR_point_pos [V_Y] / (float) cVer_numv;
     rR_point_pos [V_Z] = rR_point_pos [V_Z] / (float) cVer_numv;
}





/*
 * Given two points construct a vector
 */
void
V_MakeVector( float a [3], float b [3], float c [3] )
{
     c [V_X] = a [V_X] - b [V_X];
     c [V_Y] = a [V_Y] - b [V_Y];
     c [V_Z] = a [V_Z] - b [V_Z];
}





/*
 * Given a vector normalize it
 */
void
V_Normalize( float a [3], float b [3] ) 
{
     double	R_denom;
     
     /*	R_denom = (float) sqrt( (double) (SQ(a[V_X]) + SQ(a[V_Y]) + SQ(a[V_Z])) );*/
     R_denom = sqrt( (SQ(a[V_X]) + SQ(a[V_Y]) + SQ(a[V_Z])) );
     
     if ( 0.0 != R_denom )
     {
	  b [V_X] = a [V_X] / R_denom;
	  b [V_Y] = a [V_Y] / R_denom;
	  b [V_Z] = a [V_Z] / R_denom;
     }
     else
     {
	  b [V_X] = 0.0;
	  b [V_Y] = 0.0;
	  b [V_Z] = 0.0;
     }
}




/*
 * Dot Product using floats 
 */
float
R_DotProduct( float a [3], float b [3] )
{
     return( (a [0]*b [0]) + (a [1]*b [1]) + (a [2]*b [2]) );
}






/*
 * This is NOT the final function - just a test one
 */
int N_BestFitColor( int rrN_colormap [d_LEN_COLRAMP][3],
		   float R_fact_red,
		   float R_fact_green, 
		   float R_fact_blue )
  
{
     
     int		rrN_colormap2 [d_LEN_COLRAMP][3];
     int     rrN_colormap3 [d_LEN_COLRAMP][2];
     int		N_new_red;
     int		N_new_green;
     int		N_new_blue;
     int		irN_cmap2 = 0;
     int		irN_cmap3 = 0;
     int		i;
     
     
     
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
V_DetermineRaRd( int F_base_color,
				 float	rR_ra [3],
				 float 	rR_rd [3] )
{
	rR_ra [d_INDEX_RED]   = rR_rd [d_INDEX_RED] =
							vdevice.rrR_rard [F_base_color][d_INDEX_RED];
	rR_ra [d_INDEX_GREEN] = rR_rd [d_INDEX_GREEN] =	
							vdevice.rrR_rard [F_base_color][d_INDEX_GREEN];
	rR_ra [d_INDEX_BLUE]  = rR_rd [d_INDEX_BLUE] =
							vdevice.rrR_rard [F_base_color][d_INDEX_BLUE];
}






/*
 * Creates a colormaps ramp
 */
void
V_AddMapColor( int irF_color, int N_red, int N_green, int N_blue )
{

     /*
      * make magic
      */
     irF_color = irF_color - d_COLMAP_OFF;
     
     
     /*
      * Check to see if this is valid - or is the color we are trying to 
      * replace in use
      */
     if ( TRUE == vdevice.rF_coluse [irF_color] )
     {
	  printf( "Can Not allocate new colors, colors already in use\n" );
	  return;
     }
     
     
     /*
      * Signal that we are using this color 
      */
     vdevice.rF_coluse [irF_color] = TRUE;
     
     
     /*
      * Compute the Ra and Rd index 
      */
     V_AssignRaRd( irF_color, N_red, N_green, N_blue );
     
     
     /*
      * Compute the Color Ramp
      */
     V_AssignColorRamp( irF_color, N_red, N_green, N_blue );
}




/*
 * Creates a single color
 */
void
V_AddSingleColor( int irF_color, int N_red, int N_green, int N_blue )
{
     /*
      * Check to see if this is valid - or is the color we are trying to
      * replace in use
      */
     if ( TRUE == vdevice.rF_sinuse [irF_color] )
     {
	  printf( "Can Not allocate new colors, colors already in use\n" );
	  return;
     }
     
     /*
      * Signal that we are using this color
      */
     vdevice.rF_sinuse [irF_color] = TRUE;
     
     /*
      * Map the color
      */
#ifdef MACINTOSH
     mapcolor( irF_color, N_red * 0xFFFF / 256, N_green * 0xFFFF / 256,
	      N_blue * 0xFFFF / 256 );
#else
     mapcolor( irF_color, N_red, N_green, N_blue );
#endif
     
}




/*
 * Destroys a colormap ramp
 */
void
V_DestroyMapColor( int irF_color )
{
     /*
      * Signal that we are no longer using this color
      */
     vdevice.rF_coluse [irF_color] = FALSE;
}





/*
 * Destroys a single color 
 */
void
V_DestroySingleColor( int irF_color )
{
     if ( (1 == irF_color) || (0 == irF_color) )
     {
	  return;
     }
     
     /*
      * Signal that we are no longer using this color
      */
     vdevice.rF_sinuse [irF_color] = FALSE;
}





void
V_AssignColorRamp( int irN_color, int N_red, int N_green, int N_blue )
{
     
     int i;
     int	N_curr_col;
     int	cCol_k = 0;
     
     
     
     /*
      * Red component
      */
     N_curr_col = N_red;
     if ( 0 != N_red )
     {
	  for ( i = 0; i < d_LEN_COLRAMP; i++ )
	  { 
	       vdevice.rrrN_colormap [irN_color][d_LEN_COLRAMP-1-i][d_INDEX_RED] = 
		    N_curr_col;
	       
	       N_curr_col = abs( N_curr_col - d_SKIP);
	  }
     }
     else
     {
	  for ( i = 0; i < d_LEN_COLRAMP; i++ ) 
	  {
	       vdevice.rrrN_colormap [irN_color][i][d_INDEX_RED] = 0;
	  }
     }
     
     
     /*
      * Green component
      */
     N_curr_col = N_green;    
     if ( 0 != N_green )
     {
	  for ( i = 0; i < d_LEN_COLRAMP; i++ ) 
	  {
	       vdevice.rrrN_colormap [irN_color][d_LEN_COLRAMP-1-i][d_INDEX_GREEN]
		    = N_curr_col; 
	       
	       N_curr_col = abs( N_curr_col - d_SKIP);
	  }
     }
     else
     {
	  for ( i = 0; i < d_LEN_COLRAMP; i++ ) 
	  {
	       vdevice.rrrN_colormap [irN_color][i][d_INDEX_GREEN] = 0;
	  }
     }
     
     
     /*
      * Blue component
      */
     N_curr_col = N_blue;    
     if ( 0 != N_blue )
     {
	  for ( i = 0; i < d_LEN_COLRAMP; i++ ) 
	  {
	       vdevice.rrrN_colormap [irN_color][d_LEN_COLRAMP-1-i][d_INDEX_BLUE] 
		    = N_curr_col; 
	       
	       N_curr_col = abs( N_curr_col - d_SKIP);
	  }
     }
     else
     {
	  for ( i = 0; i < d_LEN_COLRAMP; i++ ) 
	  {
	       vdevice.rrrN_colormap [irN_color][i][d_INDEX_BLUE] = 0;
	  }
     }
     
     

     /*
      * Here we actually go out to the window manager's colormap and
      * edit that
      */
#ifdef MACINTOSH
     cCol_k = d_COLMAP_OFF + (irN_color * d_LEN_COLRAMP);
     for ( i = 0; i < d_LEN_COLRAMP; i++ )
     {
	  mapcolor( cCol_k, 
		   vdevice.rrrN_colormap [irN_color][i][d_INDEX_RED] * 0xFFFF / 256,   
		   vdevice.rrrN_colormap [irN_color][i][d_INDEX_GREEN] * 0xFFFF / 256, 
		   vdevice.rrrN_colormap [irN_color][i][d_INDEX_BLUE] * 0xFFFF / 256 );
	  cCol_k++;
     }
#else
     cCol_k = d_COLMAP_OFF + (irN_color * d_LEN_COLRAMP);
     for ( i = 0; i < d_LEN_COLRAMP; i++ )
     {
	  mapcolor( cCol_k, 
		   vdevice.rrrN_colormap [irN_color][i][d_INDEX_RED],  
		   vdevice.rrrN_colormap [irN_color][i][d_INDEX_GREEN], 
		   vdevice.rrrN_colormap [irN_color][i][d_INDEX_BLUE] );
	  cCol_k++;
     }
#endif
     
}





void
V_AssignRaRd( int irR_color, int N_red, int N_green, int N_blue )
{
     if ( 0 == N_red )
	  vdevice.rrR_rard [irR_color][d_INDEX_RED] = 0.0;
     else
	  vdevice.rrR_rard [irR_color][d_INDEX_RED] = 1.0;
     
     if ( 0 == N_green )
	  vdevice.rrR_rard [irR_color][d_INDEX_GREEN] = 0.0;
     else
	  vdevice.rrR_rard [irR_color][d_INDEX_GREEN] = 1.0;
     
     if ( 0 == N_blue )
	  vdevice.rrR_rard [irR_color][d_INDEX_BLUE] = 0.0;
     else
	  vdevice.rrR_rard [irR_color][d_INDEX_BLUE] = 1.0;
}




/*
 * changes how often objects are shaded
 */
void
V_ShadingFreq( int F_shade_freq )
{
     vdevice.F_shade_freq = F_shade_freq;
}



/*
 * Init colors
 */
void
V_InitColors( void )
{
     V_MapColor( 2, 255, 0, 0 );		/* red */
     V_MapColor( 3, 0, 255, 0 );		/* green */
     V_MapColor( 4, 255, 255, 0 );	/* yellow */
     V_MapColor( 5, 0, 0, 255 );		/* blue */
     V_MapColor( 6, 255, 0, 255 );	/* magenta */
     V_MapColor( 7, 0, 255, 255 );	/* cyan */
     
     V_MapColor( 9, 198, 45, 51 );		/* Wendy red */
     V_MapColor( 10, 139, 29, 109 );		/* Wendy purple */
     V_MapColor( 11, 58, 132, 21 );		/* Wendy green */
     V_MapColor( 12, 0, 108, 172 );	    /* Wendy blue */
     V_MapColor( 13, 227, 120, 0 );	    /* Wendy orange */
     V_MapColor( 14, 229, 180, 0 );		/* Wendy yellow */
     V_MapColor( 15, 161, 11, 0 );	    /* Wendy tan */
     V_MapColor( 16, 0, 81, 57 );	   	/* Wendy groundplane_green */
     V_MapColor( 17, 88, 88, 88 );		/* Wendy groundplane_grey */
}





/*
 * Colors range from values 0 - 21
 *
 * The first nine colors (0-d_COLMAP_OFF) are "single colors" 
 *		no colormap for them gets created, they aren't good for 
 *		using as a base	color for an object you want to shade
 *
 * By default 0 = BLACK and 1 = WHITE -- this doesn't ever change
 *
 * The rest of the colors have a nice colormap 	accompanying them. 
 *		They are great for objects you want to shade.
 */ 
void
V_MapColor( int irF_color, int N_red, int N_green, int N_blue )
{
     /*
      * Valid color ?
      */
     if ( (irF_color < 0) || (irF_color > (d_TOTAL_COLS-1)) )
     {
	  printf( "Error: Invalid Color Index\n" );
	  return;
     }
     if ( ((N_red < 0) && (N_red > 255)) || 
	 ((N_green < 0) && (N_green > 255)) ||
	 ((N_blue < 0) && (N_blue > 255)) )
     {
	  printf( "Error: Invalid Color\n" );
	  return;
     }
     
     /*
      * Given a color index decide what to do with it
      */
     if ( irF_color < d_COLMAP_OFF )
     {
	  V_AddSingleColor( irF_color, N_red, N_green, N_blue );
     }
     else
     {
	  V_AddMapColor( irF_color, N_red, N_green, N_blue );
     }
}



/*
 * Given a Color index this function frees the index so it can
 * 		be re-used (recolored)
 */ 
void
V_DestroyColor( int irF_color )
{
     /*
      * Valid color ?
      */
     if ( (irF_color < 0) || (irF_color > (d_TOTAL_COLS-1)) )
     {
	  printf( "Error: Invalid Color Index\n" );
	  return;
     }
     
     /*
      * Given a color index decide what to do with it
      */
     if ( irF_color < d_COLMAP_OFF )
     {
	  V_DestroySingleColor( irF_color );
     }
     else
     {
	  V_DestroyMapColor( irF_color );
     }
}






