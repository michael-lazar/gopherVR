/*
 * VOGL/VOGLE driver for X11.
 * 
 * Define VOGLE if this driver is really for the VOGLE Libarary.
 *
 */
#undef VOGLE

#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>

#include "vogl.h"
static	char	*me = "vogl";
#define LARGEFONT	"9x15bold"
#define SMALLFONT	"6x13bold"


#define MIN(x,y)	((x) < (y) ? (x) : (y))
#define MAX(x,y)	((x) > (y) ? (x) : (y))
#define	CMAPSIZE	256
#define	MAX_PLANES	8
#define	EV_MASK		KeyPressMask|ButtonReleaseMask|ExposureMask|ButtonPressMask

static	int		maxw = -1, maxh = -1;
static	Window		winder;
static	Display		*display;
static	int		screen;
static	unsigned long	carray[CMAPSIZE];
static	Colormap	colormap;
static	Visual		*pVis_visual;

static	Window	theDrawable = -1;
static	GC		theGC;
static	XGCValues	theGCvalues;
static	Pixmap		bbuf;		/* Back buffer pixmap */
static	int		back_used = 0;	/* Have we backbuffered ? */

static	XFontStruct	*font_id = (XFontStruct *)NULL;
XEvent			event;

static	unsigned long	colour;
static	unsigned int	h, w;
static	char		*smallf, *largef;
static	char		use_toolkit_win = 0;



#include "X11.h"

/***********************************************************************
 * Routines for the Multi-Buffering X extension
 */

#ifndef NO_MULTIBUF
#include <X11/extensions/multibuf.h>

static int         HasMultibuf = -1;
static Multibuffer Mbufs[2];
static int         CurrentBuf = 0;

static void
CheckMultibuf(void)
{
     int event_base_return, error_base_return;

     if (HasMultibuf == -1)
	  if (XmbufQueryExtension(display, &event_base_return,
				  &error_base_return))
	       HasMultibuf = 1;
	  else
	       HasMultibuf = 0;

     if (getenv("NOMULTIBUF") != NULL)
	  HasMultibuf = 0;

}
	 
static void
CreateMultiBufs(Window win)
{
     CheckMultibuf();

     if (HasMultibuf) {
	  if (2 != XmbufCreateBuffers(display, win, 2,
				      MultibufferUpdateActionBackground,
				      MultibufferUpdateHintFrequent, 
				      Mbufs)) {
	       fprintf(stderr, "Couldn't create multibufffers, punting...\n");
	  }
     }
     CurrentBuf = 1;
     theDrawable = Mbufs[CurrentBuf];
}

static void
DestroyMultiBufs(Window win)
{
     XmbufDestroyBuffers(display, win);
}

static void
SwapMultiBuf(void) 
{
     XmbufDisplayBuffers(display, 1, &Mbufs[CurrentBuf], 0, 0);

     CurrentBuf = 1 - CurrentBuf;
     theDrawable = Mbufs[CurrentBuf];
     
}

#else  /* NO_MULTIBUF */
static int         HasMultibuf = -1;

static void
CheckMultibuf(void) 
{
     HasMultibuf = 0;

}

static void CreateMultiBufs(Window win)
{
     /** Should Never be called **/;
}

static void DestroyMultiBufs(Window win)
{
     /** Should Never be called **/;
}

static void SwapMultiBuf(void)
{
     /** Should Never be called **/;
}



#endif
/**********************************************************************/

/*
 * vo_xt_set_win
 *
 *	Just sets the drawable to the partucular window.
 */
int
vo_xt_set_win(Display *dis, Drawable win, int xw, int xh)
{
	int	backb;

	backb = (theDrawable == bbuf);
	winder = win;

	vdevice.sizeX = vdevice.sizeY = MIN(xh, xw);
	vdevice.sizeSx = xw;
	vdevice.sizeSy = xh;

        if (xw > maxw || xh > maxh) {
		if (back_used && !HasMultibuf) {
			back_used = 0;
			XFreePixmap(display, bbuf);
			X11_backbuf();
		}
        }

	display = dis;
	if (backb)
		theDrawable = bbuf;
	else
		theDrawable = win;

	return(1);
}


static void UsePrivateColormap()
{
     int depth = vdevice.depth;
     XVisualInfo	Xvi_visual_info;
     static unsigned long    s_rl_plane_mask [MAX_PLANES];
     XColor		Xcl_tmp;
     XSetWindowAttributes swa;
     static cmap_atom= None;

     Colormap defcolormap = DefaultColormap(display, screen);

     /*** Does this Server support PseudoColor or DirectColor ? ***/
     if (!XMatchVisualInfo( display, screen, depth, PseudoColor,
			   &Xvi_visual_info))   {
	  if (!XMatchVisualInfo( display, screen, depth, DirectColor,
				&Xvi_visual_info)) {
	       printf( "Sorry, you are running a XServer that " );
	       printf( "doesn't support PseudoColor or DirectColor\n" );
	       exit(0);
	  } else {
	       /* We have at least DirectColor, but do we have a large
		* enough CMAP?
		*/
	       if ( CMAPSIZE > Xvi_visual_info.colormap_size )
	       {
		    printf( "Sorry, you are running a XServer that " );
		    printf( "supports DirectColor but with less than %d colors\n",
			   CMAPSIZE );
	       }
	  }
     } else {  /*We have PseudoColor, but do we have a large enough CMAP? */
	  if (CMAPSIZE > Xvi_visual_info.colormap_size) {
	       printf( "Sorry, you are running a XServer that " );
	       printf( "supports PseudoColor but with less than %d colors\n",
		      CMAPSIZE );
	  }
     }
	  
     /*
      * If we are here we support either PseudoColor or DirectColor and
      * have a large enougth CMAP
      */
     colormap = XCreateColormap ( display, winder, pVis_visual, AllocNone );
     if (!XAllocColorCells ( display, colormap, False, s_rl_plane_mask,
			    0, carray, CMAPSIZE ))
     {
	  printf( "Couldn't get %d colors \n", CMAPSIZE ); exit( 0 );
     }

	  
     /*
      * By default only two colors are set, Black and White
      */
     
	  
     if (cmap_atom == None)
	  cmap_atom = XInternAtom(display, "WM_COLORMAP_WINDOWS", False);
	  
	  
     swa.colormap = colormap;
     XChangeWindowAttributes(display, winder, CWColormap, &swa);
     XDeleteProperty(display, winder, cmap_atom);
     XInstallColormap(display, colormap);
	  
     Xcl_tmp.pixel = carray [0];  
     Xcl_tmp.red = 0;
     Xcl_tmp.green = 0;
     Xcl_tmp.blue = 0;
     Xcl_tmp.flags = DoRed | DoGreen | DoBlue;
     XStoreColor( display, colormap, &Xcl_tmp );
     XSetWindowColormap( display, winder, colormap );
     XFlush(display);
     
     Xcl_tmp.pixel = carray [1];
     Xcl_tmp.red = 255 * 255;
     Xcl_tmp.green = 255 * 255;
     Xcl_tmp.blue = 255 * 255;
     Xcl_tmp.flags = DoRed | DoGreen | DoBlue;
     XStoreColor( display, colormap, &Xcl_tmp );
     XSetWindowColormap( display, winder, colormap );
     XFlush(display);
     
}


/*
 * vo_xt_window
 *
 *	Tells VOGL/VOGLE to use a window from an X11 toolkit (eg xview)
 *	and not to make it's own window.
 */
int
vo_xt_window(Display *dis, Window win, int xw, int xh, Colormap usecolormap)
{
     int	backb, i, depth;

	
     backb = (theDrawable == bbuf);

     display = dis;
     winder = win;
     screen = DefaultScreen(display);
     pVis_visual = DefaultVisual( display, screen );
     depth = vdevice.depth = DefaultDepth(display, screen);
     theDrawable = winder;
     
     use_toolkit_win = 1;
     w = xw;
     h = xh;
     
/*     colormap = DefaultColormap(display, screen);*/
     colormap = usecolormap;
     
     /*
      * Set our standard colors...
      */
     if (vdevice.depth == 1) {
	  /*
	   * Black and white - anything that's not black is white.
	   */
	  carray[0] = BlackPixel(display, screen);
	  for (i = 1; i < CMAPSIZE; i++)
	       carray[i] = WhitePixel(display, screen);
     } else {
	  /*
	   * Color, try to get our colors close to what's in the
	   * default colormap.
	   */
	  X11_mapcolor(0, 0, 0, 0);
	  X11_mapcolor(1, 255, 255, 255);
     }
     
     if ((smallf = XGetDefault(display, me, "smallfont")) == (char *)NULL)
	  smallf = SMALLFONT;
     
     if ((largef = XGetDefault(display, me, "largefont")) == (char *)NULL)
	  largef = LARGEFONT;
     
     /*
      * Create Graphics Context and Drawable
      */
     
     theGC = XDefaultGC(display, screen);
     theGCvalues.graphics_exposures = False;
     theGCvalues.cap_style = CapButt;
     XChangeGC(display, theGC, GCGraphicsExposures|GCCapStyle, &theGCvalues);
     X11_color(0);
     
     vdevice.sizeX = vdevice.sizeY = MIN(xh, xw);
     vdevice.sizeSx = xw;
     vdevice.sizeSy = xh;
     
     if (back_used && (xw > maxw || xh > maxh)) {
	  back_used = 0;
	  XFreePixmap(display, bbuf);
	  X11_backbuf();
     }
     
     if (backb)
	  theDrawable = bbuf;
     else
	  theDrawable = win;
     
     
#ifndef VOGLE
     vdevice.devname = "X11";
#endif
     
     return(1);
}





/*
 *	vo_xt_win_size
 *
 * If the X toolkit has changed the window size, then
 * you might wish to call this routine to tell vogl/vogle about it.
 */
int
vo_xt_win_size(int xw, int xh)
{
     char backb;

     w = xw;
     h = xh;
     
     vdevice.sizeX = vdevice.sizeY = MIN(h, w);
     vdevice.sizeSx = w;
     vdevice.sizeSy = h;

     backb = (theDrawable == bbuf);
     
     if (back_used) {
	  
	  /* Have to re allocate the back buffer */
	  
	  if (HasMultibuf) {
	       DestroyMultiBufs(winder);
	       CreateMultiBufs(winder);
	  } else {
	       XFreePixmap(display, bbuf);
	       bbuf = XCreatePixmap(display,
				    (Drawable)winder,
				    (unsigned int)vdevice.sizeSx + 1,
				    (unsigned int)vdevice.sizeSy + 1,
				    (unsigned int)vdevice.depth
				    );
	  }
     }
     
     if (backb && !HasMultibuf)
	  theDrawable = (Drawable)bbuf;
	  
     return( 0 );
}





/*
 * X11_init
 *
 *	initialises X11 display.
 */
int
X11_init( void )
{
     int					i;
     int		     x, y, prefx, prefy, prefxs, prefys;
     unsigned int	     bw, depth, mask;
     Window		     rootw, childw;
     char		     *av[2], name[128], *geom;
     
     XSetWindowAttributes    theWindowAttributes;
     XWindowAttributes	     retWindowAttributes;
     XSizeHints              theSizeHints;
     unsigned long           theWindowMask;
     XWMHints                theWMHints;


     if (use_toolkit_win)
	  return(1);

     av[0] = me;
     av[1] = (char *)NULL;

     if ((display = XOpenDisplay((char *)NULL)) == (Display *)NULL) {
	  fprintf(stderr,"%s: X11_init: can't connect to X server\n", me);
	  exit(1);
     }
     
     screen = DefaultScreen(display);
     winder = RootWindow(display, screen);
     colormap = DefaultColormap(display, screen);
     depth = vdevice.depth = DefaultDepth(display, screen);
     
     /*
      * Set our standard colors...
      */
     if (vdevice.depth == 1) {
	  /*
	   * Black and white - anything that's not black is white.
	   */
	  carray[0] = BlackPixel(display, screen);
	  for (i = 1; i < CMAPSIZE; i++)
	       carray[i] = WhitePixel(display, screen);
     } else {
	  /*
	   * Color, try to get our colors close to what's in the
	   * default colormap.
	   */
	  X11_mapcolor(0, 0, 0, 0);
	  X11_mapcolor(1, 255, 255, 255);

     }
     
     getprefposandsize(&prefx, &prefy, &prefxs, &prefys);

     /*
      * NEED TO USE XGRABPOINTER here???
      */
     XQueryPointer(display, winder, &rootw, &childw, &x, &y, &x, &y, &mask);
     
     if (childw == None)
	  childw = rootw;


     XGetGeometry(display, childw, &rootw, &x, &y, &w, &h, &bw, &depth);

     theWindowAttributes.backing_store = WhenMapped;
     theWindowAttributes.save_under = True;
     theWindowAttributes.border_pixel = carray[1];


     /*
      * See if there is something in the .Xdefaults file regarding
      * VOGL/VOGLE.
      */

     if ((smallf = XGetDefault(display, me, "smallfont")) == (char *)NULL)
	  smallf = SMALLFONT;

     if ((largef = XGetDefault(display, me, "largefont")) == (char *)NULL)
	  largef = LARGEFONT;

     geom = XGetDefault(display, me, "Geometry");

     if (geom != (char *)NULL) {
	  mask = XParseGeometry(geom, &x, &y, &w, &h);

	  if (mask & XValue)
	       theSizeHints.flags |= USPosition;

	  if (mask & YValue)
	       theSizeHints.flags |= USPosition;
	  
	  if (mask & WidthValue)
	       theSizeHints.flags |= USSize;
	  
	  if (mask & HeightValue)
	       theSizeHints.flags |= USSize;

	  if (mask & XNegative)
	       x = DisplayWidth(display, screen) - 2*bw - w + x;
	  
	  if (mask & YNegative)
	       y = DisplayHeight(display, screen) - 2*bw - h + y;
	  
     } else
	  theSizeHints.flags = PPosition | PSize;
     
     if (prefx > -1) {
	  x = prefx;
	  y = prefy;
     }
     
     if (prefxs > -1) {
	  w = prefxs;
	  h = prefys;
     }
     
     if (bw == 0)
	  bw = 4;
     
     x -= bw;
     y -= bw;
     
     if (x <= 0)
	  x = 0;
     
     if (y <= 0)
	  y = 0;
     
     w -= 4 * bw;
     h -= 4 * bw;
     
     theWindowMask = CWBorderPixel|CWBackingStore;
     
     winder = XCreateWindow(display,
			    winder,
			    x, y,
			    w, h,
			    bw,
			    (int)vdevice.depth,
			    InputOutput,
			    CopyFromParent,
			    theWindowMask,
			    &theWindowAttributes
			    );
     
     theSizeHints.x = x;
     theSizeHints.y = y;
     theSizeHints.width = w;
     theSizeHints.height = h;
     
     if (vdevice.wintitle)
	  strcpy(name, vdevice.wintitle);
     else
	  sprintf(name, "%s %d (win id 0x%x)", me, getpid(), winder);
     
     XSetStandardProperties(display,
			    winder,
			    name,
			    name,
			    None,
			    av,
			    1,
			    &theSizeHints
			    );
     
     theWMHints.initial_state = NormalState;
     theWMHints.input = True;
     theWMHints.flags = StateHint | InputHint;
     XSetWMHints(display, winder, &theWMHints);
     
     XSelectInput(display, winder, EV_MASK);
     
     theDrawable = (Drawable)winder;
     
     /*
      * Create Graphics Context and Drawable
      */
     theGC = XDefaultGC(display, screen);
     theGCvalues.graphics_exposures = False;
     theGCvalues.cap_style = CapButt;
     XChangeGC(display, theGC, GCGraphicsExposures|GCCapStyle, &theGCvalues);
     theDrawable = (Drawable)winder;
     X11_color(0);
     
     XMapRaised(display, winder);
     XFlush(display);
     
     /*
      * Wait for Exposure event.
      */
     do {
	  XNextEvent(display, &event);
     } while (event.type != Expose && event.type != MapNotify);
     

     /*
      *  Let VOGL/VOGLE know about the window size.
      *  (We may have been resized..... )
      */
     if (!XGetWindowAttributes(display, winder, &retWindowAttributes)) {
	  fprintf(stderr,"Can't get window attributes.");
	  exit(1);
     }
     
     x = retWindowAttributes.x;
     y = retWindowAttributes.y;
     w = retWindowAttributes.width;
     h = retWindowAttributes.height;
     
     XTranslateCoordinates(display,
			   winder, retWindowAttributes.root,
			   0, 0,
			   &x, &y,
			   &rootw
			   );
     
     
     vdevice.sizeX = vdevice.sizeY = MIN(h, w);
     vdevice.sizeSx = w;
     vdevice.sizeSy = h;
     
     if (back_used && (maxw < w || maxh < h)) {
	  back_used = 0;
	  X11_backbuf();
     }
     
     return(1);
}





/*
 * X11_exit
 *
 *	cleans up before returning the window to normal.
 */

void
X11_exit( void )
{
	if (back_used) 
		XFreePixmap(display, bbuf);

	if (font_id != (XFontStruct *)NULL)
		XFreeFont(display, font_id);

	font_id = (XFontStruct *)NULL;

	if (use_toolkit_win)
		return;

	XDestroyWindow(display, winder);

	XSync(display, 0);

	XCloseDisplay(display);

	return;
}





/*
 * X11_draw
 *
 *	draws a line from the current graphics position to (x, y).
 *
 * Note: (0, 0) is defined as the top left of the window in X (easy
 * to forget).
 */
void
X11_draw(register const int x, register const int y)
{
	XDrawLine(display,
		theDrawable,
		theGC,
		vdevice.cpVx, vdevice.sizeSy - vdevice.cpVy,
		x, vdevice.sizeSy - y
	);

	if (vdevice.sync)
		XSync(display, 0);
}





int
X11_pnt(int x, int y)
{
	XDrawPoint(display,
		theDrawable,
		theGC,
		x, vdevice.sizeSy - y
	);

	if (vdevice.sync)
		XSync(display, 0);

	return( 0 );
}





/*
 * X11_getkey
 *
 *	grab a character from the keyboard - blocks until one is there.
 */
int
X11_getkey( void )
{
	char	c;

	do {
		XNextEvent(display, &event);
		if (event.type == KeyPress) {
			if (XLookupString((XKeyEvent *)&event, &c, 1, NULL, NULL) > 0)
				return((int)c);
			else
				return(0);
		}
	} while (event.type != KeyPress);

	return( 0 );
}





/*
 * X11_checkkey
 *
 *	Check if there has been a keyboard key pressed.
 *	and return it if there is.
 */
int
X11_checkkey( void )
{
	char	c;

	if (!XCheckWindowEvent(display, winder, KeyPressMask, &event))
		return(0);

	if (event.type == KeyPress)
		if (XLookupString((XKeyEvent *)&event, &c, 1, NULL, NULL) > 0)
			return((int)c);

	return( 0 );
}





/*
 * X11_locator
 *
 *	return the window location of the cursor, plus which mouse button,
 * if any, is been pressed.
 */
int
X11_locator(int *wx, int *wy)
{
	Window		rootw, childw;
	int		x, y;
	unsigned int	mask;

	XQueryPointer(display, winder, &rootw, &childw, &x, &y, wx, wy, &mask);

	*wy = (int)vdevice.sizeSy - *wy;

	return(mask >> 8);
}





#ifdef VOGLE
/*
 * X11_clear
 *
 * Clear the screen (or current buffer )to current colour
 */
void
X11_clear( void )
{
	XSetBackground(display, theGC, colour);
	XFillRectangle(display,
		theDrawable,
		theGC,
		0,
		0,
		(unsigned int)vdevice.sizeSx + 2,
		(unsigned int)vdevice.sizeSy + 1
	);

	if (vdevice.sync)
		XFlush(display);

	return( 0 );
}
#else 
/*
 * X11_clear
 *
 * Clear the screen (or current buffer )to current colour
 */
void
X11_clear( void )
{
	unsigned int	w = vdevice.maxVx - vdevice.minVx;
	unsigned int	h = vdevice.maxVy - vdevice.minVy;

	XSetBackground(display, theGC, colour);

	XFillRectangle(display,
		theDrawable,
		theGC,
		vdevice.minVx + 1,
		vdevice.sizeSy - vdevice.maxVy , 
		w, 
		h
	);

	if (vdevice.sync)
		XFlush(display);

	return;
}
#endif






/*
 * X11_color
 *
 *	set the current drawing color index.
 */
void
X11_color(int ind)
{
	colour = carray[ind];
	XSetForeground(display, theGC, colour);
}





/*
 * X11_mapcolor
 *
 *	change index i in the color map to the appropriate r, g, b, value.
 */
void
X11_mapcolor(int i, int r, int g, int b)
{
	int	stat;
	XColor	tmp;

	if (i >= CMAPSIZE)
	     return;


	/*
	 * For Black and White.
	 * If the index is 0 and r,g,b != 0 then we are remapping black.
	 * If the index != 0 and r,g,b == 0 then we make it black.
	 */
	if (vdevice.depth == 1) {
		if (i == 0 && (r != 0 || g != 0 || b != 0)) 
			carray[i] = WhitePixel(display, screen);
		else if (i != 0 && r == 0 && g == 0 && b == 0)
			carray[i] = BlackPixel(display, screen);
	} else {
		tmp.red = (unsigned short)(r / 255.0 * 65535);
		tmp.green = (unsigned short)(g / 255.0 * 65535);
		tmp.blue = (unsigned short)(b / 255.0 * 65535);
		tmp.flags = 0;
		tmp.pixel = (unsigned long)i;

		if ((stat = XAllocColor(display, colormap, &tmp)) == 0) {
		     unsigned long foo[2];

		     foo[0] =  tmp.pixel;

		     fprintf(stderr, "XAllocColor failed for %d,%d,%d (status = %d)\n", r,g,b,stat);
		     XFreeColors(display, colormap, foo, 1, 0);
		     if ((stat = XAllocColor(display, colormap, &tmp)) == 0) {
			  fprintf(stderr, "Really Couldn't Allocate colors!\n");
		     }
		     /*exit(1);*/
		}
		carray[i] = tmp.pixel;
	}

	XFlush(display);
}





/*
 * X11_font
 *
 *   Set up a hardware font. Return 1 on success 0 otherwise.
 *
 */
int
X11_font( char *fontfile)
{
	XGCValues	xgcvals;
	char	*name = fontfile;

	if (font_id != (XFontStruct *)NULL)
		XFreeFont(display, font_id);

	if (strcmp(fontfile, "small") == 0) {
		if ((font_id = XLoadQueryFont(display, smallf)) == (XFontStruct *)NULL) {
			fprintf(stderr, "%s X11.c couldn't open small font '%s'\n", me, smallf);
			fprintf(stderr, "You'll have to redefine it....\n");
			return(0);
		} else
			name = smallf;
		
	} else if (strcmp(fontfile, "large") == 0) {
		if ((font_id = XLoadQueryFont(display, largef)) == (XFontStruct *)NULL) {
			fprintf(stderr, "%s X11.c couldn't open large font '%s'\n", me, largef);
			fprintf(stderr, "You'll have to redefine it....\n");
			return(0);
		}
			name = largef;
	} else {
		if ((font_id = XLoadQueryFont(display, fontfile)) == (XFontStruct *)NULL) {
			fprintf(stderr, "%s X11.c couldn't open fontfile '%s'\n", me, fontfile);
			return(0);
		}
	}

	vdevice.hheight = font_id->max_bounds.ascent + font_id->max_bounds.descent;
	vdevice.hwidth = font_id->max_bounds.width;

	xgcvals.font = XLoadFont(display, name);
	XChangeGC(display, theGC, GCFont, &xgcvals);

	return(1);
}





/* 
 * X11_char
 *
 *	 outputs one char - is more complicated for other devices
 */
void
X11_char(char c)
{
	char	*s = " ";

	s[0] = c;
	XDrawString(display, theDrawable, theGC, vdevice.cpVx, (int)(vdevice.sizeSy - vdevice.cpVy), s, 1);

	if (vdevice.sync)
		XFlush(display);

	return;
}





/*
 * X11_string
 *
 *	Display a string at the current drawing position.
 */
void
X11_string(char s[])
{
	XDrawString(display, theDrawable, theGC, vdevice.cpVx, (int)(vdevice.sizeSy - vdevice.cpVy), s, strlen(s));
	if (vdevice.sync)
		XFlush(display);

	return;
}





/*
 * X11_fill
 *
 *	fill a polygon
 */
void
X11_fill(register const int n, const int x[], const int y[])
{
	XPoint	plist[20];
	register int	i;

	/* no reason to go on, Gopher Team */
	if (0 == n )
		return;

	for (i = 0; i < n; i++) {
		plist[i].x = x[i];
		plist[i].y = vdevice.sizeSy - y[i];
	}

	XFillPolygon(display, theDrawable, theGC, plist, n, Convex, 
					CoordModeOrigin);

	/*
	 * Draw an outline around each fill area to remove holes
	 */
	XDrawLines( display, theDrawable, theGC, plist, n, CoordModeOrigin);

	vdevice.cpVx = x[n-1];
	vdevice.cpVy = y[n-1];

	if (vdevice.sync)
		XFlush(display);

}





/*
 * X11_backbuf
 *
 *	Set up double buffering by allocating the back buffer and
 *	setting drawing into it.
 */
int
X11_backbuf( void )
{
     CheckMultibuf();
     if (!back_used) {
	  if (HasMultibuf) {
	       CreateMultiBufs(winder);
	  } else {
	       bbuf = XCreatePixmap(display,
				    (Drawable)winder,
				    (unsigned int)vdevice.sizeSx + 1,
				    (unsigned int)vdevice.sizeSy + 1,
				    (unsigned int)vdevice.depth
				    );
	       
	       maxw = MAX(vdevice.sizeSx + 1, maxw);
	       maxh = MAX(vdevice.sizeSy + 1, maxh);
	  }
     }
     back_used = 1;
     if (!HasMultibuf)
	  theDrawable = (Drawable)bbuf;
     
     return(1);
}





/*
 * X11_swapbuf
 *
 *	Swap the back and from buffers. (Really, just copy the
 *	back buffer to the screen).
 */
int
X11_swapbuf( void )
{
     if (HasMultibuf) {
	  SwapMultiBuf();
     } else {
	  XCopyArea(display,
		    theDrawable,
		    winder,
		    theGC,
		    0, 0,
		    (unsigned int)vdevice.sizeSx + 1,
		    (unsigned int)vdevice.sizeSy + 1,
		    0, 0
		    );
     }
     XSync(display, 0);
     return( 0 );
}





/*
 * X11_frontbuf
 *
 *	Make sure we draw to the screen.
 */
void
X11_frontbuf( void )
{
	theDrawable = (Drawable)winder;

}





/*
 * Syncronise the display with what we think has been sent to it...
 */
void
X11_sync( void )
{
	XSync(display, 0);

}





#define VORTDUMP
#undef VORTDUMP
#ifdef VORTDUMP
/*
 * HACK
 * Dump the contents of the current buffer to a VORT file....
 * ONLY WORKS WITH 8Bit Drawables!
 */
#include "vort.h"

int
X11_dump_pixmap( char *filename, int dx, int dy, int dw, int dh)
{
	XImage	*ximage;
	image	*im;
	unsigned char *line;
	static unsigned char	*rm, *gm, *bm;
	static XColor	*cols;
	int	i;

	if (dw > vdevice.sizeSx || dw < 0)
		dw = vdevice.sizeSx;
	if (dh > vdevice.sizeSy || dh < 0)
		dh = vdevice.sizeSy;

	if (dx > vdevice.sizeSx || dx < 0)
		dx = 0;
	if (dy > vdevice.sizeSy || dy < 0)
		dy = 0;

	ximage = XGetImage(display, 
			theDrawable, 
			dx, dy,
			(unsigned int)dw,
			(unsigned int)dh,
			AllPlanes,
			ZPixmap
		);

	if (!ximage) {
		fprintf(stderr, "X11_dump_pixmap: can't do XGetImage\n");
		exit(1);
	}

	if ((im = openimage(filename, "w")) == (image *)NULL) {
		fprintf(stderr, "X11_dump_pixmap: can't open %s\n", filename);
		exit(1);
	}

	if (!rm && !(rm = (unsigned char *)malloc(256))) {
		fprintf(stderr, "X11_dump_pixmap: can't alloc rm\n");
		exit(1);
	}
	if (!gm && !(gm = (unsigned char *)malloc(256))) {
		fprintf(stderr, "X11_dump_pixmap: can't alloc gm\n");
		exit(1);
	}
	if (!bm && !(bm = (unsigned char *)malloc(256))) {
		fprintf(stderr, "X11_dump_pixmap: can't alloc bm\n");
		exit(1);
	}
	if (!cols && !(cols = (XColor *)malloc(256 * sizeof(XColor)))) {
		fprintf(stderr, "X11_dump_pixmap: can't alloc cols\n");
		exit(1);
	}

	/*
	 * Get our colormap...
	 */
	for (i = 0; i < 256; i++) {
		cols[i].pixel = (unsigned long)i;
		cols[i].red = cols[i].green = cols[i].blue = 0;
		cols[i].flags = DoRed | DoGreen | DoBlue;
	}

	XQueryColors(display, colormap, cols, 256);

	for (i = 0; i < 256; i++) {
		rm[i] = (unsigned char)(cols[i].red >> 8);
		gm[i] = (unsigned char)(cols[i].green >> 8);
		bm[i] = (unsigned char)(cols[i].blue >> 8);
	}

	imagetype(im) = PIX_RLECMAP;
	imageheight(im) = dh;
	imagewidth(im) = dw;
	imagedate(im) = time(0);
	titlelength(im) = 0;
	setcmap(im, 256, rm, gm, bm);

	writeheader(im);

	line = (unsigned char *)ximage->data;
	for (i = 0; i < dh; i++) {
		writemappedline(im, line);
		line += ximage->bytes_per_line;
	}
	
	closeimage(im); 

/*	free(rm);
	free(gm);
	free(bm);
	free(cols);*/

	return( 0 );
}
#endif






#ifndef VOGLE
/*
 * X11_setlw
 *
 *	Set the line width....
 */
void
X11_setlw(short w)
{
	XGCValues vals;

	vals.line_width = w;
	XChangeGC(display, theGC, GCLineWidth, &vals);
}


/*
 * X11_setls
 *
 *	Set the line style....
 */
void
X11_setls(Linestyle lss)
{
	unsigned ls = lss;
	char	dashes[16];
	int	i, n, a, b, offset;

	if (ls == 0xffff) {
		XSetLineAttributes(display, theGC, vdevice.attr->a.lw, LineSolid, CapButt, JoinMiter);
		return;
	}

	for (i = 0; i < 16; i++)
		dashes[i] = 0;

	for (i = 0; i < 16; i++)	/* Over 16 bits */
		if ((ls & (1 << i)))
			break;

	offset = i;

#define	ON	1
#define	OFF	0
		
	a = b = OFF;
	if (ls & (1 << 0))
		a = b = ON;

	n = 0;
	for (i = 0; i < 16; i++) {	/* Over 16 bits */
		if (ls & (1 << i))
			a = ON;
		else
			a = OFF;

		if (a != b) {
			b = a;
			n++;
		}
		dashes[n]++;
	}
	n++;

	XSetLineAttributes(display, theGC, vdevice.attr->a.lw, LineOnOffDash, CapButt, JoinMiter);
	XSetDashes(display, theGC, offset, dashes, n);

}
#endif 



/*
 * this came from the comp.graphics.algorithms FAQ
 */
int
X11_PointInPolygon( int x, int y, int npol, int xp [], int yp [] )
{
     int i, j, c = 0;
     
     for (i = 0, j = npol-1; i < npol; j = i++) {
	  if ((((yp[i]<=y) && (y<yp[j])) || ((yp[j]<=y) && (y<yp[i]))) &&
	      (x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
	       c = !c;
     }
     return c;
}






/*
 * the device entry
 */
static DevEntry X11dev = {
	"X11",
	"large",
	"small",
	X11_backbuf,
	X11_char,
	X11_checkkey,
	X11_clear,
	X11_color,
	X11_draw,
	X11_exit,
	X11_fill,
	X11_font,
	X11_frontbuf,
	X11_getkey,
	X11_init,
	X11_locator,
	X11_mapcolor,
#ifndef VOGLE
	X11_setls,
	X11_setlw,
#endif
	X11_string,
	X11_swapbuf,
	X11_sync,
	X11_PointInPolygon
};






/*
 * _X11_devcpy
 *
 *	copy the X11 device into vdevice.dev.
 */
int
_X11_devcpy( void )
{
	vdevice.dev = X11dev;

	return( 0 );
}
