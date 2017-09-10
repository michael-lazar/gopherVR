#ifndef _RENDER_CYLINDER_H_
#define _RENDER_CYLINDER_H_


	/*
	 * Make Cylinders and Cones
	 */
void Cylinder_init( void );
void Cone_init( void );

void Cylinder_make( float R_bot_rad, float R_top_rad, float R_height,
					 int N_slices, int N_stacks, STRING Sz_title, int N_color );

void Cone_make( float R_bot_rad, float R_height, int N_slices, int N_stacks,
				 STRING Sz_title, int N_color );

#endif /* _RENDER_CYLINDER_H_ */
