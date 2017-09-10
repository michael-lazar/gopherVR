#ifndef _GOPHERTO3D_H_
#define _GOPHERTO3D_H_

int
GSGetAbstractblock( GopherObj *gs, char **aCh_abs );

int
B_GetAbstract( int N_item, char **paCh_abs );

void
V_OpenSessionURL( char *aCh_title, char *aCh_urltxt );

void
V_GenHistUrlListing( char *aCh_url );

char *
aCh_GetHisUrl( int iSta_picked );

void
V_RemoveHisUrl( void );

void
V_PopDirectory( void );


#endif /* _GOPHERTO3D_H_ */
