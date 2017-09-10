/********************************************************************
 * $Id: scale.c,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"

/*
 * scale
 * 
 * Set up a scale matrix and premultiply it and 
 * the top matrix on the stack.
 *
 */
void scale(register const float x, register const float y, 
	   register const float z)
{
     register Mstack *ms;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("scale: vogl not initialised");
#endif


     if (vdevice.inobject) {
	  Token *tok = newtokens(4);
	  
	  tok[0].i = SCALE;
	  tok[1].f = x;
	  tok[2].f = y;
	  tok[3].f = z;
	  
	  return;
     }
     
     /*
      * Do the operations directly on the top matrix of
      * the stack to speed things up.
      */
     ms  = vdevice.transmat;

     ms->m[0][0] *= x;
     ms->m[0][1] *= x;
     ms->m[0][2] *= x;
     ms->m[0][3] *= x;
     
     ms->m[1][0] *= y;
     ms->m[1][1] *= y;
     ms->m[1][2] *= y;
     ms->m[1][3] *= y;
     
     ms->m[2][0] *= z;
     ms->m[2][1] *= z;
     ms->m[2][2] *= z;
     ms->m[2][3] *= z;
}
