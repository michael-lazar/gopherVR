/*----------------------------------------------------------------------------
 * Header File: Standard.h
 *
 * Header file to specify all of the current conventions used for a project.
 * This must be included with each source file.
 *
 * Note that there may be multiple versions of standard.h depending on the
 * differences in project standards.
 *----------------------------------------------------------------------------*/
 
#ifndef _STANDARD_
#define _STANDARD_

#ifdef WINDOWS
#define FAR     far
#else

#ifdef DOS
#define FAR     far
#else
#define FAR
#endif

#endif

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include "hungarian.h"


#ifdef DOS
#include "dos_ext.h"
#else
typedef P_VOID  F_VOID;
#endif

/*
 * IN, OUT, and OPTIONAL specifiers used to tag function parameters.
 */
#ifndef IN

#define     IN
#define     OUT
#define     OPTIONAL

#endif

/*
 * Shortcut for the null characer.
 */
#ifndef NUL
#define     NUL         '\0'
#endif

/*
 * Often used file I/O stuff.
 */
typedef FILE    * P_FILE, ** PP_FILE;
typedef INT FD, * P_FD, ** PP_FD;

#define UNUSED_PARAMETER(x) (x==x)

typedef VOID    (*FN_DESTRUCTOR) (F_VOID);
typedef VOID    (*FN_CALLBACK) (F_VOID, F_VOID);
typedef VOID    (*FN_DEBUG_PRT) (F_VOID, P_FILE, INDEX);

typedef struct tm       TM_STRUCT,   * P_TM_STRUCT,   ** PP_TM_STRUCT;
typedef time_t          TIME_T,      * P_TIME_T,      ** PP_TIME_T;
typedef struct stat     STAT_STRUCT, * P_STAT_STRUCT, ** PP_STAT_STRUCT;    


#endif   /* _STANDARD_ */

