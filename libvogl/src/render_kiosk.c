/********************************************************************
 * $Id: render_kiosk.c,v 1.1.1.1 2002/01/18 16:34:27 lindner Exp $
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

#include "render_kiosk.h"



static void buildKiosk(  STRING Sz_title );


/* make a kiosk */

static void buildKiosk(  STRING Sz_title )
{
     long    rlN_i0 [3] = {  120, -528,   44 };
     long    rlN_i1 [3] = { -120, -528,   44 };
     long    rlN_i2 [3] = {    0, -880,  -28 };
     long    rlN_i3 [3] = {  120, -528, -100 };
     long    rlN_i4 [3] = { -120, -528, -100 };

    long    rlN_i10 [3] = {  216, -528, -244 };
    long    rlN_i11 [3] = {  216, -528,  188 };
    long    rlN_i12 [3] = { -216, -528,  188 };
    long    rlN_i13 [3] = { -216, -528, -244 };
    long    rlN_i14 [3] = { -216, -240, -244 };
    long    rlN_i15 [3] = {  216, -240, -244 };
    long    rlN_i16 [3] = {  216, -240,  188 };
    long    rlN_i17 [3] = { -216, -240,  188 };

    long    rlN_i20 [3] = {  120, -240,   96 };
    long    rlN_i21 [3] = {  216,    0,  188 };
    long    rlN_i22 [3] = { -216,    0,  188 };
    long    rlN_i23 [3] = { -120, -240,   96 };
    long    rlN_i24 [3] = {  120, -240, -148 };
    long    rlN_i25 [3] = {  216,    0, -244 };
    long    rlN_i26 [3] = { -216,    0, -244 };
    long    rlN_i27 [3] = { -120, -240, -148 };


    /*
     * Top
     */
    bgnpolygon();
        v3i( rlN_i1 );
        v3i( rlN_i2 );
        v3i( rlN_i0 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i0 );
        v3i( rlN_i2 );
        v3i( rlN_i3 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i3 );
        v3i( rlN_i2 );
        v3i( rlN_i4 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i4 );
        v3i( rlN_i2 );
        v3i( rlN_i1 );
    endpolygon();


    /*
     * Middle 
     */
    bgnpolygon();
        if ( NULL != Sz_title ) 
        {
            charstr( rlN_i10, -90.0, TEXT_KIOSK_COLOR, 100.0, 400.0, Sz_title );
        }

        v3i( rlN_i16 );
        v3i( rlN_i11 );
        v3i( rlN_i10 );
        v3i( rlN_i15 );
    endpolygon();

    bgnpolygon();
        if ( NULL != Sz_title )
        {
            charstr( rlN_i11, 180.0, TEXT_KIOSK_COLOR, 100.0, 400.0, Sz_title );
        }       

        v3i( rlN_i17 );
        v3i( rlN_i12 );
        v3i( rlN_i11 );
        v3i( rlN_i16 );
    endpolygon();

    bgnpolygon();
        if ( NULL != Sz_title )
        {
            charstr( rlN_i12, 90.0, TEXT_KIOSK_COLOR, 100.0, 400.0, Sz_title ); 
        }

        v3i( rlN_i14 );
        v3i( rlN_i13 );
        v3i( rlN_i12 );
        v3i( rlN_i17 );
    endpolygon();

    bgnpolygon();
        if ( NULL != Sz_title )
        {
            charstr( rlN_i13, 0.0, TEXT_KIOSK_COLOR, 100.0, 400.0, Sz_title ); 
        }

        v3i( rlN_i15 );
        v3i( rlN_i10 );
        v3i( rlN_i13 );
        v3i( rlN_i14 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i14 );
        v3i( rlN_i17 );
        v3i( rlN_i16 );
        v3i( rlN_i15 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i12 );
        v3i( rlN_i13 );
        v3i( rlN_i10 );
        v3i( rlN_i11 );
    endpolygon();


    /*
     * Bottom
     */

    bgnpolygon();
        v3i( rlN_i21 );
        v3i( rlN_i20 );
        v3i( rlN_i24 );
        v3i( rlN_i25 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i22 );
        v3i( rlN_i23 );
        v3i( rlN_i20 );
        v3i( rlN_i21 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i26 );
        v3i( rlN_i27 );
        v3i( rlN_i23 );
        v3i( rlN_i22 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i25 );
        v3i( rlN_i24 );
        v3i( rlN_i27 );
        v3i( rlN_i26 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i26 );
        v3i( rlN_i22 );
        v3i( rlN_i21 );
        v3i( rlN_i25 );
    endpolygon();
}


/* 
 * this function should do whatever initialization is required 
 * so that the common parts of the 3d icon can just be called as objects
 */
void Kiosk_init( void )
{

}


/* 
 * draw an instance of the 3d icon with the specified string
 * then dispose of the temporary objects
 */
void Kiosk_make(STRING Sz_title, int N_color )
{
	long N_id;
	
	N_id = genobj();
    makeobj( N_id );
        linewidth(1);
    
        pushmatrix();
            polymode( PYM_FILLNOUT );  /* for fill */
            color( N_color );
            buildKiosk( Sz_title );
        popmatrix();
        
    closeobj();
	callobj( N_id );
	delobj( N_id );
}





