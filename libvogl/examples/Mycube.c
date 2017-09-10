#include <stdio.h>
#include "vogl.h"
#include "vodevice.h"
#include "bsp.h"
#include "hungarian.h"
#include "render_kiosk.h"
#include "render_dir.h"
#include "render_search.h"
#include "render_telnet.h"
#include "render_document.h"
#include "../src/eyemotion.h"
#include <math.h>
#include "hungarian.h"

#define	CUBE_SIZE	200.0
#define	TRANS		50.0
#define	SCAL		0.1
#define GRIDSIZE 20000.0
#define GRIDWIDTH 2000.0

extern  g_F_shade_type;

void	makegroundplane();
static void GopherVRmakegroundplane();

int
main()
{
        int sceneid;
        float	tdir = TRANS;
	float	scal = 1.0 + SCAL;
	int		but;
	int		i, oldx, x, oldy, y;
	short	val;
	float	R_eye_x, R_eye_y, R_eye_z;    
	int		rockandroll;
	long	theID;
	float 	theta;
	int		click_x, click_y;
	int		dumbswitch;

	rockandroll = FALSE;

	prefsize(500L, 500L);

	winopen("lcube");

	unqdevice(INPUTCHANGE);
    qdevice(SKEY);
    qdevice(XKEY);
    qdevice(YKEY);
    qdevice(ZKEY);
    qdevice(EQUALKEY);
    qdevice(MINUSKEY);
    qdevice(ESCKEY);
    qdevice(QKEY);
    qdevice(PKEY);
    qdevice(LKEY);
    qdevice(TKEY);
    qdevice(MKEY);
    qdevice(NKEY);
    qdevice(BKEY);
	qdevice(EKEY);
    qdevice(RKEY);
    

	x = -48; y = -1782;

	/* 
	 * Wait for REDRAW event ...
	 */
	while (qread(&val) != REDRAW)
		;
	
	/* prime the pump for the object makers */
    Kiosk_init();
    Search_init();    
    Telnet_init();  
    Dir_init();  
    Doc_init();  

#ifdef NOTUSED
	V_MapColor( 11, 204, 50, 50 );       /* orange */ 
	V_MapColor( 12, 219, 112, 219 );     /* orchid */
	V_MapColor( 13, 66, 66, 111 );       /* cornflower blue */
	V_MapColor( 14, 112, 219, 147 );     /* aquamarine */
	V_MapColor( 15, 234, 173, 234 );     /* plum */
	V_MapColor( 16, 188, 143, 143 );     /* pink */
	V_MapColor( 17, 111, 66, 66 );       /* salmon */
	V_MapColor( 18, 79, 47, 47 );       /* indian red */
#endif

	hfont( "futura.l" );
	V_ShadingFreq( d_STATIC );  
	
	sceneid = genscene();
	bgnscene(sceneid);

	backface(0);		
	doublebuffer();
	gconfig();

/* now its OK to make the objects and place them in the world */
	GopherVRmakegroundplane();

    /* now its OK to make the objects and place them in the world */
    pushmatrix();       
        theID = V_StartNewGopherObject();
        Kiosk_make( "a fine salmon flavored kisok", OBJECT_KIOSK_COLOR);
        
        /* now make the circle of icons */
        
        for (theta = 0.1; theta < (2 * PI - 0.1); theta += ( PI/6)) {
           int rotation;
        
           rotation = 900 -10 *(int)((theta * 360.0) /(2* PI));
           pushmatrix();    
               translate( 2000.0 * cos(theta), 0.0, 2000.0 * sin(theta) );
               rotate( rotation, 'y' );
               if ( dumbswitch == 0 ) {
                    theID = V_StartNewGopherObject();
                  	Search_make( "Search lots of places at the University of Minnesota", BLUE );  
                    dumbswitch = 1;
               } else {
                    if ( dumbswitch == 1 ) {
                        dumbswitch = 2;
                        theID = V_StartNewGopherObject();
                    	Telnet_make( "Urge Overkill telnet", RED ); 
                    } else {
                        if ( dumbswitch == 2 ) {
                            dumbswitch = 3;
                            theID = V_StartNewGopherObject();
                    		Doc_make( "a Document of great importance goes here" , CYAN ); 
                        } else {
                            dumbswitch = 0;
                            theID = V_StartNewGopherObject();
                     		Dir_make( "University of Minnesota Campus Information", MAGENTA );
 
                        }
                    }
               }
           popmatrix();   
        }                       
    popmatrix();



/* now that the scene has been defined build a BSP tree for it */
    endscene(sceneid);

    
    perspective(600, 1.0, 0.0, 800.0);
    lookat(0.0, 0.0, 0.0, 0.0, 0.0, 800.0, 0);
    EyeTranslate( 0.0, -550.0, 200.0 );
    rockandroll = FALSE;
    
    while(1) {

        if ( rockandroll == TRUE ) {
            x = 4 * (int)getvaluator(MOUSEX);
            y = 4 * (int)getvaluator(MOUSEY);
        }

		if ( getbutton(RIGHTMOUSE) )
		{
            click_x = (int)getvaluator(MOUSEX);
            click_y = (int)getvaluator(MOUSEY);
            sceneclick( sceneid, click_x, click_y );
        }


	/* note that since we need to be able to invert the transformation matrix
   	 * it is a real problem when something is rotated exactly 90 degrees
   	 * because this make the transformation matrix ill conditioned. To avoid
   	 * this, we never rotate exactly 90 degrees. -mpm 
   	 */
	
		if ( (x % 900) == 0 ) x = x + 1;
		if ( (y % 900) == 0 ) y = y + 1;

	callscene(sceneid);

	for (i = 0; i < 200; i++) {
	     color(BLACK);
	     clear();
	     translate(0.0,0.0,tdir);
	     callscene(sceneid);
	     swapbuffers();

	}
	exit(0);
		if (qtest()) {
			but = qread(&val);
			but = qread(&val);	/* swallow up event */

			switch (but) {

            case TKEY:
                for ( i = 0; i < 200; i++ )
               	{ 
                    translate(0.0, 0.0, tdir);
                    pushmatrix();
                        rotate(-x, 'y');
                        rotate(-y, 'x');        
                        color(WHITE);
                        clear();
		        callscene(sceneid);
                    popmatrix();
                    swapbuffers();  
                }
				break;

            case MKEY:
                if ( rockandroll == TRUE ) 
                    rockandroll = FALSE;
                else
                    rockandroll = TRUE;
                break;

            case SKEY:
                scale(scal, scal, scal);
                break;

            case XKEY:
                translate(tdir, 0.0, 0.0);
                break;

            case YKEY:
                translate(0.0, tdir, 0.0);
                break;

            case ZKEY:
                translate(0.0, 0.0, tdir);
                break;

            case NKEY: {
                float tempx, tempy, tempz;
                
                    tempx = vdevice.transmat->m[3][0];
                    tempy = vdevice.transmat->m[3][1];
                    tempz = vdevice.transmat->m[3][2];
                    pushmatrix();
                    rotate( 100, 'y' );
                    popmatrix();
                }
                break;

            case BKEY: {
                float tempx, tempy, tempz;
                
                    tempx = vdevice.transmat->m[3][0];
                    tempy = vdevice.transmat->m[3][1];
                    tempz = vdevice.transmat->m[3][2];
                    rotate( -100, 'y' );
                }
                break;

            case MINUSKEY:
                tdir = -tdir;

                if (scal < 1.0)
                    scal = 1.0 + SCAL;
                else
                    scal = 1.0 - SCAL;

                break;

            case EQUALKEY:
                tdir = TRANS;
                break;
				
			case EKEY:
				break;

			case PKEY:
				break;

            case LKEY:          
		delscene(sceneid);
		sceneid = genscene();
		bgnscene(sceneid);
                translate( 0.0, 0.0, 0.0 );
                /* define the scene */

		makegroundplane();
    translate( 0.0, 0.0, 0.0 );
	theID = V_StartNewGopherObject();
    Search_make( "Search lots of places at the University of Minnesota", 0 );
    
    translate( 500.0, 0.0, 500.0 ); 
    rotate(451, 'y');
	theID = V_StartNewGopherObject();
    Kiosk_make( "a fine kisok", 0 );
    
    translate( 500.0, 0.0, 500.0 ); 
    rotate(901, 'y');
	theID = V_StartNewGopherObject();
    Telnet_make( "Urge Overkill telnet", 1 );  
    
    translate( 500.0, 0.0, 500.0 ); 
    rotate(-451, 'y');
	theID = V_StartNewGopherObject();
    Dir_make( "Directory of the Urge Overkill", 2 );  
    
    translate( 500.0, 0.0, 500.0 ); 
    rotate(-901, 'y');
	theID = V_StartNewGopherObject();
    Doc_make( "a Document of great importance goes here", 3 );  
    
               /* now that the scene has been defined build a BSP tree for it */

                endscene(sceneid);
                perspective(600, 1.0, 800.0, -800.0);
                lookat(0.0, 1000.0, -1000.0, 0.0, 800.0, 800.0, 0);
                break;
                
          case ESCKEY:
          case QKEY:
                gexit();
                exit(0);
            default:
                ;
            }
        }
    }
}



void 
makegroundplane()
{
     long my_ID;
    
     my_ID = genobj();
     makeobj(my_ID);
     pushmatrix();

     pushattributes();
        rotate(900, 'x');       
        color( GREEN );
        polymode(PYM_FILLNOUT);
        rectf(-GRIDSIZE, -GRIDSIZE, GRIDSIZE, GRIDSIZE);          
     popattributes();

     popmatrix();
     closeobj();
     callobj(my_ID );
     delobj( my_ID );
}     



/* 
 * make a not-user-selectable groundplane; it is not selectable because
 * it doesn't have a gopherobject ID. All non-selectable stuff has to be 
 * created before we place the gopher objects in the scene.
 */
static void GopherVRmakegroundplane(void)
{
        long my_ID;

/* 
 * before we start modelling the scene do two rotations to the
 * coordinate system so that things look rightside up. Note that while doing
 * a 180  degree rotation about the x axis looks like the same thing this
 * hoses up the code in BSP that checks to see if the user clicked on 
 * a gopher object; at least on the Mac its bogus unless we do both a 
 * y and z axis rotate. Probably simplest to remodel the 3d icons so
 * we don't have to twirl the coordinate systems... -mpm
 */
        rotate( 1800, 'y' );  
        rotate( 1800, 'z' );  
        
        my_ID = genobj();
        makeobj(my_ID);
        pushmatrix();
   {
            L_NUMBER    rlN_i1 [3] = { -20000,  2,      0 };
            L_NUMBER    rlN_i2 [3] = { -20000,  2,      -20000 };
            L_NUMBER    rlN_i3 [3] = {  0,      2,      -20000 };
            L_NUMBER    rlN_i4 [3] = {  0,      2,      0 };
        
            L_NUMBER    rlN_i5 [3] = { 0,               2,      20000 };
            L_NUMBER    rlN_i6 [3] = { 0,               2,      0 };
            L_NUMBER    rlN_i7 [3] = { 20000,   2,      0 };
            L_NUMBER    rlN_i8 [3] = { 20000,   2,      20000 };

            L_NUMBER    rlN_i9  [3] = { -20000, 3,      20000 };
            L_NUMBER    rlN_i10 [3] = { -20000, 3,      0 };
            L_NUMBER    rlN_i11 [3] = {      0, 3,      0 };
            L_NUMBER    rlN_i12 [3] = {      0, 3,      20000 };
        
            L_NUMBER    rlN_i13 [3] = { 20000,  3,      -20000 };
            L_NUMBER    rlN_i14 [3] = { 20000,  3,      0 };
            L_NUMBER    rlN_i15 [3] = {     0,  3,      0 };
            L_NUMBER    rlN_i16 [3] = {     0,  3,      -20000 };
        
                color(OBJECT_GROUND1_COLOR);
            bgnpolygon();
                v3i( rlN_i9 );
                v3i( rlN_i10 );
                v3i( rlN_i11 );
                v3i( rlN_i12 );
            endpolygon();

                color(OBJECT_GROUND1_COLOR);
            bgnpolygon();
                v3i( rlN_i13 );
                v3i( rlN_i14 );
                v3i( rlN_i15 );
                v3i( rlN_i16 );
            endpolygon();

                color(OBJECT_GROUND2_COLOR);
            bgnpolygon();
                v3i( rlN_i1 );
                v3i( rlN_i2 );
                v3i( rlN_i3 );
                v3i( rlN_i4 );
            endpolygon();

                color(OBJECT_GROUND2_COLOR);
            bgnpolygon();
                v3i( rlN_i5 );
                v3i( rlN_i6 );
                v3i( rlN_i7 );
                v3i( rlN_i8 );
            endpolygon();
       }                 
        popmatrix();
        closeobj();
        callobj(my_ID );
        delobj( my_ID );
   }     

