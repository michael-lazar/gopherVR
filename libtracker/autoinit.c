/* autoinit.c 
	vi:ts=3 sw=3:
 */

/* $Id: autoinit.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $ 
 * $Log: autoinit.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.13  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.7  1995/02/01  16:39:04  espie
 * Includes moved to defs.h
 *
 */


#include "defs.h"
#include "extern.h"

ID("$Id: autoinit.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")

LOCAL struct clist
	{
	struct clist *next;
	void (*func) P((void));
	} *list = 0;
	

void at_end(cleanup)
void (*cleanup) P((void));
	{
#ifdef USE_AT_EXIT
	atexit(cleanup);
#else
	struct clist *new;
	new = (struct clist *)malloc(sizeof(struct clist));
	if (!new)
		{
		(*cleanup)();
		end_all("Allocation problem");
		}
	new->next = list;
	new->func = cleanup;
	list = new;
#endif
	}
	
void end_all(s)
char *s;
	{
#ifndef USE_AT_EXIT
	struct clist *p;
#endif
	if (s)
		notice(s);
#ifndef USE_AT_EXIT
	for (p = list; p; p = p->next)
		(p->func)();			/* don't bother freeing (malloc) */
#endif
	exit(s ? 10 : 0);
	}
