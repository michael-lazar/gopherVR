/********************************************************************
 * $Id: geometry.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

/*
 * Some of these routines were written by us, others (most) were take
 * from a file found on Princeton's FTP server. The file had no indication
 * of ownership - so we don't know who to thank or credit for these routines.
 *
 *				-- the Gopher team
 */

#include "vogl.h"
#include "geometry.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif


#define EPSILON 0.000001
#define SQ(a)   ((a)*(a))




/*
 * Find intersection between the two given lines.
 * Returns TRUE if an intersection is found, FALSE otherwise.
 */

int find_line_intersect_line(float rR_a_start[3], float rR_a_end[3], 
			     float rR_b_start[3], float rR_b_end[3], 
			     float rR_pint[3])
{
    float  d1[3], d2[3];
    double det, t1;

    (void)direction_point_to_point( rR_a_start, rR_a_end, d1);
    (void)direction_point_to_point( rR_b_start, rR_b_end, d2);

    /*
     * Find where they intersect in at least 2 dimensions
     */
    det = -d1[0]*d2[1] + d1[1]*d2[0];
    if (0.0 != det)
    {
	 t1 = d2[1] * (rR_a_start[0] - rR_b_start[0]) - 
	      d2[0] * (rR_a_start[1] - rR_b_start[1]);
	 t1 /= det;
    } 
    else 
    {
	 det = -d1[0]*d2[2] + d1[2]*d2[0];
	 if ( 0.0 != det ) 
	 {
	      t1 = d2[2] * (rR_a_start[0] - rR_b_start[0]) -
		   d2[0] * (rR_a_start[2] - rR_b_start[2]);
	      t1 /= det;
	 } 
	 else 
	 {
	      det = -d1[1] * d2[2] + d1[2] * d2[1];
	      if ( 0.0 != det ) 
	      {
		   t1 = d2[2] * (rR_a_start[1] - rR_b_start[1]) -
			d2[1] * (rR_a_start[2] - rR_b_start[2]);
		   t1 /= det;
	      } 
	      else 
	      {
		   return( FALSE );
	      }
	 }
    }

    /*
     *  t1 is distance from a->from to possible point of intersection 
     * compute point of intersection 
     */
    rR_pint[0] = rR_a_start[0]; 
    rR_pint[1] = rR_a_start[1]; 
    rR_pint[2] = rR_a_start[2];
    
    vector_plus_scalar_mult(rR_pint, t1, d1);
    
    /* 
     * determine if point is on the other line as well 
     * not exactly elegant, but gets the job done
     */ 
    return( detect_point_on_segment(rR_pint, rR_b_start, rR_b_end ) );
}




/*
 * Given a 2 line segments (A and B) it checks to see if line
 * A is on line B
 */
int B_IsLineOnLine(float rR_endpnt_A1[4], float rR_endpnt_A2[4], 
		   float rR_endpnt_B1[4], float rR_endpnt_B2[4] )
{
	float   rR_tmp_point1 [3], rR_tmp_point2 [3];
	float   rR_tmp_l1 [3], rR_tmp_l2 [3];
	int		i = 0;
	
	rR_tmp_point1 [V_X] = rR_endpnt_A1 [V_X]; 
	rR_tmp_point1 [V_Y] = rR_endpnt_A1 [V_Y];
	rR_tmp_point1 [V_Z] = rR_endpnt_A1 [V_Z];

/*
printf( "Point1 = %f %f %f\n", rR_tmp_point1 [V_X], rR_tmp_point1 [V_Y], rR_tmp_point1 [V_Z] );
*/

    rR_tmp_point2 [V_X] = rR_endpnt_A2 [V_X];
	rR_tmp_point2 [V_Y] = rR_endpnt_A2 [V_Y];
	rR_tmp_point2 [V_Z] = rR_endpnt_A2 [V_Z];


/*
printf( "Point2 = %f %f %f\n", rR_tmp_point2 [V_X], rR_tmp_point2 [V_Y], rR_tmp_point2 [V_Z] );
*/

	rR_tmp_l1 [V_X] = rR_endpnt_B1 [V_X];
	rR_tmp_l1 [V_Y] = rR_endpnt_B1 [V_Y];
	rR_tmp_l1 [V_Z] = rR_endpnt_B1 [V_Z];

	rR_tmp_l2 [V_X] = rR_endpnt_B2 [V_X];
	rR_tmp_l2 [V_Y] = rR_endpnt_B2 [V_Y];
	rR_tmp_l2 [V_Z] = rR_endpnt_B2 [V_Z];

	if ( detect_point_on_segment2( rR_tmp_point1, rR_tmp_l1, rR_tmp_l2 ) ) 
		i++;

	if ( detect_point_on_segment2( rR_tmp_point2, rR_tmp_l1, rR_tmp_l2 ) )
		i++;
/*
	printf( "line is= %d\n\n", i );
*/
	if (2 == i)
		return( TRUE );
	else
		return( FALSE );
}





    /*
     * Given a Point and a Line Segment, is the point on the
     * Line Segemnt?
     */
int detect_point_on_segment( float point[3], float l1[3], float l2[3] )
{
    int coord;
    if (distance_point_to_point( l2, l1 ) < EPSILON) /* b is a point */
        if (distance_point_to_point(point, l2) < EPSILON)
            return TRUE;
        else
            return FALSE;
    if (l2[0] == l1[0]) /* segment of constant x*/
        if (l2[1] == l1[1]) /* segment of constant y */
            coord = 2;
        else
            coord = 1;
    else
        coord = 0;

    if (l2[coord] < l1[coord])
        if ( (point[coord] >= l2[coord]) && (point[coord] <= l1[coord]))
            return TRUE;
        else
            return FALSE;
    else
        if ( (point[coord] >= l1[coord]) && (point[coord] <= l2[coord]))
            return TRUE;
        else
            return FALSE;
}





    /*
     * A point is on a line segment if:
     *  - the point is either one of the endpoints
     *  - the 2 vectors made from the point to one endpoint and from
     *    the point to the other endpoint add up to the vector made
     *    from one endpoint to the other AND the angle between the
     *    two vectors contructed from the point are at 180o angle
     *    with each other.
     *
     *              vect1              vect2
     *  endpnt1<---------------*-------------------------->endpnt2
     *                       point
     *
     *  -GopherTeam
     */
int detect_point_on_segment2( float point[3], float l1[3], float l2[3] )
{
   
    float   vect1 [3], vect2 [3];
    double  dot, norm1, norm2;


    /*
     * Do a quick check on the endpoints
     */
    if ( (point [V_X] == l1 [V_X]) && (point [V_Y] == l1 [V_Y]) &&
            (point [V_Z] == l1 [V_Z]) )
    {
        return( TRUE );
    }
    if ( (point [V_X] == l2 [V_X]) && (point [V_Y] == l2 [V_Y]) &&
            (point [V_Z] == l2 [V_Z]) )
    {
        return( TRUE );
    }

    /*
     * since that didn't work
     */
    vect1 [V_X] = point [V_X] - l1 [V_X];
    vect1 [V_Y] = point [V_Y] - l1 [V_Y];
    vect1 [V_Z] = point [V_Z] - l1 [V_Z];

    vect2 [V_X] = l2 [V_X] - point [V_X];
    vect2 [V_Y] = l2 [V_Y] - point [V_Y];
    vect2 [V_Z] = l2 [V_Z] - point [V_Z];

    if ( ((vect1 [V_X] + vect2 [V_X]) != (l2 [V_X] - l1 [V_X])) ||
         ((vect1 [V_Y] + vect2 [V_Y]) != (l2 [V_Y] - l1 [V_Y])) ||
         ((vect1 [V_Z] + vect2 [V_Z]) != (l2 [V_Z] - l1 [V_Z])) )
    {
        return( FALSE );
    }

	/*
     * Neither did that
     */
    dot = vect1 [V_X]*vect2 [V_X] + vect1 [V_Y]*vect2 [V_Y] + 
				vect1 [V_Z]*vect2 [V_Z];
    norm1 = vect1 [V_X]*vect1 [V_X] + vect1 [V_Y]*vect1 [V_Y] + 
				vect1 [V_Z]*vect1 [V_Z];
    norm2 = vect2 [V_X]*vect2 [V_X] + vect2 [V_Y]*vect2 [V_Y] + 
				vect2 [V_Z]*vect2 [V_Z];

    if ( 1.0 == ( dot / (sqrt(norm1) * sqrt(norm2)) ) )
    {
        return( TRUE );
    }
    else
    {
        return( FALSE );
    }

}





	/*
	 * Compute normlized vector from a to b, store result in c.
	 * Length of c before normalization is returned.
	 */
double direction_point_to_point(float a[3], float b[3], float c[3])
{
    double len;


	c[0] = a[0] - b[0]; c[1] = a[1] - b[1]; c[2] = a[2] - b[2];
    len = sqrt(dot_product(c, c));

    if (len < EPSILON)
        return( (double) 0.0 );

    c[0] /= len; c[1] /= len; c[2] /= len;
    return((double) len );
}


	/*
	 * replaces a by a + b*c
	 */
void vector_plus_scalar_mult( float a[3], double b, float c[3] )
{
    a[0] += b*c[0]; a[1] += b*c[1]; a[2] += b*c[2];
}







	/*
	 * Already have thesse
	 */
double  distance_point_to_point( float a[3], float b[3] ) 
{
    double dret;

    dret = (a[0]-b[0])*(a[0]-b[0]) + 
            (a[1]-b[1])*(a[1]-b[1]) + 
            (a[2]-b[2])*(a[2]-b[2]);
    return( (double) sqrt(dret) );
}

double dot_product( float a[3], float b[3] )
{
    double rv;
    rv = a[0]*b[0] + a[1]*b[1] + a[2]*b[2] ;
    return((double) rv);
}






    /*
     * A Plane is created given three points
     */
int find_plane_3points(float a[3], float b[3], float c[3], float pl[4])
{
    double co_xy, co_yz, co_zx; /* cofactors */
    double det ; /* determinant*/
    double len; /* for normalization */

    co_xy = a[0] * (b[1] - c[1]) +
            b[0] * (c[1] - a[1]) +
            c[0] * (a[1] - b[1]);

    co_yz = a[1] * (b[2] - c[2]) +
            b[1] * (c[2] - a[2]) +
            c[1] * (a[2] - b[2]);

    co_zx = a[2] * (b[0] - c[0]) +
            b[2] * (c[0] - a[0]) +
            c[2] * (a[0] - b[0]) ;

    det = a[0] * (b[1]*c[2] - c[1]*b[2]) +
          b[0] * (c[1]*a[2] - a[1]*c[2]) +
          c[0] * (a[1]*b[2] - b[1]*a[2]);

    if (sqrt( (co_xy*co_xy) + (co_yz*co_yz) + (co_zx*co_zx) ) < EPSILON)
    {
        /* degenerate plane, points are colinear */
        return FALSE;
    }

    pl[0] = co_yz;
    pl[1] = co_zx;
    pl[2] = co_xy;
    pl[3] = -det;
    len = sqrt(dot_product(pl, pl));
    pl[0] /= len ; pl[1] /= len; pl[2] /= len; pl[3] /= len;
    return TRUE;
}






    /*
     * determine the intersection for the plane a with the line l
     * and return this in the point p.
     * return 0 if the line and plane are parallel even if the line
     * lies in the plane, 1 otherwise
     */
int plane_intersect_line( float plane[4], float l1[3], float l2[3], float p[3])
{
    double  den, s;
    float   q[3];
    double  dotprod1, dotprod2;
    int     i;

    q[0] = plane[0]; q[1] = plane[1]; q[2] = plane[2];

    dotprod1 = dot_product( q, l1 );
    dotprod2 = dot_product( q, l2 );
    den = dotprod1 - dotprod2;

    if ( fabs(den) < EPSILON )     /* parallel case */
          return FALSE;
    else
    {
        s = (dotprod1 + plane[3]) / den;

        for (i = 0 ; i < 3 ; i++ )
            p[i] = l1[i] * (1.0-s) + l2[i] * s;

    /*
     * We know the plane intersects the line, but does it
     * intersect the line segment?
     */
        if ( detect_point_on_segment( p, l1, l2 ) )
           return TRUE;
        else
            return FALSE;
    }
}








    /*
     * Given the count of vertices in a polygon, and a list
     * of those vertices given a point we determine if that point is
     * on the polygon
     *
     * The point is on the polygon IF the sum of the angles between
     * each pair of vertices is equal to 360degrees.
     *
     *
     * If the point is right on the border of the polygon we don't
     * count it as being on the polygon. This is wrong, BUT since
     * this routine is going to get text on the polygon, and since
     * the border is painted BLACK we assume that you didn't want
     * to put text on the border. 
     *
     * The routine to detect the point on the edges is easy, but 
     * with the above assumption we take an already expensive 
     * routine and try to make it a bit faster. If the assumption
     * is wrong we can always add the code to correct the result.
     *
     *                      -- the Gopher team
     */
int IsPointOnPolygon( float rR_point[3], int cVer_numv, float aVer_polygon[][4] )
{
    int     i;
    double  C_total_degrees = 0.0;


    /*
     * Since the last point in each polygon is also the same as the
     * first point in each polygon we don't want to take the angle
     * between the first and last point
     */
    cVer_numv = cVer_numv - 1;

    for( i = 0; i < cVer_numv; i++ )
    {
        C_total_degrees = C_total_degrees + (acos(
            cosine_angle( aVer_polygon[i], rR_point, aVer_polygon[i+1] ) )
            * R2D);
    }

    if ( fabs(360.0 - C_total_degrees) < EPSILON )
        return( TRUE );
    else
        return( FALSE );
}





    /*
     * returns the cosine of the angle from a to b to c 
     *
     * Look in math books to see why this works, the formula is:
     *
     *     <u,v>
     *   ----------- = cos theta
     *   ||u|| ||v||
     */

double cosine_angle( float rR_a[3], float rR_point[3], float rR_c[3])
{
    float   ab[3], cb[3];
    double  dot, norm1, norm2;

    ab[0] = rR_a [0] - rR_point [0]; 
    ab[1] = rR_a [1] - rR_point [1]; 
    ab[2] = rR_a [2] - rR_point [2];

    cb[0] = rR_c [0] - rR_point [0]; 
    cb[1] = rR_c [1] - rR_point [1]; 
    cb[2] = rR_c [2] - rR_point [2];

    dot   = dot_product(ab,cb);
    norm1 = dot_product(ab,ab);
    norm2 = dot_product(cb,cb);

    if ((norm1 < 1E-8) || (norm2 < 1E-8))   {
        verror( "ill formed dot product, cannot compute cosine" );
        return((double) 0.0);
    }

    return( (double) (dot / (sqrt(norm1) * sqrt(norm2))) );
}



