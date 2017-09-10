/********************************************************************
 * $Id: queue.c,v 1.1.1.1 2002/01/18 16:34:27 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include "vogl.h"
#include "vodevice.h"

/*
 * qdevice
 *
 *	enable an input device.
 */
void qdevice(Device dev)
{
	int	i, redraw_enabled;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("qdevice: vogl not initialised\n");
#endif


	if (dev > (Device)MAXDEV)
		verror("qdevice: bad device number passed\n");

	switch (dev) {
	case AKEY:
	case BKEY:
	case CKEY:
	case DKEY:
	case EKEY:
	case FKEY:
	case GKEY:
	case HKEY:
	case IKEY:
	case JKEY:
	case KKEY:
	case LKEY:
	case MKEY:
	case NKEY:
	case OKEY:
	case PKEY:
	case QKEY:
	case RKEY:
	case SKEY:
	case TKEY:
	case UKEY:
	case VKEY:
	case WKEY:
	case XKEY:
	case YKEY:
	case ZKEY:
		vdevice.kbdevents = 1;
		vdevice.enabled[dev / 8] |= 1 << (dev & 0x7);
		vdevice.enabled[(int)(dev - 'A' + 'a') / 8] |= 1 << ((dev - 'A' + 'a') & 0x7);
		break;
	case KEYBD:
		vdevice.kbdmode = 1;
		vdevice.kbdevents = 1;
		/* HACK! (Isn't this whole file a hack?)
		 * Maintain status of REDRAW event here as well
		 */
		redraw_enabled = (vdevice.enabled[REDRAW / 8] & (1 << (REDRAW & 0x7)));
		for (i = 0; i != MAXDEVTABSIZE; i++)
			vdevice.enabled[i] = 0xff;

		vdevice.enabled[0] &= 0xfe;	/* make sure 0 disabled for qtest */

		/* If it wasn't enabled, then reset it to be not enabled */
		if (!redraw_enabled)
			vdevice.enabled[REDRAW / 8] &= ~(1 << (REDRAW & 0x7));

		break;
	case MOUSE1:
	case MOUSE2:
	case MOUSE3:
		vdevice.mouseevents = 1;
		vdevice.enabled[dev / 8] |= 1 << (dev & 0x7);
		break;
	case MINUSKEY:
		vdevice.kbdevents = 1;
		vdevice.enabled['-' / 8] |= 1 << ('-' & 0x7);
		vdevice.enabled['_' / 8] |= 1 << ('_' & 0x7);
		break;
	case BACKSLASHKEY:
		vdevice.kbdevents = 1;
		vdevice.enabled['\\' / 8] |= 1 << ('\\' & 0x7);
		vdevice.enabled['|' / 8] |= 1 << ('|' & 0x7);
		break;
	case EQUALKEY:
		vdevice.kbdevents = 1;
		vdevice.enabled['=' / 8] |= 1 << ('=' & 0x7);
		vdevice.enabled['+' / 8] |= 1 << ('+' & 0x7);
		break;
	case LEFTBRACKETKEY:
		vdevice.kbdevents = 1;
		vdevice.enabled['[' / 8] |= 1 << ('[' & 0x7);
		vdevice.enabled['{' / 8] |= 1 << ('{' & 0x7);
		break;
	case RIGHTBRACKETKEY:
		vdevice.kbdevents = 1;
		vdevice.enabled[']' / 8] |= 1 << (']' & 0x7);
		vdevice.enabled['}' / 8] |= 1 << ('}' & 0x7);
		break;
	case INPUTCHANGE:
		break;
	case REDRAW:
	default:
		vdevice.kbdevents = 1;
		vdevice.enabled[dev / 8] |= 1 << (dev & 0x7);
	}
}

/*
 * unqdevice
 *
 *	disable an input device.
 */
void unqdevice(Device dev)
{
	int	i;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("qdevice: vogl not initialised\n");
#endif


	if (dev > (Device)MAXDEV)
		verror("qdevice: bad device number passed\n");

	switch (dev) {
	case AKEY:
	case BKEY:
	case CKEY:
	case DKEY:
	case EKEY:
	case FKEY:
	case GKEY:
	case HKEY:
	case IKEY:
	case JKEY:
	case KKEY:
	case LKEY:
	case MKEY:
	case NKEY:
	case OKEY:
	case PKEY:
	case QKEY:
	case RKEY:
	case SKEY:
	case TKEY:
	case UKEY:
	case VKEY:
	case WKEY:
	case XKEY:
	case YKEY:
	case ZKEY:
		vdevice.enabled[dev / 8] &= ~(1 << (dev & 0x7));
		vdevice.enabled[(int)(dev - 'A' + 'a') / 8] &= ~(1 << ((dev - 'A' + 'a') & 0x7));
		break;
	case KEYBD:
		vdevice.kbdmode = 0;
		for (i = 0; i != MAXDEVTABSIZE; i++)
			vdevice.enabled[i] = 0x00;
		break;
	case MOUSE1:
	case MOUSE2:
	case MOUSE3:
		vdevice.enabled[dev / 8] &= ~(1 << (dev & 0x7));
		break;
	case MINUSKEY:
		vdevice.enabled['-' / 8] &= ~(1 << ('-' & 0x7));
		vdevice.enabled['_' / 8] &= ~(1 << ('_' & 0x7));
		break;
	case BACKSLASHKEY:
		vdevice.enabled['\\' / 8] &= ~(1 << ('\\' & 0x7));
		vdevice.enabled['|' / 8] &= ~(1 << ('|' & 0x7));
		break;
	case EQUALKEY:
		vdevice.enabled['=' / 8] &= ~(1 << ('=' & 0x7));
		vdevice.enabled['+' / 8] &= ~(1 << ('+' & 0x7));
		break;
	case LEFTBRACKETKEY:
		vdevice.enabled['[' / 8] &= ~(1 << ('[' & 0x7));
		vdevice.enabled['{' / 8] &= ~(1 << ('{' & 0x7));
		break;
	case RIGHTBRACKETKEY:
		vdevice.enabled[']' / 8] &= ~(1 << (']' & 0x7));
		vdevice.enabled['}' / 8] &= ~(1 << ('}' & 0x7));
		break;
	case INPUTCHANGE:
		return;
	default:
		vdevice.enabled[dev / 8] &= ~(1 << (dev & 0x7));
	}

	vdevice.mouseevents = vdevice.kbdevents = 0;

	for (i = 0; i != MAXDEVTABSIZE; i++)
		if (vdevice.enabled[i] != 0) {
			if (i < 32)		/* max bits for keyboard */
				vdevice.kbdevents = 1;
			else				/* must be mouse */
				vdevice.mouseevents = 1;
		}
}

/*
 * qread
 *
 *	a poor man's qread, we only have a device queue one deep.
 * Just sit and poll in case something happens. If nothing is
 * enabled or there are no devices this returns -1.
 */
long qread(short *ret)
{
	int	a, b, c1, c2, val;
	int	eventind, eventmask, retvalue = 0;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("qread: vogl not initialised\n");
#endif


	if (vdevice.alreadyread) {
		/* May have been 'unqdevice'd by the time we get here */
		eventind = vdevice.devno / 8;
		eventmask = 1 << (vdevice.devno & 0x7);
		if (vdevice.enabled[eventind] & eventmask) {
			*ret = vdevice.data;
			if (!vdevice.kbdmode && vdevice.data == 1)
				vdevice.data = 0;
			else
				vdevice.alreadyread = 0;
			return((long) vdevice.devno);
		}
	}

	val = c1 = c2 = 0;

	eventind = 0;
	eventmask = 0;

	if (!vdevice.mouseevents && vdevice.kbdevents) {
		while (!(vdevice.enabled[eventind] & eventmask)) {
			if ((val = c1 = (*vdevice.dev.Vgetkey)()) < 0)
				return((long) val);

			eventind = c1 / 8;
			eventmask = 1 << (c1 & 0x7);
			*ret = c1;
			if (c1 >= 'a' && c1 <= 'z')
				retvalue = c1 - 'a' + 'A';
			else
				retvalue = c1;
		}
	} else if (vdevice.mouseevents && !vdevice.kbdevents) {
		while (!(vdevice.enabled[eventind] & eventmask)) {
			if ((c2 = (*vdevice.dev.Vlocator)(&a, &b)) < 0)
				return((long) c2);

			if (c2 != 0) {
				if (c2 & 0x01)
					c2 = MOUSE3;
				else if (c2 & 0x02)
					c2 = MOUSE2;
				else if (c2 & 0x04)
					c2 = MOUSE1;

				eventind = c2 / 8;
				eventmask = 1 << (c2 & 0x7);
				*ret = c2;
				retvalue = c2;
			}
		}
	} else if (vdevice.mouseevents && vdevice.kbdevents) {
		while (!(vdevice.enabled[eventind] & eventmask)) {
			while (c1 == 0 && c2 == 0) {
				val = c1 = (*vdevice.dev.Vcheckkey)();

				c2 = (*vdevice.dev.Vlocator)(&a, &b);
			}

			if (c1 < 0 && c2 < 0)
				return((long) -1);

			if (c1 != 0) {
				eventind = c1 / 8;
				eventmask = 1 << (c1 & 0x7);
				*ret = c1;
				if (c1 >= 'a' && c1 <= 'z')
					retvalue = c1 - 'a' + 'A';
				else
					retvalue = c1;
			} else {
				if (c2 & 0x01)
					c2 = MOUSE3;
				else if (c2 & 0x02)
					c2 = MOUSE2;
				else if (c2 & 0x04)
					c2 = MOUSE1;
				eventind = c2 / 8;
				eventmask = 1 << (c2 & 0x7);
				*ret = c2;
				retvalue = c2;
			}

			c1 = c2 = 0;
		}
	} else
		return((long) -1);

	if (vdevice.kbdmode && val != 0) {
		*ret = val;
		return((long) KEYBD);
	} else {
		*ret = 1;			/* insert the up event */
		vdevice.data = 0;
		vdevice.devno = retvalue;
		vdevice.alreadyread = 1;
	}

	return((long) retvalue);
}

/*
 * qreset
 *
 *	as the queue is rather short this is easy.
 */
void qreset( void )
{
	while (((*vdevice.dev.Vcheckkey)()))
		;

	vdevice.alreadyread = 0;
}

/*
 * qtest
 *
 *	Check if there is anything in the event queue. As our reads are
 * destructive at the moment, we have to save what we get in the device
 * structure.
 */
long qtest(void)
{
	int	a, b, c1, c2, val;
	int	eventind, eventmask;

#ifdef VDEVICECHECK
	if (!vdevice.initialised)
		verror("qtest: vogl not initialised\n");
#endif


	if (vdevice.alreadyread)
		return((long)vdevice.devno);

	eventind = 0;
	eventmask = 0;

	val = c1 = (*vdevice.dev.Vcheckkey)();

	c2 = (*vdevice.dev.Vlocator)(&a, &b);

	if (c1 != 0) {
		eventind = c1 / 8;
		eventmask = 1 << (c1 & 0x7);
		if (vdevice.kbdmode) {
			vdevice.data = val;
			vdevice.devno = KEYBD;
		} else {
			vdevice.data = 1;
			if (c1 >= 'a' && c1 <= 'z')
				vdevice.devno = c1 - 'a' + 'A';
			else
				vdevice.devno = c1;
		}
	} else {
		if (c2 & 0x01)
			c2 = MOUSE3;
		else if (c2 & 0x02)
			c2 = MOUSE2;
		else if (c2 & 0x04)
			c2 = MOUSE1;
		eventind = c2 / 8;
		eventmask = 1 << (c2 & 0x7);
		vdevice.data = 1;
		vdevice.devno = c2;
	}

	if (vdevice.enabled[eventind] & eventmask) {
		vdevice.alreadyread = 1;
		return((long) 1);
	}

	return((long) 0 );
}

/*
 * isqueued
 *
 *	return non-zero if a device is queued
 */
long isqueued(Device dev)
{
	return((long)vdevice.enabled[dev / 8] & (1 << (dev & 0x7)));
}

