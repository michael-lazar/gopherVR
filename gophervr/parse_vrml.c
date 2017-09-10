/********************************************************************
 * $Id: parse_vrml.c,v 1.1.1.1 2002/01/18 16:34:48 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include "vogl.h"
#ifdef MACINTOSH
#  include <fp.h>
#else
#  include <math.h>
#endif

#include "parse.h"

/*
 * A parser for VRML files.  Only do one at a time please...
 *   ***NOT*** thread safe
 *
 * See http://vrml.wired.com for more info.
 *
 */ 

enum vrmlParseStateEnum {
     VRMLBOF, 
     DEF,
     SEPARATOR,
     RENDERCULLING,

     CONE, 
     CONE_PARTS,
     CONE_BOTTOMRADIUS,
     CONE_HEIGHT,

     COORDINATE3,
     COORDINATE3_POINT,
     
     CUBE,
     CUBE_WIDTH,
     CUBE_HEIGHT,
     CUBE_DEPTH,

     CYLINDER,
     CYLINDER_PARTS,
     CYLINDER_RADIUS,
     CYLINDER_HEIGHT,

     DIRECTIONALLIGHT,
     DIRECTIONALLIGHT_ON,
     DIRECTIONALLIGHT_INTENSITY,
     DIRECTIONALLIGHT_COLOR,
     DIRECTIONALLIGHT_DIRECTION,
     
     GROUP,
     
     INDEXEDFACESET,
     INDEXEDFACESET_COORDINDEX,
     INDEXEDFACESET_MATERIALINDEX,
     INDEXEDFACESET_NORMALINDEX,
     INDEXEDFACESET_TEXTURECOORDINDEX,

     INFO,
     INFO_STRING,

     LEVELOFDETAIL,
     LEVELOFDETAIL_SCREENAREA,

     MATERIAL,
     MATERIAL_AMBIENTCOLOR,
     MATERIAL_DIFFUSECOLOR,
     MATERIAL_SPECULARCOLOR,
     MATERIAL_EMISSIVECOLOR,
     MATERIAL_SHININESS,
     MATERIAL_TRANSPARENCY,

     MATERIALBINDING,
     MATERIALBINDING_DEFAULT,

     MATRIXTRANSFORM,
     MATRIXTRANSFORM_MATRIX,

     NORMAL,
     NORMAL_VECTOR,

     NORMALBINDING,
     NORMALBINDING_VALUE,

     ORTHOGRAPHICCAMERA,
     ORTHOGRAPHICCAMERA_POSITION,
     ORTHOGRAPHICCAMERA_ORIENTATION,
     ORTHOGRAPHICCAMERA_FOCALDISTANCE,
     ORTHOGRAPHICCAMERA_HEIGHT,


     VRML
     VRMLEOF
};
typedef enum vrmlParseStateEnum vrmlParseState;

/**********************************************************************
 * Control Variables
 */

static vrmlParseState State = VRMLBOF;  /* Start at the begining... */
static int theid = -1;		        /* Object ID for this thing.. */

/*
 * Get Ready to parse...
 */

void ParseVrmlInit(void) 
{
     State = VRMLBOF;

     theid = genobj();
     makeobj(theid);

     Vcounter = 0;
     Pcounter = 0;
}


int
ParseVRMLLine(char *line)
{


     switch (State) {
     case VRMLBOF:
	  break;

     case VERTICES:
	  if (sscanf(line, "%f %f %f\n", &Vx, &Vy, &Vz) != 3) 
	       return(-1);
	  Vertices[Vcounter][V_X] = Vx;
	  Vertices[Vcounter][V_Y] = Vy;
	  Vertices[Vcounter][V_Z] = Vz;
	  
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

/*
 * Finished Parsing... Phew!
 */

int ParseVrmlDone(void)
{
     closeobj();
     return(theid);
}

int ParseVrmlCone(char *line) {
     static int wherearewe;

     line = ScanWhiteSpace(line);

     if (strncasecmp(line , "PARTS")==0) {
	  ;
     }
     else if (strncasecmp(line , "BOTTOMRADIUS")==0) {
	  ;
     } 
     else if (strncasecmp(line , "HEIGHT")==0) {
	  ;
     }


}

