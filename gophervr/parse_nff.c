/*
 * Parser for NFF files (as used in Eric Haines' SPD)
 * 
 */

#include <stdio.h>
#include <math.h>
#include "vogl.h"
#include "parse.h"

enum NFFParseStateEnum {
     NFFBOF, NFFSTART, INPOLY, INPOLYPATCH, INCONE, INVIEW, NFFEOF
};
typedef enum NFFParseStateEnum NFFParseState;

/**********************************************************************
 * Control Variables
 */

static NFFParseState  State = NFFBOF;
static int            NumPolySides = -1;   /* Used when processing Poly Verts*/
static Vector         ConeBase;
static float          ConeRadius;
static int            theid = -1;
/*
 * The viewing parameters....
 */
struct {
     float	Fx, Fy, Fz;
     float	Ax, Ay, Az;
     float	Ux, Uy, Uz;
     float	angle;
     float	hither, yon;
     int	xres, yres;
} view;

/*
 * Could have done something with the actual colours but didn't...
 * (they're currently ignored)
 */
struct	{
     float	r, g, b;
} bg, fc;

int	pr = 12;	/* Precision of cones and spheres */
int	debug = 1;

FILE	*fp;

char	buf[128];

void
ParseNFFInit(void) 
{
     State = NFFBOF;
     theid = genobj();
     makeobj(theid);
}

int
ParseNFFLine(char *line) 
{
     Vector     V;
     int	c;
     float	dum, Kd, Ks, Shine, T, i_of_r;

     Vector     Apex;
     float      ApexRadius;

     float      SphereRadius;

     switch (State) {
     case NFFBOF:
     case NFFSTART:
	  /* Find out what to do next.... */
	  switch (*line) {

	  case '#':
	  case '\t':
	       break;

	  case 'p':		/* Polygons and such */
	       line++;
	       /** Check for polypatch (pp) **/
	       if (*line == 'p') {
		    line++;
		    State = INPOLYPATCH;
	       } else
		    State = INPOLY;


	       line = ScanInt(line, &NumPolySides);
	       bgnpolygon();
	       break;

	  case 'c':		/* Cones and cylinders */
	       if (sscanf(line+1, "%f %f %f %f", &ConeBase[0],
			  &ConeBase[1], &ConeBase[2], &ConeRadius) != 4)
		    return(-1);
	       State = INCONE;
	       break;

	  case 'v':		/* View etc. */
	       State = INVIEW;
	       break;

	  case 's':
	       if (sscanf(line+1, "%f %f %f %f", &V[0], &V[1], &V[2],
			  SphereRadius) != 4)
		    return(-1);
	       ;
	       /**** Create Sphere here.... *****/
	       break;
	  case 'l':
	       /*** Lights, ignore them... ***/
	       break;
	  case 'b':
	       /*** Background color, ignore... ***/
	       if (sscanf(line+1, "%f %f %f\n", &bg.r, &bg.g, &bg.b) != 3)
		    return(-1);
	       break;

	  case 'f':
	       /*** Fill color and shading stuff, ignore... ***/
	       if (sscanf(line+1, "%f %f %f %f %f %f %f %f\n",
			  &fc.r, &fc.g, &fc.b,
			  &Kd, &Ks, &Shine, &T, &i_of_r) != 8)
		    return(-1);
	       ;

	       break;
	  }
	  break;

     case INVIEW:
	  /** Ignore view for now.... **/
	  break;

     case INPOLY:
     case INPOLYPATCH:
	  /* Scan for vertices, ignore normals for polypatch for now */
	  if (sscanf(line, "%f %f %f\n", &V[0], &V[1], &V[2]) != 3)
	       return(-1);
	  v3f(V);		/* Add point to poly */
	  NumPolySides--;
	  if (NumPolySides == 0) {
	       endpolygon();
	       State = NFFSTART;
	  }
	  break;

     case INCONE:
	  if (sscanf(line, "%f %f %f %f\n", &Apex[0], &Apex[1], &Apex[2], 
		     &ApexRadius) != 4)
	       return(-1);
	  if (ApexRadius == 0.0) {
	       /*** Plot Cone Here ***/
	       ;
	  } else {
	       /*** Plot Cylinder Here ***/
	       ;
	  }
	  State = NFFSTART;
	  
     case NFFEOF:
	  break;
     }

     return(0);
}


int
ParseNFFDone(void)
{
     closeobj();
     return(theid);
}
/*
 * read_view
 *
 *	Read in the viewing parameters
 */
read_view()
{
/*     read_three("\nfrom %g %g %g", &view.Fx, &view.Fy, &view.Fz);
     read_three("\nat %g %g %g", &view.Ax, &view.Ay, &view.Az);
     read_three("\nup %g %g %g", &view.Ux, &view.Uy, &view.Uz);
*/
     if (fscanf(fp, "\nangle %g", &view.angle) != 1)
	  perror("wanted a number for fov");

     if (fscanf(fp, "\nhither %g", &view.hither) != 1)
	  perror("wanted a number for hither");

     if (fscanf(fp, "\nresolution %d %d\n", &view.xres, &view.yres) != 2)
	  perror("wanted two numbers for resolution");

     view.yon = 1.0e38;

     if (debug) {
	  fprintf(stderr, "View is:\n");
	  fprintf(stderr, "from %g %g %g\n", view.Fx, view.Fy, view.Fz);
	  fprintf(stderr, "at %g %g %g\n", view.Ax, view.Ay, view.Az);
	  fprintf(stderr, "up %g %g %g\n", view.Ux, view.Uy, view.Uz);
	  fprintf(stderr, "angle %g\n", view.angle);
	  fprintf(stderr, "hither %g\n", view.hither);
	  fprintf(stderr, "resolution %d %d\n", view.xres, view.yres);
     }


     /*
      * Set up view ....
      */
     /*	perspective(view.angle, 1.0, view.hither, view.yon);
	lookat(view.Fx, view.Fy, view.Fz, view.Ax, view.Ay, view.Az, 0.0);*/

     /*
      * Set precision ...
      */
     circleprecision(pr);
}


