/********************************************************************
 * $Id: text.c,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#ifdef UNIX
#include "../hershey/src/htext.h"
#else
#include "htext.h"
#endif

#ifdef PC
#include <string.h>
#else
#ifdef SYS5
#include <string.h>
#define rindex strrchr
#else
#include <strings.h>
#endif
#endif
#include "vogl.h"
#include "text.h"
#include "valloc.h"


extern int			g_B_text_waiting;
extern TEXTATTR		g_Txt_curr_text;
extern int          cLtx_lines;

static	Vector	cpos;	/* Current character position */
static	int	sc_x, sc_y;

static void V_CopyString( char *, char *, int, int );
static void V_CopyStringP( char *aCh_original, char *aCh_new, 
 			int iaCh_start, int iaCh_end, int cCh_pad );
static int  N_SplitString( char *Sz_string, char *aCh_lhs, char *aCh_rhs );




/*
 * font
 * 	assigns a font.
 */
void font(short id)
{

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("font: vogl not initialised");
#endif


	if (id < 0 || id >= vdevice.maxfontnum)
		verror("font: font number is out of range");
	
	if (vdevice.inobject) {
	     Token *tok = newtokens(2);
	     tok[0].i = VFONT;
	     tok[1].i = id;
	     return;
	}

	if (id == 1) {
		if (!(*vdevice.dev.Vfont)(vdevice.dev.large)) 
			verror("font: unable to open large font");
	} else if (id == 0) {
		if (!(*vdevice.dev.Vfont)(vdevice.dev.small))
			verror("font: unable to open small font");
	}

	vdevice.attr->a.fontnum = id;
}

/*
 * charstr - Draw a string from the current character position.
 *
 *   We really modified this puppy. 
 *					- gopher team
 *   And I'm modifying it again.
 *                                      - Cameron Kaiser
 *
 *   Application programs call charstr() which in turn calls
 *	 charstr_backend()
 *
 *   The "parse" loop in objects.c in turn also calls charstr_backend()
 */
void charstr( long rlN_vec[3], float R_angle, int F_color, float R_height, \
               float R_length, char *aCh_str )
{

	charstr_backend( R_height, R_length, F_color, (float) rlN_vec [V_X], 
				(float) rlN_vec [V_Y], (float) rlN_vec [V_Z], R_angle,
				aCh_str );
}



void charstr_backend(float R_height, float R_length, int F_color, 
                     float R_vec_x, float R_vec_y, float R_vec_z, 
                     float R_angle, char *aCh_str)

{


#ifdef VDEVICECHECK
	if(!vdevice.initialised) 
		verror("charstr: vogl not initialized");
#endif


	if (vdevice.inobject) {
	     Token *tok = newtokens( 9 + strlen(aCh_str) / sizeof(Token) );

	     tok[0].i = DRAWSTR;
	     tok[1].f = R_height;
	     tok[2].f = R_length;
	     tok[3].i = F_color;
	     tok[4].f = R_vec_x;
	     tok[5].f = R_vec_y;
	     tok[6].f = R_vec_z;
	     tok[7].f = R_angle;

	     strcpy( (char *)&tok[8], aCh_str );

	     return;
	}

	V_SetTextInBox( R_height, R_length, F_color, 
						R_vec_x, R_vec_y, R_vec_z, R_angle, aCh_str ); 

}







/*
 * cmov
 *
 *	Sets the current character position.
 */
void cmov(float x, float y, float z)
{
	Vector	res;

	if (vdevice.inobject) {
	     Token *tok = newtokens(4);

	     tok[0].i = CMOV;
	     tok[1].f = x;
	     tok[2].f = y;
	     tok[3].f = z;

	     return;
	}

	cpos[V_X] = x;
	cpos[V_Y] = y;
	cpos[V_Z] = z;
	cpos[V_W] = 1.0;

	QMULTVECTOR_AFFINE(res, cpos, vdevice.transmat->m);

	/* apply the perspective transform */
	COPYVECTOR(cpos, res);
	QMULTVECTOR_BOTTOM(res, cpos, vdevice.transmatbottom->m);

	sc_x = WTOVX(res);
	sc_y = WTOVY(res);
}

 
/*
 * cmov2
 *
 *	Sets the current character position. Ignores the Z coord.
 *	
 *
 */
void cmov2(float x, float y)
{
	cmov(x, y, 0.0);
}

/*
 * cmovi
 *
 *	Same as cmov but with integer arguments....
 */
void cmovi(Icoord x, Icoord y, Icoord z)
{
	cmov((Coord)x, (Coord)y, (Coord)z);
}

/*
 * cmovs
 *
 *	Same as cmov but with short integer arguments....
 */
void cmovs(Scoord x, Scoord y, Scoord z)
{
	cmov((Coord)x, (Coord)y, (Coord)z);
}

/*
 * cmov2i
 *
 *	Same as cmov2 but with integer arguments....
 */
void cmov2i(Icoord x, Icoord y)
{
	cmov((Coord)x, (Coord)y, 0.0);
}

/*
 * cmov2s
 *
 *	Same as cmov but with short integer arguments....
 */
void cmov2s(Scoord x, Scoord y)
{
	cmov((Coord)x, (Coord)y, 0.0);
}


/*
 * strwidth
 *
 * Return the length of a string in pixels
 *
 */
long strwidth(char *s)
{
#ifdef SUN_CC
	/*
	 * SUN's ANSI CC compiler bitches here sating to use an explicit
	 * cast for strlen... it's only a warning, but this fixes it...
    	 */
	return((long)((size_t)strlen(s) * vdevice.hwidth));
#else
	return((long)(strlen(s) * vdevice.hwidth));
#endif
}

/* 
 * getheight
 *
 * Return the maximum Height of the current font
 */
long  getheight( void )
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("getheight: vogl not initialized");
#endif


	return((long)vdevice.hheight);
}

#ifdef OLD_GL
	/* This seem to have disappeared from GL 
	 * No.. wonder, it's in the C library under SYSV
	 */
/*
 * getwidth
 *
 * Return the maximum Width of the current font.
 *
 */
long getwidth(void )
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("getwidth: vogl not initialised");
#endif



	return((long)vdevice.hwidth);
}
#endif

/*
 * getcpos
 *
 * Return the current character position in screen coords 
 */
void getcpos(Scoord *cx, Scoord *cy)
{
	*cx = sc_x;
	*cy = sc_y;
}




    /*
     * Gopher's text functions
     *
     * Needed because text needs to be split by the plane in the
     * BSP code.
     *
     */
void V_SetTextInBox( float R_height, float R_length, int F_color, 
					 float R_vec_x, float R_vec_y, float R_vec_z, 
					 float R_angle, char *aCh_str )

{
	int	N_split_result;
	char *aCh_string1 = NULL;
	char *aCh_string2 = NULL;


    g_Txt_curr_text.F_curr_text_color = F_color;
    g_Txt_curr_text.R_curr_text_vec_x = R_vec_x;
    g_Txt_curr_text.R_curr_text_vec_y = R_vec_y;
    g_Txt_curr_text.R_curr_text_vec_z = R_vec_z;
    g_Txt_curr_text.R_curr_text_angle = R_angle;

	aCh_string1 = (char*)vallocate(strlen(aCh_str)); aCh_string1[0] = '\0';
	aCh_string2 = (char*)vallocate(strlen(aCh_str)); aCh_string2[0] = '\0';

	N_split_result = N_SplitString( aCh_str, aCh_string1, aCh_string2 );
	if ( 0 == N_split_result )
	{
		hboxtext( 0.0, 0.0, R_length, R_height, aCh_str, FALSE, 0 );
		g_B_text_waiting = TRUE;
	}
	else
	{
    	hboxtext( 0.0, 0.0, R_length, R_height, aCh_string1, FALSE, 0 );
		g_B_text_waiting = TRUE;
		WorldBFree( aCh_string1 );

		hboxtext( 0.0, 0.0, R_length, R_height, aCh_string2, TRUE, 1 );
    	g_B_text_waiting = TRUE;
		WorldBFree( aCh_string2 );
	}
}



	/*
	 * Right now when we split we assume that we will have at most
	 * 2 resulting lines. This is sort of bogus, this routine should
	 * return as many lines as are needed.
	 */
static int N_SplitString( char *Sz_string, char *aCh_lhs, char *aCh_rhs )
{
    int  cSz_split = 25;
    char *aCh_ptr;
    int cCh_last = 0;
    int cCh_curr = 0;
    int cCh_len;
    int cCh_lhs = 0, cCh_rhs = 0;
    int cCh_size1, cCh_size2;

    /*
     * find a middle 
     */
    cCh_len = strlen( Sz_string );
    if ( cCh_len > 8 )
    {
        cSz_split = cCh_len / 2;
    }

    if( cSz_split >= cCh_len )
    {
        return( 0 );
    }

    aCh_ptr = Sz_string;

    /* Remove spaces at the begining of the string ... */
    while (*aCh_ptr == ' ')  {
	 cCh_curr++; 
	 aCh_ptr++;
    }


    while( *aCh_ptr )
    {
	 switch (*aCh_ptr) {
	      
	 case '-':
	 case ' ':
	 case ':':
	 case '\n':
	 case ';':
	 case '&':
	      /*
	       * do we have a space exactly cSz_split chars into the string?
	       */
	      if ( cCh_curr == cSz_split )
	      {

                V_CopyString( Sz_string, aCh_lhs, 0, cSz_split );
		if (*aCh_ptr == ' ')
		     V_CopyString(Sz_string, aCh_rhs, cSz_split+1, cCh_len+1);
		else 
		     V_CopyString(Sz_string, aCh_rhs, cSz_split, cCh_len+1);

                return(1);
            }
	    else if ( cCh_curr > cSz_split )
            {
                cCh_lhs = cSz_split - cCh_last;
                cCh_rhs = cCh_curr - cSz_split;

                if ( cCh_lhs <= cCh_rhs )
                {
                    cCh_size1 = cCh_last;
                    cCh_size2 = cCh_len - cCh_last + 1;
                    if ( cCh_size1 > cCh_size2 )
                    {

                        V_CopyString(Sz_string, aCh_lhs, 0, cCh_last );
			if (*aCh_ptr == ' ') 
			     V_CopyStringP(Sz_string, aCh_rhs, cCh_last+1, 
					   cCh_len,((cCh_size1-cCh_size2)/2)+1);
			else 
			     V_CopyStringP(Sz_string, aCh_rhs, cCh_last, 
    			       cCh_len,((cCh_size1-cCh_size2)/2)+1);
                    }
                    else
                    {
                        V_CopyStringP( Sz_string, aCh_lhs, 0, cCh_last,
                                        ((cCh_size2-cCh_size1)/2)-1 );

			if (*aCh_ptr == ' ')
			     V_CopyString( Sz_string, aCh_rhs, cCh_last+1, cCh_len);
			else 
			     V_CopyString( Sz_string, aCh_rhs, cCh_last, cCh_len);
                    }

                
                    return(1);
                }
                else
                {
                    cCh_size1 = cCh_curr;
                    cCh_size2 = cCh_len-cCh_curr;
                    if ( cCh_size1 > cCh_size2 )
                    {

                        V_CopyString( Sz_string, aCh_lhs, 0, cCh_curr );
			if (*aCh_ptr == ' ')
			     V_CopyStringP( Sz_string, aCh_rhs, cCh_curr+1, 
					   cCh_len, ((cCh_size1-cCh_size2)/2)+1 );
			else 
			     V_CopyStringP( Sz_string, aCh_rhs, cCh_curr, 
					   cCh_len, ((cCh_size1-cCh_size2)/2)+1 );
                    }
                    else
                    {

                        V_CopyStringP( Sz_string, aCh_lhs, 0, cCh_curr,
                                        ((cCh_size2-cCh_size1)/2-1) );
			if (*aCh_ptr == ' ') 
			     V_CopyString( Sz_string, aCh_rhs, cCh_curr+1, cCh_len);
			else 
			     V_CopyString( Sz_string, aCh_rhs, cCh_curr, cCh_len);
			     
                    }

                    return(1);
                }
            } else {

		 cCh_last = cCh_curr;
	    }
	 default:
	      ;

        }
	 

        cCh_curr++; 
        aCh_ptr++;
    }

    /*
     * If we are here and we did not split the string then the string contains
     * no spaces in it (bummer) or it contains spaces before the midway point
     * and none after the midway point (sort of a bummer - this is the same
     * case as the LHS being smaller)
     */    
     
    /* don't bother splitting if it is 15 characters or less with no spaces */
    if ( cSz_split < 5 ) return(0);      
     
    if ( 0 == cCh_last )
    {
        cCh_size1 = cSz_split;
        cCh_size2 = cCh_len-cSz_split;;
        if ( cCh_size1 > cCh_size2 )
        {
            V_CopyString( Sz_string, aCh_lhs, 0, cSz_split );
            V_CopyStringP( Sz_string, aCh_rhs, cSz_split, cCh_len,
                            (cCh_size1-cCh_size2)/2 );
        }
        else
        {
            V_CopyStringP( Sz_string, aCh_lhs, 0, cSz_split, 
                                        (cCh_size2-cCh_size1)/2 );
            V_CopyString( Sz_string, aCh_rhs, cSz_split, cCh_len );
        }

        return(1);
    }


    if ( cCh_lhs <= cCh_rhs )
    {
        cCh_size1 = cCh_last;
        cCh_size2 = cCh_len-cCh_last+1;
        if ( cCh_size1 > cCh_size2 )
        {
            V_CopyString( Sz_string, aCh_lhs, 0, cCh_last );
            V_CopyStringP( Sz_string, aCh_rhs, cCh_last+1, cCh_len, 
                            ((cCh_size1-cCh_size2)/2)+1 );
        }
        else
        {
            V_CopyStringP( Sz_string, aCh_lhs, 0, cCh_last, 
                                        ((cCh_size2-cCh_size1)/2)-1 );
            V_CopyString( Sz_string, aCh_rhs, cCh_last+1, cCh_len );
        }

        return(1);
    }


    return(0);
}







	/*
	 * Copies the string aCh_original into the string aCh_new
	 */
static void
V_CopyString( char *aCh_original, char *aCh_new, int iaCh_start, int iaCh_end )
{
	int	i;
	int j = 0;

	for( i = iaCh_start; i < iaCh_end; i++ )
	{
		aCh_new [j] = aCh_original [i];
		j++;
	}

	aCh_new [j] = '\0';
}


   /*
	* Copies the string aCh_original into the string aCh_new
	* String aCh_new is padded with spaces on each of its sides
	*/
static void
V_CopyStringP( char *aCh_original, char *aCh_new, 
				int iaCh_start, int iaCh_end, int cCh_pad )
{
    int i,k;
    int j = 0;

	for( i = 0; i < cCh_pad; i++ )
	{	
		aCh_new [j] = ' ';
		j++;
	} 
    for( i = iaCh_start; i < iaCh_end; i++ )
    {
        aCh_new [j] = aCh_original [i];
        j++;
    }
	
	k = j;
	for( i = k; i < (k + cCh_pad); i++ )
	{
		aCh_new [j] = ' ';
		j++;
	}
  
    aCh_new [j] = '\0';
}

