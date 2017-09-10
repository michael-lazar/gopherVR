#ifndef _RENDER_SPHERE_H_
#define _RENDER_SPHERE_H_


	/*
	 * Make a Sphere 
	 */
void Sphere_init( void );

void Sphere_make( float R_radius, int N_slices, int N_stacks, 
					STRING Sz_title, int N_color );

#endif /* _RENDER_SPHERE_H_ */
