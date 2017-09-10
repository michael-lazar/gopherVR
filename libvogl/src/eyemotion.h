/********************************************************************
 * $Id: eyemotion.h,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _EYEMOTION_H_
#define _EYEMOTION_H_


/* protos for eyemotion.c */

extern void EyeInit(void);
extern void EyeRotateX(register float angle );
extern void EyeRotateY(register float angle );
extern void EyeRotateZ(register float angle );
extern void EyeTranslate(register const float xdirection, 
			 register const float ydirection,
			 register const float zdirection );

extern void EyeGoto(float x, float y, float z,
	float rx, float ry, float rz,
	float speed, void *(displayfunc)(void));


#endif /* _EYEMOTION_H_ */

