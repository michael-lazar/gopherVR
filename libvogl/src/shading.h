/********************************************************************
 * $Id: shading.h,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _SHADING_H_
#define _SHADING_H_

#include <stdio.h>
#include "vogl.h"
#include "bsp.h"

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif


#include "geometry.h"		/* for dot_product() */


int N_FlatShading( P_POLYLIST_LEAF pPll_polygon, Vector *arVer_orig,
		  int cVer_clip, int prN_x_points[], int prN_y_points[],
		  int F_called_from );

void V_CalculateIntensities( float rD_point_pos [3], float rD_light_pos [3],
			    float rD_eye_pos [3], float rD_reflect_norm [3],
			    float rD_surf_norm [3], float  rD_ra [3],
			    float rD_rd [3], float rN_intensity [3] );


void V_CalculateURVector( float rD_light [3], float rD_surf_norm [3],
			 float rD_reflect_norm [3] );

void V_FindCenterOfPolygon( Vector  *arVer_orig, int cVer_numv,
			   float rD_point_pos [3] );

int N_BestFitColor( int rrN_colormap [d_LEN_COLRAMP][3],
		   float R_fact_red,
		   float R_fact_green, 
		   float R_fact_blue );

void V_DetermineRaRd( int F_base_color, float rD_ra [3], float rD_rd [3] );

void V_MakeVector( float a [3], float b [3], float c [3] ); 

void V_Normalize( float a [3], float b [3] );

float R_DotProduct( float a [3], float b [3] );

void V_AssignColorRamp( int irN_color, int N_red, int N_green, int N_blue );

void V_AddMapColor( int irF_color, int N_red, int N_green, int N_blue );

void V_DestroyMapColor( int irF_color );

void V_AddSingleColor( int irF_color, int N_red, int N_green, int N_blue );

void V_DestroySingleColor( int irF_color );

void V_AssignRaRd( int irR_color, int N_red, int N_green, int N_blue );

void V_ShadingFreq( int F_shade_freq );

void V_InitColors( void );

void V_MapColor( int irF_color, int N_red, int N_green, int N_blue );

void V_DestroyColor( int irF_color );

#endif /* _SHADING_H_ */
