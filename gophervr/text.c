/* text.c
   make and manage the text popups */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h> 
#include <Xm/Protocols.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <X11/Xos.h>
#include <Xm/RowColumn.h>
#include <Xm/DialogS.h>
#include <sys/param.h>		/* For MAXPATHLEN */
/*#include "resources.h"*/
#include "menus.h"
#include "motiftools.h"
#include "text.h"

extern  Widget  top;

#define	PERCENT	'%'

typedef struct _textElement {
     Widget		textShell;
     Widget		textDisplay;
     Boolean		used;
     Boolean		deleteFile;
     char		*displayedFile;
     char		**wordList;
     char		*stringValue;
     struct _textElement *next;
} textElement, *textElementP;

static	textElementP	textElementHead = NULL;

/***********************************************************************
 * Prototypes 
 */
static void MenuCut(Widget, XtPointer, XtPointer);
static void MenuCopy(Widget widget, XtPointer client_data, XtPointer);
static void MenuPaste(Widget widget, XtPointer client_data, XtPointer);
static void MenuClear(Widget widget, XtPointer client_data, XtPointer);

static void GTXTdone(Widget, XtPointer , XtPointer);
static void GTXTdone(Widget , XtPointer, XtPointer);
static void GTXTprint(Widget w, XtPointer client_data, XtPointer );
static void GTXTsave(Widget w, XtPointer client_data, XtPointer call_data);

/***********************************************************************
 * MenuStuff..
 */

static MenuItem FileMenuItems[] = {
{"Open Location...", &xmPushButtonGadgetClass, 'O', "Alt<Key>O", "Alt-O", 
      NULL, NULL, NULL},
{"Close Window", &xmPushButtonGadgetClass, 'W', "Alt<Key>W", "Alt-W", 
      GTXTdone, NULL, NULL},

/* At some point we should coalesce motiftools.c filepicker with this one. */

{"Save As...", &xmPushButtonGadgetClass, 'P', "Alt<Key>P", "Alt-P", 
/*      GTXTsave, NULL, NULL}, */
		V_FileSelDiag, NULL, NULL},
{"Print...", &xmPushButtonGadgetClass, 'P', "Alt<Key>P", "Alt-P", 
      GTXTprint, NULL, NULL},
{"", &xmSeparatorGadgetClass, '\0', NULL, NULL,
      NULL, NULL, NULL},
{"Quit", &xmPushButtonGadgetClass, 'Q', "Alt<Key>Q", "Alt-Q", 
      MenuQuit_CB, (XtPointer)4, NULL},
NULL
};

static MenuItem EditMenuItems[] = {
{"Undo", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
      NULL, NULL, NULL },
{"", &xmSeparatorGadgetClass, '\0', NULL, NULL,
      NULL, NULL, NULL},
{"Cut", &xmPushButtonGadgetClass, 'X', "Alt<Key>X", "Alt-X", 
      MenuCut, NULL, NULL},
{"Copy", &xmPushButtonGadgetClass, 'C', "Alt<Key>C", "Alt-C",
      MenuCopy, "xx", NULL},
{"Paste", &xmPushButtonGadgetClass, 'V', "Alt<Key>V", "Alt-V", 
      MenuPaste, NULL, NULL},
{"Clear", &xmPushButtonGadgetClass, '\0', NULL, NULL, 
      MenuClear, NULL, NULL},
NULL
};

/**********************************************************************/
static void MenuCut(Widget widget, XtPointer client_data, XtPointer call_data)
{
     textElementP  tep = (textElementP) client_data;
     Boolean result = True;

     if (tep == NULL) 
	  return;

     result = XmTextCut(tep->textDisplay, CurrentTime);
}

/**********************************************************************/
static void MenuCopy(Widget widget, XtPointer client_data, XtPointer call_data)
{
     textElementP  tep = (textElementP) client_data;
     Boolean result = True;

     if (tep == NULL) 
	  return;

     result = XmTextCopy(tep->textDisplay, CurrentTime);
}

/**********************************************************************/
static void MenuPaste(Widget widget, XtPointer client_data, XtPointer call_data)
{
     textElementP  tep = (textElementP) client_data;
     Boolean result = True;

     if (tep == NULL) 
	  return;

     result = XmTextPaste(tep->textDisplay);
}

/**********************************************************************/
static void MenuClear(Widget widget, XtPointer client_data, XtPointer call_data)
{
     textElementP  tep = (textElementP) client_data;
     Boolean result = True;

     if (tep == NULL) 
	  return;

      XmTextClearSelection(tep->textDisplay, CurrentTime);
     return;
}
/**********************************************************************/

/* textStringToFile
   if a text widget has a string, rather than file value, copy the
   string to a file, and set the structure to reflect this so things
   will be cleaned up. */

static Boolean
textStringToFile(textElementP tep)
{
     char	*tempfile;
     FILE	*fp;

     tempfile = (char*) tempnam(NULL, NULL);
     if ((fp = fopen (tempfile, "w")) == NULL) {
	  return False;
     }
     fwrite (tep->stringValue, sizeof(char),
	     strlen(tep->stringValue), fp);
     fclose(fp);
     tep->displayedFile = tempfile;
     tep->deleteFile = True;
     free(tep->stringValue);	/* free(), not XtFree() */
     tep->stringValue = NULL;

     return True;
}


/**********************************************************************/

Widget
GTXTmenubar(Widget parent, textElementP tep)
{
     Widget menubar;
     int i;

     menubar = XmCreateMenuBar (parent, "textmenubar", NULL, 0);
     
     for (i=0; FileMenuItems[i].label != NULL; i++) {
	  FileMenuItems[i].callback_data = (XtPointer) tep;
     }
     BuildMenu(menubar, XmMENU_PULLDOWN, "File", 'F', True, FileMenuItems);
     
     for (i=0; EditMenuItems[i].label != NULL; i++) {
	  EditMenuItems[i].callback_data = (XtPointer) tep;
     }
     BuildMenu(menubar, XmMENU_PULLDOWN, "Edit", 'E', True, EditMenuItems);
     BuildMenu(menubar, XmMENU_PULLDOWN, "Help", 'H', True, StandardHelpMenu);
     XtManageChild(menubar);

     return(menubar);

}


/**********************************************************************
 * printProc
 * send the contents of a text widget to the printer */

void GTXTprint(Widget w, XtPointer client_data, XtPointer call_data)
{
     textElementP	tep = (textElementP) client_data;
     char		*fileName;
     char		printCommand[MAXPATHLEN];
     char		errorMessage[256];
     char		*out, *in, *f;
     Boolean		hasFile = False;


     if (tep->displayedFile == NULL) {
	  if (! textStringToFile(tep) ) {
	       showError(
			 "Text cannot be printed because a temporary file cannot be created.");
	       return;
	  }
     }

     fileName = tep->displayedFile;
	
     /* everywhere the print command string contains the letters
	'%s', the filename is substituted.  Other uses of '%'
	are not special.  If the print command does not contain
	at least one '%s', then the filename is added at the end
	after a space. */

     /*	out = printCommand,
	in  = appResources->printCommand;*/
     while(*in != '\0') {
	  if (*in != PERCENT) {
	       *(out++) = *(in++);
	  } else {
	       ++in;
	       if (*in == 's') {
		    ++in;
		    f = fileName;
		    while (*f != '\0') {
			 *(out++) = *(f++);
		    }
		    hasFile = True;
	       } else {
		    *(out++) = PERCENT;
		    *(out++) = *(in++);
	       }
	  }
     }

     if (! hasFile) {
	  *(out++) = ' ';
	  f = fileName;
	  while (*f != NULL) {
	       *(out++) = *(f++);
	  }
     }

     *(out++) = '\0';

     if (system(printCommand) == 127) {
	  sprintf (errorMessage,
		   "The print command could not be executed:\n%s",
		   printCommand);
	  showError(errorMessage);
     }

     return;
}


/**********************************************************************
 * saveProc
 *  save the contents of a text widget to a file 
 */

void GTXTsave(Widget	w, XtPointer client_data, XtPointer call_data)
{
     textElementP	tep = (textElementP) client_data;

     /*	if (! appResources->allowSave) { return; } */
     if (tep->displayedFile == NULL) {
	  if (! textStringToFile(tep) ) {
	       showError(
			 "Text cannot be saved because a temporary file cannot be created.");
	       return;
	  }
     }

     if (tep == NULL) return;
	
     /*	saveRequest(tep->textShell, tep->displayedFile);*/
}


/**********************************************************************
*  Done showing text file in a popup, clean up 
*/

void
GTXTdone(Widget  w, XtPointer	client_data, XtPointer cbs)
{
     textElementP	tep = (textElementP) client_data;

/*
fprintf(stderr, "I got called. tep->used: %s\n", (tep->used == True) ? "True" : "False");
*/

     if (tep == NULL) 
	  return;

     XtPopdown(tep->textShell);

     if (tep->deleteFile)
	  if (unlink(tep->displayedFile) != 0) {
	       fprintf (stderr,
			"Warning: a gopher internal file could not be removed.\n");
	       fprintf (stderr,
			"         This may indicate a system error.\n");
	  }
     
     tep->used          = False;
     if (tep->displayedFile != NULL) {
	  XtFree(tep->displayedFile);	
	  tep->displayedFile = NULL;
     }
     /*	freeWordList(tep->wordList);*/
     tep->wordList      = NULL;
     tep->deleteFile    = False;
     free(tep->stringValue);	/* free(), not XtFree() */
     tep->stringValue   = NULL;

     XmTextReplace(tep->textDisplay, 0, 
		   XmTextGetLastPosition(tep->textDisplay), NULL);
	
/*
fprintf(stderr, "I got called. tep->used: %s\n", (tep->used == True) ? "True" : "False");
*/

     return;
}


/**********************************************************************
* cleanUpTextProc
*   Allow a final chance to remove all the temp files text knows about. */

void GTXTcleanUpTextProc(void)
{
     textElementP	tep;


     for (tep=textElementHead; tep != NULL; tep = tep->next) 
	  if (tep->used && tep->deleteFile)
	       (void) unlink(tep->displayedFile);
	
     return;
}


/**********************************************************************
 * createTextShell
 * Build a text popup 
 */

textElementP
createTextShell(Widget topLevel)
{
     Widget		menubar, textForm;
     Arg		args[10];
     Cardinal	        n;
     textElementP	tep;
     XmString           donetxt = NULL;
     Atom               WM_DELETE_WINDOW;
     int                screenx, screeny;

     if ((tep = (textElementP) malloc(sizeof(textElement))) == NULL) {
	  showError("Too many windows are already open to show this file.");
	  return NULL;
     }
     tep->next = textElementHead;
     textElementHead = tep;
     tep->used = True;

     /* create TEXT display popup */
     n=0;
     XtSetArg(args[n], XtNtitle, "Gopher Text");  n++;
     tep->textShell = XtCreatePopupShell("textShell",
					 topLevelShellWidgetClass,
					 topLevel, args, n);

     
     /*** create TEXT FORM */
     textForm = XtCreateManagedWidget("textForm", xmFormWidgetClass,
				      tep->textShell, NULL, 0);


     /*** Call GTXTdone if we get a delete from the window manager */
     WM_DELETE_WINDOW = XmInternAtom(XtDisplay(tep->textShell),
				     "WM_DELETE_WINDOW", False);
     XmRemoveWMProtocolCallback(tep->textShell, WM_DELETE_WINDOW, GTXTdone,
			     tep);
     XmAddWMProtocolCallback(tep->textShell, WM_DELETE_WINDOW, GTXTdone,
			     tep);

     /*** Add a menubar */
     menubar = GTXTmenubar(textForm, tep);

     XtVaSetValues(menubar,
		   XmNtopAttachment,   XmATTACH_FORM,
		   XmNleftAttachment,  XmATTACH_FORM,
		   XmNrightAttachment, XmATTACH_FORM,
		   NULL);


     /*** create TEXT display */

     XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
     XtSetArg(args[n], XmNeditable, True); n++;
     XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
     XtSetArg(args[n], XmNwordWrap, True); n++;
     XtSetArg(args[n], XmNrows, 24); n++;
     XtSetArg(args[n], XmNcolumns, 80); n++;

     tep->textDisplay = XmCreateScrolledText(textForm, "textDisplay",
					     args, n);

     XtManageChild(tep->textDisplay);

     XtVaSetValues(XtParent(tep->textDisplay),
		   XmNtopAttachment, XmATTACH_WIDGET,
		   XmNtopWidget, menubar,
		   XmNleftAttachment, XmATTACH_FORM,
		   XmNrightAttachment, XmATTACH_FORM,
		   XmNbottomAttachment, XmATTACH_FORM,
		   NULL);



     return tep;
}


/**********************************************************************
 * displayTempFile
 * Load a text popup with a temp file 
 */

void  displayTempFile(Widget topLevel, char *title, char *fileName)
{
     Widget scrollbar;
     Cardinal	n;
     char		*textFileName;
     textElementP	tep;
     FILE            *fd;
     char            buff[512];

     for (tep=textElementHead; (tep!=NULL && tep->used); tep = tep->next);

/* I suspect I'm introducing a memory leak here, but I'm not sure how else to
   fix this crash bug where if the wm closes the window, then this routine
   tries to use the null window (but if we always create a new text shell,
   then we don't crash because we are guaranteed to have one). -- Cameron */

/*
     if (tep == NULL) {
*/
	  if ((tep = createTextShell(topLevel)) == NULL) return;
/*
	fprintf(stderr, "created text shell\n");
     }
fprintf(stderr, "continuing on\n");
*/

     if ((textFileName = (char *) XtMalloc(strlen(fileName)+1)) ==
	 NULL) {
	  showError("Unable to allocate additional memory.");
	  return;
     }
     strcpy(textFileName, fileName);
     tep->displayedFile = textFileName;
     tep->deleteFile    = True;
     tep->used = True;
     tep->wordList = NULL;
     tep->stringValue = NULL;

     /* set title and file name */

     XtVaSetValues(tep->textShell,
		   XtNtitle, title,
		   NULL);

     fd = fopen(textFileName, "r");

     n = 0;
     XtPopup(tep->textShell, XtGrabNone);

     while (fgets(buff, sizeof(buff), fd) != NULL)
     {
	  XmTextInsert(tep->textDisplay, n, buff);
	  n += strlen(buff);
     }
     fclose(fd);
/* pulled this out of the loop. no reason to update each time. */
     XmUpdateDisplay(tep->textDisplay);


	/* A side effect that the widget is now at the bottom, so we
		find our vertical scroll bar, if there is one, and
		force us back to the top. */

	XtVaGetValues(XtParent(tep->textDisplay), XmNverticalScrollBar,
		&scrollbar, NULL);
	if (scrollbar != NULL) {
		int min;

		/* Set to our minimum. */
		XtVaGetValues(scrollbar, XmNminimum, &min, NULL);
		XmScrollBarSetValues(scrollbar, min, 0, 0, 0, True);
	}
     
     return;
}


/* displayIndexTempFile
 * Load a text popup with a temp file and index words for highlighting 
 */

void
displayIndexTempFile(topLevel, title, fileName, indexString)
  Widget	topLevel;
  char	*title;
  char	*fileName;
  char	*indexString;
{
     Cardinal	n;
     char		*textFileName;
     textElementP	tep;
     FILE            *fd;
     char            buff[512];

     for (tep=textElementHead; (tep!=NULL && tep->used); tep = tep->next);

     if (tep == NULL) {
	  if ((tep = createTextShell(topLevel)) == NULL) return;
     }

     if ((textFileName = (char *) XtMalloc(strlen(fileName)+1)) ==
	 NULL) {
	  showError("Unable to allocate additional memory.");
	  return;
     }
     strcpy(textFileName, fileName);
     tep->displayedFile = textFileName;
     tep->deleteFile    = True;
     tep->used = True;
     tep->stringValue = NULL;

     /* set title and file name */

     XtVaSetValues(tep->textShell,
		   XtNtitle, title,
		   NULL);

     fd = fopen(textFileName, "r");

     n = 0;
     while (fgets(buff, sizeof(buff), fd) != NULL)
     {
	  XmTextReplace(tep->textDisplay, n, n, buff);
	  n += strlen(buff);
     }

     fclose(fd);

     XtPopup(tep->textShell, XtGrabNone);
     return;
}


/* displayTextString
   Load a text popup with the contents of a text string */

void
displayTextString(topLevel, title, string)
  Widget	topLevel;
  char	*title;
  char	*string;
{
     textElementP	tep;

     for (tep=textElementHead; (tep!=NULL && tep->used); tep = tep->next);

     if (tep == NULL) {
	  if ((tep = createTextShell(topLevel)) == NULL) return;
     }

     tep->displayedFile = NULL;
     tep->deleteFile    = False;
     tep->used	   = True;
     tep->wordList	   = NULL;
     tep->stringValue   = string;

     /* set title and file name */

     XtVaSetValues(tep->textShell,
		   XtNtitle, title,
		   NULL);


     XtPopup(tep->textShell, XtGrabNone);

     XmTextInsert(tep->textDisplay, 0, string);
     XmUpdateDisplay(tep->textDisplay);

     return;
}


showError(parm)
  char *parm;
{
     fprintf(stderr, "%s\n", parm);
}





	/*
	 * Creates FileSelection Dialog Box
	 */

/* routine to determine if a file is accessible, a directory,
 * or writable.  Return -1 on all errors or if the file is not
 * writable.  Return 0 if it's a directory or 1 if it's a plain
 * writable file.
 */
int
is_writable( char *file )
{
     struct stat s_buf;

    /* if file can't be accessed (via stat()) return. */
    if (stat (file, &s_buf) == -1)
        return -1;
    else if ((s_buf.st_mode & S_IFMT) == S_IFDIR)
        return 0; /* a directory */
    else if (!(s_buf.st_mode & S_IFREG) || access (file, W_OK) == -1)
        /* not a normal file or it is not writable */
        return -1;
    /* legitimate file */
    return 1;
}


void
V_FileSelDone(Widget Wig_diag, XtPointer client_data, XtPointer call_data) 
{
     XtPopdown((Widget)client_data);
}

void
V_FileSelDiag( Widget Wig_diag, XtPointer client_data, XtPointer call_data ) 
{
    Widget dialog, Wig_filetxt, Wig_filtxt;
	textElementP   tep = (textElementP) client_data;
    extern void exit();
    Arg args[5];
    int n = 0;


    XtSetArg (args[n], XmNfileSearchProc, do_search); n++;
    dialog = XmCreateFileSelectionDialog ( top, "Files", args, n);
	XtVaSetValues( dialog, 
					XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL,
					NULL );

	Wig_filetxt = XmFileSelectionBoxGetChild( dialog, XmDIALOG_TEXT );
	XtVaSetValues( Wig_filetxt,
						XmNautoShowCursorPosition, True,
						NULL );
	Wig_filtxt = XmFileSelectionBoxGetChild( dialog, XmDIALOG_FILTER_TEXT );
	XtVaSetValues( Wig_filtxt,
						XmNautoShowCursorPosition, True,
						NULL );

    XtSetSensitive (
        XmFileSelectionBoxGetChild (dialog, XmDIALOG_HELP_BUTTON), False);
    /* if user presses OK button, call new_file_cb() */
    XtAddCallback (dialog, XmNokCallback, new_file_cb, tep );
    /* if user presses Cancel button, exit program */
    XtAddCallback(dialog, XmNcancelCallback,V_FileSelDone, XtParent(dialog));

    XtManageChild (dialog);

}

/* a new file was selected -- check to see if it's readable and not
 * a directory.  If it's not readable, report an error.  If it's a
 * directory, scan it just as tho the user had typed it in the mask
 * Text field and selected "Search".
 */
void
new_file_cb( Widget widget, XtPointer client_data, XtPointer call_data )
{
	textElementP   tep = (textElementP) client_data;
    char *file;
	char *aCh_exec;
	int	 cCh_len;
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
    case -1 :
	 if (tep->displayedFile == NULL)
	 {
	      if (! textStringToFile(tep) )
	      {
		   Errormsg( "Text cannot be saved because a temporary file cannot be created.");

		   return;
	      }
	 }
	 
	 if (tep == NULL) return;
	 
	 cCh_len = strlen( tep->displayedFile ) + strlen( file ) + 7;
	 aCh_exec = (char *) malloc( sizeof( char) * cCh_len );
	 sprintf( aCh_exec, "mv %s %s", tep->displayedFile, file );
	 system( aCh_exec );
	 free( aCh_exec );
	 
            break;
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




/* do_search() -- scan a directory and report only those files that
 * are writable.  Here, we let the shell expand the (possible)
 * wildcards and return a directory listing by using popen().
 * A *real* application should -not- do this; it should use the
 * system's directory routines: opendir(), readdir() and closedir().
 */
void
do_search( Widget widget, XtPointer search_data )
{
    char          *mask, buf[BUFSIZ], *p;
    XmString       names[500]; /* maximum of 500 files in dir */
    int            i = 0;
    FILE          *pp, *popen();
    XmFileSelectionBoxCallbackStruct *cbs = 
        (XmFileSelectionBoxCallbackStruct *) search_data;

    if (!XmStringGetLtoR (cbs->mask, XmFONTLIST_DEFAULT_TAG, &mask))
        return; /* can't do anything */

    sprintf (buf, "/bin/ls %s", mask);
    XtFree (mask);
    /* let the shell read the directory and expand the filenames */
    if (!(pp = popen (buf, "r")))
        return;
    /* read output from popen() -- this will be the list of files */
    while (fgets (buf, sizeof buf, pp)) {
        if (p = index (buf, '\n'))
            *p = 0;
        /* only list files that are writable and not directories */
        if (is_writable (buf) == 1 &&
            (names[i] = XmStringCreateSimple (buf)))
            i++;
    }
    pclose (pp);
    if (i) {
        XtVaSetValues (widget,
            XmNfileListItems,      names,
            XmNfileListItemCount,  i,
            XmNdirSpec,            names[0],
            XmNlistUpdated,        True,
            NULL);
        while (i > 0)
            XmStringFree (names[--i]);
    } else
        XtVaSetValues (widget,
            XmNfileListItems,      NULL,
            XmNfileListItemCount,  0,
            XmNlistUpdated,        True,
            NULL);
}
