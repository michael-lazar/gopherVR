/********************************************************************
 * $Id: minv4.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"
#include "minv4.h"

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif



/*
 * minv4
 *
 *	find the inverse of the 4 by 4 matrix b using gausian elimination
 * and return it in a.
 * 
 *	(We don't actually use this yet in VOGL - maybe one day).
 */
void minv4(Matrix a, Matrix b)
{
	float	val, val2;
	int	i, j, k, ind;
	Matrix	tmp;

	identmatrix(a);

	COPYMATRIX(tmp, b);

	for (i = 0; i != 4; i++) {

		val = tmp[i][i];		/* find pivot */
		ind = i;
		for (j = i + 1; j != 4; j++) {
			if (fabs(tmp[j][i]) > fabs(val)) {
				ind = j;
				val = tmp[j][i];
			}
		}

		if (ind != i) {			/* swap columns */
			for (j = 0; j != 4; j++) {
				val2 = a[i][j];
				a[i][j] = a[ind][j];
				a[ind][j] = val2;
				val2 = tmp[i][j];
				tmp[i][j] = tmp[ind][j];
				tmp[ind][j] = val2;
			}
		}

		if (val == 0.0)
			verror("art: singular matrix in minv4.\n");

		for (j = 0; j != 4; j++) {
			tmp[i][j] /= val;
			a[i][j] /= val;
		}

		for (j = 0; j != 4; j++) {	/* eliminate column */
			if (j == i)
				continue;
			val = tmp[j][i];
			for (k = 0; k != 4; k++) {
				tmp[j][k] -= tmp[i][k] * val;
				a[j][k] -= a[i][k] * val;
			}
		}
	}
}



/****
 *
 * affine_matrix4_inverse
 *
 * Computes the inverse of a 3D affine matrix; i.e. a matrix with a dimen-
 * sionality of 4 where the right column has the entries (0, 0, 0, 1).
 *
 * This procedure treats the 4 by 4 matrix as a block matrix and
 * calculates the inverse of one submatrix for a significant perform-
 * ance improvement over a general procedure that can invert any non-
 * singular matrix:
 *          --        --          --          --
 *          |          | -1       |    -1      |
 *          | A      0 |          |   A      0 |
 *    -1    |          |          |            |
 *   M   =  |          |     =    |     -1     |
 *          | C      1 |          | -C A     1 |
 *          |          |          |            |
 *          --        --          --          --
 *
 *  where     M is a 4 by 4 matrix,
 *            A is the 3 by 3 upper left submatrix of M,
 *            C is the 1 by 3 lower left submatrix of M.
 *
 * Input:
 *   in   - 3D affine matrix
 *
 * Output:
 *   out  - inverse of 3D affine matrix
 *
 * Returned value:
 *   TRUE   if input matrix is nonsingular
 *   FALSE  otherwise
 *
 ***/

void
minv4_affine(Matrix out, Matrix in)
{
    register  float    det_1;
              float    pos, neg, temp;

#define ACCUMULATE    \
    if (temp >= 0.0)  \
        pos += temp;  \
    else              \
        neg += temp;

#define PRECISION_LIMIT (1.0e-15)

    /*
     * Calculate the determinant of submatrix A and determine if the
     * the matrix is singular as limited by the double precision
     * floating-point data representation.
     */
    pos = neg = 0.0;
    temp =  in[0][0] * in[1][1] * in[2][2];
    ACCUMULATE
    temp =  in[0][1] * in[1][2] * in[2][0];
    ACCUMULATE
    temp =  in[0][2] * in[1][0] * in[2][1];
    ACCUMULATE
    temp = -in[0][2] * in[1][1] * in[2][0];
    ACCUMULATE
    temp = -in[0][1] * in[1][0] * in[2][2];
    ACCUMULATE
    temp = -in[0][0] * in[1][2] * in[2][1];
    ACCUMULATE
    det_1 = pos + neg;

    /* Is the submatrix A singular? */
    if ((det_1 == 0.0) || (ABSVOGL(det_1 / (pos - neg)) < PRECISION_LIMIT)) {

        /* Matrix M has no inverse */
        verror("affine_matrix4_inverse: singular matrix\n");
        
    }

    else {

        /* Calculate inverse(A) = adj(A) / det(A) */
        det_1 = 1.0 / det_1;
        out[0][0] =   ( in[1][1] * in[2][2] -
                                 in[1][2] * in[2][1] )
                               * det_1;
        out[1][0] = - ( in[1][0] * in[2][2] -
                                 in[1][2] * in[2][0] )
                               * det_1;
        out[2][0] =   ( in[1][0] * in[2][1] -
                                 in[1][1] * in[2][0] )
                               * det_1;
        out[0][1] = - ( in[0][1] * in[2][2] -
                                 in[0][2] * in[2][1] )
                               * det_1;
        out[1][1] =   ( in[0][0] * in[2][2] -
                                 in[0][2] * in[2][0] )
                               * det_1;
        out[2][1] = - ( in[0][0] * in[2][1] -
                                 in[0][1] * in[2][0] )
                               * det_1;
        out[0][2] =   ( in[0][1] * in[1][2] -
                                 in[0][2] * in[1][1] )
                               * det_1;
        out[1][2] = - ( in[0][0] * in[1][2] -
                                 in[0][2] * in[1][0] )
                               * det_1;
        out[2][2] =   ( in[0][0] * in[1][1] -
                                 in[0][1] * in[1][0] )
                               * det_1;

        /* Calculate -C * inverse(A) */
        out[3][0] = - ( in[3][0] * out[0][0] +
                                 in[3][1] * out[1][0] +
                                 in[3][2] * out[2][0] );
        out[3][1] = - ( in[3][0] * out[0][1] +
                                 in[3][1] * out[1][1] +
                                 in[3][2] * out[2][1] );
        out[3][2] = - ( in[3][0] * out[0][2] +
                                 in[3][1] * out[1][2] +
                                 in[3][2] * out[2][2] );

        /* Fill in last column */
        out[0][3] = out[1][3] = out[2][3] = 0.0;
        out[3][3] = 1.0;

        return;
    }
}








