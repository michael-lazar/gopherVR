/* error.h
   function headers for error.c */

     /*---------------------------------------------------------------*/
     /* Moog           version 0.0     27 October 1992                */
     /* Xgopher        version 1.1     20 April 1991                  */
     /*                version 1.0     04 March 1991                  */
     /*                                                               */
     /* X window system client for the University of Minnesota        */
     /*                                Internet Gopher System.        */
     /*                                                               */
     /* Martin Hamilton,  Loughborough University of Technology       */
     /*                   Department of Computer Studies              */
     /*                                                               */
     /* Allan Tuchman,    University of Illinois at Urbana-Champaign  */
     /*                   Computing Services Office                   */
     /*                                                               */
     /* Jonathan Goldman, WAIS project                                */
     /*                   Thinking Machines Corporation               */
     /*                                                               */
     /* Copyright 1992 by                                             */
     /*           the Board of Trustees of the University of Illinois */
     /* Permission is granted to freely copy and redistribute this    */
     /* software with the copyright notice intact.                    */
     /*---------------------------------------------------------------*/

#include <X11/Intrinsic.h>

void makeErrorDialog(
#ifdef PROTO
	Widget		/* top */
#endif
);

void displayError(
#ifdef PROTO
	char *,		/* errorText */
	int		/* fatal */
#endif
);

void makeInfoDialog(
#ifdef PROTO
	Widget		/* top */
#endif
);

void displayInfo(
#ifdef PROTO
	char *		/* infoText */
#endif
);
