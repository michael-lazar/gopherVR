#ifndef _HTEXT_H_
#define _HTEXT_H_

typedef struct tagLINETEXT
{
	float	R_move [2];
	float	R_draw [2];

	struct	tagLINETEXT	*pLtx_next;
	struct  tagLINETEXT *pLtx_previous;
} LINETEXT, *P_LINETEXT, **PP_LINETEXT;


/* protos for htext.c */
void hfont(char *name);
int hnumchars( void );
void hsetpath(char *path);
void hgetcharsize(char c, float *width, float *height);
void hdrawchar(int c);
void htextsize(float width, float height);
float hgetfontwidth(void);
float hgetfontheight(void);
void hgetfontsize(float *cw, float *ch);
float hgetdecender(void);
float hgetascender(void);
void hcharstr(char *string);
float hstrlength(char *s);
void hboxfit(float l, float h, int nchars);
void hcentertext(int onoff);
void hrightjustify(int onoff);
void hleftjustify(int onoff);
void hfixedwidth(int onoff);
void htextang(float ang);
void hboxtext(float x, float y, float l, float h, char *s, 
				int B_pending, int C_lines );

#endif /* _HTEXT_H_ */

