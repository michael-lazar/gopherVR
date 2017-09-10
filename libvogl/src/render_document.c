/********************************************************************
 * $Id: render_document.c,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
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

#include "render_document.h"


static void buildDocument( STRING Sz_title );


static void buildDocument( STRING Sz_title )
{
    L_NUMBER    rlN_i0 [3] = {  264, -408, -56 };
    L_NUMBER    rlN_i1 [3] = {  264, -408, -24 };
    L_NUMBER    rlN_i2 [3] = { -264, -408, -24 };
    L_NUMBER    rlN_i3 [3] = { -264, -408, -56 };
    L_NUMBER    rlN_i4 [3] = { -264,    0, -56 };
    L_NUMBER    rlN_i5 [3] = {  264,    0, -56 };
    L_NUMBER    rlN_i6 [3] = { -264,    0, -24 };
    L_NUMBER    rlN_i7 [3] = {  264,    0, -24 };

/*
    L_NUMBER    rlN_i10 [3] = {  176, -368, -20 };
    L_NUMBER    rlN_i11 [3] = {  176, -368,   8 };
    L_NUMBER    rlN_i12 [3] = { -356, -368,   8 };
    L_NUMBER    rlN_i13 [3] = { -356, -368, -20 };
    L_NUMBER    rlN_i14 [3] = { -356,    0, -20 };
    L_NUMBER    rlN_i15 [3] = {  176,    0, -20 };
    L_NUMBER    rlN_i16 [3] = { -356,    0,   8 };
    L_NUMBER    rlN_i17 [3] = {  176,    0,   8 };

    L_NUMBER    rlN_i20 [3] = {  300, -448, 12 };
    L_NUMBER    rlN_i21 [3] = {  300, -448, 44 };
    L_NUMBER    rlN_i22 [3] = { -232, -448, 44 };
    L_NUMBER    rlN_i23 [3] = { -232, -448, 12 };
    L_NUMBER    rlN_i24 [3] = { -232,    0, 12 };
    L_NUMBER    rlN_i25 [3] = {  300,    0, 12 };
    L_NUMBER    rlN_i26 [3] = { -232,    0, 44 };
    L_NUMBER    rlN_i27 [3] = {  300,    0, 44 };

    L_NUMBER    rlN_i30 [3] = {  232, -408, 44 };
    L_NUMBER    rlN_i31 [3] = {  232, -408, 76 };
    L_NUMBER    rlN_i32 [3] = { -300, -408, 76 };
    L_NUMBER    rlN_i33 [3] = { -300, -408, 44 };
    L_NUMBER    rlN_i34 [3] = { -300,    0, 44 };
    L_NUMBER    rlN_i35 [3] = {  232,    0, 44 };
    L_NUMBER    rlN_i36 [3] = { -300,    0, 76 };
    L_NUMBER    rlN_i37 [3] = {  232,    0, 76 };
*/
    /*
     * Box1
     */
    bgnpolygon();
        if ( NULL != Sz_title )
        {
            charstr( rlN_i1, 180.0, TEXT_DOCUMENT_COLOR, 200.0, 500.0, Sz_title );
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
           charstr( rlN_i3, 0.0, TEXT_DOCUMENT_COLOR, 200.0, 500.0, Sz_title );
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
     * Box2

    bgnpolygon();
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
        v3i( rlN_i14 );
        v3i( rlN_i16 );
        v3i( rlN_i17 );
        v3i( rlN_i15 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i10 );
        v3i( rlN_i11 );
        v3i( rlN_i12 );
        v3i( rlN_i13 );
    endpolygon();
*/

    /*
     * Box3

    bgnpolygon();
        v3i( rlN_i26 );
        v3i( rlN_i22 );
        v3i( rlN_i21 );
        v3i( rlN_i27 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i27 );
        v3i( rlN_i21 );
        v3i( rlN_i20 );
        v3i( rlN_i25 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i25 );
        v3i( rlN_i20 );
        v3i( rlN_i23 );
        v3i( rlN_i24 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i24 );
        v3i( rlN_i23 );
        v3i( rlN_i22 );
        v3i( rlN_i26 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i24 );
        v3i( rlN_i26 );
        v3i( rlN_i27 );
        v3i( rlN_i25 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i20 );
        v3i( rlN_i21 );
        v3i( rlN_i22 );
        v3i( rlN_i23 );
    endpolygon();
*/

    /*
     * Box4

    bgnpolygon();
        v3i( rlN_i36 );
        v3i( rlN_i32 );
        v3i( rlN_i31 );
        v3i( rlN_i37 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i37 );
        v3i( rlN_i31 );
        v3i( rlN_i30 );
        v3i( rlN_i35 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i35 );
        v3i( rlN_i30 );
        v3i( rlN_i33 );
        v3i( rlN_i34 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i34 );
        v3i( rlN_i33 );
        v3i( rlN_i32 );
        v3i( rlN_i36 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i34 );
        v3i( rlN_i36 );
        v3i( rlN_i37 );
        v3i( rlN_i35 );
    endpolygon();
    bgnpolygon();
        v3i( rlN_i30 );
        v3i( rlN_i31 );
        v3i( rlN_i32 );
        v3i( rlN_i33 );
    endpolygon();
*/


}



/* 
 * this function should do whatever initialization is required 
 * so that the common parts of the 3d icon can just be called as objects
 */
void Doc_init( void  )
{
}


/* 
 * draw an instance of the 3d icon with the specified string
 * then dispose of the temporary objects
 */
void Doc_make( STRING Sz_title, int N_color )
{
	long N_id;
	
	N_id = genobj();
	makeobj( N_id );
        linewidth(1);
    
        pushmatrix();
            polymode( PYM_FILLNOUT );  /* for fill */
            color( N_color );
            buildDocument( Sz_title );
        popmatrix();
        
    closeobj();
	callobj( N_id );
	delobj( N_id );    
}

