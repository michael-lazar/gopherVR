#ifndef _TEXT_H_
#define _TEXT_H_


/* 
 * Prototypes for GTXT routines
 */

void  displayTempFile(Widget topLevel, char *title, char *fileName);
void GTXTcleanUpTextProc(void);

int 
is_writable( char *file );
void 
V_FileSelDiag( Widget Wig_diag, XtPointer client_data, XtPointer call_data );
void 
new_file_cb( Widget widget, XtPointer client_data, XtPointer call_data );
void 
new_file_cb( Widget widget, XtPointer client_data, XtPointer call_data );
void 
do_search( Widget widget, XtPointer search_data );




#endif /* _TEXT_H_ */
