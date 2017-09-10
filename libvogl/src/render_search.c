/********************************************************************
 * $Id: render_search.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include "hungarian.h"
#include "vogl.h"
#include "lines.h"
#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif

#include "render_search.h"

static void buildSearch( STRING Sz_title );



/* make a search */ 

static void buildSearch( STRING Sz_title )
{
    L_NUMBER    rlN_i0 [3] = {  248, -624, -76 };
    L_NUMBER    rlN_i1 [3] = {  248, -624,  68 };
    L_NUMBER    rlN_i2 [3] = { -280, -488,  68 };
    L_NUMBER    rlN_i3 [3] = { -280, -488, -76 };
    L_NUMBER    rlN_i4 [3] = { -280,    0, -76 };
    L_NUMBER    rlN_i5 [3] = {  248,    0, -76 };
    L_NUMBER    rlN_i6 [3] = { -280,    0,  68 };
    L_NUMBER    rlN_i7 [3] = {  248,    0,  68 };

    L_NUMBER    rlN_i8 [3] = { -280, -488, -76 };


    /*
     * Top
     */
    bgnpolygon();
        if ( NULL != Sz_title )
        {
            charstr( rlN_i1, 180.0, TEXT_SEARCH_COLOR, 200.0, 500.0, Sz_title );
        }

        v3i( rlN_i6 );
        v3i( rlN_i2 );
        v3i( rlN_i1 );
        v3i( rlN_i7 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i7 );
        v3i( rlN_i1 );
        v3i( rlN_i0 );
        v3i( rlN_i5 );
    endpolygon();
    bgnpolygon();
        if ( NULL != Sz_title )
        {
            charstr( rlN_i8, 0.0, TEXT_SEARCH_COLOR, 200.0, 500.0, Sz_title );
        }
        v3i( rlN_i5 );
        v3i( rlN_i0 );
        v3i( rlN_i3 );
        v3i( rlN_i4 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i4 );
        v3i( rlN_i3 );
        v3i( rlN_i2 );
        v3i( rlN_i6 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i0 );
        v3i( rlN_i1 );
        v3i( rlN_i2 );
        v3i( rlN_i3 );
    endpolygon();

    bgnpolygon();
        v3i( rlN_i7 );
        v3i( rlN_i5 );
        v3i( rlN_i4 );
        v3i( rlN_i6 );
    endpolygon();

}



/* 
 * this function should do whatever initialization is required 
 * so that the common parts of the 3d icon can just be called as objects
 */
void Search_init( void )
{
}



/* 
 * draw an instance of the 3d icon with the specified string
 * then dispose of the temporary objects
 */
void 
Search_make( STRING Sz_title, int N_color )
{
	long N_id;
	
	N_id = genobj();
    makeobj( N_id );
        linewidth(1);
    
        pushmatrix();
            polymode( PYM_FILLNOUT );  /* for fill */
            color( N_color );
            buildSearch( Sz_title );
        popmatrix();
        
    closeobj();
	callobj( N_id );
	delobj( N_id );    
}




