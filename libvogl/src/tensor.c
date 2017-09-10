/********************************************************************
 * $Id: tensor.c,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"

/*
 *  premulttensor
 *
 *  Premultiply the tensor b by the matrix a and place the result
 *  into the tensor c.
 */
void premulttensor(Tensor c, Matrix a, Tensor b)
{
	register	int	i, j, k;
	float		x1, x2, x3, x4;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			x1 = x2 = x3 = x4 = 0.0;
			for (k = 0; k < 4; k++) {
				x1 += a[i][k] * b[0][k][j];
				x2 += a[i][k] * b[1][k][j];
				x3 += a[i][k] * b[2][k][j];
				x4 += a[i][k] * b[3][k][j];
			}
			c[0][i][j] = x1;
			c[1][i][j] = x2;
			c[2][i][j] = x3;
			c[3][i][j] = x4;
		}
}

/*
 *  multtensor
 *
 *  Multiply the tensor b by the matrix a and place the result
 *  into the tensor c.
 */
void multtensor(Tensor c, Matrix a, Tensor b)
{
	register	int	i, j, k;
	float		x1, x2, x3, x4;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			x1 = x2 = x3 = x4 = 0.0;
			for (k = 0; k < 4; k++) {
				x1 += b[0][i][k] * a[k][j];
				x2 += b[1][i][k] * a[k][j];
				x3 += b[2][i][k] * a[k][j];
				x4 += b[3][i][k] * a[k][j];
			}
			c[0][i][j] = x1;
			c[1][i][j] = x2;
			c[2][i][j] = x3;
			c[3][i][j] = x4;
		}
}

/*
 * Copy the tensor a into b.
 */
void copytensor(Tensor b, Tensor a)
{
	register	int	i, j, k;
	
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			for (k = 0; k < 4; k++)
				b[i][j][k] = a[i][j][k];
}

/*
 * Copy the tensor a into b "transposed".
 */
void copytensortrans(Tensor b, Tensor a)
{
	register	int	i, j, k;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			for (k = 0; k < 4; k++)
				b[i][j][k] = a[i][k][j];
}
