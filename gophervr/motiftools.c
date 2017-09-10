/*
 * In this file you'll find alot of the groovy tools to call the motif toolkit
 *
 * We need this layer of abstraction because the gopher and vogl code
 * use conflicting definitions for String and Object respectively
 */


/*
 * Our first widget is a label that we can write status messages into
 */

#include <stdio.h>
#include <Xm/Label.h>		/* For status messages */
#include <X11/cursorfont.h>
#include <Xm/MessageB.h>
#include "motiftools.h"

void
do_search( Widget widget, XtPointer search_data ); /* from text.c. */
int
is_writable( char *file ); /* also from text.c */
void
WriteGopherFile(FILE *f, void *gs);


extern Widget top;
extern void deathofgopher(Widget widget, XtPointer client_data, XtPointer cbs);

int stale_fps = 0;

/**********************************************************************/
static Widget StatusWidget = NULL;

void
StatusNew(Widget parent, char *msg)
{

     if (StatusWidget == NULL) {
	  StatusWidget = XmCreateLabel(parent, "statusmsg", NULL, 0);
	  XtVaSetValues(StatusWidget,
			XmNrightAttachment, XmATTACH_FORM,
			XmNbottomAttachment,XmATTACH_FORM,
#ifdef __APPLE__
/* Leave room for resize gadget. */
			XmNrightOffset, 20,
#endif
			NULL);

	  XtManageChild(StatusWidget);
     }
     StatusUpdate(msg);

}

void
StatusUpdate(char *msg)
{
     XmString title;

     title = XmStringCreateLtoR(msg,"Body");
     XtVaSetValues(StatusWidget, 
		   XmNlabelString, title,
		   NULL);

     XmUpdateDisplay(StatusWidget);
     XmStringFree(title);
     stale_fps = 0;
}

/**********************************************************************
 * A button thing that blinks on and off when we're doing stuff
 */
#include "icons/busy1.h"
#include "icons/busy2.h"

static Widget BusyWidget = NULL;
static Pixmap Busypix[2];
static Pixmap Movepix[8];

void
BusyThingNew(Widget parent)
{
     Pixel fg, bg;
     Arg args[1];

     if (BusyWidget == NULL) {
	  Pixel fg, bg;

	  XtVaGetValues(parent, XmNforeground, &fg, 
			XmNbackground, &bg, NULL );

	  Busypix[0] = XCreatePixmapFromBitmapData(XtDisplay(top),
		    RootWindowOfScreen(XtScreen(top)), busy1_bits,
		    busy1_width, busy1_height, fg, bg, 
		    DefaultDepthOfScreen(XtScreen(top)));

	  Busypix[1] = XCreatePixmapFromBitmapData(XtDisplay(top),
		    RootWindowOfScreen(XtScreen(top)), busy2_bits,
		    busy2_width, busy2_height, fg, bg, 
		    DefaultDepthOfScreen(XtScreen(top)));


	  BusyWidget = XtVaCreateManagedWidget("busypix", xmLabelWidgetClass,
		       parent,
		       XmNlabelType, XmPIXMAP,
		       XmNlabelPixmap, Busypix[0],
	               XmNbottomAttachment,XmATTACH_FORM,
		       NULL);
	  
	  
	  XtManageChild(BusyWidget);
	  
     }
}     

void
BusyUpdate(void)
{
     static int busystate = 0;

     busystate = 1 -busystate;

     XtVaSetValues(BusyWidget, 
		   XmNlabelPixmap, Busypix[busystate],
		   NULL);

     XmUpdateDisplay(BusyWidget);

}


/**********************************************************************/

void MenuQuit_CB(Widget w, XtPointer menuitem, XtPointer call_data)
{
     GTXTcleanUpTextProc();
     deathofgopher(w, call_data, NULL);
     exit(0);
     
}


/**********************************************************************
 * Display our About Window...
 */

#define ABOUT_TITLE \
"GopherVR v0.5.1 *ALPHA*\n"


#define ABOUT_MSG \
"\n\
\251 Copyright 1995-2015 The Regents of the\n\
University of Minnesota and others\n\
\n\
Rendering Engine:\n\
 Neophytos Iacovou, Mark McCahill, Paul Lindner\n\
\n\
Application:\n\
 Paul Lindner, Neophytos Iacovou, Cameron Kaiser\n\
\n\
UMN Gopher 3.0.11 Updates:\n\
 John Goerzen\n\
\n\
Compatibility and Ports:\n\
 Cameron Kaiser, Jeroen Schot, Martin Doege,\n\
 Brian Callahan, RichardG\n\
\n\
Visit Floodgap: gopher.floodgap.com\n\
 or comments to gopher@floodgap.com\n\
Distributed Computing Services Gopher Team: R.I.P.\n\
 gopher@boombox.micro.umn.edu\n\
A Fuzzy Monkey - Lard Bucket Production"

extern XtAppContext app;

#include "icons/surfgopher.h"

void
MenuAboutBox(Widget w, XtPointer menuitem, XtPointer call_data)
{
     XmString title, body, str;
     Arg args[2];
     static Widget AboutDialog = NULL;
     Pixel fg, bg;
     Pixmap CoolImage;


     title = XmStringCreateLtoR(ABOUT_TITLE, "Title");
     body = XmStringCreateLtoR(ABOUT_MSG,"Body");

     str = XmStringConcat(title, body);

     XtSetArg(args[0], XmNmessageString, str);
     XtSetArg(args[1], XmNtitle, "About GopherVR");
     AboutDialog = XmCreateInformationDialog(top, "About Gopher", args, 2);

     XtVaGetValues(AboutDialog, XmNforeground, &fg, XmNbackground, &bg, NULL );

     CoolImage = XCreatePixmapFromBitmapData(XtDisplay(top),
		    RootWindowOfScreen(XtScreen(top)), surfgopher_bits,
		    surfgopher_width, surfgopher_height, fg, bg, 
		    DefaultDepthOfScreen(XtScreen(top)));

     XtVaSetValues(AboutDialog,
		 XmNsymbolPixmap, CoolImage,
		 NULL);

     XmStringFree(str);
     
     XtManageChild(AboutDialog);
     
     XtPopup(XtParent(AboutDialog), XtGrabNone);
}


/**********************************************************************/
void
Errormsg(char *message) 
{
     XmString str;
     static Widget ErrorDialog = NULL;
     Arg args[1];


     str = XmStringCreateLtoR(message, "Body");

     XtSetArg(args[0], XmNmessageString, str);
     ErrorDialog = XmCreateErrorDialog(top, "GopherVR Error", args, 1);

     XmStringFree(str);
     XtSetSensitive(XtNameToWidget(ErrorDialog, "Help"), False);
     XtManageChild(ErrorDialog);
     
     XtPopup(XtParent(ErrorDialog), XtGrabNone);
}     

/**********************************************************************/

void
MenuNewDocument(Widget w, XtPointer menuitem, XtPointer call_data)
{
     displayTextString(top, "Untitled", " ");
}


/**********************************************************************
 * Display the watch cursor...
 */

void
TimeoutCursors(Boolean on)
{
    static Cursor cursor;
    XSetWindowAttributes attrs;
    Display *dpy = XtDisplay(top);

    if (!cursor) 
        cursor = XCreateFontCursor (dpy, XC_watch);

    attrs.cursor = on ? cursor : None;

    /* change the main application shell's cursor to be the timeout
     * cursor or to reset it to normal.  If other shells exist in
     * this application, they will have to be listed here in order
     * for them to have timeout cursors too.
     */

    XChangeWindowAttributes (dpy, XtWindow(top), CWCursor, &attrs);

    XFlush (dpy);
}


/**********************************************************************
 *
 */


void
UseOurCursor(int cursortype)
{
     ;
}


/**********************************************************************
 * Hide the cursor
 */

void
HiddenCursors(Boolean on)
{
    static Cursor cursor;
    XSetWindowAttributes attrs;
    Display *dpy = XtDisplay(top);

    if (!cursor) 
        cursor = XCreateFontCursor (dpy, XC_watch);

    attrs.cursor = on ? cursor : None;

    /* change the main application shell's cursor to be the timeout
     * cursor or to reset it to normal.  If other shells exist in
     * this application, they will have to be listed here in order
     * for them to have timeout cursors too.
     */

    XChangeWindowAttributes (dpy, XtWindow(top), CWCursor, &attrs);

    XFlush (dpy);
}


/**********************************************************************/
AddInput(int fd, char *(proc)())
{
     XtAppAddInput(app, fd, (XtPointer)XtInputReadMask, proc, NULL);
}

/**********************************************************************/
#include <Xm/SelectioB.h>
#include <Xm/DialogS.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include "helpdiag.h"

void
PromptFor(char *txt, void *(function)())
{
     static Widget prompt;
     Arg args[3];
     XmString str;

     str = XmStringCreateLtoR(txt, txt);
     XtSetArg(args[0], XmNselectionLabelString, str);
     XtSetArg(args[1], XmNautoUnmanage, False);
     XtSetArg(args[2], XmNtitle, txt);

     prompt = XmCreatePromptDialog(top, "prompter", args, 3);
     XmStringFree(str);

     XtAddCallback(prompt, XmNokCallback, function, prompt);
     XtAddCallback(prompt, XmNcancelCallback, XtDestroyWidget, NULL);


     XtManageChild(prompt);

     XtPopup(XtParent(prompt), XtGrabNone);
}
     
void
MenuNotImplemented(Widget w, XtPointer menuitem, XtPointer call_data)
{
	Errormsg("Not implemented yet");
}

/* Handle saves and loads. We borrow do_search from text.c. */
void
savecancel(Widget Wig_diag, XtPointer client_data, XtPointer call_data)
{
        XtPopdown((Widget)client_data);
}
void
saveok(Widget widget, XtPointer client_data, XtPointer call_data)
{
    char *file;
        char *aCh_exec;
        int      cCh_len;
    XmFileSelectionBoxCallbackStruct *cbs =
        (XmFileSelectionBoxCallbackStruct *) call_data;

    /* get the string typed in the text field in char * format */
    if (!XmStringGetLtoR (cbs->value, XmFONTLIST_DEFAULT_TAG, &file))
        return;
    if (*file != '/') {
        /* if it's not a directory, determine the full pathname
         * of the selection by concatenating it to the "dir" part
         */
        char *dir, *newfile;
        if (XmStringGetLtoR (cbs->dir, XmFONTLIST_DEFAULT_TAG, &dir)) {
            newfile = XtMalloc (strlen (dir) + 1 + strlen (file) + 1);
            sprintf (newfile, "%s/%s", dir, file);
            XtFree( file);
            XtFree (dir);
            file = newfile;
        }
    }
    switch (is_writable (file)) {
    case 1 :
    case -1 : {
	FILE *f;
	if (f = fopen(file, "w")) {
		WriteGopherFile(f, client_data);
		fclose(f);
	} else {
	    Errormsg("Failed to save file.");
        }
            break;
	}
        case 0 : {
            /* a directory was selected, scan it */
            XmString str = XmStringCreateSimple (file);
            XmFileSelectionDoSearch (widget, str);
            XmStringFree (str);
            break;
       }

    }

    XtFree (file);
        Errormsg("Saved.");
}

void          
filepicker( char *suggestion, void *gs)
{                  
/* IRIX is picky. */
    Widget dialog, Wig_filetxt, Wig_filtxt;
    XmString filemask;
#ifndef __sgi
    extern void exit();
#endif
    Arg args[5];   
    int n = 0;

    filemask = XmStringCreateLocalized((suggestion) ? suggestion : "*");
         
    XtSetArg (args[n], XmNfileSearchProc, do_search); n++;
    dialog = (Widget)XmCreateFileSelectionDialog ( top, suggestion, args, n);
        XtVaSetValues( dialog, 
		XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL,
/*
		XmNpattern, filemask,
*/
                                        NULL );
         
        Wig_filetxt = (Widget)XmFileSelectionBoxGetChild( dialog, XmDIALOG_TEXT );
        XtVaSetValues( Wig_filetxt,
                                                XmNautoShowCursorPosition, True,
                                                NULL );
        Wig_filtxt = (Widget)XmFileSelectionBoxGetChild( dialog, XmDIALOG_FILTER_TEXT );
        XtVaSetValues( Wig_filtxt,
                                                XmNautoShowCursorPosition, True,
                                                NULL );
       
    XtSetSensitive ((Widget)
        XmFileSelectionBoxGetChild (dialog, XmDIALOG_HELP_BUTTON), False);
    XtAddCallback (dialog, XmNokCallback, saveok, gs);
    XtAddCallback(dialog, XmNcancelCallback, savecancel, XtParent(dialog));

    XtManageChild (dialog);

}

