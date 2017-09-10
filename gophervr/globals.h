/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1.1.1 $
 * $Date: 2002/01/18 16:34:47 $
 * $Source: /var/cvs/clients/gophervr/gophervr/globals.h,v $
 * $State: Exp $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: globals.h
 * Global variables and #defines
 *********************************************************************
 * Revision History:
 * $Log: globals.h,v $
 * Revision 1.1.1.1  2002/01/18 16:34:47  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:34  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.7  1995/06/19  18:14:10  lindner
 * Latest version
 *
 * Revision 1.6  1995/05/18  03:00:17  lindner
 * New sceneid..
 *
 * Revision 1.5  1995/04/18  03:48:45  lindner
 * Current Aspect Ratio
 *
 * Revision 1.4  1995/03/29  04:04:02  lindner
 * moo
 *
 * Revision 1.3  1995/03/27  05:12:50  lindner
 * mondo changes
 *
 * Revision 1.2  1994/05/06  22:11:29  lindner
 * Add thing here... for now...
 *
 * Revision 1.1  1994/05/06  21:48:30  lindner
 * Global variables be here ...
 *
 *
 *********************************************************************/

#include <stddef.h>		/* For NULL */

/*
 * These are some funky defines that assures that global variables are
 * declared only once.  (when globals.c includes this file with EXTERN
 * defined.
 */

#ifndef EXTERN
#define EXTERN extern
#define INIT(x)
#else
#define EXTERN
#define INIT(x) =(x)
#endif

/*** Global variables for the entire program ***/

/*** Should we save images for a movie? ***/
EXTERN int SaveMovie INIT(0);

/*** The current viewpoint.. ***/
EXTERN float EYEzval INIT(0.0);
EXTERN float EYEyval INIT(0.0);
EXTERN float EYExval INIT(0.0);

/*** The current angle of rotation (left-right) ***/
EXTERN float EYEangle INIT(0.0);

/*** The current azimuth (up-down) **/
EXTERN float EYEazimuth INIT(0.0);

/*** The current Aspect Ratio ***/
EXTERN float SCENEaspect INIT(16.0/9.0);

/*** Have we built the initial scene? ***/
EXTERN int Bootstrapped INIT(0);
EXTERN char* INITIALhost INIT(NULL);
EXTERN int  INITIALport INIT(0);

/*** Our sceneid we reuse over and over... ***/
EXTERN int oursceneid;
