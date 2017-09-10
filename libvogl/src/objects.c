/********************************************************************
 * $Id: objects.c,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "vogl.h"
#include "valloc.h"
#include "lines.h"
#include "text.h"
#include "patches.h"
#include "bsp.h"
#include "polygons.h"
#include <string.h>



typedef struct o {
	int		obno;
	TokList		*tlist;
	struct o	*next;
} VObject;

static VObject		*object_table[MAXENTS];
static long		obno = -1, omax = 0;

static void doarc(float x, float y, float xoff, float yoff, 
                  float cosine, float sine, int nsegs);
static void doarcf(float x, float y, float xoff, float yoff, 
                   float cosine, float sine, int nsegs);


/*
 * We need these global variables in order to keep track of some
 * attribute information. 
 *
 * 							-- gopher team
 */
int       irPgp_current;
int       F_curr_type;
int       g_B_text_waiting = FALSE;
TEXTATTR  g_Txt_curr_text;	
int       g_B_texture_waiting = FALSE;




/*
 * makeobj
 *
 *	start a new object.
 *
 */
void makeobj(long oob)
{
	VObject	*o;
	int	n = (int)oob;
	long   ob;

	ob = (long) oob;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("makeobj: vogl not initialised");
#endif


	for (o = object_table[n % MAXENTS]; o != (VObject *)NULL; o = o->next)
		if (o->obno == n) {
			delobj((long)n);
			break;
		}

	obno = n;
	vdevice.tokens = (TokList *)NULL;

	vdevice.inobject = 1;

	if (omax <= n)
		omax = n + 1;
}

/*
 * closeobj
 *
 *	close an object
 */
void closeobj(void)
{
	VObject	*o;

	if (!vdevice.inobject)
		verror("closeobj: not in an object");

	vdevice.inobject = 0;

	o = (VObject *)Cvallocate(1, sizeof(VObject));
	o->obno = obno;
	o->tlist = vdevice.tokens;
	o->next = object_table[obno % MAXENTS];

	object_table[obno % MAXENTS] = o;

	obno = -1;
}

/*
 * delobj
 *
 *	deletes an object, freeing its memory
 */
void delobj(long ob)
{
     VObject	*o, *lo;
     TokList	*tl, *ntl;
     int	n = ob;
     
     for (lo = o = object_table[n % MAXENTS]; o != (VObject *)NULL; lo = o, o = o->next)
	  if (o->obno == n)
	       break;
     
     if (o != (VObject *)NULL) {
	  for (tl = o->tlist; tl != (TokList *)NULL; tl = ntl) {
	       ntl = tl->next;
	       if (tl->toks)
		    WorldBFree((char * )tl->toks);
	       
	       WorldBFree((char * ) tl);
	  }
	  if (lo == object_table[n % MAXENTS])
	       object_table[n % MAXENTS] = (VObject *)NULL;
	  else
	       lo->next = o->next;
	  WorldBFree((char * ) o);
     }
}

/*
 * genobj
 *
 *	generates a unique object identifier
 */
long genobj(void)
{
     return(omax++);
}

/*
 * getopenobj
 *
 *	returns the object currently being edited, -1 if none.
 */
long getopenobj(void)
{
	return((long)obno);
}

/*
 * doarc
 *
 *	draw an arc or circle.
 */
static void doarc(float x, float y, float xoff, float yoff, float cosine, float sine, int nsegs)
{
	float	cx, cy, dx, dy;
	int	i;

	cx = x + xoff;
	cy = y + yoff;
	move2(cx, cy);

	for (i = 0; i < nsegs; i++)  {
		dx = cx - x;
		dy = cy - y;
		cx = x + dx * cosine - dy * sine;
		cy = y + dx * sine + dy * cosine;
		draw2(cx, cy);
	}
}

/*
 * doarcf
 *
 *	draw a filled arc or circle.
 */
static void doarcf(float x, float y, float xoff, float yoff, float cosine, float sine, int nsegs)
{
	float	cx, cy, dx, dy;
	int	i;

	cx = x + xoff;
	cy = y + yoff;
	pmv2(cx, cy);

	for (i = 0; i < nsegs; i++)  {
		dx = cx - x;
		dy = cy - y;
		cx = x + dx * cosine - dy * sine;
		cy = y + dx * sine + dy * cosine;
		pdr2(cx, cy);
	}

	pclos();
}

/*
 * callobj
 *
 *	draws an object
 */
void callobj(long ob)
{
	int	n = ob;

	VObject			*o;
	TokList			*tl;
	Matrix			prod, tmpmat;
	Tensor			S;
	int				i, j;
	float			cx, cy, cz, *m;
	register Token	*t, *et, *pt;

#ifdef UNIX
	int sync;
#endif

	irPgp_current = -1; 
	/* start at -1 since we increment the index into the depth sort array before we work on it */


#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("callobj: vogl not initialised");
#endif


	if (vdevice.inobject) {
		t = newtokens(2);

		t[0].i = CALLOBJ;
		t[1].i = n;

		return;
	}

	for (o = object_table[n % MAXENTS]; o != (VObject *)NULL; o = o->next)
		if (o->obno == n)
			break;

	if (o == (VObject *)NULL)
		return;

#ifdef UNIX
	if ((sync = vdevice.sync))
		vdevice.sync = 0;
#endif

	for (tl = o->tlist; tl != (TokList *)NULL; tl = tl->next) {
		t = tl->toks;
		et = &tl->toks[tl->count];
		while (t != et) {
			switch (t->i) {
			case ARC:
				doarc(t[1].f, t[2].f, t[3].f, t[4].f, t[5].f, t[6].f, t[7].i);
				t += 8;
				break;
			case ARCF:
				doarcf(t[1].f, t[2].f, t[3].f, t[4].f, t[5].f, t[6].f, t[7].i);
				t += 8;
				break;
			case BACKBUFFER:
				backbuffer(t[1].i);
				t += 2;
				break;
			case FRONTBUFFER:
				frontbuffer(t[1].i);
				t += 2;
				break;
			case SWAPBUFFERS:
				swapbuffers();
				t += 1;
				break;
			case BACKFACING:
				backface(t[1].i);
				t += 2;
				break;
			case CALLOBJ:
				callobj(t[1].i);
				t += 2;
				break;
			case CIRCLE:
				doarc(t[1].f, t[2].f, t[3].f, 0.0, t[4].f, t[5].f, t[6].i);
				draw2(t[1].f + t[3].f, t[2].f);
				t += 7;
				break;
			case CIRCF:
				doarcf(t[1].f, t[2].f, t[3].f, 0.0, t[4].f, t[5].f, t[6].i);
				t += 7;
				break;
			case RECTF:
				pmv2(t[1].f, t[2].f);
				pdr2(t[3].f, t[2].f);
				pdr2(t[3].f, t[4].f);
				pdr2(t[1].f, t[4].f);
				pdr2(t[1].f, t[2].f);

				F_curr_type = RECTF;	 
				pclos();
				t += 5;
				break;
			case CLEAR:
				printf( "clear\n" );
				REALCLEARSCREEN;
				t++;
				break;
			case COLOR:
				color(t[1].i);
				/* vdevice.attr->a.color */
				t += 2;
				break;
			case DRAW:
				draw(t[1].f, t[2].f, t[3].f);
				t += 4;
				break;
			case DRAWSTR:
				charstr_backend( t[1].f, t[2].f, t[3].i, t[4].f, t[5].f, 
									t[6].f, t[7].f, (char * )&t[8]);

				t += 9 + strlen( (char *) &t[8] ) / sizeof(Token);
				break;
			case VFONT:
				font(t[1].i);
				t += 2;
				break;
			case LOADMATRIX:
				m = (float *)vdevice.transmat->m;
				for (i = 0; i < 16; i++)
					*m++ = (++t)->f;

				vdevice.cpVvalid = 0;           /* may have changed mapping from world to device coords */
				t++;

				break;
			case MAPCOLOR:
				mapcolor(t[1].i, t[2].i, t[3].i, t[4].i);
				t += 5;
				break;
			case MOVE:
				move(t[1].f, t[2].f, t[3].f);
				t += 4;
				break;
			case MULTMATRIX:
				m = (float *)tmpmat;
				for (i = 0; i < 16; i++)
					*m++ = (++t)->f;

				mult4x4(prod, tmpmat, vdevice.transmat->m);
				loadmatrix(prod);
				t++;
				break;
			case POLY:
				polyobj(t[1].i, &t[2], 0);
				t += 2 + 3 * t[1].i;
				break;
			case POLYF:
				F_curr_type = POLYF;
				polyobj(t[1].i, &t[2], 1);
				t += 2 + 3 * t[1].i;
				break;
			case CMOV:
				cmov(t[1].f, t[2].f, t[3].f);
				t += 4;
				break;
			case POPATTRIBUTES:
				popattributes();
				t++;
				break;
			case POPMATRIX:
				popmatrix();
				t++;
				break;
			case POPVIEWPORT:
				popviewport();
				t++;
				break;
			case PUSHATTRIBUTES:
				pushattributes();
				t++;
				break;
			case PUSHMATRIX:
				pushmatrix();
				t++;
				break;
			case PUSHVIEWPORT:
				pushviewport();
				t++;
				break;
			case POLYMODE:
				polymode(t[1].i);
				t += 2;
				break;
			case RCURVE:
				i = (++t)->i;
				cx = (++t)->f;
				cy = (++t)->f;
				cz = (++t)->f;
				m = (float *)tmpmat;
				for (j = 0; j < 16; j++)
					*m++ = (++t)->f;
				mult4x4(prod, tmpmat, vdevice.transmat->m);
				drcurve(i, prod);
				vdevice.cpW[V_X] = cx;
				vdevice.cpW[V_Y] = cy;
				vdevice.cpW[V_Z] = cz;
				t++;
				break;
			case RPATCH:
				pt = t + 10;
				cx = (++t)->f;
				cy = (++t)->f;
				cz = (++t)->f;
				for (i = 0; i < 4; i++)
					for (j = 0; j < 4; j++) {
						S[0][i][j] = (pt++)->f;
						S[1][i][j] = (pt++)->f;
						S[2][i][j] = (pt++)->f;
						S[3][i][j] = (pt++)->f;
					}

				transformtensor(S, vdevice.transmat->m);
				drpatch(S, t[1].i, t[2].i, t[3].i, t[4].i, t[5].i, t[6].i);

				vdevice.cpW[V_X] = cx;
				vdevice.cpW[V_Y] = cy;
				vdevice.cpW[V_Z] = cz;
				t = pt;
				break;
			case VIEWPORT:
				viewport(t[1].i, t[2].i, t[3].i, t[4].i);
				t += 5;
				break;
			case LINESTYLE:
				setlinestyle((Linestyle)t[1].i);
				t += 2;
				break;
			case LINEWIDTH:
				linewidth((short)t[1].i);
				t += 2;
				break;
			case TRANSLATE:
				translate(t[1].f, t[2].f, t[3].f);
				t += 4;
				break;
			case SCALE:
				/*
				 * Do the operations directly on the top matrix of
				 * the stack to speed things up.
				 */

				vdevice.transmat->m[0][0] *= t[1].f;
				vdevice.transmat->m[0][1] *= t[1].f;
				vdevice.transmat->m[0][2] *= t[1].f;
				vdevice.transmat->m[0][3] *= t[1].f;

				vdevice.transmat->m[1][0] *= t[2].f;
				vdevice.transmat->m[1][1] *= t[2].f;
				vdevice.transmat->m[1][2] *= t[2].f;
				vdevice.transmat->m[1][3] *= t[2].f;

				vdevice.transmat->m[2][0] *= t[3].f;
				vdevice.transmat->m[2][1] *= t[3].f;
				vdevice.transmat->m[2][2] *= t[3].f;
				vdevice.transmat->m[2][3] *= t[3].f;

				t += 4;
				break;
			case ROTATE:
				rot(t[1].f, (char)t[2].i);
				t += 3;
				break;
			default: {
						char	buf[BUFSIZ];

						sprintf(buf, "vogl: internal error in callobj (Token type %d used)", t->i);
						verror(buf);
						exit(1);
				      }
			}
		}
	}


#ifdef UNIX
	if (sync) {
		vdevice.sync = 1;
		(*vdevice.dev.Vsync)();
	}
#endif
}

/*
 * isobj
 *
 *	returns 1 if there is an object n, 0 otherwise.
 */
long isobj( long ob)
{
	long	n = ob;

	VObject	*o;

	for (o = object_table[n % MAXENTS]; o != (VObject *)NULL; o = o->next)
		if (o->obno == n)
			break;

	return((long) ( o != (VObject *)NULL ));
}

