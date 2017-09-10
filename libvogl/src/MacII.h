/********************************************************************
 * $Id: MacII.h,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

/* prototypes for MacII.c */
void SetUpColors( void );
void InitMacintosh(void);
void InitMacintosh(void);
void MacII_exit( void );
void MacII_draw(int x, int y);
int MacII_getkey( void );
int MacII_checkkey( void );
int MacII_locator(int *wx, int *wy);
void MacII_clear(void);
void MacII_color(int ind);
void MacII_mapcolor(int i, int r, int g, int b);
int MacII_font(char *fontfile);
void MacII_char(char c);
void MacII_string( char	s[] );
void MacII_fill(int n, int x[], int y[]);
int MacII_PointInPolygon( int ScreenX, int ScreenY, int n, int x[], int y[]);
int MacII_backbuf( void );
void MacII_frontb( void );
void MacII_noop( void );
void MacII_setls(Linestyle lstyle);
void MacII_setlw(short lwidth);
int MacII_swapbuf(void);
void _MacII_devcpy(void);
int MacII_init(void); 
