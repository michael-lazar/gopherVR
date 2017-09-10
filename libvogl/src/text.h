/********************************************************************
 * $Id: text.h,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _TEXT_H_
#define _TEXT_H_


/* protos for text.c */

void charstr_backend(float R_height, float R_length, int F_color, 
                     float R_vec_x, float R_vec_y, float R_vec_z, 
                     float R_angle, char *aCh_str);
void V_SetTextInBox( float R_height, float R_length, int F_color, 
					 float R_vec_x, float R_vec_y, float R_vec_z, 
					 float R_angle, char *aCh_str );

#endif /* _TEXT_H_ */
