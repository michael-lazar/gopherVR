/********************************************************************
 * $Id: render_telnet.c,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
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

#include "render_telnet.h"


static void buildTelnet( STRING Sz_title );


static void buildTelnet( STRING Sz_title )
{
    L_NUMBER    rlN_i0 [3] = {  320, -48, -132 };
    L_NUMBER    rlN_i1 [3] = {  320, -48,  108 };
    L_NUMBER    rlN_i2 [3] = { -304, -48,  108 };
    L_NUMBER    rlN_i3 [3] = { -304, -48, -132 };
    L_NUMBER    rlN_i4 [3] = { -304,   0, -132 };
    L_NUMBER    rlN_i5 [3] = {  320,   0, -132 };
    L_NUMBER    rlN_i6 [3] = { -304,   0,  108 };
    L_NUMBER    rlN_i7 [3] = {  320,   0,  108 };

    L_NUMBER    rlN_i10 [3] = {  272, -432, -84 };
    L_NUMBER    rlN_i11 [3] = {  272, -432,  60 };
    L_NUMBER    rlN_i12 [3] = { -256, -432,  60 };
    L_NUMBER    rlN_i13 [3] = { -256, -432, -84 };
    L_NUMBER    rlN_i14 [3] = { -256,  -48, -84 };
    L_NUMBER    rlN_i15 [3] = {  272,  -48, -84 };
    L_NUMBER    rlN_i16 [3] = { -256,  -48,  60 };
    L_NUMBER    rlN_i17 [3] = {  272,  -48,  60 };


    /*
     * Bottom
     */
    bgnpolygon();
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
        v3i( rlN_i4 );
        v3i( rlN_i6 );
        v3i( rlN_i7 );
        v3i( rlN_i5 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i0 );
        v3i( rlN_i1 );
        v3i( rlN_i2 );
        v3i( rlN_i3 );
    endpolygon();


    /*
     * Top
     */
    bgnpolygon();
        if ( NULL != Sz_title )
        {
            charstr( rlN_i11, 180.0, TEXT_TELNET_COLOR, 200.0, 500.0, Sz_title );
        }

        v3i( rlN_i16 );
        v3i( rlN_i12 );
        v3i( rlN_i11 );
        v3i( rlN_i17 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i17 );
        v3i( rlN_i11 );
        v3i( rlN_i10 );
        v3i( rlN_i15 );
    endpolygon();
    bgnpolygon();
        if ( NULL != Sz_title )
        {
            charstr( rlN_i13, 0.0, TEXT_TELNET_COLOR, 200.0, 500.0, Sz_title );
        }

        v3i( rlN_i15 );
        v3i( rlN_i10 );
        v3i( rlN_i13 );
        v3i( rlN_i14 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i14 );
        v3i( rlN_i13 );
        v3i( rlN_i12 );
        v3i( rlN_i16 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i10 );
        v3i( rlN_i11 );
        v3i( rlN_i12 );
        v3i( rlN_i13 );
    endpolygon();
}



/* 
 * this function should do whatever initialization is required 
 * so that the common parts of the 3d icon can just be called as objects
 */
void Telnet_init( void  )
{
}


/* 
 * draw an instance of the 3d icon with the specified string
 * then dispose of the temporary objects
 */
void Telnet_make( STRING Sz_title, int N_color )
{
	long N_id;
	
	N_id = genobj();
    makeobj( N_id );
        linewidth(1);
    
        pushmatrix();
            polymode( PYM_FILLNOUT );  /* for fill */
            color( N_color );
            buildTelnet( Sz_title );
        popmatrix();
        
    closeobj();
	callobj( N_id );
	delobj( N_id );    
}

