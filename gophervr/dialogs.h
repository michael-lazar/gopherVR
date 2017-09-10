#ifndef _DIALOGS_H_
#define _DIALOGS_H_


extern	void	V_OpenSessionURL( char *aCh_title, char *aCh_urltxt );


#define d_FIELD_WIDTH 35


typedef struct {
    char *label;
    void (*callback)();
    XtPointer   data;
} ActionAreaItem;



	/* structs used for passing data back and forth callbacks */
typedef struct tagTwoTextW
{
    int    F_which_one;
	int    F_which_help;	
    Widget Wig_text1;
	Widget Wig_text2;
	Widget Wig_parent;
} TWOTEXTW, *P_TWOTEXTW, **PP_TWOTEXTW;

typedef struct tagOneTextW
{
	int    F_which_help;
	Widget Wig_text;
	Widget Wig_parent;
} ONETEXTW, *P_ONETEXTW, **PP_ONETEXTW;


	/* structs used to init dialogs in array */
typedef struct tagTwoTextDesc
{
	char	*aCh_string1;
	char	*aCh_string2;
	void	(*f_V_callback)();
} TWOTEXTDESC, *P_TWOTEXTDESC, **PP_TWOTEXTDESC;

typedef struct tagOneTextDesc
{
	char    *aCh_string;
	void    (*f_V_callback)();
} ONETEXTDESC, *P_ONETEXTDESC, **PP_ONETEXTDESC;



void
V_OpenGeneralDiag( Widget widget, XtPointer client_data, XtPointer call_data );
void
V_OpenTwoEntryDiag( Widget widget, XtPointer client_data, XtPointer call_data );
void
CreateActionArea( P_ONETEXTW pOtw_data, void (*f_V_callback)() );
void
CreateActionArea2( P_TWOTEXTW pTtw_data, void (*f_V_callback)() );
void
V_DiagClose1( Widget w, XtPointer client_data, XtPointer call_data );
void
V_DiagClose2( Widget w, XtPointer client_data, XtPointer call_data );
void
V_ClearOneText( Widget w, XtPointer client_data, XtPointer call_data );
void
V_ClearTwoText( Widget w, XtPointer client_data, XtPointer call_data );
void
activate_cb( Widget Wig_text, XtPointer client_data, XtPointer call_data );
void
V_SetActiv1( Widget Wig_text, XtPointer client_data, XtPointer call_data );
void
V_SetActiv2( Widget Wig_text, XtPointer client_data, XtPointer call_data );



void
f_V_URL_ok( Widget w, XtPointer client_data, XtPointer call_data );
void
f_V_SEARCH_ok( Widget w, XtPointer client_data, XtPointer call_data );
void
f_V_FTP_ok( Widget w, XtPointer client_data, XtPointer call_data );


#endif /* _DIALOGS_H_ */
