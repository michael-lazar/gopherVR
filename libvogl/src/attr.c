/********************************************************************
 * $Id: attr.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include "vogl.h"
#include "valloc.h"

static	Astack	*asfree = (Astack *)NULL;
static	void copyattributes(Attribute *a, Attribute *b);

/*
 * copyattributes
 *
 *	Copies attribute stack entries from b to a
 */
static	void copyattributes(Attribute *a, Attribute *b)
{
	a->color = b->color;
	a->fontnum = b->fontnum;
	a->ls = b->ls;
	a->lw = b->lw;
	a->backface = b->backface;
}

/*
 * pushattributes
 *
 * save the current attributes on the matrix stack
 *
 */
void pushattributes(void)
{
	Astack	*nattr;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("pushattributes:  vogl not initialised");
#endif
	
	if (vdevice.inobject) {
	     Token *p = newtokens(1);

	     p[0].i = PUSHATTRIBUTES;
	     return;
	}

	if (asfree != (Astack *)NULL) {
		nattr = vdevice.attr;
		vdevice.attr = asfree;
		asfree = asfree->back;
		vdevice.attr->back = nattr;
		copyattributes(&vdevice.attr->a, &nattr->a);
	} else {	
		nattr = (Astack *)vallocate(sizeof(Astack));
		nattr->back = vdevice.attr;
		copyattributes(&nattr->a, &vdevice.attr->a);
		vdevice.attr = nattr;
	}
}

/*
 * popattributes
 *
 * pop the top entry on the attribute stack 
 *
 */
void popattributes(void)
{
	Astack	*nattr;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("popattributes: vogl not initialised");
#endif

	
	if (vdevice.inobject) {
	     Token *p = newtokens(1);

	     p[0].i = POPATTRIBUTES;
	     return;
	}

	if (vdevice.attr->back == (Astack *)NULL) 
		verror("popattributes: attribute stack is empty");
	else {
		font(vdevice.attr->back->a.fontnum);
		nattr = vdevice.attr;
		vdevice.attr = vdevice.attr->back;
		nattr->back = asfree;
		asfree = nattr;
	}

	(*vdevice.dev.Vsetls)(vdevice.attr->a.ls);
	(*vdevice.dev.Vsetlw)(vdevice.attr->a.lw);

	color(vdevice.attr->a.color);
}
