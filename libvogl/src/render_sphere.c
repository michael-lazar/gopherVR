#include <stdio.h>
#include "hungarian.h"
#include "vogl.h"
#include "lines.h"
#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif

#include "render_sphere.h"



static void buildSphere( float R_radius, int N_slices, int N_stacks, 
                         STRING Sz_title );
 


/* make a sphere */
static void buildSphere( float R_radius, int N_slices, int N_stacks, 
						 STRING Sz_title )
{
    float	R_rho, R_drho, R_theta, R_dtheta;
	float 	R_crho, R_crdrho, R_srdrho;
	float	R_ctheta, R_stheta;
    float   x, y, z;
    int     i, j;
    float   rR_vert [3];
    int     N_id;

	R_drho = PI / (float) N_stacks;
	R_dtheta = 2.0 * PI / (float) N_slices;
	R_rho = -PI/2.0;

	for( i = 0; i < N_stacks; i++ )
	{
		R_crho = cos( R_rho );
		R_crdrho = cos( R_rho + R_drho );
		R_srdrho = sin( R_rho + R_drho );

		bgnqstrip();
			R_theta = 0.0;
			for( j = 0; j < N_slices + 1; j++ )
			{
				if( j == N_slices )
				{
					R_theta = 0.0;
				}

				R_ctheta = cos( R_theta );
				R_stheta = sin( R_theta );
	
				rR_vert [0] = ( R_ctheta * R_crdrho ) * R_radius;
				rR_vert [1] = ( R_stheta * R_crdrho ) * R_radius;
				rR_vert [2] = ( R_srdrho ) * R_radius;
				v3f( rR_vert );

				rR_vert [0] = ( R_ctheta * R_crho ) * R_radius;
				rR_vert [1] = ( R_stheta * R_crho ) * R_radius;
				rR_vert [2] = ( sin( R_rho ) ) * R_radius;
				v3f( rR_vert );

				R_theta += R_dtheta;
			}
		endqstrip();

		R_rho += R_drho;
	}
}




/* 
 * this function should do whatever initialization is required 
 * so that the common parts of the 3d icon can just be called as objects
 */
void Sphere_init( void )
{
}



/* 
 * draw an instance of the 3d icon with the specified string
 * then dispose of the temporary objects
 */
void Sphere_make( float R_radius, int N_slices, int N_stacks, 
					STRING Sz_title, int N_color )
{
	long N_id;
	
	N_id = genobj();
    makeobj( N_id );
        linewidth(1);
    
        pushmatrix();
            polymode( PYM_FILLNOUT );  /* for fill */
            color( N_color );
			buildSphere( R_radius, N_slices, N_stacks, Sz_title );
        popmatrix();
        
    closeobj();
	callobj( N_id );
	delobj( N_id );    
}
