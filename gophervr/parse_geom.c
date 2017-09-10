/********************************************************************
 * $Id: parse_geom.c,v 1.1.1.1 2002/01/18 16:34:47 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "vogl.h"
#ifdef MACINTOSH
#  include <fp.h>
#else
#  include <math.h>
#endif

#include "parse.h"

/*
 * A parser for GEOM/OFF files.  Only do one at a time please...
 *   ***NOT*** thread safe
 *
 * Geom files look like this:
 *
 *  NumVertices NumPolys Unknown
 *  V1x V1y V1z
 *  V2x V2y V2z
 *  ....
 *  numpolysides Vnum1 Vnum2 ....
 *  ...
 *
 */ 

enum GeomParseStateEnum {
     GEOMBOF, VERTICES, POLYS, GEOMEOF
};
typedef enum GeomParseStateEnum GeomParseState;

/**********************************************************************
 * Control Variables
 */

static int NumV;		        /* Number of Vertices in Object */
static int NumP;		        /* Number of polys in Object  */
static int Vcounter, Pcounter;	        /* Looping variables */
static GeomParseState State = GEOMBOF;  /* Start at the begining... */
static Vector *Vertices = NULL;	        /* Vertex list */
static int theid = -1;		        /* Object ID for this thing.. */

/*
 * Get Ready to parse...
 */

void ParseGeomInit(void) 
{
     State = GEOMBOF;

     theid = genobj();
     makeobj(theid);

     Vcounter = 0;
     Pcounter = 0;
}


int
ParseGeomLine(char *line)
{
     int NumHuh;
     float Vx, Vy, Vz;
     int Vnum, NumPsides;

     switch (State) {
     case GEOMBOF:
	  if (sscanf(line, "%i %i %i\n", &NumV, &NumP, &NumHuh) != 3)
	       return(-1);
	  Vertices = (Vector *) malloc(sizeof (Vector) * NumV);
	  State = VERTICES;
	  break;

     case VERTICES:
	  if (sscanf(line, "%f %f %f\n", &Vx, &Vy, &Vz) != 3) 
	       return(-1);
	  Vertices[Vcounter][V_X] = Vx;
	  Vertices[Vcounter][V_Y] = Vy;
	  Vertices[Vcounter++][V_Z] = Vz;
	  
	  if (Vcounter == NumV) 
	       State = POLYS;
	  break;

     case POLYS:
	  line = ScanInt(line, &NumPsides);

	  bgnpolygon();
	  while (NumPsides > 0) {
	       line = ScanInt(line, &Vnum);
	       v3f(Vertices[Vnum-1]);
	       NumPsides--;
	  }
	  endpolygon();
	  Pcounter++;
	  if (Pcounter == NumP)
	       State = GEOMEOF;
	  break;
     case GEOMEOF:
	  break;
     }
     return(1);
}

int ParseGeomDone(void)
{
     if (Vertices != NULL)
	  WorldBFree(Vertices);
     closeobj();
     return(theid);
}






