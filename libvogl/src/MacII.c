/********************************************************************
 * $Id: MacII.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/


#define WindowIndependent
/*  #define ClassicVogl  */

/*
 * Vogle driver for Mac II
 *
 */
#include <stdio.h>

#include <quickdraw.h>
#include <qdoffscreen.h>
#include <errno.h>
#include <fp.h>

#include "vogl.h"

RGBColor MyWhite = { 0xFFFF, 0xFFFF, 0xFFFF };	
RGBColor MyBlack = { 0x0000, 0x0000, 0x0000 };	


#define DOPROFILE 
#ifdef DOPROFILE 
#include "Profiler.h"
#define SUFFIX "PPC"
#endif


/* some Mac includes below */
#include <Types.h>
#include <Memory.h>
#include <Quickdraw.h>
#include <Fonts.h>
#include <Events.h>
#include <Windows.h>
#include <OSUtils.h>
#include <ToolUtils.h>
/* end add */

#define MIN(x,y)	((x) < (y) ? (x) : (y))


/****
 * Globals
 ****/
CWindowPtr		voglWindow;  //moved to vogl.h
RGBColor MyColorIndexArray[255];  //moved to vogl.h



static short			voglWidth;
static short			voglHeight;
static GDHandle			mainGDevice;
static GWorldPtr		voglWorld;
static Boolean			isScreen;



#include "MacII.h"



void
SetUpColors( void )
{
	/* set up the RGB colors for the 8 default colors
	 */
	MyColorIndexArray[ RED ].red = 0xDD6B;
	MyColorIndexArray[ RED ].green = 0x08C2;
	MyColorIndexArray[ RED ].blue = 0x06A2;
	
	MyColorIndexArray[ CYAN ].red = 0x0241;
	MyColorIndexArray[ CYAN ].green = 0xAB54;
	MyColorIndexArray[ CYAN ].blue = 0xEAFF;
	
	MyColorIndexArray[ MAGENTA ].red = 0xF2D7;
	MyColorIndexArray[ MAGENTA ].green = 0x0856;
	MyColorIndexArray[ MAGENTA ].blue = 0x84EC;
	
	MyColorIndexArray[ GREEN ].red = 0x0000;
	MyColorIndexArray[ GREEN ].green = 0x64AF;
	MyColorIndexArray[ GREEN ].blue = 0x11B0;
	
	MyColorIndexArray[ YELLOW ].red = 0xFC00;
	MyColorIndexArray[ YELLOW ].green = 0x0F37D;
	MyColorIndexArray[ YELLOW ].blue = 0x052F;
	
	MyColorIndexArray[ BLUE ].red = 0x0000;
	MyColorIndexArray[ BLUE ].green = 0x0000;
	MyColorIndexArray[ BLUE ].blue = 0xD400;
	
	MyColorIndexArray[ WHITE ].red = 0x0000;
	MyColorIndexArray[ WHITE ].green = 0x0000;
	MyColorIndexArray[ WHITE ].blue = 0x0000;
	
	MyColorIndexArray[ BLACK ].red = 0xFFFF;
	MyColorIndexArray[ BLACK ].green = 0xFFFF;
	MyColorIndexArray[ BLACK ].blue = 0xFFFF;
}



/****
 * InitMacintosh()
 *
 * Initialize all the managers & memory
 * this is used when we are running standalone vogl
 * it is NOT called if we are running inside a Mac
 * program which creates windows and inits the Mac 
 * for us...
 *
 ****/

void InitMacintosh(void)
{
	Rect	voglRect;

	MaxApplZone();
	
	InitGraf(&qd.thePort); 

	InitFonts();
	FlushEvents(everyEvent, 0);
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0L);
	InitCursor();

	voglWidth = 608;
	voglHeight = 288;

	SetRect (&voglRect, 50, 50, voglWidth+50, voglHeight+50);
	voglWindow = (CWindowPtr)NewCWindow(0L, &voglRect, "\pVogl Window", true,
					noGrowDocProc, (WindowPtr) -1L, true, 0);
	ShowWindow ((WindowPtr)voglWindow);
	BringToFront ((WindowPtr)voglWindow);
	SetPort ((WindowPtr)voglWindow);
	RGBBackColor( &MyBlack ); 

}
/* end InitMacintosh */


/*
 * MacII_init
 *
 *	creates and initialises a drawing window
 */
int MacII_init(void)
{
	Rect	*wBounds;
	QDErr	result;
	
#ifdef ClassicVogl
	InitMacintosh();
#endif

#ifdef WindowIndependent
	SetPort ((WindowPtr)voglWindow);
#endif
	
	mainGDevice = GetMainDevice();
	vdevice.depth = (**(**mainGDevice).gdPMap).pixelSize;
    vdevice.sizeX = voglWidth;
    vdevice.sizeY = voglHeight;
    
/* 
pixels are square on the Mac so scaling should be equal on both x and y axis
	vdevice.sizeSx = vdevice.sizeSy =  MIN(voglWidth, voglHeight);
*/
	vdevice.sizeSx = vdevice.sizeSy =  voglWidth;

	/* Off screen stuff */	
	wBounds = &(*((CGrafPtr)voglWindow)->portPixMap)->bounds;
	SetOrigin (-wBounds->left, -wBounds->top); /* convert portRect to global */	
	result = NewGWorld (&voglWorld, 0, &voglWindow->portRect, 0L, 0L, noNewDevice);	
	SetOrigin (0, 0);
	
	/* fill the offscreen buffer with black */
	SetGWorld ((CGrafPtr)voglWorld, 0L);
	EraseRect( &voglWindow->portRect );
	RGBBackColor( &MyBlack ); 
	SetPort ((WindowPtr)voglWindow);
	isScreen = TRUE;

	/* End of off screen stuff */

	SetUpColors();
	return( TRUE );
}




/*
 * MacII_Window_init
 *
 *	initialises drawing stuff... 
 *  this assumes that something else created the window
 * but we have to tell vogl where to draw.
 */
void
MacII_Window_init(CWindowPtr The_voglWindow, Rect The_voglRect )
{
	voglWindow = (CWindowPtr)The_voglWindow;
	voglWidth = The_voglRect.right - The_voglRect.left;
	voglHeight= The_voglRect.bottom - The_voglRect.top;
}





/*
 * redisplay
 *
 *	redisplay the window.
 */
static void
redisplay( void )
{
}




/*
 * MacII_exit
 *
 *	cleans up before returning the window to normal.
 */
void MacII_exit( void )
{
}



/*
 * MacII_draw
 *
 *	draws a line from the current graphics position to (x, y).
 *
 * Note: (0, 0) is defined as the top left of the window on a MacII (easy
 * to forget).
 */
void MacII_draw(int x, int y)
{
	MoveTo (vdevice.cpVx, vdevice.sizeY - vdevice.cpVy);
	LineTo (x, vdevice.sizeY - y);
}


/*
 * MacII_getkey
 *
 *	grab a character from the keyboard.
 */
int MacII_getkey( void )
{
	EventRecord	theEvent;
	Boolean		done;
	int			ch;

	done = FALSE;
	
	do {
 		if (WaitNextEvent (everyEvent, &theEvent, (short)0, 0L))
 		{
			switch (theEvent.what)
			{
				case keyDown:   		/* Handle key inputs, fall into AutoKey code */
				case autoKey:   		/* Handle key inputs */
					ch = (int)theEvent.message & (int)charCodeMask; /* Get character pressed */
					done = TRUE;
					break;  			/* End of doing a keystroke */
				default:
					break;
			}
 		}
 	} while (!done);
	return(ch);
}


/*
 * MacII_checkkey
 *
 *	Check if a keyboard key has been hit. If so return it.
 */

int MacII_checkkey( void )
{
	EventRecord	theEvent;
	int			ch;

 	if (WaitNextEvent (everyEvent, &theEvent, (short)0, 0L))
 	{
		switch (theEvent.what)
		{
			case keyDown:   		/* Handle key inputs, fall into AutoKey code */
			case autoKey:   		/* Handle key inputs */
				ch = (int)theEvent.message & (int)charCodeMask; /* Get character pressed */
				return(ch);
				break;  			/* End of doing a keystroke */
			default:
				break;
		}
 	}
	return(0);
}
		

/*
 * MacII_locator
 *
 *	return the window location of the cursor, plus which mouse button,
 * if any, is been pressed.
 */
int MacII_locator(int *wx, int *wy)
{
	EventRecord	theEvent;
	int			but;
	Point		where;

	but = 0;

 	if (WaitNextEvent (everyEvent, &theEvent, 0, 0L))
 	{
		switch (theEvent.what)
		{
			case mouseDown:   		/* Handle mouse down */
				but |= 1;
				break;  			/* End of doing a mouseDown */
			default:
				break;
		}
 	}
 	where = theEvent.where;
 	GlobalToLocal (&where);
 	*wx = where.h;
 	*wy = (int)vdevice.sizeY - where.v;
	return(but);
}


/*
 * MacII_clear
 *
 *	Clear the screen to current clour
 */
void MacII_clear(void)
{
	RGBBackColor( &MyBlack );
	PaintRect (&voglWindow->portRect);  /* PaintRect to set the color... */
}


/*
 * MacII_color
 *
 *	set the current drawing color index.
 */
void MacII_color(int ind)
{

		RGBForeColor( &MyColorIndexArray[ ind ] );
}


/*
 * MacII_mapcolor
 *
 *	change index i in the color map to the appropriate r, g, b, value.
 */
void MacII_mapcolor(int i, int r, int g, int b)
{
/*	if (i >= 8 )   //this is a application defined color */
	{
		MyColorIndexArray[i].red = r;
		MyColorIndexArray[i].green = g;
		MyColorIndexArray[i].blue = b;
	}	
}


/*
 * MacII_font
 *
 *   Set up a hardware font. Return 1 on success 0 otherwise.
 *
 */
int MacII_font(char *fontfile)
{
	FontInfo fontInfo;
	
	TextFont (monaco);
	GetFontInfo (&fontInfo);
	vdevice.hheight = fontInfo.ascent + fontInfo.descent + fontInfo.leading;
	vdevice.hwidth = fontInfo.widMax;

	return(1);
}


/* 
 * MacII_char
 *
 *	 outputs one char - is more complicated for other devices
 */
void MacII_char(char c)
{
	MoveTo (vdevice.cpVx, (int)(vdevice.sizeY - vdevice.cpVy));
	DrawChar (c);
}


/*
 * MacII_string
 *
 *	Display a string at the current drawing position.
 */
void MacII_string( char	s[] )
{
	MoveTo (vdevice.cpVx, (int)(vdevice.sizeY - vdevice.cpVy));
	drawstring (s);
}


/*
 * MacII_fill
 *
 *	fill a polygon
 */
void MacII_fill(int n, int x[], int y[])
{
	PolyHandle		poly;
	int				i;

	poly = OpenPoly ();
		MoveTo (x[0], vdevice.sizeY - y[0]);
		for (i = 1; i < n; i++) {
			LineTo (x[i], vdevice.sizeY - y[i]);
		}
	ClosePoly ();
	FramePoly (poly);
	PaintPoly (poly);
	KillPoly (poly);

	vdevice.cpVx = x[n-1];
	vdevice.cpVy = y[n-1];
}



/*
 * MacII_PointInPolygon
 *
 *	is an x, y screen coordinate point inside a screen polygon?
 */
int MacII_PointInPolygon( int ScreenX, int ScreenY, int n, int x[], int y[])
{
	int			i;
	int			inside;
	Point		pt;
	PolyHandle	poly;
	RgnHandle	rgn;
		
	pt.v = ScreenY;
	pt.h = ScreenX;
	rgn = NewRgn();
	OpenRgn();
	
	poly = OpenPoly ();
		MoveTo (x[0], vdevice.sizeY - y[0]);
		for (i = 1; i < n; i++) {
			LineTo (x[i], vdevice.sizeY - y[i]);
		}
		LineTo(x[0], vdevice.sizeY - y[0]);
		//weird... we get away without having a line back to the start
		// for the polygon fill routine, but here we have to do this or
		// there are intermittent problems with detecting the point
		// in the polygon...
		
	ClosePoly ();
	FramePoly (poly);	
	CloseRgn( rgn );

	inside = PtInRgn(pt, rgn);
	
	KillPoly (poly);
	DisposeRgn( rgn );
	return( inside );
}








/*
 * BAD_MacII_fill
 *
 *	scanline fill a polygon (buggy and slower than the Quickdraw fill... -mpm)

BAD_MacII_fill(n, rawx, rawy)
	int	n, rawx[], rawy[];
{
	int		i, j;
	int 	xScanStart[ 1000 ]; // for now assume there are 1000 scanlines
	int 	xScanEnd[ 1000 ];
	int		y, xi, xf, mi, mf, xs, xe, ye, ys;
	int		ymin, ymax, xdelta, ydelta, twoydelta;
	
	if (n > 128)
		verror("vogl: more than 128 points in a polygon");

	// initialize the scanline arrays
	for ( i = 0; i < 1000; i++ ) {
		xScanStart[ i ] = -1;
		xScanEnd[ i ] = -1;		
	}
	
	ymin = 1000; 
	ymax = 0;

	for ( j = 0; j < ( n - 1 ); j++ ) {		
		ye = rawy[ j + 1 ];
		ys = rawy[ j ];
		xe = rawx[ j + 1 ];
		xs = rawx[ j ];
		
		if ( ys > ye ) {
			ys = rawy[ j + 1 ];
			ye = rawy[ j ];
			xs = rawx[ j + 1 ];
			xe = rawx[ j ];
		}
		ydelta = ( ye - ys );
		twoydelta = 2 * ydelta;
		xdelta = ( xe - xs );
		
		if (ydelta != 0 ) {
			if ( ymin > ys ) 
				ymin = ys;
			if ( ymax < ye ) 
				ymax = ye;
			
			xi = xs;
			xf = -ydelta;
			mi = xdelta / ydelta;
			mf = 2 * ( xdelta % ydelta );
			for ( y = ys; y < ( ye - 1 ) ; y++ ) {
			
				// output the points into the scanline arrays
				if ( xScanStart[ y ] == -1 ) {
					xScanStart[ y ] = xi;
				} else {
					if ( xScanEnd[ y ] == -1 ) {	
						xScanEnd[ y ] = xi;	
					} else {
						SysBeep(10);
					}
				}			
				
				xi = xi + mi;
				xf = xf + mf;
				if ( xf > 0 ) {
					xi = xi + 1;
					xf = xf - twoydelta;
				}
			}
		} else { //horizontal line
			MoveTo( rawx[ j ], vdevice.sizeY - rawy[ j ]);
			LineTo( rawx[ j + 1 ], vdevice.sizeY - rawy[ j ] );
		}
	}
	
	for ( y = ymin; y < ymax; y++ ) {
		if (( xScanStart[ y ] != -1 ) || ( xScanEnd[ y ] != -1 )) {
			MoveTo( xScanStart[ y ], vdevice.sizeY - y );
			LineTo( xScanEnd[ y ], vdevice.sizeY - y );
		} 		
	}

	vdevice.cpVx = rawx[ n - 1 ];
	vdevice.cpVy = rawy[ n - 1 ];
}

*/
 


/*
 * MacII_backbuf
 *
 *	swap to memory only drawing (backbuffer) - a little slow but it
 * works on everything. Where we can, we use the frame buffer.
 */
int MacII_backbuf( void )
{
	if (voglWorld == 0L)
		return(-1);

	if (isScreen == TRUE)
	{
		SetGWorld ((CGrafPtr)voglWorld, 0L);
		isScreen = FALSE;
	}

	return(1);
}



/*
 * MacII_swapbuf
 *
 *	swap the front and back buffers.
 */
int MacII_swapbuf(void)
{
	
	if (voglWorld == 0L) {
		SysBeep(10);
		SysBeep(10);
		SysBeep(10);
	
	};

	SetGWorld ((CGrafPtr)voglWorld, 0L);
	RGBForeColor( &MyBlack );
	RGBBackColor( &MyWhite );
	
	SetPort ((WindowPtr)voglWindow);
	RGBForeColor( &MyBlack );
	RGBBackColor( &MyWhite );

	StdBits((BitMap*)*voglWorld->portPixMap, &voglWindow->portRect, 
			&voglWindow->portRect, srcCopy, 0L);
			
	SetGWorld ((CGrafPtr)voglWorld, 0L);
	RGBBackColor( &MyBlack ); // set the background color so we can call eraserect to clear the GWorld

	return( 1 );
}



/*
 * MacII_frontb
 *
 *	draw in the front buffer
 */
void MacII_frontb( void )
{
	SetPort ((WindowPtr)voglWindow);
	isScreen = TRUE;
}


/*
 * Does nothing....
 */
void MacII_noop( void )
{
}


#ifndef VOGLE
/*
 * Haven't got around to these for VOGL yet.
 */
void MacII_setls(Linestyle lstyle)
{
}


void MacII_setlw(short lwidth)
{
}
#endif


/*
 * the device entry
 */
static DevEntry MacIIdev = {
	"MacII",
	"monaco",
	"chicago",
	MacII_backbuf,
	MacII_char,
	MacII_checkkey,
	MacII_clear,
	MacII_color,
	MacII_draw,
	MacII_exit,
	MacII_fill,
	MacII_font,
	MacII_frontb,
	MacII_getkey,
	MacII_init,
	MacII_locator,
	MacII_mapcolor,
#ifndef	VOGLE
	MacII_setls,
	MacII_setlw,
#endif
	MacII_string,
	MacII_swapbuf,
	MacII_noop,  /* -mpm noop'ed out the synchronize display call...*/
	MacII_PointInPolygon /* a new function to make double clicking on objects possible */
};

/*
 * _MacII_devcpy
 *
 *	copy the MacII device into vdevice.dev.
 */
void _MacII_devcpy(void)
{
	vdevice.dev = MacIIdev;
}
