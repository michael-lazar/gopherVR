#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <sys/time.h>

/* this should really be a preference setting, based on magnitude. */
#define FPS_MAGNITUDE 30000

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>	/* For keycodes (XK_Shift_L) */
#include <X11/X.h>		/* For XEvent 'types' (KeyPress etc.) */
#include <Xm/CutPaste.h>	/* For Clipboard routines */
#include <Xm/Protocols.h>
/* paranoia for old X11s */
#ifndef PropertyNotify
#define PropertyNotify          28
#endif
#ifndef ConfigureNotify
#define ConfigureNotify		22
#endif

#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/Command.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/Screen.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/MessageB.h>

#include "icons/forward.h"
#include "icons/rotleft.h"
#include "icons/rotright.h"

#include "vodevice.h"
#include "globals.h"
#include "boolean.h"
#include "menus.h"
#include "motiftools.h"
#include "guidefs.h"

#include "vogltools.h"

extern int ClickToChoice(short x, short y);
extern char *ChoiceToString(int choice, int which);

/*************************
 * Globals
 */

Display		*dpy;
Window		win;
GC		gc;

int no_fps = 0;

Widget canvas, top, Wig_top2, main_w;
XtAppContext  app;
int canvas_w = 576;
int canvas_h = 324;
int menuwin_w = 576;
int menuwin_h = 460;
int menuwin_x = 0;
int menuwin_y = 440;

extern int usec_render;

int debounce = 0;

struct timeval dubblbubbl;
int clickmaybe = 0;
int	vinited = 0;

void		drawscene();
void ResetFps();
void    jumpto(int x, int y);
void    Afoo()
{
     ;
}
void    mousemove_cb();
void    mouserotate_cb();
void    mouseselect();

/************* Some CallBacks... *****************/

void
deathofgopher(Widget  widget, XtPointer   client_data, XtPointer cbs)
{

/* We get a chance to save our window positions for later. Cameron */

	Window root, child;
	XWindowAttributes wnattr;
	int rootx, rooty, winx, winy, mask;
	int x, y, w, h;
	char fnam[255];
	FILE *fpout;

	/* The obvious thing (XGetGeometry(), XGetWindowAttributes()) won't work
   		right on most rootless environments, so we're sneaky and use
   		XQueryPointer(). */

	if (top == NULL || !getenv("HOME"))
		return; /* don't even try, something is really wrong */

	if(XQueryPointer(XtDisplay(top), XtWindow(top), &root, &child,
			&rootx, &rooty, &winx, &winy, &mask) == True) {
		x = rootx - winx;
		y = rooty - winy;

/* I can't seem to account for the menu bar in X11.app. wtf? Cameron */
/* gophwin has the same problem */
#if defined(__APPLE__)
#warning using menubar kludge for X11.app
		y -= 22;
#endif
 
		w = 0;
		h = 0;

		if(XGetWindowAttributes(XtDisplay(canvas), XtWindow(canvas),
				&wnattr) != 0) {
			w = wnattr.width;
			h = wnattr.height;
		} else {
			return; /* forget it */
		}
	} else {
		return; /* forget it */
	}

	V_RememberListWinSize(); /* make sure our menuwin_* is up to date */

	if(strlen(getenv("HOME")) > 128) /* OMG. */
		return;
	sprintf(fnam, "%s/.gophervrwindows", getenv("HOME"));
	if (fpout = fopen(fnam, "w")) {
		fprintf(fpout, "%i %i %i %i %i %i %i %i\n",
			x, y, w, h,
			menuwin_x, menuwin_y, menuwin_w, menuwin_h);
		fclose(fpout);
	} else {
		fprintf(stderr, "Warning: could not write %s\n", fnam);
	}
}

resize()
{
     Dimension       w, h;
     Arg             arg[2];
     float           tmp;

     XtSetArg(arg[0], XtNwidth, &w);
     XtSetArg(arg[1], XtNheight, &h);
     XtGetValues(canvas, arg, 2);
     
     tmp = w;
     tmp = (float) (tmp/(float)h);
     SCENEaspect = tmp;

     vo_xt_win_size((int)w, (int)h);
     reshapeviewport();
     drawscene(); /* just in case */
}

void
repaint()
{
     drawscene();
}


/*
 * Callback to change EYEangle
 *
 * pass it client_data of "l" to rotate left,
                          "r" to rotate right.
 */

void 
Rotate_CB(widget, client_data, call_data)
  Widget    widget;
  XtPointer client_data;
  XtPointer call_data;
{
     register float incr;
     int keepgoing;
     XEvent tempevent;

     XmPushButtonCallbackStruct *cbs = 
	  (XmPushButtonCallbackStruct *) call_data;
     XEvent *e = cbs->event;
     keepgoing = 1;

     if (cbs->reason == XmCR_ARM) {
       while(keepgoing) {
	  incr = 15.0;
          //if (e->type == KeyPress)
               incr *= (usec_render / (float)FPS_MAGNITUDE / (float)4);
	  if (*(char*)client_data == 'r')
               incr = -incr;

	  EYEangle += incr;
	  drawscene();
	  if (XtAppPending(app)) {
	       XPeekEvent(dpy, &tempevent);
	       if (tempevent.type == PropertyNotify ||
			tempevent.type == ConfigureNotify)
		    XNextEvent(dpy, &tempevent); /* Flush the event */
	       else
	            keepgoing = 0;
	  }
       }
     }
     /** Ignore everything else **/
     else if (cbs->reason == XmCR_DISARM)
	  ;
     else
	  ;

}

/*
 * Callback to move forward and back
 *
 * pass it client_data of "f" to move forward
 *                        "b" to move backward
 */

void 
Move_CB(widget, client_data, call_data)
  Widget    widget;
  XtPointer client_data;
  XtPointer call_data;
{
     int keepgoing;
     XEvent tempevent;
     XmPushButtonCallbackStruct *cbs = 
        (XmPushButtonCallbackStruct *) call_data;

     keepgoing = 1;
     if (cbs->reason == XmCR_ARM) {
	  while (keepgoing) {
/*
	       if (*(char*)client_data == 'f') {
		    EYExval += 100.0 * cos(EYEangle) * (usec_render / (float)FPS_MAGNITUDE / (float)4);
		    EYEzval += 100.0 * sin(EYEangle) * (usec_render / (float)FPS_MAGNITUDE / (float)4);
	       }
	       else {
		    EYExval -= 100.0 * cos(EYEangle) * (usec_render / (float)FPS_MAGNITUDE / (float)4);
		    EYEzval -= 100.0 * sin(EYEangle) * (usec_render / (float)FPS_MAGNITUDE / (float)4);
	       }
*/
	       EYEzval = 50.0 * (usec_render / (float)FPS_MAGNITUDE );

	       drawscene();
	       if (XtAppPending(app)) {
	            XPeekEvent(dpy, &tempevent);
	            if (tempevent.type == PropertyNotify ||
			tempevent.type == ConfigureNotify)
		        XNextEvent(dpy, &tempevent); /* Flush the event */
	            else
	                keepgoing = 0;
	       }
	  }
     }
     /** Ignore everything else **/
     else if (cbs->reason == XmCR_DISARM)
	  ;
     else
	  ;

}



static void keyboardevent(Widget, XEvent *, String *, int*);
/***/

XtActionsRec actions[] = {
	{"repaint", 	(XtActionProc)repaint},
	{"resize", 	(XtActionProc)resize},
        {"mousemove",   (XtActionProc)mousemove_cb},
	{"mouserotate", (XtActionProc)mouserotate_cb},
        {"keyboard",    (XtActionProc)keyboardevent},
	{"mouseselect", (XtActionProc)mouseselect}
};

/* mouseselect() is officially legacy as of 0.5 */

/* For some reason the Motif double-click doesn't work right with this
   system as we've defined it, so we manually debounce double-click 1
   in mouseselect(). -- Cameron */

String trans =
	"<Expose>:	   repaint()\n \
	 <Configure>:	   resize()\n \
         <Btn3Down>:       mousemove()\n \
         <Btn3Up>:         mousemove()\n \
         <Btn2Down>:       mouseselect()\n \
         <Btn2Up>:         mouseselect()\n \
         <Btn1Down>:       mousemove()\n \
         <Btn1Up>:         mousemove()\n \
         <KeyDown>:          keyboard()\n \
         <KeyUp>:            keyboard()\n";

/* I've decided to deprecate mouserotate(); we don't really need the
   same code in two different places. Right now, it exists in limbo,
   and may be removed when I feel motivated. -- Cameron */

         /* Shift<Btn1Down>:    mouserotate()\n \ */


/*
 * Note that SGI's version of motif complains if you have whitespace here..
 */

String fallback_resources[] = {
     "*textDisplay*fontList: *-courier-bold-r-normal-*-12-*-*-*-*-*-*-1",
     "*scroll_list*fontList: *-courier-bold-r-normal-*-12-*-*-*-*-*-*-1",
     "*fontList: *-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-1,*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-1=Title,*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-1=Body,*-courier-bold-r-normal-*-12-*-*-*-*-*-*-1=Text",
     NULL
};

#ifndef __sgi
#include <X11/extensions/multibuf.h>
#endif

main(argc, argv)
  int argc;
  char *argv[];
{
     int major, minor;
     Widget        menubar, menu;
     Widget        rc, pb, form, label;
     Pixel	  fg, bg;
     Pixmap        Pix_rotleft, Pix_forward, Pix_rotright;
     static Colormap    my_colormap, def_colormap;
     XColor      current_color;
     int         ncolors, i;
     XColor      *Colors;
     Atom	 WM_DELETE_WINDOW;

     extern void   CB_Quit();
     Arg           args[5];
     int           n = 0;
     char          *host;
     int           port;
     
     
     /* init locale */
     setlocale( LC_ALL, "" );
     
     /*    XtSetLanguageProc (NULL, NULL, NULL);*/
     
     /* initialize toolkit and create toplevel shell */
     
     top = XtVaAppInitialize (&app, "GopherVR",
			      NULL, 0, &argc, argv, fallback_resources,
			      NULL);

     XtVaSetValues(top,
		   XtNtitle, "Gopher VR",
		   NULL);

     if(getenv("HOME") &&
	strlen(getenv("HOME")) < 127) {
		FILE *fpos;
		char fnam[255];
		int x, y;
		
		sprintf(fnam, "%s/.gophervrwindows", getenv("HOME"));
		if(fpos = fopen(fnam, "r")) {
			fscanf(fpos, "%i %i %i %i %i %i %i %i",
				&x, &y, &canvas_w, &canvas_h,
				&menuwin_x, &menuwin_y, &menuwin_w,
					&menuwin_h);
			fclose(fpos);
			XtVaSetValues(top,
				XtNx, x, XtNy, y, NULL);
		}
     }

     dpy = XtDisplay(top);

     if (argc < 2) {
	  INITIALhost = "gopher.floodgap.com";
	  INITIALport = 70;
     } else {
	  INITIALhost = argv[1];
	  INITIALport = atoi(argv[2]);
     }
     
     /***********************************************************************
      * Initialize a private colormap...
      */

     ncolors = DisplayCells(dpy, DefaultScreen(dpy));
     Colors = malloc(sizeof(XColor) * (ncolors + 1));

     def_colormap = DefaultColormap(dpy, DefaultScreen(dpy));

     /** Copy the first 30 colors... **/

     for( i = 0; i < 32; i++ ) {
	  Colors[i].pixel = i;
	  Colors[i].flags = DoRed|DoGreen|DoBlue;
     }
     XQueryColors(dpy, def_colormap, Colors, 32);

     my_colormap = 
	  XCreateColormap(dpy, DefaultRootWindow(dpy), 
			  DefaultVisual(dpy, DefaultScreen(dpy)),
			  AllocNone);

     /** Copy the current colormap to our colormap **/
     for (i=0; i < 32; i++) {
	  XAllocColor(dpy, my_colormap, &Colors[i]);
     }

/*     XStoreColors(dpy, my_colormap, Colors, 32);*/

     free(Colors);

     XtVaSetValues(top, 
		   XmNwidthInc, 8,
		   XmNheightInc, 8,
		   XtNcolormap, my_colormap,
		   NULL);


     
     /***********************************************************************
      * MainWindow for the application -- contains menubar, ScrolledText
      * and CommandArea.
      */
     
     main_w = XtVaCreateManagedWidget ("main_w",
			  xmMainWindowWidgetClass, top,
			  XmNcommandWindowLocation, XmCOMMAND_BELOW_WORKSPACE,
			  XmNshowSeparator, True,
		          XmNcolormap, my_colormap,
			  NULL);

     menubar = Create_Menubar(main_w);
     
     form = XtVaCreateManagedWidget("form", xmFormWidgetClass, main_w, NULL);
     
     
     /* Create a bunch of buttons */
     rc = XtVaCreateWidget ("rc", xmRowColumnWidgetClass, form,
			    XmNorientation,     XmHORIZONTAL,		   
			    XmNnumColumns,      1,
			    XmNpacking,         XmPACK_COLUMN,
			    XmNleftAttachment,  XmATTACH_FORM,
			    XmNbottomAttachment,XmATTACH_FORM,
			    NULL);
     
     XtVaGetValues( main_w, XmNforeground, &fg, XmNbackground, &bg, NULL );
     
     
     Pix_rotleft = XCreatePixmapFromBitmapData( XtDisplay(rc), 
		    RootWindowOfScreen(XtScreen(rc)), rotleft_bits,
		    rotleft_width, rotleft_height, fg, bg, 
		    DefaultDepthOfScreen(XtScreen(main_w)));


     pb = XtVaCreateManagedWidget( "rotleft", xmPushButtonWidgetClass, rc,
				  XmNlabelType, XmPIXMAP, 
				  XmNlabelPixmap, Pix_rotleft, 
				  NULL );
     XtAddCallback(pb, XmNarmCallback, Rotate_CB, "l");
     XtAddCallback(pb, XmNdisarmCallback, Rotate_CB, NULL);
     XtManageChild(pb);

     

     Pix_forward = XCreatePixmapFromBitmapData( XtDisplay(rc),  
		       RootWindowOfScreen(XtScreen(rc)), forward_bits, 
		       forward_width, forward_height, fg, bg, 
		       DefaultDepthOfScreen(XtScreen(main_w)) );

     pb = XtVaCreateManagedWidget( "forward", xmPushButtonWidgetClass, rc,
				 XmNlabelType, XmPIXMAP, 
				 XmNlabelPixmap, Pix_forward, NULL );

     XtAddCallback(pb, XmNarmCallback, Move_CB, "f");
     XtAddCallback(pb, XmNdisarmCallback, Move_CB, NULL);
     XtManageChild(pb);

     
     
     Pix_rotright = XCreatePixmapFromBitmapData( XtDisplay(rc),  
		    RootWindowOfScreen(XtScreen(rc)), rotright_bits, 
                    rotright_width, rotright_height, fg, bg,
                    DefaultDepthOfScreen(XtScreen(main_w)) );

     pb = XtVaCreateManagedWidget( "rotright", xmPushButtonWidgetClass, rc,
				 XmNlabelType, XmPIXMAP,
				 XmNlabelPixmap, Pix_rotright, NULL );

     XtAddCallback(pb, XmNarmCallback, Rotate_CB, "r");
     XtAddCallback(pb, XmNdisarmCallback, Rotate_CB, NULL);
     XtManageChild(pb);


     XtManageChild(rc);
    
     BusyThingNew(rc);
     StatusNew(form, "");
     StatusUpdate("0001fps - GopherVR - University of Minnesota");


     /* Create canvas */
     XtAppAddActions(app, actions, XtNumber(actions));
     
     /* Hook delete wm events so we can save our dimensions. -- Cameron */
     WM_DELETE_WINDOW = XmInternAtom(XtDisplay(top), "WM_DELETE_WINDOW",
		False);
     XmRemoveWMProtocolCallback(top, WM_DELETE_WINDOW, deathofgopher,
		NULL);
     XmAddWMProtocolCallback(top, WM_DELETE_WINDOW, deathofgopher, NULL);

     /* See if we have a ~/.gophervrwindows to get window positions from. */

     /** 576, 324 are good values... **/

     canvas = XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass, 
		     form,
		     XmNtranslations, XtParseTranslationTable(trans),
		     XmNwidth, canvas_w,
		     XmNheight, canvas_h,
                     XmNresizePolicy,        XmNONE,
		     XmNtopAttachment,       XmATTACH_FORM,
		     XmNbottomAttachment,    XmATTACH_WIDGET,
		     XmNrightAttachment,     XmATTACH_FORM,
                     XmNleftAttachment,      XmATTACH_FORM,
                     XmNbottomWidget,        rc,
		     NULL);

     XtAddCallback(canvas, XmNexposeCallback, repaint, NULL);
     XtAddCallback(canvas, XmNinputCallback, drawscene, NULL);
     XtManageChild(canvas);
     
     
     /* Add widgets to the MainWindow */
     
     XmMainWindowSetAreas (main_w, menubar, NULL,
			   NULL, NULL, form);
     
     XtRealizeWidget(top);     
     XSetWindowColormap(dpy, XtWindow(top), my_colormap);
     
     V_GenMenuListWin();
     
     
     win = XtWindow(canvas);
     
     vo_xt_window(dpy, win, canvas_w, canvas_h, my_colormap);
     
     vinited = 1;
     
     drawscene();
     BusyUpdate();
     XtAppMainLoop (app);
}




void
GravityBounce(float v0, float zincr)
{
     float y, t, y0, oldy, g, deltat, azim, oldazim;
     
     y0 = 0.0;
     v0 = 8.0;
     y  = 0.0;
     deltat = (1.0/8.0);
     t  = deltat;
     g  = 9.8;
     oldazim = 0.0;
     
     
     /** Simulate a gravitational force **/
     /** y(t) = -.5 gt^2 + v0t + y0 **/
     do {
	  oldy = y;
	  oldazim = azim;
	  y = -.5 *  g * t * t + v0 * t + y0;
	  if (y <0.0) {
	       y = 0.0;  /** Hit the ground... **/
	       
	  }
	  
	  EYEyval = 600 * ( oldy - y);
	  EYEzval = zincr;
	  /*	       EyeRotateX(-azim);*/
	  /*	       EyeTranslate(0.0, 1000.0 * (oldy - y), 0.0);*/
	  
	  /*	       azim = 30 * atan(y); 
		       printf ("Azimuth = %f, %f\n", azim, azim - oldazim);
		       EyeRotateX(oldazim - azim);*/
	  drawscene();
	  t += deltat;
     }  while (y > 0.0);
     
}




void
keyboardevent(Widget widget, XEvent *event, String *args, int *num_args)
{
     float xval, yval;
     XEvent tempevent;
     XKeyEvent *thekey;
     char buf[5];
     KeySym keysym;
     boolean Justonce = False;
     boolean keepgoing = 1;

     if (event == NULL)
	 return;
     
     thekey = (XKeyEvent *) event;

     do {
	  XLookupString(thekey, buf, sizeof(buf), &keysym, NULL);

	  switch (keysym) {

	  case XK_bracketleft:
	       EyeLocationUp(1);
	       Justonce = True;
	       break;
	  case XK_bracketright:
	       EyeLocationUp(0);
	       Justonce = True;
	       break;
	       
	  case XK_space:
	       EyeAirJordan(drawscene, 7.0, 0.0, 1400.0, 24.0);
	       Justonce = 1;
	       break;

/*
not supported
m saves moves
s ... ??

	  case 'm':
	       SaveMovie = 1- SaveMovie;
	       break;

	  case 's':
	       break;
	       
these don't debounce properly and frankly I might not support them anyway.

	  case XK_Right:
	  case XK_R12:
	       EYEangle -= 20.0 * (usec_render / (float)FPS_MAGNITUDE);
	       break;
	       
	  case XK_Left:
	  case XK_R10:
	       EYEangle += 20.0 * (usec_render / (float)FPS_MAGNITUDE);
	       break;
	       
	  case XK_Up:
	  case XK_R8:
	       EYEzval = 75.0 * (usec_render / (float)FPS_MAGNITUDE);
	       break;
	       
	  case XK_Down:
	  case XK_R14:
	       EYEzval = -75.0 * (usec_render / (float)FPS_MAGNITUDE);
	       break;
	       
*/
	  }

	  drawscene();
     
	  if (XtAppPending(app)) {
	       XPeekEvent(dpy, &tempevent);
	       if (tempevent.type == KeyRelease) {
		    XNextEvent(dpy, &tempevent); /* Flush the event */
	       }
	       keepgoing = 0;
	  }
	  if (thekey->type != KeyRelease) {
	       keepgoing = 0;
	  }


     } while (keepgoing);
	  
}



static boolean
ShiftKeyDown(XEvent *event)
{
     XKeyEvent *thekey;
     KeySym keysym;
     char buf[5];

     if (event == NULL)
	 return(False);

     if (event->type != KeyPress && event->type != KeyRelease) 
	  return(False);

     thekey = (XKeyEvent *) event;

     XLookupString(thekey, buf, sizeof(buf), &keysym, NULL);

     if (keysym == XK_Shift_L || keysym == XK_Shift_R)
	  return(TRUE);

     return(FALSE);
     
}

static boolean
ControlKeyDown(event)
  XEvent *event;
{
     XKeyEvent *thekey;
     KeySym keysym;
     char buf[5];

     if (event == NULL)
	 return(False);

     if (event->type != KeyPress && event->type != KeyRelease) 
	  return(False);

     thekey = (XKeyEvent *) event;

     XLookupString(thekey, buf, sizeof(buf), &keysym, NULL);

     if (keysym == XK_Control_L || keysym == XK_Control_R)
	  return(TRUE);

     return(FALSE);
}


static boolean
MetaKeyDown(event)
  XEvent *event;
{
     XKeyEvent *thekey;
     KeySym keysym;
     char buf[5];

     if (event == NULL)
	 return(False);

     if (event->type != KeyPress && event->type != KeyRelease) 
	  return(False);

     thekey = (XKeyEvent *) event;

     XLookupString(thekey, buf, sizeof(buf), &keysym, NULL);

     if (keysym == XK_Meta_L || keysym == XK_Meta_R ||
	 keysym == XK_Alt_L || keysym == XK_Alt_R)
	  return(TRUE);

     return(FALSE);
}

/* This is the routine where we move with the mouse button. */
/* This is also the routine where in 0.04 we detect we have NOT moved
	with the mouse button, and try to promote double clicks of
	that sort into middle button presses. */

void
mousemove_cb(widget, event, args, num_args)
  Widget widget;
  XEvent *event;
  String *args;
  int    *num_args;
{
     XButtonEvent *bevent;
     XWindowAttributes wnattr;
     XEvent       tempevent;
     short        x, y;
     float        rotatefactor, movefactor;
     short        startx, starty;
     int          wwidth, wheight;
     unsigned int button;
     Arg          arg[2];
     boolean      Shiftdown = FALSE;
     boolean      Controldown = FALSE;
     float usec_factor;

     bevent = (XButtonEvent *) event;
     button = bevent->button;
     
     if (bevent->type == ButtonRelease)
	  return;

     	startx = getvaluator(MOUSEX);
     	starty = getvaluator(MOUSEY);

/* Because our window size may have changed, we ask for it. Obviously
   it can't change as long as the user is dragging, so we only need to get
   it once. If this becomes a drag, we'll put it in resize(). HA HA FUNNY!@ */

     if(XGetWindowAttributes(bevent->display, bevent->window, &wnattr) != 0) {
	wwidth = wnattr.width;
	wheight = wnattr.height;
     } else {
	fprintf(stderr, "having trouble with windows?\n");
	wwidth = (int)startx;
	wheight = (int)starty;
     }

     do {
	  do {
	       /** Track the mouse... **/
	       x = (int)getvaluator(MOUSEX);
	       y = (int)getvaluator(MOUSEY);
/* this should really be a preference, based on magnitude */
               usec_factor = (float)usec_render / FPS_MAGNITUDE;
               if (usec_factor == 0.0) usec_factor = 1.0;
	       
	       if (!Shiftdown) {
		; /* previously rotatefactor went here */
	       }
	       
	       if (!Controldown) {
		    ;
	       }
		    rotatefactor =  ((float)((int)startx - x) / (float) wwidth)
			* usec_factor;
		    EYEangle += 20.0 * rotatefactor;
	       movefactor   = ((float)(y - (int)starty) / (float) wheight )
			* usec_factor;
	       EYEzval  = movefactor * 200.0;

		no_fps = 0;
	       drawscene();
	  } while (!XtAppPending(app));
	  ;
	  XPeekEvent(dpy, &tempevent);
#if(0)
/* This doesn't really work and I don't like it. -- Cameron */
	  /** Check to see if it's the shift key.. **/
	  if (ShiftKeyDown(&tempevent) == True) {
	       Shiftdown = 1-Shiftdown;
	  } else if (ControlKeyDown(&tempevent) == True) {
	       Controldown = 1-Controldown;
	  } else {
	       break;  /** Not an event we can handle ... **/
	 } 
#else
        /* Dequeue certain irrelevant events some wms send us. */

        if (tempevent.type == MotionNotify ||
	    tempevent.type == ConfigureNotify ||
            tempevent.type == PropertyNotify || /* wtf, XQuartz */
        /* if this is a button down and it's the same buttons, loop */
        /* button release will correctly fall through */
          ((tempevent.type == ButtonPress &&
            tempevent.xbutton.button == button)))
	    XNextEvent(dpy, &tempevent); /* dequeue it */
	else
	    break;
#endif
     } while (1); 

     if (startx == x && starty == y) {
	/* we didn't move. */
	if (clickmaybe) { /* maybe a double click ... */
		struct timeval secondclick;

		(void)gettimeofday(&secondclick, NULL);
		clickmaybe = 0;
		if (dubblbubbl.tv_sec == secondclick.tv_sec ||
			(dubblbubbl.tv_usec > secondclick.tv_usec &&
			 dubblbubbl.tv_sec+1 == secondclick.tv_sec)) {
			if (button == Button1)
/* If double click is button 1, select/jump */
				jumpto(x, y);
			else {
/* If double click is other than button 1, pop up a window */
				XmString str;
				static Widget InfoDialog = NULL;
				Arg args[1];
				char statusline[1024];
				int choice = ClickToChoice(x, y);
				if (choice == -1) return;

				if (strlen(ChoiceToString(choice, 0))>511 ||
				    strlen(ChoiceToString(choice, 1))>511)
					/* yikes */
					return;

				sprintf(statusline, "%s\n%s",
					ChoiceToString(choice, 0),
					ChoiceToString(choice, 1));
				str =
XmStringCreateLtoR(statusline, "Body");
				XtSetArg(args[0], XmNmessageString, str);
				InfoDialog = XmCreateErrorDialog(top, 
					ChoiceToString(choice, 1), args, 1);
				XmStringFree(str);
				XtSetSensitive(XtNameToWidget(InfoDialog, "Help"), False);
				XtManageChild(InfoDialog);
				XtPopup(XtParent(InfoDialog), XtGrabNone);
			}
			return;
		}
	}
	(void)gettimeofday(&dubblbubbl, NULL);
	clickmaybe = button;
	if (button != Button1) {
		/* Get the object */
		char statusline[256];
		int choice = ClickToChoice(x, y);

		no_fps = 1;
		if (choice == -1) return;
		sprintf(statusline, "%s",
			ChoiceToString(choice, 1));
		StatusUpdate(statusline);
		return;
	}
     } else {
	no_fps = 0;
	clickmaybe = 0;
     }
     TimeoutCursors(0);
}


/* We don't use this right now. */
void
mouserotate_cb(widget, event, args, num_args)
  Widget widget;
  XEvent *event;
  String *args;
  int    *num_args;
{
     XButtonEvent *bevent;
     short        x, y;
     float        rotatefactor, movefactor, usec_factor;
     Dimension    w, h;
     Arg          arg[2];
     boolean      Shiftdown = FALSE;
     boolean      Controldown = FALSE;

     bevent = (XButtonEvent *) event;

     if (bevent->type == ButtonRelease)
	  return;

     XtSetArg(arg[0], XtNwidth, &w);
     XtSetArg(arg[1], XtNheight, &h);
     XtGetValues(widget, arg, 2);
     w /= 2;
     h /= 2;
     
     do {
	  /** Track the mouse... **/
	  x = (int)getvaluator(MOUSEX);
	  y = (int)getvaluator(MOUSEY);
/* this should really be a preference, based on magnitude */
               usec_factor = (float)usec_render / FPS_MAGNITUDE;
               if (usec_factor == 0.0) usec_factor = 1.0;
	  
	  rotatefactor =  ((float)((int)w - x) / (float) w) * usec_factor;
	  RotateAboutAxis(4.0 * rotatefactor);
	  drawscene();
     } while (!XtAppPending(app)); /* XXX: Fix for PropertyNotify */

}




void
mouseselect(widget, event, args, num_args)
  Widget widget;
  XEvent *event;
  String *args;
  int    *num_args;
{
     XButtonEvent *bevent;
     int        x, y;
     float        rotatefactor, movefactor;
     Dimension    w, h;
     Arg          arg[2];
     boolean      Shiftdown = FALSE;
     int moo;

     bevent = (XButtonEvent *) event;
     if (bevent->type == ButtonRelease)
	  return;
     XtSetArg(arg[0], XtNwidth, &w);
     XtSetArg(arg[1], XtNheight, &h);
     XtGetValues(widget, arg, 2);

     x = (int)getvaluator(MOUSEX);
     y = (int)getvaluator(MOUSEY);

     jumpto(x,y);
}

void
jumpto(x,y)
  int x;
  int y;
{

     sceneclick(oursceneid, x,y);
     ResetFps();
     drawscene();
     TimeoutCursors(True);

     if (SelectGopherDir() < 0)
	  EyeAirJordan(drawscene, .5, 150.0, 200.0, 3.0);
	  /** Nothing to select, jump instead... **/;

     TimeoutCursors(False);
     drawscene();

}

/*
 * Pop up an index dialog 
 */


void
Options_Menu_CB(Widget w, XtPointer menuitem, XtPointer call_data)
{
     int dynamic;

     dynamic = (XmToggleButtonGetState(w) == True) ? 1 : 0;
     if (dynamic)
	  SetDynamicShading();
     else
	  SetStaticShading();

     drawscene();
}


void
Edit_Copy_CB(Widget widget, 
	     XtPointer client_data, 
	     XtPointer call_data)
{
    unsigned long item_id = 0;  /* clipboard item id */
    int           status;
    XmString      clip_label;
    char          buf[32];
    static int    cnt;
    Display      *dpy = XtDisplayOfObject (widget);
    Window        window = XtWindowOfObject (widget);
    char         *data = (char *) client_data;

    sprintf (buf, "%s-%d", data, ++cnt); /* make each copy unique */

    clip_label = XmStringCreateSimple ("to_clipbd");

    /* start a copy -- retry till unlocked */
    do
        status = XmClipboardStartCopy (dpy, window,
            clip_label, CurrentTime, NULL, NULL, &item_id);
    while (status == ClipboardLocked);

    XmStringFree (clip_label);

    /* copy the data (buf) -- pass "cnt" as private id for kicks */
    do
        status = XmClipboardCopy (dpy, window, item_id, "STRING",
            buf, (long) strlen (buf)+1, cnt, NULL);
    while (status == ClipboardLocked);

    /* end the copy */
    do
        status = XmClipboardEndCopy (dpy, window, item_id);
    while (status == ClipboardLocked);

}

void
popuptextfile(char *title, char *tempfile) {
	displayTempFile(top, title, tempfile);
}

/**********************************************************************/
void
popupsearchcallback(Widget w, XtPointer menuitem, XtPointer call_data)
{
     XmSelectionBoxCallbackStruct *cbs = (XmSelectionBoxCallbackStruct*) 
	  call_data;
     char *text;
     
     if (XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &text) == True) { 
	  XtPopdown(XtParent(w));
	  XtDestroyWidget(w);
	  XSync(dpy, False);
	  doSearch(text);
	  free(text);
     } else {
	  XtDestroyWidget(w);
     }

}

void
popupsearchdeally(char *title) {
     char searchprompt[512];

     sprintf(searchprompt, "Search '%s' for:", title);
     PromptFor(searchprompt, popupsearchcallback);
}

/**********************************************************************/
void
OpenURLcallback(Widget w, XtPointer menuitem, XtPointer call_data)
{
     XmSelectionBoxCallbackStruct *cbs = (XmSelectionBoxCallbackStruct*) 
	  call_data;
     char *text;
     
     if (XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET, &text) == True) { 
	  XtPopdown(XtParent(w));
	  XtDestroyWidget(w);
	  XSync(dpy, False);
	  V_OpenSessionURL(text, text);
	  free(text);
     } else {
	  XtDestroyWidget(w);
     }
}

void
MenuOpenURL(Widget w, XtPointer menuitem, XtPointer call_data)
{
     PromptFor("Open Location:", OpenURLcallback);
}

/**********************************************************************/
void
MenuOverview(Widget w, XtPointer menuitem, XtPointer call_data)
{
     EyeAirJordan(drawscene, 7.0, 0.0, 1400.0, 24.0);
}

void
MenuJumpForward(Widget w, XtPointer menuitem, XtPointer call_data)
{
     EyeAirJordan(drawscene, .5, 150.0, 200.0, 3.0);
}

void
MenuJumpUp(Widget w, XtPointer menuitem, XtPointer call_data)
{
     EyeAirJordan(drawscene, 0.0, 0.0, 1400.0 , 24.0);
}


void
MenuSpinOut(Widget w, XtPointer menuitem, XtPointer call_data)
{
     JumpOutofView(drawscene);

}

void
MenuSicko(Widget w, XtPointer menuitem, XtPointer call_data)
{
     MotionSickness(drawscene);

}

void
MenuInitialView(Widget w, XtPointer menuitem, XtPointer call_data)
{
     Go_InitialView();
}

void
MenuMoveUp(Widget w, XtPointer menuitem, XtPointer call_data)
{
     EyeLocationUp(1);
}

void
MenuMoveDown(Widget w, XtPointer menuitem, XtPointer call_data)
{
     EyeLocationUp(0);
}

extern void ReloadCurrentDir();
void
MenuReload(Widget w, XtPointer menuitem, XtPointer call_data)
{
        ReloadCurrentDir();
}

