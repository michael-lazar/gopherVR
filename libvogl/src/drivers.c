/********************************************************************
 * $Id: drivers.c,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
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
#include "vogl.h"
#include "vodevice.h"
#include "drivers.h"
#include "valloc.h"
#include "lines.h"
#include "shading.h"

#ifdef MACINTOSH
#include "MacII.h"
#endif


struct vdev	vdevice;

static FILE	*fp;

static int	allocated = 0;


/* device-independent function routines */

/*
 * voutput
 *
 *	redirect output - only for postscript, hpgl (this is not a feature)
 */
void voutput(char *path)
{
	char	buf[128];

	if ((fp = fopen(path, "w")) == (FILE *)NULL) {
		sprintf(buf, "voutput: couldn't open %s", path);
		verror(buf);
	}
}

/*
 * _voutfile
 *
 *	return a pointer to the current output file - designed for internal
 * use only.
 */
FILE * _voutfile( void )
{
	return(fp);
}

/*
 * verror
 *
 *	print an error on the graphics device, and then exit. Only called
 * for fatal errors. We assume that stderr is always there.
 *
 */
void verror(char *str)
{
#ifdef MSWIN
	mswin_verror(str);
	if (vdevice.initialised)
		gexit();
#else
	if (vdevice.initialised)
		gexit();

	fprintf(stderr, "%s\n", str);
#endif
	exit(1);
}

void viniterror(char *str)
{
	fprintf(stderr, "%s: vogl not initialised\n", str);
	exit(1);
}

/*
 * gexit
 *
 *	exit the vogl/vogle system
 *
 */
void gexit( void )
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("gexit: vogl not initialised");
#endif


	(*vdevice.dev.Vexit)();

	vdevice.devname = (char *)NULL;
	vdevice.initialised = 0;
	fp = stdout;
}

/*
 * getdevice
 *
 *	get the appropriate device table structure
 */
static void getdevice(char *device);
static void getdevice(char *device)
{
	char	buf[100];
	
#ifdef MACINTOSH
	if (strncmp(device, "MacII", 5) == 0)
		_MacII_devcpy();
	else
#endif

#ifdef SUN
	if (strncmp(device, "sun", 3) == 0)
		_SUN_devcpy();
	else
#endif
#ifdef PIXRECT
	if (strncmp(device, "pixrect", 7) == 0)
		_PIXRECT_devcpy();
	else
#endif
#ifdef X11
	if (strncmp(device, "X11", 3) == 0)
		_X11_devcpy();
	else
#endif
#ifdef DECX11
	if (strncmp(device, "decX11", 6) == 0)
		_DECX11_devcpy();
	else
#endif
#ifdef NeXT
	if (strncmp(device, "NeXT", 4) == 0)
		_NeXT_devcpy();
	else
#endif
#ifdef POSTSCRIPT
	if (strncmp(device, "postscript", 10) == 0) {
		_PS_devcpy();
	} else
	if (strncmp(device, "ppostscript", 11) == 0) {
		_PSP_devcpy();
	} else
	if (strncmp(device, "cps", 3) == 0) {
		_CPS_devcpy();
	} else
	if (strncmp(device, "pcps", 4) == 0) {
		_PCPS_devcpy();
	} else
#endif
#ifdef HPGL
	if (strncmp(device, "hpgla1", 6) == 0)
		_HPGL_A1_devcpy();
	else if (strncmp(device, "hpgla3", 6) == 0)
		_HPGL_A3_devcpy();
	else if (strncmp(device, "hpgla4", 6) == 0)
		_HPGL_A4_devcpy();
	else if (strncmp(device, "hpgla2", 6) == 0 || strncmp(device, "hpgl", 4) == 0)
		_HPGL_A2_devcpy();
	else
#endif
#ifdef DXY
	if (strncmp(device, "dxy", 3) == 0)
		_DXY_devcpy();
	else
#endif
#ifdef TEK
	if (strncmp(device, "tek", 3) == 0)
		_TEK_devcpy();
	else
#endif
#ifdef GRX
	if (strncmp(device, "grx", 3) == 0)
		_grx_devcpy();
	else
#endif
#ifdef HERCULES
	if (strncmp(device, "hercules", 8) == 0)
		_hgc_devcpy();
	else
#endif
#ifdef MSWIN
	if (strncmp(device, "mswin", 5) == 0)
		_mswin_devcpy();
	else
#endif
#ifdef CGA
	if (strncmp(device, "cga", 3) == 0)
		_cga_devcpy();
	else
#endif
#ifdef EGA
	if (strncmp(device, "ega", 3) == 0)
		_ega_devcpy();
	else
#endif
#ifdef VGA
	if (strncmp(device, "vga", 3) == 0)
		_vga_devcpy();
	else
#endif
#ifdef SIGMA
	if (strncmp(device, "sigma", 5) == 0)
		_sigma_devcpy();
	else
#endif
	{
	     if (*device == 0)
		  sprintf(buf, "vogl: expected the enviroment variable VDEVICE to be set to the desired device.\n");
	     else
		  sprintf(buf, "vogl: %s is an invalid device type\n", device);
#ifdef MACINTOSH
	     fprintf(stderr, "MacII\n");
#endif
#ifdef MSWIN
		mswin_verror(buf);
#else
	     fputs(buf, stderr);
	     fprintf(stderr, "The devices compiled into this library are:\n");
#endif
#ifdef SUN
		fprintf(stderr, "sun\n");
#endif
#ifdef PIXRECT
		fprintf(stderr, "pixrect\n");
#endif
#ifdef X11
		fprintf(stderr, "X11\n");
#endif
#ifdef DECX11
		fprintf(stderr, "decX11\n");
#endif
#ifdef NeXT
		fprintf(stderr, "NeXT\n");
#endif
#ifdef POSTSCRIPT
		fprintf(stderr, "postscript\n");
		fprintf(stderr, "ppostscript\n");
		fprintf(stderr, "cps\n");
		fprintf(stderr, "pcps\n");
#endif
#ifdef HPGL
		fprintf(stderr, "hpgla1\n");
		fprintf(stderr, "hpgla2 (or hpgl)\n");
		fprintf(stderr, "hpgla3\n");
		fprintf(stderr, "hpgla4\n");
#endif
#ifdef DXY
		fprintf(stderr, "dxy\n");
#endif
#ifdef TEK
		fprintf(stderr, "tek\n");
#endif
#ifdef HERCULES
		fprintf(stderr, "hercules\n");
#endif
#ifdef CGA
		fprintf(stderr, "cga\n");
#endif
#ifdef EGA
		fprintf(stderr, "ega\n");
#endif
#ifdef VGA
		fprintf(stderr, "vga\n");
#endif
#ifdef SIGMA
		fprintf(stderr, "sigma\n");
#endif
#ifdef GRX
		fprintf(stderr, "grx\n");
#endif
		exit(1);
	}
}

/*
 * vinit
 *
 * 	Just set the device name. ginit and winopen are basically
 * the same as the VOGLE the vinit function.
 *
 */
void vinit(char *device)
{
	vdevice.devname = device;

	/* the first gopherspace object must have an ID > 0 */
	vdevice.CurrentGSpaceID = 0;

	vdevice.F_shade_freq = d_STATIC; /*dynamic shading is too slow */
}

/*
 * winopen
 *
 *	use the more modern winopen call (this really calls ginit),
 * we use the title if we can
 */
long winopen(char *title)
{
	vdevice.wintitle = title;

	ginit();

	return((long) 1);
}

/*
 * ginit
 *
 *	by default we check the environment variable, if nothing
 * is set we use the value passed to us by the vinit call.
 */
void ginit(void)
{
	char	*dev;
	int	i;

#ifdef MACINTOSH
	if (vdevice.devname == (char *)NULL) {
		dev = "MacII";
		if (dev == (char *)NULL)
			getdevice("");  
		else
			getdevice(dev);
	} else 
		getdevice(vdevice.devname);
#else
	if (vdevice.devname == (char *)NULL) {
		if ((dev = getenv("VDEVICE")) == (char *)NULL)
			getdevice("X11");
		else
			getdevice(dev);
	} else 
		getdevice(vdevice.devname);
#endif
	if (vdevice.initialised)
		gexit();

	if (!allocated) {
		allocated = 1;
		deflinestyle(0, 0xffff);
		
		vdevice.transmat = (Mstack *)vallocate(sizeof(Mstack));
		vdevice.transmat->back = (Mstack *)NULL;
		
		vdevice.transmatbottom = (Mstack *)vallocate(sizeof(Mstack));
		vdevice.transmatbottom->back = (Mstack *)NULL;
		
		vdevice.attr = (Astack *)vallocate(sizeof(Astack));
		vdevice.attr->back = (Astack *)NULL;
		
		vdevice.viewport = (Vstack *)vallocate(sizeof(Vstack));
		vdevice.viewport->back = (Vstack *)NULL;
		
		vdevice.bases = (Matrix *)vallocate(sizeof(Matrix) * 10);
		vdevice.enabled = (char *)vallocate(MAXDEVTABSIZE);
	}

	for (i = 0; i < MAXDEVTABSIZE; i++)
		vdevice.enabled[i] = 0;

	/* NOTE:
	 * There is a slight behaviour change from previous versions of VOGL
	 * if you define FIRST_REDRAW... you always get a REDRAW event as the
	 * first event in the queue.
	 */
#define FIRST_REDRAW 1
#ifdef FIRST_REDRAW
	/*
	 * Arrange for a REDRAW to be the first thing in the queue...
         * (winopen always enters a REDRAW in the queue on a real SGI).
	 */
	vdevice.alreadyread = TRUE;
	vdevice.data = 0;
	vdevice.devno = REDRAW;
	vdevice.enabled[REDRAW / 8] |= (1 << (REDRAW & 0x7));
#else
	vdevice.alreadyread = FALSE;
	vdevice.data = 0;
	vdevice.devno = 0;
#endif
	vdevice.kbdmode = vdevice.mouseevents = vdevice.kbdevents = 0;

	vdevice.concave = 0;
	vdevice.clipoff = 0;
#ifdef UNIX
	vdevice.sync = 1;
#endif
	vdevice.cpW[V_W] = 1.0;			/* never changes */

	vdevice.maxfontnum = 2;

	vdevice.attr->a.lw = 1;
	vdevice.attr->a.fontnum = 0;
	vdevice.attr->a.mode = 0;
	vdevice.attr->a.backface = 0;

	if ((*vdevice.dev.Vinit)()) {
		vdevice.initialised = 1;
		vdevice.CurrentGSpaceID = 0;

		vdevice.inobject = 0;
		vdevice.inpolygon = 0;

    /*
     * Init colormap related variables
     *
     * Signal that the first two elements (B/W) are in use
     * and signal that the rest aren't
     *
     * Also, signal that the colorramps are all free
     */
        vdevice.rF_sinuse [0] = TRUE;
        vdevice.rF_sinuse [1] = TRUE;
        for (i = 2; i < d_COLMAP_OFF; i++)
        {
            vdevice.rF_sinuse [i] = FALSE;
        }
        for (i = 0; i < d_COLORRAMPS; i ++)
        {
            vdevice.rF_coluse [i] = FALSE;
            vdevice.rrR_rard [i][0] = vdevice.rrR_rard [i][1] =
                vdevice.rrR_rard [i][2] = 1.0;
        }
		V_InitColors();

		viewport((Screencoord)0, (Screencoord)vdevice.sizeSx - 1,
			(Screencoord)0, (Screencoord)vdevice.sizeSy - 1);

		ortho2(0.0, (Coord)(vdevice.sizeSx - 1), 0.0, (Coord)(vdevice.sizeSy - 1));

		/*
		 * Before we call move for the first time we should
		 * init vdevice.transmat->m and vdevice.transmatbottom->m
		 * since they get called inside of move()
		 */
		V_ZeroMatrix( vdevice.transmat->m );
		V_ZeroMatrix( vdevice.transmatbottom->m );
		move(0.0, 0.0, 0.0);

		font(0);	/* set up default font */

	} else {
		fprintf(stderr, "vogl: error while setting up device\n");
		exit(1);
	}

	setlinestyle(0);
}

/*
 * gconfig
 *
 *	thankfully a noop.
 */
void gconfig(void)
{
}

/*
 * vnewdev
 *
 * reinitialize vogl to use a new device but don't change any
 * global attributes like the window and viewport settings.
 */
void vnewdev(char *device)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("vnewdev: vogl not initialised\n");
#endif


	pushviewport();	

	(*vdevice.dev.Vexit)();

	vdevice.initialised = 0;

	getdevice(device);

	(*vdevice.dev.Vinit)();

	vdevice.initialised = 1;

	/*
	 * Need to update font for this device...
	 */
	font(vdevice.attr->a.fontnum);

	popviewport();
}

/*
 * vgetdev
 *
 *	Returns the name of the current vogl device 
 *	in the buffer buf. Also returns a pointer to
 *	the start of buf.
 */
char	* vgetdev(char	*buf)
{
	/*
	 * Note no exit if not initialized here - so that gexit
	 * can be called before printing the name.
	 */
	if (vdevice.dev.devname)
		strcpy(buf, vdevice.dev.devname);
	else
		strcpy(buf, "(no device)");

	return(&buf[0]);
}

/*
 * getvaluator
 *
 *	similar to the VOGLE locator only it returns either x (MOUSEX) or y (MOUSEY).
 */
long getvaluator(Device dev)
{
	int	a, b, c;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("getvaluator: vogl not initialised");
#endif


	c = (*vdevice.dev.Vlocator)(&a, &b);

	if (c != -1) {
		if (dev == MOUSEX)
			return((long)a);
		else 
			return((long)b);
	}

	return((long) -1);
}

/*
 * getbutton
 *
 *	returns the up (or down) state of a button. 1 means down, 0 up,
 * -1 invalid.
 */
long getbutton(Device dev)
{
	int	a, b, c;

	if (dev < 256) {
		c = (*vdevice.dev.Vcheckkey)();
		if (c >= 'a' && c <= 'z')
			c = c - 'a' + 'A';
		if (c == dev)
			return((long)1);
		return((long) 0);
	} else if (dev < 261) {
		c = (*vdevice.dev.Vlocator)(&a, &b);
		if (c & 0x01 && dev == MOUSE3)
			return((long)1);
		if (c & 0x02 && dev == MOUSE2)
			return((long)1);
		if (c & 0x04 && dev == MOUSE1)
			return((long) 1);
		return((long) 0);
	}

	return((long) -1);
}

/*
 * Get the values of the valuators in devs and put them into vals
 */
void  getdev(long n, Device devs[], short vals[])
{
	int	i;

	for( i=0; i < n; i++)
		vals[i] = (short)getvaluator(devs[i]);
}


/*
 * clear
 *
 *	clears the screen to the current colour, excepting devices
 * like a laser printer where it flushes the page.
 *
 */
void clear(void)
{

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("clear: vogl not initialised");
#endif


	if (vdevice.inobject) {
		Token *tok = newtokens(1);
		tok->i = CLEAR;

		return;
	}

	REALCLEARSCREEN;
}

/*
 * colorf
 *
 *	set the current colour to colour index given by
 * the rounded value of f.
 *
 */
void colorf(float f)
{
	color((int)(f + 0.5));
}

/*
 * color
 *
 *	set the current colour to colour index number i.
 *
 */
void color(int i)
{

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("color: vogl not initialised");
#endif


	if (vdevice.inobject) {
		Token *tok = newtokens(2);

		tok[0].i = COLOR;
		tok[1].i = i;
		return;
	}

	vdevice.attr->a.color = i;
	REALSETCOLOR(i);
}

long getcolor( void )
{
	return((long)vdevice.attr->a.color);
}

/*
 * mapcolor
 *
 *	set the color of index i.
 */
void mapcolor(Colorindex i, short r, short g, short b)
{

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("mapcolor: vogl not initialised");
#endif


	if (vdevice.inobject) {
		Token *tok = newtokens(5);

		tok[0].i = MAPCOLOR;
		tok[1].i = i;
		tok[2].i = r;
		tok[3].i = g;
		tok[4].i = b;

		return;
	}
	(*vdevice.dev.Vmapcolor)(i, r, g, b);
}

/*
 * getplanes
 *
 *	Returns the number if bit planes on a device.
 */
long getplanes(void)
{
#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("getdepth: vogl not initialised\n");
#endif


	return((long)vdevice.depth);
}

/*
 * reshapeviewport
 *
 *	Simply sets the viewport to the size of the current window
 */
void reshapeviewport(void)
{
	viewport(0, vdevice.sizeSx - 1, 0, vdevice.sizeSy - 1);
}

/*
 * winconstraints
 *		- does nothing
 */
void winconstraints(void)
{
}

/*
 * keepaspect
 *		- does nothing
 */
void keepaspect(void)
{
}

/*
 * shademodel
 *		- does nothing
 */
void shademodel(long model)
{
}

/*
 * getgdesc
 *
 *	Inquire about some stuff....
 */
long getgdesc(long inq)
{	
	/*
	 * How can we know before the device is inited??
	 */

	switch (inq) {
	case GD_XPMAX:
		if (vdevice.initialised)
			return((long)vdevice.sizeSx);
		else
			return((long) 500);	/* A bullshit number */
	case GD_YPMAX:
		if (vdevice.initialised)
			return((long)vdevice.sizeSy);
		else
			return((long) 500 );
	default:
		return((long) -1);
	}
}

/*
 * foregound
 * 		Dummy - does nothing.
 */
void foreground(void )
{
}

/*
 * vsetflush
 *
 * Controls flushing of the display - we can get considerable
 * Speed up's under X11 using this...
 */
void vsetflush(int yn)
{
#ifdef UNIX
	vdevice.sync = yn;
#endif
}

/*
 * vflush
 *
 * Explicitly call the device flushing routine...
 * This is enabled for object so that you can force an update
 * in the middle of an object, as objects have flushing off
 * while they are drawn anyway.
 */
void vflush(void )
{

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("vflush: vogl not initialised");
#endif


	if (vdevice.inobject) {
		Token *tok = newtokens(1);
		tok->i = VFLUSH;

		return;
	}
#ifdef UNIX
	(*vdevice.dev.Vsync)();
#endif
}


/* 
 * getorigin
 *
 *	Returns the origin of the window. This is a dummy.
 */
void getorigin(long *x, long *y)
{
	*x = *y = 0;
}

/*
 * getsize
 *
 *	Returns the approximate size of the window (some window managers
 *	stuff around with your borders).
 */
void getsize(long *x, long *y)
{
	*x = (long)vdevice.sizeSx;
	*y = (long)vdevice.sizeSy;
}

