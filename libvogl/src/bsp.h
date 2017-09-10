/********************************************************************
 * $Id: bsp.h,v 1.1.1.1 2002/01/18 16:34:27 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#ifndef _BSP_H_
#define _BSP_H_

#include <stdio.h>
#include "vogl.h"


typedef struct tagPOLYLISTENTRY
{
     int                F_type;
     
     /* the vertex points on the polygon */
     int     		cVer_numv;
     Vector   		*rVer_polygon;

     float   		R_norm_x, R_norm_y, R_norm_z;
     float   		R_lastx, R_lasty, R_lastz;
     int     		F_color;
     int     		N_fill;
     int		B_split;
     int     		F_Backfacing;
     int         	B_text;
     P_POLYGONLINE	pPgt_string;
     int                cPgt_lines;
     float		R_text_vec_x, R_text_vec_y, R_text_vec_z;
     int		F_text_color;
     int		B_texture;
} POLYGON_ENTRY, *P_POLYGON_ENTRY, **PP_POLYGON_ENTRY;


typedef struct tagPOLYLISTLEAF
{
     int     		F_type;

     /* the unique ID of the Gopherspace Object this polygon is part of */
     int		GSpace_Object_ID;

     /* the vertex points on the polygon */
     int     		cVer_numv;
     Vector		*rVer_polygon;
     
     /* the vertex points of the polygon this polygon was spawn from */
     int                cVer_orig_numv;
     Vector             *arVer_original;

     float   		Surface_norm_x, Surface_norm_y, Surface_norm_z;
     float   		R_lastx, R_lasty, R_lastz;
     int     		F_color;
     int		F_shaded_color;
     int     		N_fill;
     int                B_split;
     int                F_Backfacing;
     int                B_text;
     P_POLYGONLINE	pPgt_string;
     int                cPgt_lines;
     int                F_text_color;
     int                B_texture;

     struct  tagPOLYLISTLEAF  *pPolylist_next;
     struct  tagPOLYLISTLEAF  *pPolylist_previous;
} POLYLIST_LEAF, *P_POLYLIST_LEAF, **PP_POLYLIST_LEAF;


typedef struct tagBSP_NODE 
{
     struct tagPOLYLISTLEAF *root;
     struct tagBSP_NODE     *backChild;
     struct tagBSP_NODE     *frontChild;
} BSP_NODE, *P_BSP_TREE, **PP_BSP_TREE;


extern float rEyeTransX;
extern float rEyeTransY;
extern float rEyeTransZ;

/*
 * Functions
 */

int           genscene(void);

void          bgnscene(int sceneid);
void          endscene(int sceneid);
void          callscene(int sceneid);
void          delscene(int sceneid);

int           V_StartNewGopherObject(void );

void          V_AddPolygonList(P_POLYGON_ENTRY pPolylist_newleaf ) ;
void          V_ClickedOnObjectBSP(register const int ScreenX,
				   register const int ScreenY );


/*int  N_FlatShading(P_POLYLIST_LEAF,Vector, int,int,int,int);*/

/**********************************************************************
 * Make things a lot more readable... 
 */
#define POLYLgetType(a)   ((a)->F_type)
#define POLYLsetType(a,b) ((a)->F_type = (b))

#define POLYLsetTextColor(a,b) ((a)->F_text_color=(b))
#define POLYLgetTextColor(a)   ((a)->F_text_color)

#define POLYLgetGspaceID(a)    ((a)->GSpace_Object_ID)
#define POLYLsetGspaceID(a,b)  ((a)->GSpace_Object_ID=(b))

#define POLYLgetNumVertices(a)   ((a)->cVer_numv)
#define POLYLsetNumVertices(a,b) ((a)->cVer_numv=(b))


#endif /* _BSP_H_ */



