/********************************************************************
 * $Id: hungarian.h,v 1.1.1.1 2002/01/18 16:34:29 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 * Portions Copyright (C) 1991, 1992 The University of Melbourne
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

/*-----------------------------------------------------------------------------
 * Header File: Hungarian.h
 *
 * Header file to specify all of the master types for the Hungarian naming
 * convention.  This must be included with each source file.
 *----------------------------------------------------------------------------*/
#ifndef _HUNGARIAN_
#define _HUNGARIAN_

/* Convenient defines. Standard C Base Types, Should Always Match C Types. */
#define CONST           const
#define ENUM            enum
#define REGISTER        register
#define STATIC          static
#define INT             int
#define VOID            void
#define VOLATILE        volatile
#define GLOBAL


/* Other Pre-Defined Base Types, Derived From C Base Types. */    /* notation */
typedef  unsigned       BOOL,       * P_BOOL,       ** PP_BOOL;     /* B   */
typedef  unsigned       BIT;                                        /* Bi# */
typedef  unsigned char  BYTE,       * P_BYTE,       ** PP_BYTE;     /* By  */
typedef  char           CHAR,       * P_CHAR,       ** PP_CHAR;     /* Ch  */
typedef  double         DOUBLE,     * P_DOUBLE,     ** PP_DOUBLE;   /* D   */
typedef  unsigned long  DWORD,      * P_DWORD,      ** PP_DWORD;    /* Dw  */
typedef  int            FLAG,       * P_FLAG,       ** PP_FLAG;     /* F   */
typedef  float          FLOAT,      * P_FLOAT,      ** PP_FLOAT;    /* R   */
typedef  int            NUMBER,     * P_NUMBER,     ** PP_NUMBER;   /* N   */
typedef  unsigned       U_NUMBER,   * PU_NUMBER,    ** PPU_NUMBER;  /* uN  */
typedef  short          S_NUMBER,   * PS_NUMBER,    ** PPS_NUMBER;  /* sN  */
typedef  unsigned short US_NUMBER,  * PUS_NUMBER,   ** PPUS_NUMBER; /* usN */
typedef  long           L_NUMBER,   * PL_NUMBER,    ** PPL_NUMBER;  /* lN  */
typedef  unsigned long  UL_NUMBER,  * PUL_NUMBER,   ** PPUL_NUMBER; /* ulN */
typedef  const P_CHAR   STRING,     * P_STRING,     ** PP_STRING;   /* Sz  */
typedef  void                       * P_VOID,       ** PP_VOID;     /* V   */
typedef  unsigned short WORD,       * P_WORD,       ** PP_WORD;     /* W   */


/* Constructors Which Require A Typedef. */                       /* notation */
typedef  int            COUNT,      * P_COUNT,      ** PP_COUNT;    /* c */
typedef  int            INDEX,      * P_INDEX,      ** PP_INDEX;    /* i */
typedef  int            OFFSET,     * P_OFFSET,     ** PP_OFFSET;   /* o */


/* Defined For Convenience, Used For Bool. */
#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

#endif   /* _HUNGARIAN_ */
