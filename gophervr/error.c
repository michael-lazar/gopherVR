/* error.c
   functions to handle error and notice popups */

     /*---------------------------------------------------------------*/
     /* Moog           version 0.0     27 October 1992                */
     /* Xgopher        version 1.1     20 April 1991                  */
     /*                version 1.0     04 March 1991                  */
     /*                                                               */
     /* X window system client for the University of Minnesota        */
     /*                                Internet Gopher System.        */
     /*                                                               */
     /* Martin Hamilton,  Loughborough University of Technology       */
     /*                   Department of Computer Studies              */
     /*                                                               */
     /* Allan Tuchman,    University of Illinois at Urbana-Champaign  */
     /*                   Computing Services Office                   */
     /*                                                               */
     /* Jonathan Goldman, WAIS project                                */
     /*                   Thinking Machines Corporation               */
     /*                                                               */
     /* Copyright 1992 by                                             */
     /*           the Board of Trustees of the University of Illinois */
     /* Permission is granted to freely copy and redistribute this    */
     /* software with the copyright notice intact.                    */
     /*---------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>

#include "error.h"
/*#include "gui.h"*/

static Widget	topLevel;
static Widget	errorDialog;
static Widget	infoDialog;


/* errorOkProc
   User has read and accepted the error */

static void
errorOkProc(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{
	XtUnmanageChild(errorDialog);

	return;
}


/* errorExitProc
   Fatal errors exit here */

static void
errorExitProc(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{
	XtUnmanageChild(errorDialog);

	exit(14);
}


/* makeErrorDialog
   create popup dialog for reporting errors */

void
makeErrorDialog(top)
Widget	top;
{
	Arg		args[3];
	Cardinal	n = 0;

	topLevel = top;
	n = 0;
	XtSetArg(args[n], XtNtitle, "moog error");  n++;
	errorDialog = XmCreateErrorDialog(topLevel, "errorDialog", args, n);

	XtUnmanageChild(XmMessageBoxGetChild(errorDialog, 
					     XmDIALOG_HELP_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(errorDialog,
					     XmDIALOG_CANCEL_BUTTON));

	XtAddCallback(errorDialog, XmNokCallback, errorOkProc, NULL);
}


/* displayError
   enable popup dialog for saving files */

void
displayError(errorText, fatal)
char	*errorText;
Boolean	fatal;
{
	XtVaSetValues(errorDialog,
		      XmNmessageString,
		      XmStringCreateLtoR(errorText, XmSTRING_DEFAULT_CHARSET),
		      NULL);

	if (fatal)
		XtAddCallback(errorDialog, XmNokCallback, 
			      errorExitProc, NULL);

	XtManageChild(errorDialog);

	return;
}

/* info popup and stuff */

/* infoOkProc
   User has read and accepted the information */

static void
infoOkProc(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{
	XtUnmanageChild(infoDialog);

	return;
}


/* makeInfoDialog
   create popup dialog for reporting infos */

void
makeInfoDialog(top)
Widget	top;
{
	Widget		infoOkButton;
	Arg		args[3];
	Cardinal	n = 0;

	topLevel = top;
	n = 0;
	XtSetArg(args[n], XtNtitle, "moog information");  n++;
	infoDialog = XmCreateInformationDialog(topLevel, "infoDialog",
					args, n);

	XtUnmanageChild(XmMessageBoxGetChild(infoDialog,
					     XmDIALOG_HELP_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(infoDialog,
					     XmDIALOG_CANCEL_BUTTON));

	XtAddCallback(infoDialog, XmNokCallback, infoOkProc, NULL);
}


/* displayInfo
   enable popup dialog for saving files */

void
displayInfo(infoText)
char	*infoText;
{
	XtVaSetValues(infoDialog, 
		      XmNmessageString, 
		        XmStringCreateLtoR(infoText, XmSTRING_DEFAULT_CHARSET),
		      NULL);

	XtManageChild(infoDialog);

	return;
}
