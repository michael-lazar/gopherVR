/********************************************************************
 * $Id: matrix.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/


#include <stdio.h>
#include "vogl.h"
#include "matrix.h"
#include "valloc.h"

static	Mstack	*msfree = (Mstack *)NULL;

/*
 *  MatrixMode...
 */ 

void
mmode(MatrixMode m)
{
     switch (m) {
     case MSINGLE:
	  /**  Do everything on a single matrix... bad, very bad... **/
	  break;
     case MPROJECTION:
	  /** Modifies stuff on the "bottom" matrix **/
	  break;
     case MVIEWING:
	  /** Modifies stuff on the "Top" matrix.. **/
	  break;
     case MTEXTURE:
	  break;
     }
}


/*
 * copyvector
 *
 * Copy the 4 vector b to a.
 * (Use the #define COPYVECTOR instead)
 */
void copyvector(register Vector a, register Vector b)
{
	a[0] = b[0];
	a[1] = b[1];
	a[2] = b[2];
	a[3] = b[3];
}

 
/*
 * copymatrix
 *
 * Copy the  4 x 4 matrix b to the 4 x 4 matrix a
 *
 */
void copymatrix(register Matrix a, register Matrix b)
{
	register int	i;
	register float	*pa, *pb;

	pa = (float *)a;
	pb = (float *)b;
	for(i = 0; i < 16; i++)
		*(pa++) = *(pb++);

	/* Pointer arithmetic is usuallly pretty slow.. */
}

/*
 * copytranspose
 *
 *	copy the transpose of the 4 x 4 matrix b to the 4 x 4 matrix a.
 */
void copytranspose(register Matrix a, register Matrix b)
{
     register int	i, j;
     
     for(i = 0; i < 4; i++)
	  for(j = 0; j < 4; j++)
	       a[i][j] = b[j][i];
}

/*
 * Retreive the top matrix on the stack and place it in m
 */
void getmatrix(Matrix m)
{
     COPYMATRIX(m, vdevice.transmat->m);
}


/*
 * V_ZeroMatrix
 *
 * Returns a 4 x 4 zero'ed out matrix
 */
void V_ZeroMatrix( Matrix a )
{
     register float  *p;
     
     for (p = (float *)a; p != (float *)a + 16; p++)
	  *p = 0.0;
     
}


/*
 * pushmatrix
 *
 * Push the top matrix of the stack down, placing a copy
 * of it on the top of the stack.
 *
 */
void pushmatrix(void )
{
	Mstack	*temp;

	if (vdevice.inobject) {
	     Token *p = newtokens(1);
	     p->i = PUSHMATRIX;
	     return;
	}

	if (msfree != (Mstack *)NULL) {
		temp = vdevice.transmat;
		vdevice.transmat = msfree;
		msfree = msfree->back;
		vdevice.transmat->back = temp;
		COPYMATRIX(vdevice.transmat->m, temp->m);
	} else {
		temp = (Mstack *)vallocate(sizeof(Mstack));
		temp->back = vdevice.transmat;
		COPYMATRIX(temp->m, vdevice.transmat->m);
		vdevice.transmat = temp;
	}
}



/*
 * popmatrix
 *
 * Pop the top matrix from the stack.
 *
 */
void popmatrix(void )
{
	Mstack	*oldtop;

	if (vdevice.inobject) {
	     Token *p = newtokens(1);
	     p->i = POPMATRIX;
	     return;
	}

	if (vdevice.transmat->back == (Mstack *)NULL)
		verror("popmatrix: matrix stack empty");
	else {
		oldtop = vdevice.transmat;
		vdevice.transmat = vdevice.transmat->back;
		oldtop->back = msfree;
		msfree = oldtop;
	}

	vdevice.cpVvalid = 0;	/* may have changed mapping from world to device coords */
}

/*
 * loadmatrix
 *
 * Replace the top matrix on the stack
 *
 */
void loadmatrix(const Matrix mat)
{
	register int	i;
	register float	*cm, *mp;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("loadmatrix: vogl not initialised");
#endif


	if (vdevice.inobject) {
	     Token *p = newtokens(17);

		p[0].i = LOADMATRIX;
		cm = (float *)mat;
		for (i = 0; i < 16; i++)
			(++p)->f = *cm++;

		return;
	}

	cm = (float *)vdevice.transmat->m;
	mp = (float *)mat;
	for (i = 0; i < 16; i++)
		*cm++ = *mp++;

	vdevice.cpVvalid = 0;		/* may have changed mapping from world to device coords */
}

/*
 * mult4x4
 *
 *	multiply 4 x 4 matrices b and c assigning them into a. Readers are
 * reminded that speed can be important here.
 *
 */
void mult4x4(register Matrix a, register Matrix b, register Matrix c)
{
     a[0][0] = b[0][0]*c[0][0] + b[0][1]*c[1][0] + b[0][2]*c[2][0] + b[0][3]*c[3][0];
     a[0][1] = b[0][0] * c[0][1] + b[0][1] * c[1][1] + b[0][2] * c[2][1] + b[0][3] * c[3][1];
     a[0][2] = b[0][0] * c[0][2] + b[0][1] * c[1][2] + b[0][2] * c[2][2] + b[0][3] * c[3][2];
     a[0][3] = b[0][0] * c[0][3] + b[0][1] * c[1][3] + b[0][2] * c[2][3] + b[0][3] * c[3][3];
     
     a[1][0] = b[1][0] * c[0][0] + b[1][1] * c[1][0] + b[1][2] * c[2][0] + b[1][3] * c[3][0];
     a[1][1] = b[1][0] * c[0][1] + b[1][1] * c[1][1] + b[1][2] * c[2][1] + b[1][3] * c[3][1];
     a[1][2] = b[1][0] * c[0][2] + b[1][1] * c[1][2] + b[1][2] * c[2][2] + b[1][3] * c[3][2];
     a[1][3] = b[1][0] * c[0][3] + b[1][1] * c[1][3] + b[1][2] * c[2][3] + b[1][3] * c[3][3];
     
     a[2][0] = b[2][0] * c[0][0] + b[2][1] * c[1][0] + b[2][2] * c[2][0] + b[2][3] * c[3][0];
     a[2][1] = b[2][0] * c[0][1] + b[2][1] * c[1][1] + b[2][2] * c[2][1] + b[2][3] * c[3][1];
     a[2][2] = b[2][0] * c[0][2] + b[2][1] * c[1][2] + b[2][2] * c[2][2] + b[2][3] * c[3][2];
     a[2][3] = b[2][0] * c[0][3] + b[2][1] * c[1][3] + b[2][2] * c[2][3] + b[2][3] * c[3][3];
     
     a[3][0] = b[3][0] * c[0][0] + b[3][1] * c[1][0] + b[3][2] * c[2][0] + b[3][3] * c[3][0];
     a[3][1] = b[3][0] * c[0][1] + b[3][1] * c[1][1] + b[3][2] * c[2][1] + b[3][3] * c[3][1];
     a[3][2] = b[3][0] * c[0][2] + b[3][1] * c[1][2] + b[3][2] * c[2][2] + b[3][3] * c[3][2];
     a[3][3] = b[3][0] * c[0][3] + b[3][1] * c[1][3] + b[3][2] * c[2][3] + b[3][3] * c[3][3];
}






/*
 * multmatrix
 *
 * Premultipy the top matrix on the stack by "mat"
 *
 */
void multmatrix(const Matrix mat)
{
     Matrix           prod;
     register float   *m;
     int              i;

     if (vdevice.inobject) {
	  Token *p = newtokens(17);

	  p[0].i = MULTMATRIX;
	  m = (float *)mat;
	  for (i = 0; i < 16; i++)
	       (++p)->f = *m++;
	  
	  return;
     }

     mult4x4(prod, mat, vdevice.transmat->m);
     loadmatrix(prod);
}

/*
 * identmatrix
 *
 * Return a 4 x 4 identity matrix
 *
 */
void identmatrix(Matrix a)
{
	register float	*p;

	for (p = (float *)a; p != (float *)a + 16; p++)
		*p = 0.0;

	a[0][0] = a[1][1] = a[2][2] = a[3][3] = 1.0;
}

/*
 * multvector
 *
 * Multiply the vector a and the matrix b to form v. Need it to be snappy again.
 * 
 */
void multvector(register Vector v, register Vector a, register Matrix b)
{
	v[0] = a[0] * b[0][0] + a[1] * b[1][0] + a[2] * b[2][0] + a[3] * b[3][0];
	v[1] = a[0] * b[0][1] + a[1] * b[1][1] + a[2] * b[2][1] + a[3] * b[3][1];
	v[2] = a[0] * b[0][2] + a[1] * b[1][2] + a[2] * b[2][2] + a[3] * b[3][2];
	v[3] = a[0] * b[0][3] + a[1] * b[1][3] + a[2] * b[2][3] + a[3] * b[3][3];
}

/*
 * premultvector
 *
 * PreMultiply the vector a and the matrix b to form v. 
 * Need it to be snappy again.
 * 
 */
void premultvector(Vector v, Vector a, Matrix b)
{
	v[0] = a[0] * b[0][0] + a[1] * b[0][1] + a[2] * b[0][2] + a[3] * b[0][3];
	v[1] = a[0] * b[1][0] + a[1] * b[1][1] + a[2] * b[1][2] + a[3] * b[1][3];
	v[2] = a[0] * b[2][0] + a[1] * b[2][1] + a[2] * b[2][2] + a[3] * b[2][3];
	v[3] = a[0] * b[3][0] + a[1] * b[3][1] + a[2] * b[3][2] + a[3] * b[3][3];
}

#ifdef NOTUSED
/*
 * printmat
 *
 *	print s and then dump matrix m. Useful for debugging, you get
 * sick of typing in the print loop otherwise.
 */
void printmat(char *s, Matrix m)
{
	int	i, j;

	printf("%s\n", s);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			printf("%f ",m[i][j]);
		printf("\n");
	}
}
void printvect( char *s, Vector v)
{
	printf("%s %f %f %f %f\n", s, v[0], v[1], v[2], v[3]);
}

#endif /* NOTUSED */
