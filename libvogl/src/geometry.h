/********************************************************************
 * $Id: geometry.h,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_



/* protos from geometry.c */

int find_line_intersect_line( float rR_a_start[3], float rR_a_end[3], 
                              float rR_b_start[3], float rR_b_end[3], 
                              float rR_pint[3]);

int B_IsLineOnLine( float rR_endpnt_A1[4], float rR_endpnt_A2[4], 
                    float rR_endpnt_B1[4], float rR_endpnt_B2[4] );


int detect_point_on_segment( float point[3], float l1[3], float l2[3] );
int detect_point_on_segment2( float point[3], float l1[3], float l2[3] );
double direction_point_to_point( float a[3], float b[3], float c[3]);
void vector_plus_scalar_mult( float a[3], double b, float c[3] );
double distance_point_to_point( float a[3], float b[3] );
double dot_product( float a[3], float b[3] );
int find_plane_3points( float a[3], float b[3], float c[3], float pl[4]);
int plane_intersect_line( float plane[4], float l1[3], float l2[3], float p[3]);
int IsPointOnPolygon( float rR_point[3], int cVer_numv, 
                      float aVer_polygon[][4] );
double cosine_angle( float rR_a[3], float rR_point[3], float rR_c[3]);

#endif /* _GEOMETRY_H_ */
