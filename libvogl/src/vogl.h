/********************************************************************
 * $Id: vogl.h,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _VOGL_H_
#define _VOGL_H_

#ifdef MACINTOSH
#  define MACINTOSH
#endif

#undef VDEVICECHECK


#ifdef PC	/* Stupid pox head crap */
char	*vallocate();
char	*malloc();
#endif


/*
 * VOGL is always defined if a header file is from the 
 * VOGL library. In cases where you do use some VOGL
 * initialisation routines like vinit, just put #ifdef VOGL...
 * around.
 */
#ifndef VOGL
#define	VOGL
#endif

#ifndef TRUE
#define	TRUE	1
#endif

#ifndef FALSE
#define	FALSE	0
#endif

/*
 * shademodel defines..
 */

#define FLAT                    0       /* For compatibility */
#define GOURAUD                 1       /* For compatibility */

/*
 * Misc defines...
 */
#define GD_XPMAX 1
#define GD_YPMAX 2

/*
 * standard VOGL color indices
 */
#define BLACK   0
#define WHITE   1
#define RED     2
#define GREEN   3
#define YELLOW  4
#define BLUE    5
#define MAGENTA 6
#define CYAN    7

/* color indexes for text on various gopherspace objects */
#ifdef MACINTOSH
#define TEXT_DOCUMENT_COLOR 	0
#define TEXT_DIRECTORY_COLOR 	0
#define TEXT_KIOSK_COLOR 		0
#define TEXT_SEARCH_COLOR 		0
#define TEXT_TELNET_COLOR		0
#else
#define TEXT_DOCUMENT_COLOR 	1
#define TEXT_DIRECTORY_COLOR 	1
#define TEXT_KIOSK_COLOR 		1
#define TEXT_SEARCH_COLOR 		1
#define TEXT_TELNET_COLOR		1
#endif
/* color indexes for various gopherspace objects */
#define OBJECT_DOCUMENT_COLOR 	13
#define OBJECT_DIRECTORY_COLOR 	12
#define OBJECT_KIOSK_COLOR 		 9
#define OBJECT_SEARCH_COLOR 	10
#define OBJECT_TELNET_COLOR		14
#define	OBJECT_URL_COLOR	11

#define OBJECT_GROUND1_COLOR	16
#define OBJECT_GROUND2_COLOR	17



/*
 * Used in shaders
 */
#define d_DYNAMIC       1       /* should the shading be dynamic */
#define d_STATIC        2       /* or should it be static */

#define d_COLORRAMPS    13      /* number of ramps */
#define d_LEN_COLRAMP   19      /* length of ramp */ 
#define d_SKIP          4      /* skip between each RGB segment in ramp */ 
#define d_SKIP2			3		/* d_SKIP - 1 */
#define d_COLMAP_OFF    9       /* 256 - ( d_COLORRAMPS * d_LEN_COLRAMP ) */ 
#define d_TOTAL_COLS	22		/* d_COLORRAMPS + d_COLMAP_OFF */
#define d_INDEX_RED     0
#define d_INDEX_GREEN   1
#define d_INDEX_BLUE    2

#define d_FROM_POLYGON  1
#define d_FROM_BSP      2

/*
 * when (if ever) we need the precision
 */
#ifdef DOUBLE
#define	float	double
#endif

/*
 * How to convert degrees to radians, and other common constants
 */
#define	PI              3.141592
#define PITIMES2        6.283185        /* 2 * pi */
#define PIOVER2         1.570796        /* pi / 2 */
#define D2R             0.017453        /* convert degrees to radians */
#define R2D             57.29578        /* convert radians to degrees */
#define SQRT2           1.414214        /* sqrt(2) */
#define SQRT3           1.732051        /* sqrt(3) */

/*
 * Fast sign and abs routines
 */

#if defined(__alpha)
typedef int dword;
typedef unsigned int udword;
#elif defined(__sgi)
/* MIPSpro throws a hissy if C99 isn't on for stdint.h. */
typedef signed int dword;
typedef unsigned int udword;
#else
#include <stdint.h> 
typedef int32_t dword;
typedef uint32_t udword;
#endif

#define NUMBITS	   (sizeof(dword)*8)
#define FLOATtoLONG(x) *(dword *) &(x)
#define FASTSIGN(x) (((udword)(x)) >> (NUMBITS-1))
#define FASTABS(x)  ((x) & ~(1 << (NUMBITS-1)))

/*
 * Misc defines... 
 */
#define ABSVOGL(a)	(((a)<0) ? -(a) : (a))
#define MAXVOGL(x, y)	((x) > (y) ? (x) : (y))
#define MINVOGL(x, y)	((x) < (y) ? (x) : (y))
#define SQ(a)           ((a)*(a))
/*
 * miscellaneous typedefs and type defines
 */
typedef float	Vector[4];
typedef float	Matrix[4][4];
typedef float	Tensor[4][4][4];
typedef short	Angle;
typedef float	Coord;
typedef long	Icoord;
typedef short	Scoord;
typedef long	Object;
typedef short	Screencoord;
typedef unsigned short	Linestyle;

typedef unsigned short	Device;

typedef unsigned short	Colorindex;


/*
 * when register variables get us into trouble
 */
#ifdef NOREGISTER
#define	register
#endif

/*
 * max number of vertices in a ploygon
 */
#define	MAXVERTS	20

/*
 * Max number of scenes
 */
#define MAXSCENES       20

/***********************************************************************
 * object definitions
 */
#define MAXENTS		101		/* size of object table */
#define	MAXTOKS		100		/* num. of tokens alloced at once in
					   an object  */

/***********************************************************************
 * Polygon fill modes for "polymode"
 */
#define PYM_POINT		0
#define PYM_LINE		0
#define PYM_FILL		1
#define PYM_HOLLOW		1
#define PYM_FILLNOUT	2
#define PYM_BSP			3

/**********************************************************************
 * Arguments for the mmode() fcn
 */

enum MmodeEnum {
     MSINGLE = 1,
     MPROJECTION = 2,
     MVIEWING = 3,
     MTEXTURE = 4
};
typedef enum MmodeEnum MatrixMode;


/***********************************************************************
 * functions which can appear in objects
 */
#define	ARC		1
#define	CALLOBJ		3
#define	CIRCLE		5
#define	CLEAR		6
#define	COLOR		7
#define	DRAW		8
#define	DRAWSTR		10
#define	VFONT		12
#define	LOADMATRIX	15
#define	MAPCOLOR	16
#define	MOVE		17
#define	MULTMATRIX	18
#define	POLY		19
#define	POPATTRIBUTES	22
#define	POPMATRIX	23
#define	POPVIEWPORT	24
#define	PUSHATTRIBUTES	25
#define	PUSHMATRIX	26
#define	PUSHVIEWPORT	27
#define	RCURVE		28
#define	RPATCH		29
#define	SECTOR		30
#define	VIEWPORT	33
#define	BACKBUFFER	34
#define	FRONTBUFFER	35
#define	SWAPBUFFERS	36
#define	BACKFACING	37
#define	TRANSLATE	38
#define	ROTATE		39
#define	SCALE		40

#define	ARCF		41
#define	CIRCF		42
#define	POLYF		43
#define	RECTF		44
#define	POLYMODE	45
#define	CMOV		46
#define	LINESTYLE	47
#define	LINEWIDTH	48

#define RAW_DRAW    60
#define RAW_MOVE   	62 

/*
 * Non standard call...
 */
#define	VFLUSH		70

/*
 * States for bgn* and end* calls
 */
#define	NONE		0	/* Just set current spot */
#define	VPNT		1	/* Draw dots		 */
#define	VLINE		2	/* Draw lines		 */
#define	VCLINE		3	/* Draw closed lines	 */
#define	VPOLY		4	/* Draw a polygon 	 */
#define VTMESH		5       /* Draw a triangular mesh*/
#define VQSTRIP		6       /* Draw a quadralateral mesh*/

/*
 * data types for object tokens
 */
typedef union tk {
	int		i;
	float		f;
} Token;

typedef struct tls {
	int		count;
	Token		*toks;
	struct tls	*next;
} TokList;

/*
 * double buffering modes.
 */
#define	SINGLE		1

/*
 * attributes
 */
typedef struct {
	char		backface,
			mode;			/* Which mode are we in */
	int		color;
	int		fontnum;
	Linestyle	ls;			
	short		lw;			/* Linewidth */
} Attribute;

/*
 * viewport
 */
typedef struct vp {
	float	left;
	float	right;
	float	bottom;
	float	top;
} Viewport; 

/*
 * stacks
 */
typedef	struct	ms {	/* Matrix stack entries	*/
	Matrix		m;
	struct	ms	*back;
} Mstack;

typedef	struct	as {	/* Attribute stack entries */
	Attribute	a;
	struct	as	*back;
} Astack;

typedef	struct	vs {	/* Viewport stack entries */
	Viewport	v;
	struct	vs	*back;
} Vstack;

/*
 * vogle device structures
 */
typedef struct dev {
	char	*devname;							/* name of device */
	char	*large,								/* name of large font */
			*small;								/* name of small font */
	int		(*Vbackb)(void);					/* Set drawing in back buffer */
	void	(*Vchar)(char c);					/* Draw a hardware character */
	int		(*Vcheckkey)(void);					/* Ckeck if a key was hit */
	void	(*Vclear)(void);					/* Clear the screen to current color */
	void	(*Vcolor)(int ind);					/* Set current color */
	void 	(*Vdraw)(int x, int y);				/* Draw a line */
	void	(*Vexit)(void);						/* Exit graphics */
	void	(*Vfill)(int n, int x[], int y[]);	/* Fill a polygon */
	int		(*Vfont)(char *fontfile);			/* Set hardware font */
	void	(*Vfrontb)(void);					/* Set drawing in front buffer */
	int		(*Vgetkey)(void);					/* Wait for and get the next key hit */
	int		(*Vinit)(void);						/* Initialise the device */
	int		(*Vlocator)(int *wx, int *wy);		/* Get mouse/cross hair position */
	void	(*Vmapcolor)(int i, int r, int g, int b);		/* Set color indicies */
	void	(*Vsetls)(Linestyle lstyle);		/* Set linestyle */
	void	(*Vsetlw)(short lwidth);			/* Set linewidth */
	void 	(*Vstring)(char	s[]);				/* Draw a hardware string */
	int		(*Vswapb)(void);					/* Swap front and back buffers */
	void	(*Vsync)(void);						/* Sync display */
	int		(*VPointInPolygon)( int ScreenX, int ScreenY, int n, int x[], int y[]);		/* screen point inside a polygon on the screen */
} DevEntry;

typedef struct vdev {
	char	initialised,
			clipoff,
			inobject,
			inpolygon,
			fill,			/* polygon filling */
			cpVvalid,		/* is the current device position valid */
			sync,			/* Do we syncronise the display */
			inbackbuffer,		/* are we in the backbuffer */
			clipplanes;		/* active clipping planes */
	void	(*pmove)(void),		/* Polygon moves */
			(*pdraw)(void);		/* Polygon draws */
	TokList	*tokens;		/* ptr to list of tokens for current object */
	Mstack	*transmat;		/* top of transformation stack */
	Mstack	*transmatbottom;		/* bottom of transformation stack */
	Astack	*attr;			/* top of attribute stack */
	Vstack	*viewport;		/* top of viewport stack */
	float	hheight, hwidth;	/* hardware character height, width */
	Vector	cpW,			/* current postion in world coords */
			cpWtrans,		/* current world coords transformed */
			upvector;		/* world up */
	int		depth,			/* # bit planes on screen */
			maxVx, minVx,
			maxVy, minVy,
			sizeX, sizeY, 		/* size of square on screen */
			sizeSx, sizeSy,		/* side in x, side in y (# pixels) */
			cpVx, cpVy;
	DevEntry	dev;
	float	savex,			/* Where we started for v*() */
			savey,
			savez;
	char	bgnmode;		/* What to do with v*() calls */
	int		save;			/* Do we save 1st v*() point */

	char	*wintitle;		/* window title */

	char	*devname;		/* pointer to device name */

	Matrix	tbasis, ubasis, *bases; /* Patch stuff */
	
	char	*enabled;		/* pointer to enabled devices mask */
	int		maxfontnum;

	char	alreadyread;		/* queue device stuff */
	char	kbdmode;		/* are we in keyboard mode */
	char	mouseevents;		/* are mouse events enabled */
	char	kbdevents;		/* are kbd events enabled */
	int		devno, data;

	int		concave;		/* concave polygons? */
	int 	F_shade_freq;  /*dynamic or static shading? either d_STATIC or d_DYNAMIC*/
	int		CurrentGSpaceID; /* the ID to give the current object being created*/
	int 	ClickedGopherspaceObject; /* the objected clicked upon */
	float   rrR_rard [d_COLORRAMPS][3];
	int     rF_coluse [d_COLORRAMPS];
	int     rF_sinuse [d_COLMAP_OFF];
	int     rrrN_colormap [d_COLORRAMPS][d_LEN_COLRAMP][3];
} VDevice;

extern VDevice	vdevice;		/* device structure */


	/*
	 * Structure used to hold endpoints of the line used to
	 * paint words on a polygon.
	 */
typedef struct tagPolygonText
{
	float   R_move [3];
	float   R_draw [3];
} POLYGONLINE, *P_POLYGONLINE, **PP_POLYGONLINE;




	/*
	 * Structure used to temporarily hold text attributes
	 */
typedef struct tagTextAttr
{
	int     F_curr_text_color;
	float   R_curr_text_vec_x;
	float   R_curr_text_vec_y;
	float   R_curr_text_vec_z;
	float   R_curr_text_angle;
} TEXTATTR, *P_TEXTATTR, **PP_TEXTATTR;




#define	V_X	0			/* x axis in cpW */
#define	V_Y	1			/* y axis in cpW */
#define	V_Z	2			/* z axis in cpW */
#define	V_W	3			/* w axis in cpW */

/*
 * function definitions
 */
 
#ifdef MACINTOSH
extern void MacII_Window_init( CWindowPtr The_voglWindow, Rect The_voglRect );
#endif



/*
 * arc routines
 */
extern void arcprecision(int noseg);
extern void circleprecision(int noseg);
extern void arc(Coord x, Coord y, Coord radius, Angle sang, Angle eang);
extern void arcs(Scoord x, Scoord y, Scoord radius, Angle sang, Angle eang);
extern void arci(Icoord x, Icoord y, Icoord radius, Angle sang, Angle eang);
extern void arcf( Coord x, Coord y, Coord radius, Angle sang, Angle eang);
extern void arcfs(Scoord x, Scoord y, Scoord radius, Angle sang, Angle eang);
extern void arcfi( Icoord x, Icoord y, Icoord radius, Angle sang, Angle  eang);
extern void circ( Coord x, Coord y, Coord radius);
extern void circs( Scoord x, Scoord y, Scoord radius);
extern void circi( Icoord x, Icoord y, Icoord radius);
extern void circf( Coord x, Coord y, Coord radius);
extern void circfs( Scoord x, Scoord y, Scoord radius);
extern void circfi(Icoord x, Icoord y, Icoord radius);

/*
 * attr routines
 */
extern void	popattributes(void);
extern void	pushattributes(void);


/*
 * curve routines
 */
extern void curvebasis(short id);
extern void curveprecision(short nsegments);
extern void rcrv(Coord geom[4][4]);
extern void crv(Coord geom[4][3]);
extern void drcurve(int n, Matrix r);
extern void crvn(long n, Coord geom[][3]);
extern void rcrvn(long n, Coord geom[][4]);
extern void curveit(short n);

/*
 * draw routines
 */
extern void raw_draw(float x, float  y, float z);
extern void draw(register float x, register float y, 
		 register float z);
extern void draws(Scoord x, Scoord y, Scoord z);
extern void drawi(Icoord x, Icoord y, Icoord z);
extern void draw2(float x, float y);
extern void draw2s(Scoord x, Scoord y);
extern void	draw2i(Icoord x, Icoord y);
extern void	rdr(float dx, float dy, float dz);
extern void rdrs(Scoord dx, Scoord dy, Scoord dz);
extern void rdri(Icoord dx, Icoord dy, Icoord dz);
extern void rdr2(float dx, float dy);
extern void rdr2s(Scoord dx, Scoord dy);
extern void	rdr2i(Icoord dx, Icoord dy);
extern void	bgnline( void );
extern void	endline( void );
extern void	bgnclosedline( void );
extern void	endclosedline(void );

/*
 * device routines
 */
extern void qdevice(Device dev);
extern void	unqdevice(Device dev);
extern void qreset( void );
extern long qtest(void);
extern long qread(short *ret);
extern long isqueued(Device dev);
extern long winopen(char *title);
extern void gexit( void );
extern void ginit(void);
extern void gconfig(void);
extern long getgdesc(long inq);
extern void shademodel(long model);
extern void clear(void);
extern void colorf(float f);
extern void color(int i);
extern void mapcolor(Colorindex i, short r, short g, short b);
extern long getplanes(void);
extern long getvaluator(Device dev);
extern long getbutton(Device dev);
extern void	getdev(long n, Device devs[], short vals[]);
extern void vinit(char *device);
extern void vnewdev(char *device);

extern void voutput(char *path);
extern void verror(char *str);
extern char	*vgetdev(char *buf);

/*
 * mapping routines
 */
extern void CalcW2Vcoeffs(void);
extern int WtoVx(float p[]);
extern int WtoVy(float p[]);

/*
 * drawing mode stuff (not implemented..)
 */
#define NORMALDRAW              0x010
#define PUPDRAW                 0x020
#define OVERDRAW                0x040
#define UNDERDRAW               0x080
#define CURSORDRAW              0x100

extern void drawmode(long mode);

/*
 * matrix stack routines
 */
extern void getmatrix(Matrix m);
extern void popmatrix(void);
extern void loadmatrix(const Matrix mat);
extern void pushmatrix(void);
extern void multmatrix(const Matrix mat);
extern void multmatrix_affine(Matrix mat);
extern void mmode(MatrixMode);

/*
 * Z-Buffer routines (not implemented..)
 */

extern void zbuffer(int);

#define ZF_NEVER                0
#define ZF_LESS                 1
#define ZF_EQUAL                2
#define ZF_LEQUAL               3
#define ZF_GREATER              4
#define ZF_NOTEQUAL             5
#define ZF_GEQUAL               6
#define ZF_ALWAYS               7

extern void zfunction(long);

/***********************************************************************
 * Lighting routines
 */

#define LMNULL                  0.0

/* MATERIAL properties */
#define DEFMATERIAL             0
#define EMISSION                1
#define AMBIENT                 2
#define DIFFUSE                 3
#define SPECULAR                4
#define SHININESS               5
#define COLORINDEXES            6
#define ALPHA                   7

/* LIGHT properties */
#define DEFLIGHT                100
#define LCOLOR                  101
#define POSITION                102
#define SPOTDIRECTION           103
#define SPOTLIGHT               104

/* LIGHTINGMODEL properties */
#define DEFLMODEL               200
#define LOCALVIEWER             201
#define ATTENUATION             202
#define ATTENUATION2            203
#define TWOSIDE                 204


/* TARGET constants */
#define MATERIAL                1000
#define BACKMATERIAL            1001
#define LIGHT0                  1100
#define LIGHT1                  1101
#define LIGHT2                  1102
#define LIGHT3                  1103
#define LIGHT4                  1104
#define LIGHT5                  1105
#define LIGHT6                  1106
#define LIGHT7                  1107
#define LMODEL                  1200

/* lmcolor modes */
#define LMC_COLOR               0
#define LMC_EMISSION            1
#define LMC_AMBIENT             2
#define LMC_DIFFUSE             3
#define LMC_SPECULAR            4
#define LMC_AD                  5
#define LMC_NULL                6



extern void lmbind(short, short );
extern void lmcolor(long );
extern void lmdef(short, short, short, const float[] );


/***********************************************************************
 * move routines
 */
extern void move(const Coord x, const Coord y, const Coord  z);
extern void moves(Scoord x, Scoord y, Scoord z);
extern void movei(Icoord x, Icoord y, Icoord z);
extern void move2(Coord x, Coord y);
extern void move2s(Scoord x, Scoord y);
extern void move2i(Icoord x, Icoord y);
extern void rmv(Coord dx, Coord dy, Coord dz);
extern void rmvs(Scoord dx, Scoord dy, Scoord dz);
extern void rmvi(Icoord dx, Icoord dy, Icoord dz);
extern void rmv2(float dx, float dy);
extern void rmv2s(Scoord dx, Scoord dy);
extern void rmv2i(Icoord dx, Icoord dy);


/*
 * point routines
 */
extern void pnt(float x, float y, float z);
extern void pnts(Scoord x, Scoord y, Scoord z);
extern void pnti(Icoord x, Icoord y, Icoord z);
extern void pnt2(Coord x, Coord y);
extern void pnt2s(Scoord x, Scoord y);
extern void pnt2i(Icoord x, Icoord y);
extern void	bgnpoint(void);
extern void	endpoint(void);


/*
 * polygon routines.
 */
extern void concave(const long yesno);
extern void backface(const int onoff);
extern void frontface(const int onoff);
extern void polymode(const long mode);
extern void poly2(const long nv, float dp[][2]);
extern void poly2i(const long nv, Icoord dp[][2]);
extern void poly2s(const long nv, Scoord dp[][2]);
extern void polyi(const long nv, Icoord dp[][3]);
extern void polys(const long nv, Scoord dp[][3]);
extern void polf2(const long nv, float dp[][2]);
extern void polf2i(const long nv, Icoord dp[][2]);
extern void polf2s(const long nv, Scoord dp[][2]);
extern void polfi(const long nv, Icoord dp[][3]);
extern void polfs(const long nv, Scoord dp[][3]);
extern void poly(const long nv, float dp[][3]);
extern void polf(const long nv, float dp[][3]);
extern void pmv(float x,float  y, float z);
extern void pmvi(Icoord x, Icoord y, Icoord z);
extern void pmv2i(Icoord x, Icoord y);
extern void pmvs(Scoord x, Scoord y, Scoord z);
extern void pmv2s(Scoord x, Scoord y);
extern void pmv2(float x, float y);
extern void pdr(Coord x, Coord y, Coord z);
extern void rpdr(Coord dx, Coord dy, Coord dz);
extern void rpdr2(Coord dx, Coord dy);
extern void rpdri(Icoord dx, Icoord dy, Icoord dz);
extern void rpdr2i(Icoord dx, Icoord dy);
extern void rpdrs(Scoord dx, Scoord dy, Scoord dz);
extern void rpdr2s(Scoord dx, Scoord dy);
extern void rpmv(Coord dx, Coord dy, Coord dz);
extern void rpmv2(Coord dx, Coord dy);
extern void rpmvi(Icoord dx, Icoord dy, Icoord dz);
extern void rpmv2i(Icoord dx, Icoord dy);
extern void rpmvs(Scoord dx, Scoord dy, Scoord dz);
extern void rpmv2s(Scoord dx, Scoord dy);
extern void pdri(Icoord x, Icoord y, Icoord z);
extern void pdr2i(Icoord x, Icoord y);
extern void pdrs(Scoord x, Scoord y);
extern void pdr2s(Scoord x, Scoord y);
extern void pdr2(float x, float y);
extern void pclos(void);
extern void bgnpolygon(void);
extern void endpolygon(void);

/*
 * rectangle routines
 */
extern void rect(Coord x1, Coord y1, Coord x2, Coord y2);
extern void recti(Icoord x1, Icoord y1, Icoord x2, Icoord y2);
extern void rects(Scoord x1, Scoord y1, Scoord x2, Scoord y2);
extern void rectf(Coord x1, Coord y1, Coord x2, Coord y2);
extern void rectfi(Icoord x1, Icoord y1, Icoord x2, Icoord y2);
extern void rectfs(Scoord x1, Scoord y1, Scoord x2, Scoord y2);


/*
 * transformation routines
 */
extern void scale(register const float x, register const float y, 
		  register const float z);
extern void translate(float x, float y, float z);
extern void rot(float r, char axis);
extern void GRotate( float r, char axis, Matrix Mat_new, Matrix Mat_old );

/* extern void rotate(Angle r, char axis); */
#define rotate(r,a) rot(((float)r)/10.0,a)

/*
 * window definition routines
 */
extern void ortho(Coord left, Coord right, Coord bottom, Coord top, Coord hither, Coord yon);
extern void ortho2(Coord left, Coord right, Coord bottom, Coord top);
extern void lookat(Coord vx, Coord vy, Coord vz, Coord px, Coord py, Coord pz, Angle twist);
extern void window(Coord left, Coord right, Coord bottom, Coord top, Coord hither, Coord yon);
extern void polarview(Coord dist, Angle azim, Angle inc, Angle twist);
extern void perspective(Angle ifov, float aspect, Coord hither, Coord yon);

/*
 * tensor routines
 */
extern void multtensor(Tensor c, Matrix a, Tensor b);
extern void copytensor(Tensor b, Tensor a);
extern void premulttensor(Tensor c, Matrix a, Tensor b);
extern void copytensortrans(Tensor b, Tensor a);


/*
 * general matrix and vector routines
 */
extern void mult4x4(register Matrix a, register Matrix b, register Matrix c);
extern void	copymatrix(register Matrix a, register Matrix b);
extern void identmatrix(Matrix a);
extern void copytranspose(register Matrix a, register Matrix b);

extern void multvector(register Vector v, register Vector a, register Matrix b);
extern void copyvector(register Vector a, register Vector b);
extern void premultvector(Vector v, Vector a, Matrix b);


/*
 * Lighting routines
 */

extern void     lmbind( short, short );
extern void     lmcolor( long );
extern void     lmdef( short, short, short, const float[] );

/*
 * object routines
 */
extern void makeobj(long ob);
extern void closeobj(void);
extern void delobj(long ob);
extern long genobj(void);
extern long getopenobj(void);
extern void callobj(long ob);
extern long isobj( long ob);

extern Token * newtokens(int num);


/*
 * patch routines.
 */
extern void defbasis(short id, Matrix mat);
extern void patchbasis(long tb, long ub);
extern void patchcurves(long nt, long nu);
extern void patchprecision(long tseg, long useg);
extern void patch(Matrix geomx, Matrix geomy, Matrix geomz);
extern void rpatch(Matrix geomx, Matrix geomy, Matrix geomz, Matrix geomw);


/*
 * v routines
 */
extern void vcall(float vector[], int len);
extern void v4f(float vec[4]);
extern void v3f(float vec[3]);
extern void v2f(float vec[2]);
extern void v4d(double vec[4]);
extern void v3d(double vec[3]);
extern void v2d(double vec[2]);
extern void v4i(long vec[4]);
extern void v3i(long vec[3]);
extern void v2i(long vec[2]);
extern void v4s(short vec[4]);
extern void v3s(short vec[3]);
extern void v2s(short vec[2]);

/*
 * Misc color routines..
 */

void RGBmode(void);
void cmode(void);

/*
 * c routines...
 */
void c3s(short cv[3]);
void c3i(long  cv[3]);
void c3f(float cv[3]);
void c4s(short cv[4]);
void c4i(int   cv[4]);
void c4f(float cv[4]);

/*
 * n routines (for normals and shading...)
 */

void n3f(float nv[3]);

/*
 * text routines
 */
extern void hfont(char *name);
extern void font(short id);
extern void charstr( long rlN_vec[3], float R_angle, int F_color, float R_height, 
             float R_length, char *aCh_str );
extern void cmov(float x, float y, float z);
extern void cmov2(float x, float y);
extern void cmovi(Icoord x, Icoord y, Icoord z);
extern void cmovs(Scoord x, Scoord y, Scoord z);
extern void cmov2i(Icoord x, Icoord y);
extern void cmov2s(Scoord x, Scoord y);
#ifdef OLD_GL
extern void long getwidth(void );
#endif
extern long strwidth(char *s);
extern long getheight( void );
extern void getcpos(Scoord *cx, Scoord *cy);


/*
 * routines for manipulating the viewport
 */
extern void viewport(Screencoord xlow, Screencoord xhigh, Screencoord ylow, Screencoord yhigh);
extern void popviewport( void);
extern void	pushviewport( void);
extern void getviewport( Screencoord *left, Screencoord *right, Screencoord *bottom, Screencoord *top);


/*
 * routines for retrieving the graphics position
 */
extern void getgp(Coord *x, Coord *y, Coord*z );
extern void	getgpos(Coord *x, Coord *y, Coord *z, Coord *w);

/*
 * routines for handling the buffering
 */
extern void singlebuffer(void);
extern void doublebuffer(void);
extern void swapbuffers( void );
extern void frontbuffer(int yes);
extern void backbuffer(int yes);

/*
 * routines for window sizing and positioning
 */
extern void prefposition(long x1, long x2, long y1, long y2);
extern void prefsize(long x, long y);
extern void getprefposandsize(int *x, int *y, int *xs, int *ys);

/*
 * Misc control routines
 */
extern void vsetflush(int yn);
extern void vflush(void );

/*
 * vestigal appendix routines
 */
extern void yobbarays(int onoff);


#endif /* _VOGL_H_ */


extern float	Vcx, Vcy, Vsx, Vsy;  /*used for screen mapping Macros*/

/* some defines of macros we use to inline what used to be function calls */
#define WTOVX(p) ((int)(p[0] * Vsx / p[3] + Vcx))
#define WTOVY(p) ((int)(p[1] * Vsy / p[3] + Vcy))
#define COPYVECTOR(a,b)   { a[0] = b[0]; a[1] = b[1]; a[2] = b[2]; a[3] = b[3]; }
#define MULTVECTOR(a,b,c) { a[0] = b[0] * c[0][0] + b[1] * c[1][0] + b[2] * c[2][0] + b[3] * c[3][0]; a[1] = b[0] * c[0][1] + b[1] * c[1][1] + b[2] * c[2][1] + b[3] * c[3][1]; a[2] = b[0] * c[0][2] + b[1] * c[1][2] + b[2] * c[2][2] + b[3] * c[3][2]; a[3] = b[0] * c[0][3] + b[1] * c[1][3] + b[2] * c[2][3] + b[3] * c[3][3]; }

#define QMULTVECTOR(a,b,c) { register float x=b[0],y=b[1],z=b[2],w=b[3]; a[0] = x * c[0][0] + y * c[1][0] + z * c[2][0] + w * c[3][0]; a[1] = x * c[0][1] + y * c[1][1] + z * c[2][1] + w * c[3][1]; a[2] = x * c[0][2] + y * c[1][2] + z * c[2][2] + w * c[3][2]; a[3] = x * c[0][3] + y * c[1][3] + z * c[2][3] + w * c[3][3]; }
#define QMULTVECTOR_AFFINE(a,b,c) { register float x=b[0],y=b[1],z=b[2],w=b[3]; a[0] = x * c[0][0] + y * c[1][0] + z * c[2][0] + w * c[3][0]; a[1] = x * c[0][1] + y * c[1][1] + z * c[2][1] + w * c[3][1]; a[2] = x * c[0][2] + y * c[1][2] + z * c[2][2] + w * c[3][2]; a[3] =  w * c[3][3]; }
#define QMULTVECTOR_AFFINE_WONE(a,b,c) { register float x=b[0],y=b[1],z=b[2]; a[0] = x * c[0][0] + y * c[1][0] + z * c[2][0] + c[3][0]; a[1] = x * c[0][1] + y * c[1][1] + z * c[2][1] + c[3][1]; a[2] = x * c[0][2] + y * c[1][2] + z * c[2][2] + c[3][2]; a[3] =  c[3][3]; }

#define QMULTVECTOR_BOTTOM(a,b,c) { register float x=b[0],y=b[1],z=b[2],w=b[3]; a[0] = x * c[0][0] + z * c[2][0] + w * c[3][0]; a[1] =  y * c[1][1] + z * c[2][1] + w * c[3][1]; a[2] =  z * c[2][2] + w * c[3][2]; a[3] =  z * c[2][3] + w * c[3][3]; }
#define COPYMATRIX(a,b) copymatrix(a,b)

#ifdef MACINTOSH
	#include <quickdraw.h>
	extern CWindowPtr voglWindow;
	extern RGBColor MyColorIndexArray[255]; 
	
	#define REALDRAW(a,b)           MoveTo(vdevice.cpVx, vdevice.sizeY - vdevice.cpVy);LineTo(a, vdevice.sizeY - b);
	#define REALCLEARSCREEN         EraseRect (&voglWindow->portRect);		
	#define REALFILLPOLYGON(a,b,c)  {PolyHandle poly;int i;poly = OpenPoly ();MoveTo (b[0], vdevice.sizeY - c[0]);for (i = 1; i < a; i++) {LineTo(b[i],vdevice.sizeY - c[i]);}ClosePoly();FramePoly (poly);PaintPoly (poly);KillPoly (poly);vdevice.cpVx = b[a-1];vdevice.cpVy = c[a-1];}	
	#define REALSETCOLOR(a)         RGBForeColor(&MyColorIndexArray[ a ])

#elif defined(UNIX)
#       include "../drivers/X11.h"
#       define REALDRAW(a,b)          X11_draw(a,b)
#       define REALCLEARSCREEN        X11_clear()
#       define REALFILLPOLYGON(a,b,c) X11_fill(a,b,c)
#       define REALSETCOLOR(a)        X11_color(a)
#else
#       define REALDRAW(a,b) 			(*vdevice.dev.Vdraw)(a, b)
#       define REALCLEARSCREEN     	(*vdevice.dev.Vclear)()
#       define REALFILLPOLYGON(a,b,c)	(*vdevice.dev.Vfill)(a, b, c)
#       define REALSETCOLOR(a)			(*vdevice.dev.Vcolor)(a)
#endif

