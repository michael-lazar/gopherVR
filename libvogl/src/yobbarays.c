/********************************************************************
 * $Id: yobbarays.c,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include "vogl.h"

struct	YOBBA	{
		union {
			struct {
				unsigned alpha : 1;
				unsigned beta  : 2;
				unsigned fixe  : 3;
				unsigned kralb  : 1;
			} vals;
			struct {
				unsigned char yobbav;
			} yobbavals;
		} yobba;
} *yobbaray;

/*
 * yobbarays
 *
 *	Turns on (or off) yobba rays, as described by Larry Dart's friend.
 *
 *	onoff <> 0 - YOBBARAYS ON.
 *	onoff =  0 - YOBBARAYS OFF.
 */
void yobbarays(int onoff)
{
	yobbaray = (struct YOBBA *)onoff;
}
