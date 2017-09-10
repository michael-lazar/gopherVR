/* This is the Gopher Menu window. */

#include <stdlib.h>
#include <stdio.h>
#include <Xm/List.h>
#include <Xm/LabelG.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Protocols.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Frame.h>

#ifndef SVR4
#  include <strings.h>
#else
#  include <string.h>
#endif

#include <memory.h>
#include <stdio.h>
#include "gophwin.h"
#include "menus.h"

extern	void	V_SelMenuItem( int N_item );
extern	int		B_GetAbstract( int N_item, char **aCh_abs );
extern	void	V_OpenSessionURL( char *aCh_title, char *aCh_urltxt );
extern	void	V_GenHistUrlListing( char *aCh_url );
extern	char	*aCh_GetHisUrl( int iSta_picked );
extern	void	V_RemoveHisUrl( void );
extern	void	V_PopDirectory( void );
extern	void	V_PrintUR( int );

extern	Widget	top, Wig_top2;
static	Widget	sg_Wig_list, sg_Wig_form1, sg_Wig_menubar;
static	Widget	sg_Wig_title, sg_Wig_abs, sg_Wig_hist; 
extern  int     menuwin_w, menuwin_h, menuwin_x, menuwin_y;
	/* provided by burrower */

void V_RememberListWinSize()
{
	/* Utility function to shadow menuwin_* when we close this window.
		Used here, and by burrower when the app terminates. */

	Window root, child;
	XWindowAttributes wnattr;
	int rootx, rooty, winx, winy, mask;
	int x = menuwin_x;
	int y = menuwin_y;
	int w = menuwin_w;
	int h = menuwin_h;

	if (Wig_top2 == NULL) return; /* hope we did it right before!! */

	/* The obvious thing (XGetGeometry(), XGetWindowAttributes()) won't work
		right on most rootless environments, so we're sneaky and use
		XQueryPointer(). */

	if(XQueryPointer(XtDisplay(Wig_top2), XtWindow(Wig_top2), &root, &child,
			&rootx, &rooty, &winx, &winy, &mask) == True) {
		x = rootx - winx;
		y = rooty - winy;

/* I can't seem to account for the menu bar in X11.app. wtf? Cameron */
/* burrower has the same problem */
#if defined(__APPLE__)
#warning using menubar kludge for X11.app

		y -= 22;
#endif
	
		if(XGetWindowAttributes(XtDisplay(Wig_top2), XtWindow(Wig_top2),
				&wnattr) != 0) {
			w = wnattr.width;
			h = wnattr.height;
		}
		menuwin_x = x;
		menuwin_y = y;
		menuwin_w = w;
		menuwin_h = h + 18; /* sigh */
       	 }
}

void
V_CloseListWin(Widget  w, XtPointer   client_data, XtPointer cbs)
{
	/* Remember our window top and left so we can reopen it later. */
	V_RememberListWinSize();

	/* Null our pointer so we don't go trying to update a dead window. */
	Wig_top2 = NULL;
}

/* for the dynamically resizing menu */
int mynewsize(int height)
{
	int newsize;

	newsize = 15 + ((height - 460)/18);
	if (newsize < 1) newsize = 1;
	return newsize;
}

void
V_ResizeMenuWin(Widget w, XtPointer client_data, XEvent *event, Boolean *crap)
{
	Arg args[1];
	XConfigureEvent *cevent = (XConfigureEvent *) event;
	int newsize;

	if (cevent->type != ConfigureNotify) return;

	XtSetArg(args[0], XmNvisibleItemCount, mynewsize(cevent->height));
	XtSetValues(sg_Wig_list, args, 1);
}

void
V_GenMenuListWin( void )
{
    Arg         args[16];
    int         n = 0, i = 0;
    XmString	Xst_title;
/*
    int		N_topheight, N_topx, N_topy;
*/
    Atom	WM_DELETE_WINDOW;

    if (Wig_top2 != NULL) return;


    /* create PopUpshell */
    Wig_top2 = XtVaCreatePopupShell( "Gopher Menu", 
				    transientShellWidgetClass, top, 
				    NULL );

#if(0)
    XtVaGetValues(top, XmNheight, &N_topheight, 
		  XmNx, &N_topx, XmNy, &N_topy, NULL ); 
    XtVaSetValues( Wig_top2, XtNx, N_topx, XtNy, N_topy + N_topheight + 25, NULL );
#endif

    XtVaSetValues( Wig_top2, XtNx, menuwin_x, XtNy, menuwin_y, NULL );

    /* Make sure we get notified if the wm deletes us. -- Cameron */
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(Wig_top2),
	"WM_DELETE_WINDOW", False);
    XmRemoveWMProtocolCallback(Wig_top2, WM_DELETE_WINDOW, V_CloseListWin,
	NULL);
    XmAddWMProtocolCallback(Wig_top2, WM_DELETE_WINDOW, V_CloseListWin, NULL);

    /* Make sure we are notified for resizes so we can resize our menu list.
	-- Cameron */
    XtAddEventHandler(Wig_top2, StructureNotifyMask, False,
	V_ResizeMenuWin, NULL);

    /* form */
    sg_Wig_form1 = XtVaCreateWidget( "Gopenu",
				    xmFormWidgetClass, Wig_top2,
				    NULL );

	/* the menu bar */
	sg_Wig_menubar = Create_Menubar( sg_Wig_form1 ); 
	XtVaSetValues( sg_Wig_menubar,
					XmNtopAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					NULL );

	/* recent history selection */
	n = 0;
    XtSetArg( args[n], XmNvisibleItemCount, 5 ); n++;
    XtSetArg( args[n], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE ); n++;
    XtSetArg( args[n], XmNwidth, menuwin_h ); n++;
    XtSetArg( args[n], XmNdoubleClickInterval, 500 ); n++;
	XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget, sg_Wig_menubar ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
    sg_Wig_hist = XmCreateScrolledList( sg_Wig_form1, "Recent List", args, n );
    XtAddCallback( sg_Wig_hist, XmNdefaultActionCallback, V_HisCall, NULL );
    XtAddCallback( sg_Wig_hist, XmNbrowseSelectionCallback, V_HisBrowse, NULL);
    XtManageChild( sg_Wig_hist );

    /* current title */
    Xst_title = XmStringCreateSimple( "Gopher VR" );
    sg_Wig_title = XtVaCreateManagedWidget( "Gopher VR",
                    xmPushButtonWidgetClass, sg_Wig_form1,
                    XmNlabelString, Xst_title,
                    XmNrecomputeSize, False,
                    XmNleftAttachment, XmATTACH_FORM,
                    XmNrightAttachment, XmATTACH_FORM,
                    XmNtopAttachment, XmATTACH_WIDGET,
                    XmNtopWidget, sg_Wig_hist,
                    NULL );
    XmStringFree( Xst_title );
    XtAddCallback( sg_Wig_title, XmNactivateCallback, V_SelCall2, NULL );


	/* menu selection */
	n = 0;
    XtSetArg( args[n], XmNvisibleItemCount, mynewsize(menuwin_h) ); n++;
	XtSetArg( args[n], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE ); n++;
	XtSetArg( args[n], XmNwidth, menuwin_w ); n++;
	XtSetArg( args[n], XmNdoubleClickInterval, 350 ); n++;
	XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
	XtSetArg( args[n], XmNtopWidget, sg_Wig_title ); n++;
	XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
	XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
    sg_Wig_list = XmCreateScrolledList( sg_Wig_form1, "scroll_list", args, n );
    XtAddCallback( sg_Wig_list, XmNdefaultActionCallback, V_SelCall, NULL );
    XtAddCallback( sg_Wig_list, XmNbrowseSelectionCallback, V_SelBrowse, NULL );
    XtManageChild( sg_Wig_list );

	/* create window for abstracts */
	sg_Wig_abs = XtVaCreateManagedWidget( "abstract",
					xmTextWidgetClass, sg_Wig_form1,
					XmNvalue, "\n",
					XmNautoShowCursorPosition, False,
					XmNcursorPositionVisible, False,
					XmNeditable, False,
					XmNeditMode, XmMULTI_LINE_EDIT,
					XmNrows, 3,
					XmNscrollBarDisplayPolicy, XmAS_NEEDED,
					XmNtopAttachment, XmATTACH_WIDGET,
					XmNtopWidget, sg_Wig_list,
					XmNleftAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					NULL );


	XtManageChild( sg_Wig_form1 );
    XtPopup( Wig_top2, XtGrabNonexclusive );
}


/* We should eventually backstore these with an XmStringTable.

    str_list = (XmStringTable) XtMalloc (n * sizeof (XmString));

    for (i = 0; i < n; i++)
        str_list[i] = XmStringCreateLocalized (months[i]);

    i = 0;

*/

/*
 * Add a new GopherItem to the MENU view of the directory
 */
void
V_GenMenuListing( char *aCh_text, int N_pos )
{
    XmString	icon, str, Xst_str;
/*    icon = XmStringCreateLtoR("", "Title");*/
        if (Wig_top2 == NULL) return;
    str = XmStringCreateSimple( aCh_text );

/*    Xst_str = XmStringConcat(icon, str);*/

    XmListAddItemUnselected( sg_Wig_list, str, N_pos );
    XmStringFree( str );
}




	/*
	 * THIS MUST BE CALLED ***BEFORE*** V_GenMenuListing()
	 *
	 * you must delete the old list before building a new list.
	 */
void
V_DelMenuListing( void )
{
	int         i = 0, caCh_last_num = 0;

        if (Wig_top2 == NULL) return;
	XtVaGetValues( sg_Wig_list, XmNitemCount, &caCh_last_num, NULL );

	for ( i = 0; i < caCh_last_num; i++ )
	{
		XmListDeletePos( sg_Wig_list, 1 );
	}
}





void
V_SetMenuTitle( char *aCh_title )
{
	XmString    Xst_title;

        if (Wig_top2 == NULL) return;
	Xst_title = XmStringCreateSimple( aCh_title );
	XtVaSetValues( sg_Wig_title, 
					XmNlabelString, Xst_title,
					NULL );
	XmStringFree( Xst_title );
}





static void
V_SelCall( Widget sg_Wig_list, XtPointer client_data, XtPointer call_data )
{
	int i=0;
	XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;

        if (Wig_top2 == NULL) return;
	V_SelMenuItem( cbs->item_position );
}

static void
V_SelBrowse( Widget sg_Wig_list, XtPointer client_data, XtPointer call_data )
{
	char	*aCh_abs = NULL;

	XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;

        if (Wig_top2 == NULL) return;
	/* Update Abstract info - if any */
	if( B_GetAbstract( cbs->item_position, &aCh_abs ) )
	{
		XmTextSetString( sg_Wig_abs, aCh_abs );
		free( aCh_abs );
	}
	else
	{
		XmTextSetString( sg_Wig_abs, "" );
	}
}




	/*
	 * Routines that deal with the Recent History List
	 */

void
V_AddStringToList(char *s, int indent)
{
    char spaces[10];		/* Only space out 10 levels... */
    XmString    Xst_str, Xst_spaces, Xst_title;
    int x;
    
    if (Wig_top2 == NULL) return; /* no window to add */

    /* Create a space string.. */
    for (x = 0; (x < 10) && (x < indent); x++) {
	 spaces[x] = ' ';
    }
    spaces[x] = '\0';

    Xst_spaces = XmStringCreateSimple(spaces);
    Xst_title  = XmStringCreateSimple(s);
    Xst_str = XmStringConcat(Xst_spaces, Xst_title);

    /*** Add item title to history list ***/
    XmListAddItemUnselected( sg_Wig_hist, Xst_str, indent);

    XmStringFree(Xst_str);
    XmStringFree(Xst_spaces);
    XmStringFree(Xst_title);
}

/*
 * Add a new GopherItem to the recent history list
 */
static int  caCh_entries = 0;

void
V_GenHistListing( char *aCh_title, char *aCh_url )
{
    V_AddStringToList(aCh_title, ++caCh_entries);

    /* Add url to array */
    V_GenHistUrlListing( aCh_url );
}

static void
V_HisBrowse( Widget Wig_list, XtPointer client_data, XtPointer call_data )
{
	XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
        if (Wig_top2 == NULL) return;
	V_PrintURL( cbs->item_position );
}



static void
V_HisCall( Widget Wig_list, XtPointer client_data, XtPointer call_data )
{
    int i;
	int N_pos;
    XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
    char *aCh_title;


        if (Wig_top2 == NULL) return;
	/* item selected */
	N_pos = cbs->item_position;

	/* we are already at the bottom of the list, no need to go there */
	if ( N_pos == caCh_entries)
		return;

	/* top level already, where do you want to go? */
	if ( (1 == caCh_entries) && (1 == N_pos) )
		return;



	/* get the title */
    XmStringGetLtoR( cbs->item, XmFONTLIST_DEFAULT_TAG, &aCh_title );

	/* unpop some items */
    for ( i = caCh_entries; i > N_pos; i-- )
    {
		V_PopDirectory();
		V_DestroyHistoryData();
    }

	/* jump to the new location */
/*   	V_OpenSessionURL( aCh_title, aCh_GetHisUrl( N_pos ) );*/
	V_AfterHistory();
}


void
V_DestroyHistoryData( void ) 
{

	if ( 1 == caCh_entries )
		return;

        if (Wig_top2 == NULL) return;
	XmListDeletePos( sg_Wig_hist, caCh_entries );
	V_RemoveHisUrl();
	caCh_entries--;
}



	/*
	 * Routine that deals with the go-up one level pushbutton
	 */
static void
V_SelCall2( Widget Wig_w, XtPointer client_data, XtPointer call_data )
{
	Widget      Wig_hist;
	Arg         args[16];
	int         n = 0;


	if ( 1 == caCh_entries )
		return;
        if (Wig_top2 == NULL) return;

	V_SelMenuItem( 0 );
}
