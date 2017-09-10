/********************************************************************
 * $Id: shading_d.h,v 1.1.1.1 2002/01/18 16:34:27 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _SHADING_D_H_
#define _SHADING_D_H_

#include <stdio.h>
#include "vogl.h"
#include "bsp.h"

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif



int N_DFlatShading( P_POLYLIST_LEAF pPll_polygon, Vector *arVer_orig,
					int cVer_clip, int prN_x_points[], int prN_y_points[],
					int F_called_from );

void V_DCalculateIntensities( double rD_point_pos [3], double rD_light_pos [3],
							 double rD_eye_pos [3], double rD_reflect_norm [3],
							 double rD_surf_norm [3], double  rD_ra [3],
							 double rD_rd [3], int rN_intensity [3] );

void V_DCalculateURVector( double rD_light [3], double rD_surf_norm [3],
						  double rD_reflect_norm [3] );

void V_DFindCenterOfPolygon( Vector  *arVer_orig, int cVer_numv,
							double rD_point_pos [3] );

int N_DBestFitColor( int rrN_colormap [8][3], int N_red, int N_green, 
						int N_blue );

void V_DDetermineRaRd( int F_base_color, double rD_ra [3], double rD_rd [3] );

void V_DMakeVector( double a [3], double b [3], double c [3] ); 

void V_DNormalize( double a [3], double b [3] );

double D_DDotProduct( double a [3], double b [3] );



#endif /* _SHADING_D_H_ */
