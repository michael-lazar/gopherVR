/* defs.h 
	vi:ts=3 sw=3:
 */

/* $Id: defs.h,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $ 
 * $Log: defs.h,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:37  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.5  1995/02/01  16:39:04  espie
 * Includes moved to defs.h
 *
 * Revision 4.4  1995/01/28  09:23:59  espie
 * Added #ifdef for MIN/MAX.
 *
 * Revision 4.2  1994/07/04  13:57:32  espie
 * Binary stuff.
 * Fixed up ansi C stupid bug.
 * Added prototypes, and `generic' values.
 * Better portability
 * Stupid fix + type casts.
 * Amiga support.
 * Protracker commands.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef MALLOC_NOT_IN_STDLIB
#include <malloc.h>
#endif

#ifndef EXPAND_WILDCARDS
#define EXPAND_WILDCARDS(x,y)
#endif

#ifdef BINARY_HEEDED
#define READ_ONLY    "rb"
#define WRITE_ONLY   "wb"
#else
#define READ_ONLY    "r"
#define WRITE_ONLY   "w"
#endif

#define LOCAL static
/* X is too short */
#define XT extern

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef MIN
#define MIN(A,B) ((A)<(B) ? (A) : (B))
#endif
#ifndef MAX
#define MAX(A,B) ((A)>(B) ? (A) : (B))
#endif
     
#define D fprintf(stderr, "%d\n", __LINE__);

typedef union
   {
   unsigned long scalar;
   float real;
   GENERIC pointer;
   } VALUE;

/* predefinitions for relevant structures */
struct tag;
struct channel;
struct song;
struct automaton;
struct sample_info;
struct event;



