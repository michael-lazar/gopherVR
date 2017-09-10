/*
 * This file is directly from the VOGLE library. It's simply had a few
 * gratuitous name changes and some comments added.
 * ... and fixed for endianness (Cameron Kaiser)
 */

/* oh, this is horrible */
#if defined(__ppc__) || defined(__powerpc__) || (defined(__m68k__) && !defined(MACINTOSH)) || defined(__sparc__) || defined(__sgi)
#warning using fixes for big endian
#define endian_fix_short(x) (x = ((x >> 8) + ((x & 255) << 8)))
#define SCSIZEY_FACTOR 3
#define LOGICAL_Y_FACTOR 900
#else
#warning assuming little endian. if this is wrong, fix this file!!
#define endian_fix_short(x) (x)
#define SCSIZEY_FACTOR 0
#define LOGICAL_Y_FACTOR 26 
#endif

#ifdef SGI
#include <gl.h>
#include <device.h>
#else
#include "vogl.h"
#include "vodevice.h"
#include "htext.h"
#include "halloc.h"
#include "valloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "check.h"


#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif

#ifndef PATH_SIZE
#define	PATH_SIZE	256
#endif

#ifdef PC
#ifndef FONTLIB
#define	FONTLIB		"c:\\lib\\hershey\\"
#endif
#else
#ifndef FONTLIB
#define	FONTLIB		"/usr/local/lib/hershey/"
#endif
#endif


#define	XCOORD(x)	(int)((x) - 'R')
#define	YCOORD(y)	(int)('R' - (y))

#ifndef PI
#define PI 	3.14159265358979
#endif

#ifndef D2R
#define D2R	(PI / 180.0)
#endif

#define LEFT		0	/* The default */
#define CENTERED	1
#define RIGHT		2

static	float	tcos = 1.0, tsin = 0.0;			/* For rotations */
static	float	SCSIZEX = 1.0, SCSIZEY = 1.0;		/* Scale factors */
static	int	Justify = LEFT;
static	int	Fixedwidth = 0;			/* Some flags	 */
static	short	nchars;					/* No. in font	 */
int	hLoaded = 0;


static	char	old_font[PATH_SIZE] = "";	/* So we don't have to reload it */
static	char	fpath[PATH_SIZE] = "";	


static	struct	{
	int	as;	/* Max ascender of a character in this font */
	int	dec;	/* Max decender of a character in this font */
	int	mw;	/* Max width of a character in this font */
	char	*p;	/* All the vectors in the font */
	char	**ind;	/* Pointers to where the chars start in p */
} ftab;


int hershfont(char *fontname);



/*
 * Added so that we can get a hook into the Hershey lib. so
 * we could tie it into our BSP code.
 *                                      GopherTeam
 */
extern	int g_B_text_waiting;
extern	int	g_B_texture_waiting;

float R_curr_logical_x;
float R_curr_logical_y;
float R_curr_logical_z;

float R_height_of_chars;
float g_R_yoff;

P_LINETEXT	pLtx_head;
int			cLtx_lines;






/*
 * hfont
 * 	loads in a hershey font.
 */
void hfont(char *name)
{
	/*
	 * check we aren't loading the same font twice in a row
	 */
	if (*name == '/') {
		if (strcmp(strrchr(name, '/') + 1, old_font) == 0)
			return;

	} else if (strcmp(name, old_font) == 0)
		return;

	/*
	 * Try and load it
	 */
	if (!hershfont(name)) {
		fprintf(stderr, "hershlib: problem reading font file '%s'.\n", name);
		exit(1);
	}

	/*
	 * Save the name of it...
	 */
	if (*name == '/')
		strcpy(old_font, strrchr(name, '/') + 1);
	else 
		strcpy(old_font, name);

}

/*
 * hnumchars
 *
 *	Return the number of characters in the currently loaded hershey font.
 */
int hnumchars( void )
{

	check_loaded("hnumchars");

	return((int)nchars);
}

/*
 * hsetpath
 *
 *	Set the path of the directory to look for fonts.
 */
void hsetpath(char *path)
{
	int	l;

	strcpy(fpath, path);

	l = strlen(fpath);

#ifdef PC
	if (fpath[l] != '\\')
		strcat(fpath, "\\");
#else
	if (fpath[l] != '/')
		strcat(fpath, "/");
#endif
}




 
 
/*
 * hershfont
 *
 * Load in a hershey font. First try the environment, then the font library,
 * if that fails try the current directory, otherwise return 0.
 */
#ifdef MACINTOSH
#warning MACINTOSH defined, is that what we want?
/* load hershey fonts the Macintosh way */
static int hershfont(char *fontname)
{
	int	i, offset;
	unsigned short	nvects, n;
	Handle  myHandle;
	ResType myType = 'hrsh';
	short result;
	short *pShort;
	
	/* 
	 * hardwired to futura font (resource ID 128)    -mpm
	 */	 
	myHandle = GetResource( 'hrsh', 128 );
	if ( ( result = ResError()) != 0 ) {
		verror( "error getting hershey font resource");
	}
	
	/* 
	 * lock the resource into memory since we need to keep 
	 * the hershey font definitions around long term. -mpm
	 */	 
	HLock( myHandle );
	
	pShort = (short *)(*myHandle);
	nchars = pShort[0];
	nvects = pShort[1];
	ftab.as  = (int) pShort[2];
	ftab.dec = (int) pShort[3];
	ftab.mw  = (int) pShort[4];

	/*
	 *  Allocate space for it all....
	 */
	if (hLoaded) {
		if (ftab.ind[0])
			WorldBFree((char *)ftab.ind[0]);
		if (ftab.ind)
			WorldBFree((char *)ftab.ind);
		hLoaded = 0;
	}

	ftab.ind = (char **) hallocate( ((sizeof(char *)) * (nchars + 1)) );
	ftab.p = (char *) hallocate( (2 * nvects) );

	/*
	 *  As we read in each character, figure out what ind should be
	 */
	offset = 5; 
	for (i = 0; i < nchars; i++) {
		n =  pShort[ offset ];
		ftab.p = (char *) ( &pShort[ offset + 1 ] );

		offset = offset + ( n / 2 ) + 1; 		
		ftab.ind[i] = ftab.p;
		ftab.p += n;
	}


	ftab.ind[nchars] = ftab.p;	/* To Terminate the last one */
	hLoaded = 1;
	return(1);
}
#endif /* end of Macintosh-style load hershfont loader */


#ifdef UNIX

/*
 * hershfont
 *
 * Load in a hershey font. First try the environment, then the font library,
 * if that fails try the current directory, otherwise return 0.
 */
int hershfont(char *fontname)
{
     FILE	*fp;
     int	i;
     short	nvects, n;
     char	*flib;
     char	path[PATH_SIZE];	
     
     if (fpath[0] != '\0') {
	  strcpy(path, fpath);
	  strcat(path, fontname);
     } else if ((flib = getenv("HFONTLIB")) != (char *)NULL) {
	  strcpy(path, flib);
#ifdef PC
	  strcat(path, "\\");
#else
	  strcat(path, "/");
#endif
	  strcat(path, fontname);
     } else if ((flib = getenv("VFONTLIB")) != (char *)NULL) {
	  strcpy(path, flib);	/* To be compatible with VOGLE */
#ifdef PC
	  strcat(path, "\\");
#else
	  strcat(path, "/");
#endif
	  strcat(path, fontname);
     } else {
	  strcpy(path, FONTLIB);
#ifdef PC
	  strcat(path, "\\");
#else
	  strcat(path, "/");
#endif
	  strcat(path, fontname);
     }
     
#ifdef PC
     if ((fp = fopen(path, "r+b")) == (FILE *)NULL) 
	  if ((fp = fopen(fontname, "r+b")) == (FILE *)NULL) {
#else
	       if ((fp = fopen(path, "r")) == (FILE *)NULL)
		    if ((fp = fopen(fontname, "r")) == (FILE *)NULL) {
#endif
			 fprintf(stderr, "hershlib: Can't open Hershey fontfile '%s' or './%s'.\n", path, fontname);
			 exit(1);
		    }

	       if (fread(&nchars, sizeof(nchars), 1, fp) != 1)
		    return (0);
		endian_fix_short(nchars);

#ifdef DEBUG
	       printf("nchars = %d\n", nchars);
#endif

	       if (fread(&nvects, sizeof(nvects), 1, fp) != 1)
		    return(0);
		endian_fix_short(nvects);

#ifdef DEBUG
	       printf("nvects = %d\n", nvects);
#endif
	       if (fread(&n, sizeof(n), 1,  fp) != 1)
		    return(0);
		endian_fix_short(n);

#ifdef DEBUG
	       printf("ftab.as = %d\n", n);
#endif
	       ftab.as = (int)n;

	       if (fread(&n, sizeof(n), 1, fp) != 1)
		    return(0);
		endian_fix_short(n);

#ifdef DEBUG
	       printf("ftab.dec = %d\n", n);
#endif
	       ftab.dec = (int)n;

	       if (fread(&n, sizeof(n), 1, fp) != 1)
		    return(0);
		endian_fix_short(n);

#ifdef DEBUG
	       printf("ftab.mw = %d\n", n);
#endif

	       ftab.mw = (int)n;

	       /*
		*  Allocate space for it all....
		*/
	       if (hLoaded) {
		    if (ftab.ind[0])
			 WorldBFree((char *)ftab.ind[0]);
		    if (ftab.ind)
			 WorldBFree((char *)ftab.ind);
		    hLoaded = 0;
	       }

	       ftab.ind = (char **)hallocate(sizeof(char *)*(nchars + 1));

	       ftab.p = (char *)hallocate((unsigned)(2 * nvects));

	       /*
		*  As we read in each character, figure out what ind should be
		*/

	       for (i = 0; i < nchars; i++) {
		    if (fread(&n , sizeof(n), 1, fp) != 1)
			 return(0);
			endian_fix_short(n);

#ifdef DEBUG
	fprintf(stderr, "loaded char %i (needs to be %i bytes)\n", i, (int)n);
#endif

		    if (fread(ftab.p, 1, (unsigned)n, fp) != (unsigned)n)
			 return(0);

		    ftab.ind[i] = ftab.p;
		    ftab.p += n;
	       }

	       ftab.ind[nchars] = ftab.p; /* To Terminate the last one */

	       fclose(fp);
	       hLoaded = 1;
#ifdef DEBUG
	fprintf(stderr, "loaded font\n");
#endif
	       return(1);
}
#endif /* end of Unix-style load hershfont loader */
     
/*
 * hgetcharsize
 *
 *	get the width and height of a single character. At the moment, for
 * the hershey characters, the height returned is always that of the
 * difference between the maximun descender and ascender.
 *
 */
void hgetcharsize(char c, float *width, float *height)
{
     check_loaded("hgetcharsize");
     
     *height = (float)(ftab.as - ftab.dec) * SCSIZEY;
     
     if (Fixedwidth)
	  *width = (float)ftab.mw * SCSIZEX;
     else
	  *width = (float)(ftab.ind[c - 32][1] - ftab.ind[c - 32][0]) * SCSIZEX;
}



/*
 * hdrawchar
 *
 * Display a character from the currently loaded font.
 */
void hdrawchar(int c)
{
     int	Move, i, x, y, xt, yt;
     char	*p, *e;
     float	xs, ys, xp, yp, tmp, xtmp, ytmp;
     P_LINETEXT  pLtx_tmp;
     
     
     check_loaded("hdrawchar");
     
     if ((i = c - 32) < 0)
	  i = 0;
     if (i >= nchars)
	  i = nchars - 1;
     
     Move = 1;
     
     xt = yt = 0;
     if (Justify == LEFT) 
     {
	  xt = (Fixedwidth ? -ftab.mw / 2 : XCOORD(ftab.ind[i][0]));
	  yt = ftab.dec;
     } 
     else if (Justify == RIGHT) 
     {
	  xt = (Fixedwidth ?  ftab.mw / 2 : -XCOORD(ftab.ind[i][0]));
	  yt = ftab.dec;
     }
     
#ifdef MACINTOSH
     /* 
      * since we are working from indexes into the locked
      * hershey font resource, we want to skip over the
      * address holding the count of vector. Unix avoids this
      * by copying from a file into a new structure, but in Mac
      * land we only move pointers where possible. -mpm
      */
     e = ftab.ind[i + 1] - 2;
#else
     e = ftab.ind[i + 1];
#endif
     p = ftab.ind[i] + 2;
     
     xtmp = ytmp = 0.0;
     
     while(p < e) 
     {
	  x = XCOORD((int)*p++);
	  y = YCOORD((int)*p++);
	  if (x != -50)  			/* means move */
	  {
	       xp = (float)(x - xt) * SCSIZEX;
	       yp = (float)(y - yt) * SCSIZEY;
	       tmp = xp;
	       xp = tcos*tmp - tsin*yp;
	       yp = tsin*tmp + tcos*yp;
	       xs = xp - xtmp;
	       ys = yp - ytmp;
	       xtmp = xp;
	       ytmp = yp;
	       if (Move) 
	       {
		    Move = 0;
		    /*rmv((Coord)xs, (Coord)ys, (Coord)0.0);*/
		    R_curr_logical_x = R_curr_logical_x + xs;
		    R_curr_logical_y = R_curr_logical_y + ys;
	       } 
	       else
	       {
		    /*move( R_curr_logical_x, R_curr_logical_y, 0.0 );*/
		    /*draw( R_curr_logical_x+xs, R_curr_logical_y+ys, 0.0 );*/
		    
		    pLtx_tmp = (LINETEXT *) hallocate( sizeof(LINETEXT) );
		    
	/*
         * Below we subtract the height of the chars. from the Y axis 
	 * Because Hershey draws the characters from bottom to top, this
	 * allows us to later specify the upper left-hand point of the polygon
	 * we want to splat the characters on, without having to do nasty
	 * subtractions in order to place the character correctly
	 */
		    pLtx_tmp->R_move [V_X] = R_curr_logical_x; 
		    pLtx_tmp->R_move [V_Y] = R_curr_logical_y - R_height_of_chars 
			 - g_R_yoff;
		    pLtx_tmp->R_draw [V_X] = R_curr_logical_x + xs;
		    pLtx_tmp->R_draw [V_Y] = R_curr_logical_y + ys 
			 - R_height_of_chars - g_R_yoff;
		    
		    /*
		     * count of lines added - helps with the calloc()
		     *           -- see pclos() for more details
		     */
		    cLtx_lines = cLtx_lines + 1;
		    
		    if ( NULL == pLtx_head )
		    {
			 pLtx_tmp->pLtx_next = NULL;
			 pLtx_tmp->pLtx_previous = NULL;
			 pLtx_head = pLtx_tmp;
		    }
		    else
		    {
			 /* insert the new element at the
			    head of the list */
			 pLtx_tmp->pLtx_previous = NULL;
			 pLtx_tmp->pLtx_next = pLtx_head;
			 pLtx_head->pLtx_previous = pLtx_tmp;
			 pLtx_head = pLtx_tmp;
		    }
		    
		    R_curr_logical_x = R_curr_logical_x + xs;
		    R_curr_logical_y = R_curr_logical_y + ys;
		    
	       }	
	  } 
	  else 
	  {
	       Move = 1;
	  }
     }
     
     /*
      * Move to right hand of character.
      * BLARK
      */
     tmp = Fixedwidth ? (float)ftab.mw : (float)(ftab.ind[i][1] - 
						 ftab.ind[i][0]);
     
     tmp *= SCSIZEX;
     xs = tcos * tmp - xtmp;
     ys = tsin * tmp - ytmp;
     /*rmv((Coord)xs, (Coord)ys, 0.0);*/
     R_curr_logical_x = R_curr_logical_x + xs;
     R_curr_logical_y = R_curr_logical_y + ys;
     R_curr_logical_z = R_curr_logical_z;
     
}



/*
 * htextsize
 *
 * set software character scaling values 
 *
 * Note: Only changes software char size. Should be called
 * after a font has been loaded.
 *
 */
void htextsize(float width, float height)
{
     float	a;
     
     check_loaded("htextsize");
     
     a = (float)MAXVOGL((int)ftab.mw, (int)(ftab.as - ftab.dec));
     
     SCSIZEX = width / ABSVOGL(a);
     SCSIZEY = height / ABSVOGL(a);
}

/*
 * hgetfontwidth
 *
 * Return the maximum Width of the current font.
 *
 */
float hgetfontwidth(void)
{
     check_loaded("hgetfontwidth");

     return((float)(SCSIZEX * MAXVOGL((int)ftab.mw, (int)(ftab.as - ftab.dec))));
}

/* 
 * hgetfontheight
 *
 * Return the maximum Height of the current font
 */
float hgetfontheight(void)
{
     check_loaded("hgetfontheight");
     
     return((float)(SCSIZEY * MAXVOGL((int)ftab.mw, (int)(ftab.as - ftab.dec))));
}

/*
 * hgetfontsize
 *
 * Get the current character size in user coords.
 * For software Hershey fonts, the character width is that of
 * a the widest character and the height the height of the tallest.
 *
 */
void hgetfontsize(float *cw, float *ch)
{
     check_loaded("hgetfontsize");
     
     *cw = hgetfontwidth();
     *ch = hgetfontheight();
}

/*
 * hgetdecender
 *
 *	Return the maximum decender of the current font.
 * 	(In world coords).
 */
float hgetdecender(void)
{
     check_loaded("hgetdecender");
     
     return((float)ftab.dec * SCSIZEY);
}

/*
 * hgetascender
 *
 *	Return the maximum assender of the current font.
 * 	(In world coords).
 */
float hgetascender(void)
{
     check_loaded("hgetascender");
     
     return((float)ftab.as * SCSIZEY);
}

/*
 * hcharstr
 *
 * Draw a string from the current pen position.
 *
 */
void hcharstr(char *string)
{
     float	width, height, cx, cy;
     char	*str = string, c;
     int	oldJustify;
     
     
     check_loaded("hcharstr");
     
     height = hgetfontheight();
     width = hstrlength(string);
     
     cx = cy = 0.0;
     if (Justify == CENTERED) 
     {
	  height /= 2.0;
	  width /= 2.0;
	  cx =  height * tsin - width * tcos;
	  cy = -height * tcos - width * tsin;
     } 
     else if (Justify == RIGHT) 
     {
	  height = 0.0;
	  cx =  height * tsin - width * tcos;
	  cy = -height * tcos - width * tsin;
     }
     
     /*rmv((Coord)cx, (Coord)cy, (Coord)0.0);*/
     R_curr_logical_x = R_curr_logical_x + cx;
     R_curr_logical_y = R_curr_logical_y + cy;
     
     
     /*
      * For the duration of hershey strings, turn off
      * "Justify" as we have already compensated
      * for it in hcharstr()
      */
     oldJustify = Justify;
     Justify = 0;
     
     while ((c = *str++))
	  hdrawchar(c);
     
     Justify = oldJustify;
     
}

/*
 * istrlength
 *
 * Find out the length of a string in raw "Hershey coordinates".
 */
static	int istrlength(char *s);
static	int istrlength(char *s)
{
     char	c;
     int	i, len = 0;
     
     if (Fixedwidth)
	  return((int)(strlen(s) * ftab.mw));
     else {
	  while ((c = *s++)) {
	       if ((i = (int)c - 32) < 0 || i >= nchars)
		    i = nchars - 1;
	       
	       len += (ftab.ind[i][1] - ftab.ind[i][0]);
	  }
	  return (len);
     }
}

/*
 * hstrlength
 *
 * Find out the length (in world coords) of a string.
 *
 */
float hstrlength(char *s)
{
     check_loaded("hstrlength");
     
     return((float)(istrlength(s) * SCSIZEX));
}

/*
 * hboxtext
 *
 * Draw text so it fits in a "box" - note only works with hershey text
 */
void hboxtext(float x, float y, float l, float h, char *s, 
				int B_pending, int C_lines )
{
     float	oscsizex, oscsizey;
     P_LINETEXT  pLtx_clear;
     
     check_loaded("hboxtext");
     
     
     
     if ( FALSE == g_B_text_waiting )
     {
	  pLtx_head = NULL;
	  cLtx_lines = 0;
	  g_R_yoff = 0.0;
     }
     else
     {
	  
	  /*
	   * Something is wrong = no polygon picked up the last text
	   * that was waiting to be picked up - remove the old text
	   * before we deal with the new text.
	   *
	   * OR
	   *
	   * we should append more text to the current list of text, this
	   * is a new line of text
	   */
	  
	  if ( FALSE == B_pending )
	  {
	       while (pLtx_head)
	       {
		    pLtx_clear = pLtx_head->pLtx_next;
		    WorldBFree( (char *) pLtx_head );
		    pLtx_head = pLtx_clear;
	       }
	       
	       pLtx_head = NULL;
	       cLtx_lines = 0;
	       g_R_yoff = 0.0;
	  }
	  else
	  {
	       /* 
		* squish the text lines together a bit -mpm
		*			g_R_yoff = (h * (float)C_lines) + (h * 0.1 * (float)C_lines ); 
		*/
	       g_R_yoff = (h * (float)C_lines) * 0.9; 
	       
	  }
     }
     
     
     R_height_of_chars = h;
     oscsizex = SCSIZEX;
     oscsizey = SCSIZEY;
     /*
      * set width so string length is the same a "l" 
      */
     SCSIZEX = l / (float)istrlength(s);
     SCSIZEY = h / (float)(ftab.as - ftab.dec) + SCSIZEY_FACTOR;
     
     /* 
      * set character height so it's the same as "h" 
      */
/* THIS IS WHERE WE FIX THE HEIGHT */
/* I don't know why this works, but it does. */
     
     /*move2(x, y);*/
     R_curr_logical_x = x;
     R_curr_logical_y = y - LOGICAL_Y_FACTOR;
     R_curr_logical_z = 0.0;
     
     hcharstr(s);
     
     SCSIZEX = oscsizex;
     SCSIZEY = oscsizey;
}

/*
 * hboxfit
 *
 * Set up the scales etc for text so that a string of "nchars" characters
 * of the maximum width in the font fits in a box.
 */
void hboxfit(float l, float h, int nchars)
{
     check_loaded("hboxfit");
     
     SCSIZEX = l / (float)(nchars * ftab.mw);
     SCSIZEY = h / (float)(ftab.as - ftab.dec);
}

/*
 * The following can be set without a font actually being loaded....
 */
/*
 * hcenter
 *
 *	Turns centering of text on or off
 */
void hcentertext(int onoff)
{
     Justify = (onoff != 0 ? CENTERED : LEFT);
}

/*
 * hrightjustify
 *
 *	Right Justifies the text.
 */
void hrightjustify(int onoff)
{
     Justify = (onoff != 0 ? RIGHT : LEFT);
}

/*
 * hleftjustify
 *
 *	Left Justifies the text. (the default).
 */
void hleftjustify(int onoff)
{
     Justify = (onoff != 0 ? LEFT : RIGHT);
}

/*
 * fixedwidth
 *
 *	Turns fixedwidth text on or off
 */
void hfixedwidth(int onoff)
{
     Fixedwidth = onoff;
}

/*
 * htextang
 *
 * set software character angle in degrees
 *
 * strings will be written along a line 'ang' degrees from the 
 * horizontal screen direction
 *
 * Note: only changes software character angle
 *
 */
void htextang(float ang)
{
     
     tcos = cos((double)(ang * D2R));
     tsin = sin((double)(ang * D2R));
}

