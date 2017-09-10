/********************************************************************
 * $Id: globals.h,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

/*
 * These are some funky defines that assures that global variables are
 * declared only once.  (when globals.c includes this file with EXTERN
 * defined.
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_


#ifndef EXTERN
#define EXTERN extern
#define INIT(x)
#else
#define EXTERN
#define INIT(x) =(x)
#endif

/*** Global variables for the entire program ***/

EXTERN float EYExval INIT(8682.625);
EXTERN float EYEyval INIT(-2750.0);
EXTERN float EYEzval INIT(18764.615234);

/* the point that the eye is looking towards */
EXTERN float REFxval INIT(5669.421387);
EXTERN float REFyval INIT(-1200.0);
EXTERN float REFzval INIT(12361.274414);

/* The current angle of rotation */
EXTERN float EYEangle INIT(3.581417);

/* The current perspective angle of rotation (in tenths of a degree) */
EXTERN int EYEperspectiveangle INIT(-400);

EXTERN int XrotationAngle INIT(0);
EXTERN int YrotationAngle INIT(0);
EXTERN int ZrotationAngle INIT(0);

#endif /* _GLOBALS_H_ */
