#ifndef _GOPHWIN_H_
#define _GOPHWIN_H_


#define d_MAX_HISTORY	25


void
V_GenMenuListWin( void );

void
V_GenMenuListing( char *aCh_text, int N_pos );

void
V_DelMenuListing( void );

void
V_SetMenuTitle( char *aCh_title );

static void
V_SelCall( /*Widget w, XtPointer p1, XtPointer p2 */ );

static void
V_SelBrowse( /*Widget Wig_list, XtPointer client_data, XtPointer call_data*/ );

static void
V_SelCall2( /* Widget Wig_list, XtPointer client_data, XtPointer call_data */ );

static void
V_HisBrowse( /*Widget Wig_list, XtPointer client_data, XtPointer call_data */);

static void
V_HisCall( /*Widget w, XtPointer p1, XtPointer p2 */ );

void
V_GenHistListing( char *aCh_title, char *aCh_url );

void
V_DestroyHistoryData( void );

void
V_RememberListWinSize (void);

#endif /* _GOPHWIN_H_ */
