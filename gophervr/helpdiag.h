#ifndef _HELPDIAG_H_
#define _HELPDIAG_H_



void V_HelpWin( Widget w, XtPointer client_data, XtPointer call_data );
void V_ScanHelpFile();
void V_TwoPercents1();
void V_GetTopicName();
void V_GetTopicBody();
void V_TwoPercents2( char rCh_desc[], int *irCh_index );
void V_AddToTopicList( char *aCh_topic, int cCh_len );
void V_AddToTopicBodyList( char *aCh_desc, int cCh_len );


#if defined(__linux__)
#define HELP_FILE "/usr/share/doc/gophervr/gophervr.help"
#else
#define HELP_FILE "./gophervr.help"
#endif
#define d_MAX_TOPIC_NAME    80
#define d_MAX_TOPIC_BODY    4096
#define m_IS_PERCENT( Ch_chr ) ( Ch_chr == '%' ) ? 1 : 0
#define m_IS_NEW_LINE( Ch_chr ) ( Ch_chr == '\n' ) ? 1 : 0


typedef struct tagTopics
{
    char    *aCh_name;
    char    *aCh_desc;
} TOPICS, *P_TOPICS, **PP_TOPICS;




#endif /* _HELPDIAG_H_ */
