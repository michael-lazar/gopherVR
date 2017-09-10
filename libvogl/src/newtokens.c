/********************************************************************
 * $Id: newtokens.c,v 1.1.1.1 2002/01/18 16:34:30 lindner Exp $
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

static TokList		*current = NULL;

/*
 * newtokens
 *
 *	returns the space for num tokens
 */
Token * newtokens(int num)
{
	TokList	*tl;
	Token	*addr;
	Token   *temptokens;
	int	size;

	if (vdevice.tokens == (TokList *)NULL || current == NULL ||
	    num >= MAXTOKS - current->count) {
	     if ((tl = (TokList *)vallocate(sizeof(TokList))) == (TokList *)NULL)
		  verror("newtokens: vallocate returns NULL");
	     
	     tl->count = 0;
	     tl->next = (TokList *)NULL;
	     
	     if (vdevice.tokens != (TokList *)NULL)
		  current->next = tl;
	     else 
		  vdevice.tokens = tl;
	     
	     if (num > MAXTOKS)
		  size = num;
	     else
		  size = MAXTOKS;

	     temptokens = (Token *)vallocate(size * sizeof(Token));

	     if (temptokens== (Token *)NULL)
		  verror("newtokens: malloc returns NULL");

	     tl->toks = temptokens;
	     tl->count = 0;
	     tl->next = (TokList *)NULL;
	     current = tl;
	} 

	addr = &current->toks[current->count];
	current->count += num;

	return(addr);
}
