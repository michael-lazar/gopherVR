/********************************************************************
 * $Id: buffer.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"

static	int	BufferSync = 1;


/*
 * backbuffer
 *
 *	swap drawing to backbuffer - returns -1 if no
 * backbuffer is available.
 */
void backbuffer(int yes)
{

#ifdef VDEVICECHECK
     if (!vdevice.initialised)
	  verror("backbuffer: vogl not initialised.");
#endif

     
     if (vdevice.inobject) {
	  Token *tok = newtokens(2);
	  tok[0].i = BACKBUFFER;
	  tok[1].i = yes;
	  return;
     }
     
     if (vdevice.attr->a.mode == SINGLE)
	  return;
     
     if (yes) {
	  if ((*vdevice.dev.Vbackb)() < 0)
	       verror("device doesn't support double buffering\n");
	  
	  vdevice.inbackbuffer = 1;
#ifdef UNIX
	  vdevice.sync = 0;
#endif
     } else
	  vdevice.inbackbuffer = 0;
     
}

/*
 * frontbuffer
 *
 *	start drawing in the front buffer again. This
 * will always work!
 */
void frontbuffer(int yes)
{

#ifdef VDEVICECHECK
     if (!vdevice.initialised)
	  verror("frontbuffer: vogl not initialised.");
#endif

     
     if (vdevice.inobject) {
	  Token	*tok = newtokens(2);
	  tok[0].i = FRONTBUFFER;
	  tok[1].i = yes;
	  return;
     }

     if (vdevice.attr->a.mode == SINGLE)
	  return;
     
     if (yes) {
	  (*vdevice.dev.Vfrontb)();
	  vdevice.inbackbuffer = 0;
#ifdef UNIX
	  vdevice.sync = BufferSync;
#endif
     } else
	  backbuffer( yes );
}

/*
 * swapbuffers
 *
 *	swap the back and front buffers - returns -1 if
 * no backbuffer is available.
 */
void swapbuffers( void )
{
     if (vdevice.inobject) {
	  Token	*tok = newtokens(1);
	  tok[0].i = SWAPBUFFERS;
	  return;
     }

#ifdef VDEVICECHECK
     if (!vdevice.initialised)
	  verror("swapbuffers: vogl not initialised.");
#endif


     if ((*vdevice.dev.Vswapb)() < 0)
	  verror("swapbuffers device doesn't support double buffering\n");
     
}

/*
 * doublebuffer()
 *
 *	Flags our intention to do double buffering....
 *	tries to set it up etc etc...
 */
void doublebuffer(void)
{
#ifdef VDEVICECHECK
     if (!vdevice.initialised)
	  verror("doublebuffer: vogl not initialised.");
#endif

     
     if ((*vdevice.dev.Vbackb)() < 0)
	  verror("device doesn't support double buffering\n");

     vdevice.inbackbuffer = 1;
#ifdef UNIX
     BufferSync = vdevice.sync;
     vdevice.sync = 0;
#endif
}

/*
 * singlebuffer()
 *
 *	Goes back to singlebuffer mode....(crock)
 */
void singlebuffer(void)
{ 
     if (vdevice.attr->a.mode == SINGLE)
	  return;

     (*vdevice.dev.Vfrontb)();

     vdevice.inbackbuffer = 0;

#ifdef UNIX
     if (BufferSync)
	  vdevice.sync = 1;
#endif
}
