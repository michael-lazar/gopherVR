/* This is the code that handles generating the high-level 3D world. */

#define _GNU_SOURCE
#include <stdio.h>		/* For tempnam() */
#include "GDgopherdir.h"
#include "GSgopherobj.h"
#include "BLblock.h"
#include "vogl.h"
#include "String.h"		/* For strchr() */
#include <math.h>		/* cos, sin.. */
#include "bsp.h"
#include "globals.h"
#include "eyemotion.h"
#include "gophwin.h"
#include "gopherto3d.h"
#include "STAarray.h"

#include "parse.h"
#include <sys/wait.h>		/* waitpid() stuff */

static GopherDirObj *CurrentDir = NULL;
static GopherDirObj *OldDirs[30];
static StrArray *Sta_urls = NULL;
static int          iLevel =0;


/*
 * Display a scene given a GopherDirObj, assumes that everything has
 * been initialized
 */
extern int BusyUpdate();
extern void	V_DestroyHistoryData();	

static void
pushgopher(GopherDirObj *ZeDir)
{

     OldDirs[iLevel]= ZeDir;
     iLevel ++;
}

static int
popgopher(GopherDirObj **ZeDir)
{

     if (iLevel == 0)
	  return(-1);

     iLevel --;

     *ZeDir =  OldDirs[iLevel];

     return(0);
}



static int 
gmoo()
{
     printf("Got stuff\n");
}

/**********************************************************************/
void
GDrenderScene(GopherDirObj *gd, boolean SpiralLayout)
{
     int       numobjects, i;
     GopherObj *gs;
     long	theID;
     float further = 1.0;
     float theta = 0.1;
     float thetaIncrease;
     float thetaLimit;
     float radius = 2000.0;
     float twoPI;
     long descCounter;
	 int	N_count_pos = 1;
     int   BlockResult = 0;

     if (gd == NULL)
	  return;

	 /* 
	  * remove the old items from the menu view
	  * before building a new menu
	  */
	 V_DelMenuListing( );

     pushmatrix();
      theID = V_StartNewGopherObject();
	  V_SetMenuTitle( GDgetTitle(gd) );
      Kiosk_make( GDgetTitle(gd) , RED);
     popmatrix();

      theta = 0.1;


     thetaIncrease = (PI/6);
     thetaLimit = (PITIMES2 - 0.1);
     descCounter = 0;


     numobjects = GDgetNumitems(gd);

     for (i=0; i < numobjects ; i++) {
	  int rotation;
	  char *text;
          char supertext[512];

	  gs = GDgetEntry(gd, i);
	  text = GSgetTitle(gs);
	  
	  BusyUpdate();
	  pushmatrix();	

	  theID = V_StartNewGopherObject();	       
	  
	  BlockResult = GSparse3Dblock(gs);

	  if (BlockResult == 0 ) {
	       translate( further * radius * cos(theta), -(radius-2000.0)/8.0,
			  further * radius * sin(theta) );
	       rotation = 900 -10 *(int)((theta * 360.0) /(2* PI));
	       rotate( rotation, 'y' );
	  }
	  if (SpiralLayout == TRUE)
	       further += 0.05;
	  
/* poor man's snprintf() */
#define ADD_ITYPE(x) ((strlen(text) > 500) ? 0 : sprintf(supertext, "%s %s", x, text))

	  if (BlockResult != 2) {
	       switch (GSgetType(GDgetEntry(gd, i))) {
	       case A_DIRECTORY:
		    ADD_ITYPE("DIR>   ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Dir_make(text, OBJECT_DIRECTORY_COLOR);
		    break;
	       case A_INDEX:
		    ADD_ITYPE("QUERY> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Search_make(text, OBJECT_SEARCH_COLOR);
		    break;
		    
	       case A_TELNET:
		    ADD_ITYPE("TELNT> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Telnet_make(text, OBJECT_TELNET_COLOR);
		    break;
		    
	       case A_TN3270:
		    ADD_ITYPE("T3270> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Telnet_make(text, OBJECT_DIRECTORY_COLOR);
		    break;
		    
	       case A_SOUND:
		    ADD_ITYPE("AUDIO> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Sound_make(text, OBJECT_DOCUMENT_COLOR);
		    break;

	       case A_WORLD:
		    ADD_ITYPE("WORLD> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Doc_make(text, OBJECT_DOCUMENT_COLOR);
		    break;

	       case A_INFO:
		    ADD_ITYPE("       ");
		/* make a menu entry, but not an object -- ck */
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;

	       case A_HTML:
		{
			int is_url = 0;
			String *path = GSgetPath(gs);
			if(!strncmp(path, "URL:", 4) ||
					!strncmp(path, "/URL:", 5)) {
				is_url = 1;
				ADD_ITYPE("URL>   ");
			} else
				ADD_ITYPE("HTML>  ");
		    	V_GenMenuListing(
				supertext, N_count_pos ); N_count_pos++; 	
		    	Doc_make(text, (is_url) ? OBJECT_URL_COLOR :
					OBJECT_DOCUMENT_COLOR);
			break;
		}


	       case A_GIF:
		    ADD_ITYPE("GIF>   ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Doc_make(text, OBJECT_DOCUMENT_COLOR);
		    break;

	       case A_IMAGE:
		    ADD_ITYPE("IMAGE> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Doc_make(text, OBJECT_DOCUMENT_COLOR);
		    break;

	       case A_FILE:
		    ADD_ITYPE("TEXT>  ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Doc_make(text, OBJECT_DOCUMENT_COLOR);
		    break;

		    
	       default:
		    ADD_ITYPE("FILE>  ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    Doc_make(text, OBJECT_DOCUMENT_COLOR);
		    break;
	       }
	  }
	  popmatrix();   
	  theta += thetaIncrease;
	  if (theta > thetaLimit) {
	       /* start a new circle */
	       theta = 0.1;
	       if (SpiralLayout == FALSE)
		    radius += 1000.0;
	       thetaIncrease = thetaIncrease * (radius -1000.0)/ radius;
	  }
     }						

     /* now that the scene has been defined build a BSP tree for it */
}



/**********************************************************************
 * Check to see if there's a Gopher+ block around to parse
 * Returns true if it found stuff to parse, false otherwise
 */

int
GSparse3Dblock(GopherObj *gs)
{
     Blockobj *bl;
     char *blocktext;
     int i;
     int changed = FALSE;
     int changetest;

     bl = GSfindBlock(gs, "3D");
     if (bl == NULL)
	  return(FALSE);

     for (i = 0 ; i < BLgetNumLines(bl); i++) {
	  if (changetest = Parse3dBlockLine(BLgetLine(bl,i)))
	       if (changetest > changed)
		    changed = changetest;
     }

     return(changed);
}


boolean
Parse3dBlockLine(char *txt)
{
     char    *cp, *colon;
     char    tmpvals[80];
     boolean changed = FALSE;

     float x,y,z, rx, ry, rz, sx, sy, sz, r, g, b;

     cp = txt;


     colon = strchr(cp, ':');

     if (!colon)
	  return(FALSE);

     strncpy(tmpvals, colon+1, 80);
     tmpvals[79] = '\0';

     
     if (strncasecmp(cp, "Location:",9) == 0) {
	  char *cp2 = strchr(tmpvals, ',');
	  char *cp3 = NULL;

	  if (cp2 == NULL) return(FALSE);
	  *cp2 = '\0';
	  x = atof(tmpvals);
	  cp3 = cp2+1;

	  cp2 = strchr(cp3, ',');
	  if (cp2 == NULL) return(FALSE);
	  *cp2 = '\0';
	  y = atof(cp3);
	  cp3 = cp2+1;

	  z = atof(cp3);
	       
	  translate(x,y,z);
	  changed = TRUE;
	       
     } else if (strncasecmp(cp, "Orientation:",12) == 0) {
	  char *cp2 = strchr(tmpvals, ',');
	  char *cp3 = NULL;
	  
	  if (cp2 == NULL) return(FALSE);
	  *cp2 = '\0';
	  rx = atof(tmpvals);
	  cp3 = cp2+1;
	  
	  cp2 = strchr(cp3, ',');
	  if (cp2 == NULL) return(FALSE);
	  *cp2 = '\0';
	  ry = atof(cp3);
	  cp3 = cp2+1;
	  
	  rz = atof(cp3);
	  
	  if (rx != 0) rot(rx, 'x');
	  if (ry != 0) rot(ry, 'y');
	  if (rz != 0) rot(rz, 'z');

	  changed = TRUE;
	  
     } else if (strncasecmp(cp, "Scale:",6) == 0) {
	  char *cp2 = strchr(tmpvals, ',');
	  char *cp3 = NULL;
	  
	  if (cp2 == NULL) return(FALSE);
	  *cp2 = '\0';
	  sx = atof(tmpvals);
	  cp3 = cp2+1;
	  
	  cp2 = strchr(cp3, ',');
	  if (cp2 == NULL) return(FALSE);
	  *cp2 = '\0';
	  sy = atof(cp3);
	  cp3 = cp2+1;
	  
	  sz = atof(cp3);
	  
	  scale(sx,sy,sz);

	  changed = TRUE;
	  
     } else if (strncasecmp(cp, "Color: ",7) == 0) {
	  char *cp2 = strchr(tmpvals, ',');
	  char *cp3 = NULL;
	  
	  if (cp2 == NULL) return(FALSE);
	  *cp2 = '\0';
	  r = atof(tmpvals);
	  cp3 = cp2;
	  
	  cp2 = strchr(cp3, ',');
	  if (cp2 == NULL) return(FALSE);
	  *cp2 = '\0';
	  g = atof(cp3);
	  cp3 = cp2;
	  
	  cp2 = strchr(cp3, ',');
	  if (cp2 == NULL) return(FALSE);
	  *cp2 = '\0';
	  b = atof(cp3);
	  cp3 = cp2;
	  
	  ;;;;;;
	  changed = TRUE;
     }
     else if (strncasecmp(cp, "Object: ",8) == 0) {
	  char *url = cp + 8;
	  char *tmpfile = (char*)tempnam(NULL, NULL);
          FILE *f;
	  int id;

	  if (tmpfile != NULL) {
	       f = fopen(tmpfile, "w+");
	       URLtoFile(url, f, "", BusyUpdate);
	       fflush(f);
	       rewind(f);

	       /**** Need to lookup the views to find a good one..  *****/
	       id = ParseGeomFile(f);
	       callobj(id);
	       delobj(id);
	       return(2);
	  }
     } else if (strncasecmp(cp, "AmbientSound: ", 14) == 0) {
	  char *url = cp + 14;
	  char *tmpfile = (char*)tempnam(NULL, NULL);
	  FILE *f;
	  int id;

	  if (tmpfile != NULL) {
	       f = fopen(tmpfile, "w+");
	       URLtoFile(url, f, "", BusyUpdate);
	       fclose(f);

	       /*
		* Fork and play the sound...
		*/
	       PlayAudio(tmpfile);
	  }

     }

     return(changed);
}

/**********************************************************************
 * Play audio in the background...
 */

PlayAudio(char *song)
{
     static char *zefile;
     static int pid = -1;
     int status;
     

#if defined(sun) || defined(sgi) || defined(linux)
     if (pid != -1) {
	  /* Kill off the old process... */
	  kill(pid, SIGINT);
	  sleep(1);
	  waitpid(pid, &status, WNOHANG);
	  kill(pid, SIGKILL);
	  waitpid(pid, &status, WNOHANG);
	  unlink(zefile);
	  free(zefile);
     }
     zefile = strdup(song);
     if ((pid = fork()) < 0) {
	  fprintf(stderr, "Problems forking off!\n");
     }
     else if (pid == 0) {
	  /* Child Process */
	  TrackerPlay(zefile);
	  exit(0);
     }
#endif
}
     

/**********************************************************************
 */
int
Load_Index_or_Dir(GopherObj *ZeGopher, GopherDirObj *newdir,
		  char      *Searchmungestr)
{
     int failed = 0;
     int sockfd;
     int i, numbytes;
     static char DirTitle[512];
     GopherDirObj *NewDir = NULL;
     char tmpstr[80];

     sprintf(tmpstr, "Contacting %s, %d...\n", GSgetHost(ZeGopher), 
	     GSgetPort(ZeGopher));
     StatusUpdate(tmpstr);
     if ((sockfd = GSconnect(ZeGopher)) <0) {
	  StatusUpdate("Unable to connect...");
	  Errormsg("Unable to Connect");
	  failed = 1;
     }
     else {
	  StatusUpdate("Sending Request...");
	  if (GSgetType(ZeGopher) == A_DIRECTORY) {
	       GStransmit(ZeGopher, sockfd, NULL, "$", NULL);
	  }
	  else if (GSgetType(ZeGopher) == A_INDEX) {
	       GStransmit(ZeGopher, sockfd, Searchmungestr, "$", NULL);
	  }
	  StatusUpdate("Receiving Data...");

	  numbytes = GSrecvHeader(ZeGopher, sockfd);


	  if (numbytes == 0) {

	       failed = 1;
	       StatusUpdate("Empty...");
	       return(failed);
	  }

	  if (GSisGplus(ZeGopher))
	       GDplusfromNet(newdir, sockfd, BusyUpdate);
	  else
	       GDfromNet(newdir, sockfd, BusyUpdate);


	  if (GDgetNumitems(newdir) <= 0) {
	       closenet(sockfd);
	       return(failed);
	  }

	  GDsetTitle(newdir, GSgetTitle(ZeGopher));

	  GDsetLocation(newdir, ZeGopher);

	  
     }
     i = closenet(sockfd);

     StatusUpdate("Done...");
	  
     return(failed);
}

extern void drawscene();
void
JumpOutofView(void (*displayfunc)(),
	     float Moveup, float seconds);
void
JumpIntoView(void (*displayfunc)(),
	     float Moveup, float seconds);

/**********************************************************************
 * Try to render any gopher stuff we have..
 */

void
RenderGophers(GopherDirObj *gd, boolean SpiralLayout)
{
     delscene(oursceneid);
     oursceneid = genscene();
     bgnscene(oursceneid);

     makegroundplane();

     GDrenderScene(gd, SpiralLayout);

     StatusUpdate("Rendering Scene...");

     endscene(oursceneid);

     StatusUpdate("0000fps - Gopher VR - University of Minnesota");

     perspective(600, 16.0/9.0, 0.0, 800.0);
     lookat(0.0, 0.0, 0.0, 0.0, 0.0, 800.0, 0);
     EyeTranslate( 0.0, -550.0, 200.0 );

     /* Render once to get the right colors.. */
     
     REALCLEARSCREEN;        
     callscene(oursceneid);
     
     JumpIntoView(drawscene, 1400.0, 12.0);
     drawscene();

}

static int Search_Item = -1;


doSearch(char *text)
{
     GopherDirObj *gd = GDnew(20);
     Load_Index_or_Dir(GDgetEntry(CurrentDir, Search_Item), gd, text);
     
     CurrentDir = gd;
     
     RenderGophers(CurrentDir, TRUE);

}



/*
 * Load up a new Gopher Item, given an index into the current one
 */

int
SelectGopherDir()
{
     GopherDirObj *gd;
     GopherObj    *gs = NULL;
     int          choice = vdevice.ClickedGopherspaceObject;

     choice -= 2;

     if (CurrentDir == NULL || choice < -1) {
	  return(-1);
     }

     if (choice == -1) {
	  /** Go back up a level **/
	  GopherDirObj *tempGdir = CurrentDir;

	  V_DestroyHistoryData();
	  if (popgopher(&CurrentDir) == 0 && tempGdir != CurrentDir) {
	       GDdestroy(tempGdir);
	  } else {
	       return 0;
	  }
	  RenderGophers(CurrentDir, FALSE);
	  return 0;
     }

     gs = GDgetEntry(CurrentDir, choice);

     if (gs == NULL) {
	  return(-1);
     }

     pushgopher(CurrentDir);

     switch (GSgetType(gs)) {
     case A_INDEX:
	  Search_Item = choice;
	  /** Pop up an index dialog **/
	  popupsearchdeally(GSgetTitle(gs));
	  break;

     case A_TELNET:
     {
	  char moo[256];

	  sprintf(moo, "xterm -e telnet \"%s\"&", GSgetHost(gs));
	  system(moo);
	  break;
     }

     case A_TN3270:
     {
	  char moo[256];
	  
	  sprintf(moo, "xterm -e tn3270 \"%s\"&", GSgetHost(gs));
	  system(moo);
	  break;
     }

     case A_DIRECTORY:
		/* add to history list */
	  V_GenHistListing( GSgetTitle( gs ), GSgetURL( gs, NULL ) );

	  gd = GDnew(20);
	  Load_Index_or_Dir(gs, gd, NULL);

	  CurrentDir = gd;
	  
	  RenderGophers(CurrentDir, FALSE);
	  
	  break;

     case A_FILE:
     {
	  char *tmpfile = (char*)tempnam(NULL, NULL);
	  FILE *f;

	  if ((f = fopen(tmpfile, "w")) == NULL)
	       return 0;
	  GStoFile(gs, f, "", BusyUpdate);
	  fclose(f);
	  popuptextfile(GSgetTitle(gs), tmpfile);
     }
	  break;

     case A_SOUND:
     {
	  char tmpstr[512];
	  char *tmpfile = (char*)tempnam(NULL, NULL);
	  FILE *f;

	  if ((f = fopen(tmpfile, "w")) == NULL)
	       return 0;
	  GStoFile(gs, f, "audio/basic", BusyUpdate);
	  fclose(f);
#if defined(__APPLE__)
          sprintf(tmpstr, "open -b \"/Applications/QuickTime Player.app\" %s", tmpfile);
#else
	  sprintf(tmpstr, "play %s &", tmpfile);
#endif
	  system (tmpstr);
	  break;
     }

     case A_IMAGE:
     case A_GIF:
     {
	  char tmpstr[512];
	  char *tmpfile = (char*) tempnam(NULL, NULL);
	  FILE *f;
/* 10.5+ Preview.app don't like files with no extensions. Suck. 10.4 works. */
#if defined(__APPLE__)
          char *fext = (GSgetType(gs) == A_GIF) ? "gif" : "jpg";

          if(strlen(tmpfile) > 500) {
               fprintf(stderr, "unexpectedly long tmpfile: %s\n", tmpfile);
               return 0;
          }
          sprintf(tmpstr, "%s.%s", tmpfile, fext);
	  if ((f = fopen(tmpstr, "w")) == NULL)
#else
	  if ((f = fopen(tmpfile, "w")) == NULL)
#endif
	       return 0;
	  GStoFile(gs, f, "image/gif", BusyUpdate);
	  fclose(f);
#if defined(__APPLE__)
          sprintf(tmpstr, "open %s.%s", tmpfile, fext);
#else
	  sprintf(tmpstr, "xv %s &", tmpfile);
#endif
	  system (tmpstr);
	  break;
     }

     case A_HTML:
     {
	  char tmpstr[512];
	  char *tmpfile = (char*) tempnam(NULL, NULL);
	  FILE *f;
          String *path = GSgetPath(gs);

          /* if the selector is a hURL, bail now */
          if(!strncmp(path, "URL:", 4) || !strncmp(path, "/URL:", 5)) {
               char *url = strchr(path, ':');
               url++;
#if defined(__APPLE__)
	       sprintf(tmpstr, "open %s", url);
#else
               sprintf(tmpstr, "lynx %s", url);
#endif
               system(tmpstr);
               break;
          }

          if(strlen(tmpfile) > 500) {
               fprintf(stderr, "unexpectedly long tmpfile: %s\n", tmpfile);
               return 0;
          }

break;
}

     case A_WORLD:
     {
	  break;
     }

     default:
		filepicker(basename(GSgetPath(gs)), gs);
		break;
     }
return 0;
}

void
WorldStart(char *host, int port, char *path)
{
     GopherObj *gs = GSnew();
     GopherDirObj *gd;

     GSsetTitle(gs, "Home Gopher Server");
     GSsetHost(gs, host);
     GSsetPort(gs, port);
     GSsetPath(gs, path);
     GSsetType(gs, A_DIRECTORY);

	/* add to history list */
	V_GenHistListing( GSgetTitle( gs ), GSgetURL( gs, NULL ) );
	V_GenHistUrlListing( GSgetURL( gs, NULL ) );

     gd = GDnew(20);

     Load_Index_or_Dir(gs, gd, NULL);

     CurrentDir = gd;

     RenderGophers(gd, FALSE);
}

/*
 * URLtoFile
 */

int
URLtoFile(char *url,
	 FILE *f,
	 char *view,
	 int (*twirlfn)())
{
     int result;

     GopherObj *gs = GSnew();

     GSfromURL(gs, url, "", 0, 0);

     result = GStoFile(gs, f, view, twirlfn);

     return(result);

}

/*
 * This fcn transfers a file from a gopher server into f.
 *
 */

boolean
GStoFile(GopherObj *gs,
	 FILE *f,
	 char *view,
	 int (*twirlfn)())
{
     int numread, sockfd;
     char buf[1024];
     int bytemethod;
     int line = 0, i;

     /*** Check for gopher+ and multiple views ***/

     if ((sockfd = GSconnect(gs)) <0) {
	  return(FALSE);
     }

     /** Send out the request **/
     GStransmit(gs, sockfd, NULL, "+", view);
     bytemethod = GSrecvHeader(gs, sockfd);

     if (bytemethod == 0) {
	  return(FALSE);
     }
	  

     if (GSisText(gs, view)) {
	  char cso_click = '\0';

	  while (readline(sockfd, buf, sizeof(buf)) > 0) {
	       ZapCRLF(buf);
	       line ++;
	       if (*buf == '.' && *(buf+1) == '\0')
		    break;



	       if (GSgetType(gs) == A_CSO) {
		    if (*buf == '2')
			 break;
		    
		    if ((*buf >= '3') && (*buf <= '9'))  {
			 fprintf(f, "%s\n", GSgetPath(gs));
			 fprintf(f, "%s\n", buf+4);
			 break;
		    }
		    if (*buf == '-') {


			 if (buf[1] >= '3' && buf[1] <= '9') {
			      fprintf(f, "%s\n", GSgetPath(gs));
			      fprintf(f, "%s\n", buf+5);
			 }
			 else {
			      char *colonpos = strchr(buf+5,':');
			      i = '\0';
			      if (colonpos != NULL && *(colonpos-1) != cso_click) {
				   fprintf(f, "-------------------------------------------------------\n");
				   cso_click = *(colonpos-1);
			      }
			      fputs((colonpos ? colonpos+1 : buf+6), f);
			      fputc('\n', f);
			 }
		    }
	       }
	       else {
		    fputs(buf, f);
		    /** Don't cut long lines... **/
		    if (strlen(buf) < sizeof(buf))  
			 putc('\n', f);
	       }
	       if ((line % 25) == 0)
		    twirlfn();
	  }
     }
     else {
	       
	  while ((numread = readrecvbuf(sockfd, buf, sizeof buf)) > 0) {
	       if (fwrite(buf, numread, 1, f) == 0) {
		    closenet(sockfd);
		    return(FALSE);
	       }
	       twirlfn();
	  }
     }

     if (GSgetType(gs) == A_CSO)
	  writestring(sockfd, "quit\r\n");

     closenet(sockfd);
     return(TRUE);
}


	/* called from V_SelCall() */
void
V_SelMenuItem( int N_item )
{
	/*
	 * select an time, add the +1 offset to map from menu space to VR space
	 */
	vdevice.ClickedGopherspaceObject = N_item + 1;
	SelectGopherDir();
}






int
B_GetAbstract( int N_item, char **paCh_abs )
{
	GopherObj	*gs;

	gs = GDgetEntry( CurrentDir, N_item - 1 );
	if (gs == NULL) 
	{
		return( FALSE );
	}

	return( GSGetAbstractblock( gs, paCh_abs ) );
}





int
GSGetAbstractblock( GopherObj *gs, char **aCh_abs )
{
	Blockobj *bl;
	char *blocktext;
	int i;
	int	N_numlines;
	int	N_len = 0;
	boolean changed = FALSE;


	bl = GSfindBlock(gs, "ABSTRACT");
	if (bl == NULL)
		return(FALSE);


	N_numlines = BLgetNumLines(bl);

	for (i = 0 ; i < N_numlines; i++)
	{
		N_len = strlen( BLgetLine( bl, i ) ) + N_len + i;
	}
	*aCh_abs = (char *) malloc( sizeof(char) * N_len + 1 );

	
	*aCh_abs[0] = '\0';

	for (i = 0 ; i < N_numlines; i++) 
	{
	     strcat( *aCh_abs, BLgetLine( bl, i ));
	     strcat( *aCh_abs, "\n");
	}
	(*aCh_abs) [N_len] = '\0'; 
	changed = TRUE;

	return( changed );
}


V_AfterHistory()
{
	RenderGophers(CurrentDir, FALSE);
}

void
V_OpenSessionURL( char *aCh_title, char *aCh_urltxt )
{
    GopherObj *gs = GSnew();
    GopherDirObj *gd;
    int doneflags = 0;

    if(strstr(aCh_urltxt, "://") == NULL) {
	Errormsg("Not a valid URL.");
	return;
    }
    
    if(strncmp(aCh_urltxt, "gopher://", 9)) {
	Errormsg("I'm not that kind of girl!");
	return;
    }

    doneflags = GSfromURL( gs, aCh_urltxt, "turnip.com", 416, doneflags );
    GSsetTitle( gs, aCh_title );

    gd = GDnew(20);

    Load_Index_or_Dir(gs, gd, NULL);

    CurrentDir = gd;
    
    /* add to history list */
    V_GenHistListing( GSgetTitle( gs ), aCh_urltxt );
    V_GenHistUrlListing( GSgetURL( gs, NULL ) );

    RenderGophers(gd, FALSE);
}



/*
 * Add a new GopherItem's URL to the recent history list
 * these should be in gophwin.c but we need to keep conflicting
 * types for String Gopher/X away from each other
 */
void
V_GenHistUrlListing( char *aCh_url )
{
	String      *pStr_str;

	if ( NULL == Sta_urls )
		Sta_urls = STAnew( 20 );

    /*
     * Add url to array
     */
    pStr_str = STRnew();
    STRset( pStr_str, aCh_url );
    STApush( Sta_urls, pStr_str );
    STRdestroy( pStr_str );
}

char *
aCh_GetHisUrl( int iSta_picked )
{
	return( STAgetText( Sta_urls, iSta_picked ) );
}

void
V_RemoveHisUrl( void )
{
	STApop( Sta_urls );
}

void
V_PopDirectory( void )
{
	popgopher(&CurrentDir);
}

void
V_PrintURL( int i )
{
	printf("<URL:%s>\n", STAgetText( Sta_urls, i ) );
}

void
WriteGopherFile(FILE *f, void *gs)
{
	GopherObj *gos = gs;
	GStoFile(gos, f, "application/octet-stream", BusyUpdate);
}

int
ClickToChoice(short x, short y)
{
	GopherDirObj *gd;
	GopherObj    *gs = NULL;
	int          choice;

	sceneclick(oursceneid, x, y);
	choice = vdevice.ClickedGopherspaceObject;
	choice -= 2;
	if (choice < 0) {
		/* choice = -1 is back, otherwise error */
		return -1;
	}
	return choice;
}

char *
ChoiceToString(int choice, int which)
{
	GopherObj *gs = NULL;

	if (choice < 0) return NULL;
	gs = GDgetEntry(CurrentDir, choice);
	if (!gs) return NULL;
	return (which) ? GSgetTitle(gs) : GSgetURL(gs, NULL);
}

void
ReloadCurrentDir()
{
	  RenderGophers(CurrentDir, FALSE);
}

extern void V_GenMenuListWin();
extern void V_AddStringToList(char *s, int indent);

void
MenuShowGopherMenu()
{
	int i, numobjects, BlockResult;
	GopherObj *gs = NULL;
	int	N_count_pos = 1;

	/* Open the window, which now has nothing in it */
	V_GenMenuListWin();

	/* Repopulate the history list */
	if (iLevel)
	{
		int i;
		for (i=0; i<iLevel; i++)
	  		V_AddStringToList(GDgetTitle( OldDirs[i] ), i+1);
	}
	V_AddStringToList( GDgetTitle( CurrentDir ), iLevel+1);
	V_SetMenuTitle( GDgetTitle( CurrentDir ));

	/* Repopulate the menu list, using an abbreviated form
		of the menu renderer. */
        numobjects = GDgetNumitems(CurrentDir);

     for (i=0; i < numobjects ; i++) {
	  char *text;
          char supertext[512];

	  gs = GDgetEntry(CurrentDir, i);
	  text = GSgetTitle(gs);
	  
	  BusyUpdate();
	  BlockResult = GSparse3Dblock(gs);

/* poor man's snprintf() */
#ifndef ADD_ITYPE
#define ADD_ITYPE(x) ((strlen(text) > 500) ? 0 : sprintf(supertext, "%s %s", x, text))
#endif

	  if (BlockResult != 2) {
	       switch (GSgetType(GDgetEntry(CurrentDir, i))) {
	       case A_DIRECTORY:
		    ADD_ITYPE("DIR>   ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;

	       case A_INDEX:
		    ADD_ITYPE("QUERY> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;
		    
	       case A_TELNET:
		    ADD_ITYPE("TELNT> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;
		    
	       case A_TN3270:
		    ADD_ITYPE("T3270> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;
		    
	       case A_SOUND:
		    ADD_ITYPE("AUDIO> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;

	       case A_WORLD:
		    ADD_ITYPE("WORLD> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;

	       case A_INFO:
		    ADD_ITYPE("       ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;

	       case A_HTML:
		{
			int is_url = 0;
			String *path = GSgetPath(gs);
			if(!strncmp(path, "URL:", 4) ||
					!strncmp(path, "/URL:", 5)) {
				is_url = 1;
				ADD_ITYPE("URL>   ");
			} else
				ADD_ITYPE("HTML>  ");
		    	V_GenMenuListing(
				supertext, N_count_pos ); N_count_pos++; 	
			break;
		}


	       case A_GIF:
		    ADD_ITYPE("GIF>   ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;

	       case A_IMAGE:
		    ADD_ITYPE("IMAGE> ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;

	       case A_FILE:
		    ADD_ITYPE("TEXT>  ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;

		    
	       default:
		    ADD_ITYPE("FILE>  ");
		    V_GenMenuListing( supertext, N_count_pos ); N_count_pos++; 	
		    break;
	       }
	  }
	}
}
