/********************************************************************
 * $Id: lines.h,v 1.1.1.1 2002/01/18 16:34:28 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _LINES_H_
#define _LINES_H_


/* protos for lines.c */


void deflinestyle(short n, Linestyle ls);
void setlinestyle(short n);
void linewidth(short w);
void linewidthf(float w);

#endif /* _LINES_H_ */
