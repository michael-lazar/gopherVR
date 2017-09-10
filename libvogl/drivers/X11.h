#ifndef _X11_H_INC
#define _X11_H_INC

#include <X11/Xlib.h>

int  X11_backbuf( void );
void X11_mapcolor(int i, int r, int g, int b);
void X11_color(int);
int  vo_xt_window(Display *dis, Window win, int xw, int xh, Colormap);
int  vo_xt_win_size(int xw, int xh);
int  X11_init( void );
void X11_exit( void );
void X11_draw(register const int x, register const int y);
int  X11_pnt(int x, int y);
int  X11_getkey( void );
int  X11_checkkey( void );
int  X11_locator(int *wx, int *wy);
void X11_clear( void );
void X11_color(int ind);
void X11_mapcolor(int i, int r, int g, int b);
int  X11_font( char *fontfile);
void X11_char(char c);
void X11_string(char s[]);
void X11_fill(register const int n, const int x[], const int y[]);
int  X11_backbuf( void );
int  X11_swapbuf( void );
void X11_frontbuf( void );
void X11_sync( void );
void X11_setlw(short w);
void X11_setls(unsigned short lss);
int  X11_PointInPolygon( int x, int y, int npol, int xp [], int yp [] );
int  _X11_devcpy( void );


#ifdef __sgi
#define NO_MULTIBUF
#endif

#endif /* _X11_H_INC */
