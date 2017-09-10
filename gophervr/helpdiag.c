/* Written by Dan Heller and Paula Ferguson.  
 * Copyright 1994, O'Reilly & Associates, Inc.
 * Permission to use, copy, and modify this program without
 * restriction is hereby granted, as long as this copyright
 * notice appears in each copy of the program source code.
 * This program is freely distributable without licensing fees and
 * is provided without guarantee or warrantee expressed or implied.
 * This program is -not- in the public domain.
 */

#include <stdio.h>
#include <Xm/List.h>
#include <Xm/LabelG.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <assert.h>

#if !defined(SVR4) && !defined(__clang__)
#include <strings.h>
#else
# include <string.h>
#endif

#include <memory.h>
#include <stdio.h>

#include "guidefs.h"
#include "helpdiag.h"

#ifdef _AIX
#  include <regex.h>
#endif
#ifdef __sgi
#include <libgen.h>
#endif
#ifdef __OpenBSD__
/* No libcompat on OpenBSD, use the POSIX regexp. */
char *posix_re_comp(char *regex);
int posix_re_exec(char *string);
#define re_comp(a) (posix_re_comp(a))
#define re_exec(a) (posix_re_exec(a))
#endif

/* For debugging */
#define EMITTER(logic) logic ( EOF != ( Ch_chr = getc(Fp_help) ) )


extern	Widget	top;

FILE    *Fp_help;
TOPICS  Top_list [100];
int     cTop_count = 0;
int     B_once = 0;
static	B_read = 0;
Widget  sctext_w;


	/*
	 * When you modify this struct make sure the appropriate record
	 * exists in the file "gophervr.help"
	 */
char	*raCh_helpdefs [4] = 
	{	"Using Help", 
		"ftp session",
		"URLs",
		"Search" };


void
V_HelpWin( Widget w, XtPointer client_data, XtPointer call_data )
{
    Widget        form_m, form1, form2, list_w, text_w, dismiss_w, pop_w;
    Widget		  label_ht, label_hd, label_sh;
    int			  iraCh_index = (int) client_data;
    Arg           args[9];
    int           n = 0, i = 0;
    int			  width, height;
    Position	  x, y;
    XmString      slabel_ht, slabel_hd, slabel_sh, sdismiss;
    void          add_item(), search_item(), sel_callback(), close_help();

    /* create PopUpshell */
    
    pop_w = XtVaCreatePopupShell( "hn", transientShellWidgetClass, top, NULL );

    /* create form */
    form_m = XtVaCreateWidget ("help_screen", xmFormWidgetClass, pop_w, NULL );
    
    /*
     * Help Topics
     */
    slabel_ht = XmStringCreateSimple ("Help Topics:");
    label_ht = XtVaCreateManagedWidget ("list_lable", 
					xmLabelWidgetClass, form_m,
					XmNtopAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_FORM,
					XmNlabelString, slabel_ht, 
					XmNtopOffset, 10,
					XmNleftOffset, 10,
					NULL);
    XmStringFree (slabel_ht);
    
    form1 = XtVaCreateWidget ("form", 
			      xmFormWidgetClass, form_m,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, label_ht,
			      XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			      XmNleftWidget, label_ht,
			      XmNtopOffset, 5,
			      NULL);
    XtManageChild (form1);
    XtSetArg (args[n], XmNvisibleItemCount, 10); n++;
    list_w = XmCreateScrolledList (form1, "scrolled_list", args, n);
    XtAddCallback (list_w, XmNdefaultActionCallback, sel_callback, NULL);
    XtAddCallback (list_w, XmNbrowseSelectionCallback, sel_callback, NULL);
    XtManageChild (list_w);
    
    /* check to see that we can open the help file before going further */
    if (0 == B_read)
    {
	 if ( NULL == ( Fp_help = fopen( HELP_FILE, "r" ) ) )
        {
	     printf( "Error: Cannot Open Help File.\n" );
	     return;
        }
	 else
	 {
#ifndef __sgi
	      V_ScanHelpFile();
#endif
	 }
	 
	 B_read = 1;
    }
    
    /* add the list of help topics */
    for( i=0; i < cTop_count; i++ )
    {
	 add_item( list_w, Top_list [i].aCh_name );
    }
    
    /*
     * Help Descriptions
     */
#ifdef __sgi
    slabel_hd = XmStringCreateSimple ("Help not yet implemented for Irix");
#else
    slabel_hd = XmStringCreateSimple ("Help Descriptions:");
#endif

    label_hd = XtVaCreateManagedWidget ("list_lable",
					xmLabelWidgetClass, form_m,
					XmNleftAttachment, XmATTACH_WIDGET,
					XmNleftWidget, form1,
					XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
					XmNtopWidget, label_ht,
					XmNlabelString, slabel_hd, 
					XmNleftOffset, 50,
					NULL);
    XmStringFree (slabel_hd);
	
    form2 = XtVaCreateWidget ("form", 
			      xmFormWidgetClass, form_m,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, label_hd,
			      XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
			      XmNleftWidget, label_hd,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNtopOffset, 5,
			      XmNrightOffset, 10,
			      NULL);
    XtManageChild (form2);
    n = 0;
    XtSetArg(args[n], XmNrows,      10); n++;
    XtSetArg(args[n], XmNcolumns,   60); n++; 
    XtSetArg(args[n], XmNeditable,  False); n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
    XtSetArg(args[n], XmNwordWrap,  True); n++;
    XtSetArg(args[n], XmNtitle, "GopherVR Help"); n++;
    sctext_w = XmCreateScrolledText( form2, "sc_text", args, n );

    XtVaSetValues(sctext_w,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  NULL);

    XtManageChild (sctext_w);
    
    /*
     * Search for help
     */ 
    slabel_sh = XmStringCreateSimple ("Search for Topic:");
    label_sh = XtVaCreateManagedWidget ("search_label", 
					xmLabelWidgetClass, form_m,
					XmNtopAttachment, XmATTACH_WIDGET,
					XmNtopWidget, form1,
					XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
					XmNleftWidget, form1,
					XmNlabelString, slabel_sh, 
					XmNtopOffset, 20,
					NULL);
    XmStringFree (slabel_sh);
    text_w = XtVaCreateManagedWidget ("search_text",
				      xmTextFieldWidgetClass, form_m,
				      XmNbottomAttachment, XmATTACH_FORM,
				      XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
				      XmNleftWidget, label_sh,
				      XmNtopAttachment, XmATTACH_WIDGET,
				      XmNtopWidget, label_sh,
				      XmNtopOffset, 5,
				      XmNbottomOffset, 10,
				      XmNcolumns, 25, 
				      NULL);
    XtAddCallback (text_w, XmNactivateCallback, search_item, list_w);
    XmTextFieldSetString ( text_w, raCh_helpdefs [iraCh_index] );
    
    
    /*
     * dismiss
     */
    sdismiss = XmStringCreateSimple ( "Dismiss" );
    dismiss_w = XtVaCreateManagedWidget ("dismiss",
					 xmPushButtonWidgetClass, form_m,
					 XmNlabelString, sdismiss,
					 XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
					 XmNrightWidget, form2,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNbottomOffset, 10,
					 NULL);
    XmStringFree (sdismiss);
    XtAddCallback (dismiss_w, XmNactivateCallback, close_help, pop_w);
    XtManageChild (dismiss_w);
    
    
    XtManageChild (form_m);
    XtPopup ( pop_w, XtGrabNonexclusive );
}





/* Add item to the list in alphabetical order.  Perform binary
 * search to find the correct location for the new item position.
 * This is the callback routine for the Add: TextField widget.
 */
void
add_item( Widget list_w, char *newtext )
{
    char *text;
    XmString str, *strlist;
    int u_bound, l_bound = 0;


    if (!newtext || !*newtext) {
        /* non-null strings must be entered */
        /*XtFree (newtext);*/
        return;
    }

    XtVaGetValues (list_w, XmNitemCount, &u_bound, XmNitems, &strlist, NULL);
    u_bound--;

    /* perform binary search */
    while (u_bound >= l_bound) {
        int i = l_bound + (u_bound - l_bound)/2;
        if (!XmStringGetLtoR (strlist[i], XmFONTLIST_DEFAULT_TAG, &text))
            break;
        if (strcmp (text, newtext) > 0)
            u_bound = i-1; /* newtext comes before item */
        else
            l_bound = i+1; /* newtext comes after item */
        XtFree (text);
    }
    str = XmStringCreateSimple (newtext);
    /*XtFree (newtext);*/
    /* positions indexes start at 1, so increment accordingly */
    XmListAddItemUnselected (list_w, str, l_bound+1);
    XmStringFree (str);
}






#if !defined(SYSV) || defined(_AIX)
    extern char *re_comp();
#endif /* SYSV */


/* find the item in the list that matches the specified pattern */
void
search_item( Widget text_w, XtPointer client_data, XtPointer call_data )
{
    Widget list_w = (Widget) client_data;
    char *exp, *text, *newtext = XmTextFieldGetString (text_w);
    XmString *strlist, *selectlist = NULL;
    int matched, cnt, j = 0;

    if (!newtext || !*newtext) {
        /* non-null strings must be entered */
        XtFree (newtext);
        return;
    }

    /* compile expression into pattern matching library */
#if defined(SYSV) && !defined(_AIX)
    if (!(exp = regcmp (newtext, NULL))) {
        printf ("Error with regcmp(%s)\n", newtext);
        XtFree (newtext);
        return;
    }
#else /* BSD */
    if (exp = re_comp (newtext)) {
        printf ("Error with re_comp(%s): %s\n", newtext, exp);
        XtFree (newtext);
        return;
    }
#endif /* SYSV */

    /* get all the items in the list ... we're going to search each one */
    XtVaGetValues (list_w,
		   XmNitemCount, &cnt,
		   XmNitems,     &strlist,
		   NULL);
    while (cnt--) {
	 /* convert item to C string */
	 if (!XmStringGetLtoR (strlist[cnt], XmFONTLIST_DEFAULT_TAG, &text))
	      break;
	 /* do pattern match against search string */
#if defined(SYSV) && !defined(_AIX)
	 /* returns NULL if match failed */
	 matched = regex (exp, text, NULL) != NULL;
#else  /* BSD */
	 /* -1 on error, 0 if no-match, 1 if match */
	 matched = re_exec (text) > 0;
#endif /* SYSV */
	 if (matched) {
	      selectlist = (XmString *) XtRealloc (selectlist,
						   (j+1) * (sizeof (XmString *)));
	      selectlist[j++] = XmStringCopy (strlist[cnt]);
	 }
	 XtFree (text);
    }
#ifdef SYSV
    free (exp);  /* this must be freed for regcmp() */
#endif /* SYSV */
    XtFree (newtext);
    /* set the actual selected items to be those that matched */
    XtVaSetValues (list_w,
		   XmNselectedItems,     selectlist,
		   XmNselectedItemCount, j,
		   NULL);
    while (j--)
	 XmStringFree (selectlist[j]);
    XmTextFieldSetString (text_w, "");
}





void
sel_callback( Widget list_w, XtPointer client_data, XtPointer call_data )
{
     int i=0;

     XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
     char *choice;

     XmStringGetLtoR (cbs->item, XmFONTLIST_DEFAULT_TAG, &choice);
     /*printf ("selected item: %s (%d)\n", choice, cbs->item_position);*/

     for ( i=0; i < cTop_count; i++ )
     {
	  if ( 0 == strcmp( choice, Top_list [i].aCh_name ) )
	  {
	       XmTextSetString (sctext_w, Top_list [i].aCh_desc );
	  }
     }

     XtFree (choice);
}





void
close_help( Widget w, XtPointer client_data, XtPointer call_data )
{
     Widget shell = (Widget) client_data;

     XtDestroyWidget (shell);
}







void
V_ScanHelpFile( void )
{
     int	Ch_chr;

     EMITTER(while)
     {
	  if ( m_IS_PERCENT( Ch_chr ) )
	  {	
	       V_TwoPercents1();
	  }
     }
}

void
V_TwoPercents1( void )
{
     int Ch_chr;

     EMITTER(if)
     {
	  if ( m_IS_PERCENT( Ch_chr ) )
	  {
	       V_GetTopicName();
	  }
     }
}



void
V_GetTopicName( void )
{
    int 	Ch_chr;
     char	rCh_name [d_MAX_TOPIC_NAME];
     int		irCh_index = 0;


     memset( rCh_name, '\0', d_MAX_TOPIC_NAME );
     EMITTER(while)
     {
	  assert(irCh_index < d_MAX_TOPIC_NAME);
	  if ( m_IS_NEW_LINE( Ch_chr ) )
	  {               
	       /* did we actually get a topic name */
	       if (0 == irCh_index)
		    return;

	       rCh_name [irCh_index] = '\0';
	       V_AddToTopicList( rCh_name, irCh_index );
	       V_GetTopicBody();			
	       return;
	  }
	  else
	  {
	       rCh_name [irCh_index] = (char)Ch_chr; irCh_index++;
	  }
     }
}




void
V_GetTopicBody( void )
{
     int     Ch_chr;
     char    rCh_desc [d_MAX_TOPIC_BODY];
     int     irCh_index = 0;

     memset( rCh_desc, '\0', d_MAX_TOPIC_BODY );
     EMITTER(while)
     {
	  assert(irCh_index < d_MAX_TOPIC_BODY);
	  if ( m_IS_NEW_LINE( Ch_chr ) )
	  {               
	       rCh_desc [irCh_index] = (char)Ch_chr; irCh_index++;
	       V_TwoPercents2( rCh_desc, &irCh_index );
	  }
	  else
	  { 

	       rCh_desc [irCh_index] = (char)Ch_chr; irCh_index++;
	  }
     }

     /* 
      * When we get to the EOF the callstack will begin to unravel,
      * we only want to call this once
      */
     if (0 == B_once)
     {
	  V_AddToTopicBodyList( rCh_desc, irCh_index );
	  B_once = 1;
     }

}



void
V_TwoPercents2( char rCh_desc[], int *irCh_index )
{
     int Ch_chr;

     /* Get first (if any) % */
     EMITTER(if)
     {
	  if ( m_IS_PERCENT( Ch_chr ) )
	  {

	       rCh_desc[*irCh_index] = (char)Ch_chr; (*irCh_index)++;

	       /* Get second (if any) % */
               EMITTER(if)
	       {
		    if ( m_IS_PERCENT( Ch_chr ) )
		    {
			 rCh_desc[(*irCh_index)-1] = '\0';
			 V_AddToTopicBodyList( rCh_desc, *irCh_index );
			 V_GetTopicName( );
		    }
		    else
		    {
			 rCh_desc[*irCh_index] = (char)Ch_chr; (*irCh_index)++;
			 return;
		    }
	       }
	  }
	  else
	  {
	       ungetc( Ch_chr, Fp_help );
	       return;
	  }
     }
}




void
V_AddToTopicList( char *aCh_topic, int cCh_len )
{
     Top_list [cTop_count].aCh_name = (char *) malloc( sizeof(char) * cCh_len);
     strncpy( Top_list [cTop_count].aCh_name, aCh_topic, cCh_len );
     Top_list [cTop_count].aCh_name [cCh_len] = '\0';
}


void
V_AddToTopicBodyList( char *aCh_desc, int cCh_len )
{
     Top_list [cTop_count].aCh_desc = (char *) malloc( sizeof(char) * cCh_len);
     strncpy( Top_list [cTop_count].aCh_desc, aCh_desc, cCh_len );
     Top_list [cTop_count].aCh_desc[cCh_len] = '\0';

     cTop_count++;
}
