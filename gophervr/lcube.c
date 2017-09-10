#include <stdio.h>
#include "vogl.h"
#include "globals.h"
#include <math.h>
#include "standard.h"
#include "vogltools.h"

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
#include <sys/time.h>

extern int stale_fps;
extern int no_fps;
int usec_render = 0;
int last_fps = 0;

static void
Init_GL()
{
     winopen("Gopher VR...");
     ginit();
     vsetflush(0);
     hfont( "futura.hfont" ); /* f**king Mac OS X didn't like the .l. */

     V_ShadingFreq( d_STATIC ); 

     backface(0); 
     doublebuffer();
     gconfig();
}

void
PreloadObjects()
{
     Kiosk_init();
     Search_init();    
     Telnet_init();  
     Dir_init();  
     Doc_init();  
     Sound_init();
}

#include "uofm.h"

void
InitialScene(void)
{
     long	theID;
     FILE       *foo;

     Init_GL();

     oursceneid = genscene();

     bgnscene(oursceneid);

/*     PlayAudio("/home/arcwelder/lindner/mods/beyond.mod");*/

     makegroundplane();

/* now its OK to make the objects and place them in the world */

     pushmatrix();
{
     int id = ParseArray(UofMnff, "world/nff-spd");
     if (id != -1) {
	  translate(0.0, -100.0, 0.0);
	  scale(2.0, 2.0, 2.0);
	  callobj(id);
	  rot(180.0, 'y');
	  translate(0.0, 0.0, -1.0);
	  callobj(id);
     }
}
     popmatrix();

     endscene(oursceneid);    
     EyeInitialPoint();

}



void
Bootstrap(char *host, int port)
{
     long	theID;
     sleep(1);
     PreloadObjects();

     /* now its OK to make the objects and place them in the world */

     WorldStart(host, port, "");
}


 

static float Eyeangle_ds = 0.0;
static float Eyeazimuth_ds = 0.0;
static float Eyey = 0.0, Eyex = 0.0, Eyez = 0.0;

static long currentscene = 0;

/* This was the old FPS routine. It's not high resolution enough for us.
   I'm keeping it more as a souvenir.
   -- Cameron */
#if(0)
static 
bgntime()
{
     static int oldtime;
     static int frames = 0;
     int timenow;

     timenow = time(NULL);
     frames ++;

     if (timenow != oldtime) {
	  printf("frame rate %f fps\n", (float)frames/(timenow - oldtime));
	  frames = 0;
     }
     oldtime = timenow;
}
#endif

void
drawscene(void)
{     
     float theta;
     char  name[20];
     struct timeval time1, time2;
     char status_line[128];
     int fps;

     (void)gettimeofday(&time1, NULL);

     if (Bootstrapped == 0) {
	  Bootstrapped++;
	  InitialScene();
     } else if (Bootstrapped == 1) {
	  Bootstrapped++;
	  Bootstrap(INITIALhost, INITIALport);
     }

     mapcolor(8, 0, 13, 38); /** The color of the sky... **/
     color(8);
     clear();
     if (Eyeangle_ds != EYEangle) {
	  EyeRotateY(Eyeangle_ds - EYEangle);
	  Eyeangle_ds = EYEangle;
     }

     if (Eyeazimuth_ds != EYEazimuth) {
	  EyeRotateX(Eyeazimuth_ds - EYEazimuth);
	  Eyeazimuth_ds = EYEazimuth;
     }	  

     if (EYEzval != 0.0) {
	  EyeTranslate(0.0, 0.0, EYEzval);
	  EYEzval = 0.0;
     }

     if (EYEyval != 0.0) {
	  EyeTranslate(0.0, EYEyval, 0.0);
	  EYEyval = 0.0;
     }


     callscene(oursceneid);
     swapbuffers();

     (void)gettimeofday(&time2, NULL);

     /* figure out render time, which is used for proportional movement */

     usec_render = (time2.tv_sec <= time1.tv_sec)
	? (time2.tv_usec - time1.tv_usec)
	: ((time2.tv_sec - time1.tv_sec) == 1) /* most likely case */
		? (1000000 + time2.tv_usec - time1.tv_usec) 
: ((1000000*(time2.tv_sec-time1.tv_sec)) + time2.tv_usec - time1.tv_usec) ;
/* most expensive case, but if we're under 1fps, then who cares */

/* We have to be careful here with our updates:
   * statusupdate goes haywire if we change digit width on it. f.ex., for
     using raw usec_render, it was best to sprintf with %08i.
   * updating the status line is EXPENSIVE. profiling shows updating it every
     frame makes it a significant part of the real impact of updates.
   So we use a little heuristic to only do it every second-ish, i.e,
   * only if the tv_sec changed */

     if (usec_render == 0) usec_render = 1;
     fps = 1000000 / usec_render; 
     if (fps < 1) fps = 1;
     if (fps >= 10000) fps = 9999;

     if (!no_fps && (!stale_fps || time2.tv_sec != time1.tv_sec)) {
	sprintf(status_line,
			"%04i fps - Gopher VR - University of Minnesota", fps);
	StatusUpdate(status_line);
	stale_fps = 1;
     }
     last_fps = fps;

/*#define VORT_MOVIE*/
#ifdef VORT_MOVIE
     if (SaveMovie) {
	  sprintf(name, "/home/huskerdu/tmp/vp.%d", currentscene);
	  currentscene++;
	  printf("Dumping %s\n", name);
	  X11_dump_pixmap(name, 0,0, 576, 324);
     }
#endif

}

void ResetFps() { last_fps = 0; }

SetDynamicShading()
{
     V_ShadingFreq(d_DYNAMIC);
}

SetStaticShading()
{
     V_ShadingFreq(d_STATIC);
}

Go_InitialView()
{
     EyeInit();
     EyeInitialPoint();
     drawscene();
}



