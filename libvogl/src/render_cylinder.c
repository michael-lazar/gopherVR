#include <stdio.h>
#include "hungarian.h"
#include "vogl.h"
#include "lines.h"
#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif

#include "render_cylinder.h"



static void buildCylinder( float R_botrad, float R_toprad, float R_height,
                           int N_slices, int N_stacks, STRING Sz_title );



/* make a cylinder */
static void buildCylinder( float R_botrad, float R_toprad, float R_height, 
			  int N_slices, int N_stacks, STRING Sz_title )
{
    float R_a, R_da;
    float R_twopi = 2.0 * PI;
    float x, y;
    float rR_vert [3];
    int   N_id;


	R_da = 2.0*PI / N_slices;

	bgnqstrip();
		for( R_a = 0.0; R_a < R_twopi; R_a += R_da)
		{
			x = cos( R_a );
			y = sin( R_a );

			rR_vert [0] = x * R_toprad;
			rR_vert [1] = y * R_toprad;
			rR_vert [2] = R_height;
			v3f( rR_vert );

			rR_vert [0] = x * R_botrad;
			rR_vert [1] = y * R_botrad;
			rR_vert [2] = 0.0;
			v3f( rR_vert );
		}

		rR_vert [0] = R_toprad;
		rR_vert [1] = 0.0;
		rR_vert [2] = R_height;
		v3f( rR_vert );

		rR_vert [0] = R_botrad;
		rR_vert [2] = 0.0;
		v3f( rR_vert );
	endqstrip();

	bgnpolygon();
		rR_vert [2] = R_height;
		for ( R_a = 0.0; R_a < R_twopi; R_a += R_da )
		{
			rR_vert [0] = cos( R_a ) * R_toprad;
			rR_vert [1] = sin( R_a ) * R_toprad;
			v3f( rR_vert );
		}
	endpolygon();

	bgnpolygon();
		rR_vert [2] = 0.0;
		for ( R_a = R_twopi; R_a > 0.0; R_a -= R_da )
		{
			rR_vert [0] = cos( R_a ) * R_botrad;
			rR_vert [1] = sin( R_a ) * R_botrad;
			v3f( rR_vert );
		}
	endpolygon();
}




/* 
 * this function should do whatever initialization is required 
 * so that the common parts of the 3d icon can just be called as objects
 */
void Cylinder_init( void )
{
}

void Cone_init( void )
{
}




/* 
 * draw an instance of the 3d icon with the specified string
 * then dispose of the temporary objects
 */
void Cylinder_make( float R_botrad, float R_toprad, float R_height,
                     int N_slices, int N_stacks, STRING Sz_title, int N_color )
{
	long N_id;
	
	N_id = genobj();
    makeobj( N_id );
        linewidth(1);
    
        pushmatrix();
            polymode( PYM_FILLNOUT );  /* for fill */
            color( N_color );
            buildCylinder( R_botrad, R_toprad, R_height, N_slices, N_stacks,
							 Sz_title );
        popmatrix();
        
    closeobj();
	callobj( N_id );
	delobj( N_id );    
}


void Cone_make( float R_botrad, float R_height, int N_slices, int N_stacks,
				 STRING Sz_title, int N_color )
{
    long N_id;

    N_id = genobj();
    makeobj( N_id );
        linewidth(1);

        pushmatrix();
            polymode( PYM_FILLNOUT );  /* for fill */
            color( N_color );
            buildCylinder( R_botrad, 0.0, R_height, N_slices, N_stacks, 
							Sz_title );
        popmatrix();

    closeobj();
    callobj( N_id );
    delobj( N_id );   
}
