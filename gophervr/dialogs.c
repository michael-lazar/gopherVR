#include <stdlib.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include <Xm/SeparatoG.h>
#include <Xm/RowColumn.h>
#include <Xm/FileSB.h>
#include <Xm/CascadeBG.h>
#include <Xm/TextF.h>
#include <Xm/MessageB.h>

#include "guidefs.h"
#include "helpdiag.h"
#include "dialogs.h"


extern Widget top;


TWOTEXTDESC g_rTtd_list [4] = 
	{	{NULL,	NULL}, 
		{"Anonymous FTP Host", "Selector (Optional)", f_V_FTP_ok},
		{NULL, NULL},
		{NULL, NULL}
	};

ONETEXTDESC g_rOtd_list [4] =
	{	{NULL,  NULL},
		{NULL,  NULL},
		{"Enter a URL:", f_V_URL_ok},
		{"Search for:", f_V_SEARCH_ok}
	};




char *Search = "Search items for:";


/*
 * Create Dialog with 1 text widget
 */
void
V_OpenGeneralDiag( Widget widget, XtPointer client_data, XtPointer call_data )
{
     Widget dialog, rc, Wig_text;
     
     int    irOtd_index = (int) client_data;
     XmString string;
     Arg args[5];
     int n = 0;
     P_ONETEXTW  pOtw_pass;
     
     
     if ((pOtw_pass = (P_ONETEXTW) malloc( sizeof(ONETEXTW) )) == NULL)
     {
	  showError("Too many windows are already open.");
	  return;
     }
     pOtw_pass->F_which_help = irOtd_index;

     /* Create a TemplateDialog that will contain the control area
      * and the action area buttons for the dialog
      */
     string = XmStringCreateLocalized ("Dialog Shell");
     XtSetArg (args[n], XmNdialogTitle, string); n++;
     XtSetArg (args[n], XmNautoUnmanage, False); n++;
     dialog = XmCreateTemplateDialog ( top, "dialog", args, n);
     XmStringFree (string);
     pOtw_pass->Wig_parent = dialog;
     
     
     /* create the control area which contains a  Label gadget 
      * and a TextField widget.
      */
     rc = XtVaCreateWidget("control_area", 
			   xmRowColumnWidgetClass, dialog, 
			   NULL);
     
     /* Label1 Text1 */
     if ( d_SEARCH == irOtd_index )
     {
	  string = XmStringCreateLocalized ( Search );
	  XtVaSetValues( dialog, 
			XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL,
			NULL );
     }
     else
     {
	  string = 
	       XmStringCreateLocalized ( g_rOtd_list [irOtd_index].aCh_string);
     }
     XtVaCreateManagedWidget("label", 
			     xmLabelGadgetClass, rc, 
			     XmNlabelString, string, 
			     NULL);
     XmStringFree (string);
     
     n = 0;
     XtSetArg(args[n], XmNwidth, d_FIELD_WIDTH ); n++; 
     Wig_text = XtVaCreateManagedWidget("text-field", 
					xmTextFieldWidgetClass, rc, 
					NULL);
     pOtw_pass->Wig_text = Wig_text;
     /* RowColumn is full -- now manage */
     XtManageChild (rc);
     
     /* Create the action area. */
     CreateActionArea( pOtw_pass, g_rOtd_list [irOtd_index].f_V_callback );
     
     /* callback for Return in TextField.  Use dialog as client data */
     XtAddCallback (Wig_text, XmNactivateCallback, activate_cb, dialog);
     
     XtManageChild (dialog);
     XtPopup (XtParent (dialog), XtGrabNone);
}




/*
 * Create Dialog with 2 text widgets
 */
void 
V_OpenTwoEntryDiag( Widget widget, XtPointer client_data, XtPointer call_data )
{
    Widget      dialog, rc, Wig_text1, Wig_text2;
    int         irTtd_index = (int) client_data;
    P_TWOTEXTW  pTtw_pass;
    XmString    string;
    Arg         args[5];
    int         n = 0;


    if ((pTtw_pass = (P_TWOTEXTW) malloc( sizeof(TWOTEXTW) )) == NULL)
    {
        showError("Too many windows are already open.");
        return;
    }

    /* set-up struct that holds our data */ 
    pTtw_pass->F_which_one = 1;
    pTtw_pass->F_which_help = irTtd_index;
    
    /* Create a TemplateDialog that will contain the control area
     * and the action area buttons for the dialog
     */
    string = XmStringCreateSimple ("Dialog Shell");
    XtSetArg (args[n], XmNdialogTitle, string); n++;
    XtSetArg (args[n], XmNautoUnmanage, False); n++;
    dialog = XmCreateTemplateDialog ( top, "dialog", args, n );
    XmStringFree (string);
	pTtw_pass->Wig_parent = dialog; 

    /* create the control area which contains 2 Label gadgets
     * and 2 TextField widgets.
     */
    rc = XtVaCreateWidget( "control_area", 
                                xmRowColumnWidgetClass, dialog, 
                                NULL );

    /* Label1 Text1 */
    string = XmStringCreateSimple ( g_rTtd_list [irTtd_index].aCh_string1 );
    XtVaCreateManagedWidget( "label1", 
                                    xmLabelGadgetClass, rc, 
                                    XmNlabelString, string, 
                                    NULL );
    XmStringFree (string);

    n = 0;
    XtSetArg( args[n], XmNwidth, d_FIELD_WIDTH ); n++; 
    Wig_text1 = XtVaCreateManagedWidget("text-field1", 
                                    xmTextFieldWidgetClass, rc,
                                    NULL);
	pTtw_pass->Wig_text1 = Wig_text1;

    /* Label2 Text2 */
    string = XmStringCreateSimple ( g_rTtd_list [irTtd_index].aCh_string2 );
    XtVaCreateManagedWidget( "label2", 
                                    xmLabelGadgetClass, rc, 
                                    XmNlabelString, string, 
                                    NULL );
    XmStringFree (string);

    n = 0;
    XtSetArg( args[n], XmNwidth, d_FIELD_WIDTH ); n++; 
    Wig_text2 = XtVaCreateManagedWidget( "text-field2", 
                                    xmTextFieldWidgetClass, rc,
                                    NULL);
	pTtw_pass->Wig_text2 = Wig_text2;


    /* RowColumn is full -- now manage */
    XtManageChild (rc);

    /* Create the action area. */
    CreateActionArea2( pTtw_pass, g_rTtd_list [irTtd_index].f_V_callback );

    /* callback for Return in TextField.  Use dialog as client data */
    XtAddCallback (Wig_text1, XmNactivateCallback, activate_cb, dialog);
    XtAddCallback (Wig_text1, XmNfocusCallback, V_SetActiv1, pTtw_pass ); 

    XtAddCallback (Wig_text2, XmNactivateCallback, activate_cb, dialog);
    XtAddCallback (Wig_text2, XmNfocusCallback, V_SetActiv2, pTtw_pass ); 


    XtManageChild (dialog);
    XtPopup (XtParent (dialog), XtGrabNone);
}





	/*
	 * Create the Action Area for a 1 text widget Dialog Box
	 */
void
CreateActionArea(P_ONETEXTW pOtw_data, void (*f_V_callback)() )
{
    Widget widget;
    int i;

    widget = XtVaCreateManagedWidget( "OK",
                    xmPushButtonWidgetClass, pOtw_data->Wig_parent, 
                    XmNshowAsDefault, i == 0,
                    XmNdefaultButtonShadowThickness, 1,
                    NULL);
    XtAddCallback( widget, XmNactivateCallback, f_V_callback, pOtw_data);
    XtVaSetValues ( pOtw_data->Wig_parent, XmNdefaultButton, widget, NULL);

    widget = XtVaCreateManagedWidget( "Clear",
                    xmPushButtonWidgetClass, pOtw_data->Wig_parent,
                    XmNshowAsDefault, i == 0,
                    XmNdefaultButtonShadowThickness, 1,
                    NULL);
    XtAddCallback( widget, XmNactivateCallback, V_ClearOneText, pOtw_data ); 

    widget = XtVaCreateManagedWidget( "Cancel",
                    xmPushButtonWidgetClass, pOtw_data->Wig_parent,
                    XmNshowAsDefault, i == 0,
                    XmNdefaultButtonShadowThickness, 1,
                    NULL);
    XtAddCallback( widget, XmNactivateCallback, V_DiagClose1, pOtw_data );

    widget = XtVaCreateManagedWidget( "Help",
                    xmPushButtonWidgetClass, pOtw_data->Wig_parent,
                    XmNshowAsDefault, i == 0,
                    XmNdefaultButtonShadowThickness, 1,
                    NULL);
    XtAddCallback( widget, XmNactivateCallback, V_HelpWin, 
		  (XtPointer)pOtw_data->F_which_help);
}


    /*
     * Create the Action Area for a 2 text widget Dialog Box
     */
void
CreateActionArea2( P_TWOTEXTW pTtw_data, void (*f_V_callback)() ) 
{
    Widget widget;
    int i;

	widget = XtVaCreateManagedWidget( "OK",
                    xmPushButtonWidgetClass, pTtw_data->Wig_parent, 
					XmNshowAsDefault, i == 0,
                    XmNdefaultButtonShadowThickness, 1, 
					NULL);
	XtAddCallback( widget, XmNactivateCallback, f_V_callback, pTtw_data );
    XtVaSetValues ( pTtw_data->Wig_parent, XmNdefaultButton, widget, NULL );

    widget = XtVaCreateManagedWidget( "Clear",
                    xmPushButtonWidgetClass, pTtw_data->Wig_parent, 
                    XmNshowAsDefault, i == 0,
                    XmNdefaultButtonShadowThickness, 1, 
                    NULL);
    XtAddCallback( widget, XmNactivateCallback, V_ClearTwoText, pTtw_data );

    widget = XtVaCreateManagedWidget( "Cancel",
                    xmPushButtonWidgetClass, pTtw_data->Wig_parent, 
                    XmNshowAsDefault, i == 0,
                    XmNdefaultButtonShadowThickness, 1, 
                    NULL);
    XtAddCallback( widget, XmNactivateCallback, V_DiagClose2, pTtw_data );

    widget = XtVaCreateManagedWidget( "Help",
                    xmPushButtonWidgetClass, pTtw_data->Wig_parent, 
                    XmNshowAsDefault, i == 0,
                    XmNdefaultButtonShadowThickness, 1, 
                    NULL);
    XtAddCallback( widget, XmNactivateCallback, V_HelpWin, 
								(XtPointer)pTtw_data->F_which_help);
}




	/*
	 * Callback function for the "Cancel" button
	 */
void
V_DiagClose1(Widget w, XtPointer client_data, XtPointer call_data )
{
	P_ONETEXTW	pOtw_data = (P_ONETEXTW) client_data;
	Widget		Wig_destroy = pOtw_data->Wig_parent;

	XtDestroyWidget( Wig_destroy );
    free(pOtw_data);
}

void
V_DiagClose2( Widget w, XtPointer client_data, XtPointer call_data )
{
    P_TWOTEXTW	pTtw_data = (P_TWOTEXTW) client_data;
	Widget		Wig_destroy = pTtw_data->Wig_parent;

	XtDestroyWidget( Wig_destroy );
    free( pTtw_data );
}




	/*
	 * Callback function for the "Clear" button
	 */
void
V_ClearOneText( Widget w, XtPointer client_data, XtPointer call_data )
{
	P_ONETEXTW pOtw_data = (P_ONETEXTW) client_data;
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) call_data;

    XmTextFieldSetString (pOtw_data->Wig_text, "");
}


void
V_ClearTwoText( Widget w, XtPointer client_data, XtPointer call_data )
{
    P_TWOTEXTW pTtw_data = (P_TWOTEXTW) client_data;

    if (1 == pTtw_data->F_which_one)
    {
        XmTextFieldSetString (pTtw_data->Wig_text1, "");
    }
    else
    {
        XmTextFieldSetString (pTtw_data->Wig_text2, "");
    }

}



	/*
	 * Misc functions
	 */
void
activate_cb( Widget Wig_text, XtPointer client_data, XtPointer call_data )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) call_data;
    Widget dflt, dialog = (Widget) client_data;

    XtVaGetValues (dialog, XmNdefaultButton, &dflt, NULL);

    if (dflt)
        XtCallActionProc (dflt, "ArmAndActivate", cbs->event, NULL, 0);
}


void
V_SetActiv1( Widget Wig_text, XtPointer client_data, XtPointer call_data )
{
	P_TWOTEXTW pTtw_data = (P_TWOTEXTW) client_data;

	(*pTtw_data).F_which_one = 1;
}

void
V_SetActiv2( Widget Wig_text, XtPointer client_data, XtPointer call_data )
{
	 P_TWOTEXTW pTtw_data = (P_TWOTEXTW) client_data;

	(*pTtw_data).F_which_one = 2;
}






	/*
 	 * OK button callbacks
	 */

	/* single entry callback */
void
f_V_URL_ok( Widget w, XtPointer client_data, XtPointer call_data )
{
	P_ONETEXTW pOtw_data = (P_ONETEXTW) client_data;
	Widget      Wig_destroy = pOtw_data->Wig_parent;
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) call_data;
    char *text = XmTextFieldGetString (pOtw_data->Wig_text);

	XtDestroyWidget( Wig_destroy );
	free(pOtw_data);

	V_OpenSessionURL( text, text );
    /*XtFree (text);*/
}

void
f_V_SEARCH_ok( Widget w, XtPointer client_data, XtPointer call_data )
{
    P_ONETEXTW pOtw_data = (P_ONETEXTW) client_data;
    Widget      Wig_destroy = pOtw_data->Wig_parent;
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) call_data;
    char *text = XmTextFieldGetString (pOtw_data->Wig_text);

    XtFree (text);

    XtDestroyWidget( Wig_destroy );
    free(pOtw_data);

    doSearch(text);
}


	/* double entry callbacks */
void
f_V_FTP_ok( Widget w, XtPointer client_data, XtPointer call_data )
{
    P_TWOTEXTW pTtw_data = (P_TWOTEXTW) client_data;
	Widget      Wig_destroy = pTtw_data->Wig_parent;
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) call_data;
    char *aCh_text1 = XmTextFieldGetString ( pTtw_data->Wig_text1 );
    char *aCh_text2 = XmTextFieldGetString ( pTtw_data->Wig_text2 );

    printf ("FTP String1 = %s\nFTP String2 = %s\n", aCh_text1, aCh_text2 );
    XtFree (aCh_text1);
    XtFree (aCh_text2);

	XtDestroyWidget( Wig_destroy );
	free( pTtw_data );
}
