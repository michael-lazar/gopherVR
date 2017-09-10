/********************************************************************
 * $Id: bsp.c,v 1.1.1.1 2002/01/18 16:34:27 lindner Exp $
 *
 * Copyright 1995 by the Regents of the University of Minnesota
 *
 * see the file "COPYRIGHT" in the distribution for conditions of use.
 *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "vogl.h"
#include "bsp.h"
#include "minv4.h"
#include "polygons.h"
#include "shading.h"
#include "geometry.h"
#include "valloc.h"

#ifdef MACINTOSH
#include <fp.h>
#else
#include <math.h>
#endif

#define EPSILON2 0.001

enum PolyLocationEnum {
     IN_FRONT   = 0,
     IN_BACK    = 1,
     SPLIT      = 2,
     IN_NEITHER = 3
};
typedef enum PolyLocationEnum PolyLocation;

/***********************************************************************
 * Our array of scenes...
 */

struct BspTree_struct
{
     P_POLYLIST_LEAF pPolyList_head;
     P_BSP_TREE  pBSP_tree_root;
     
     int gF_shaded_already;
};
typedef struct BspTree_struct BspTree;

static BspTree    *Scenes[MAXSCENES];
static int        scenenum  = 0;



/* 
 * the transformed back to world cordinates eyepoint. VERY useful when
 * calculating flat shading on polygons dynamically since you want to
 * know where the eye is to do this properly -mpm 
*/

float rEyeTransX;
float rEyeTransY;
float rEyeTransZ;

static int ClickedScreenX;
static int ClickedScreenY;

static POLYLIST_LEAF *DUMB_BSP_selectAndRemovePoly(P_POLYLIST_LEAF 
						   *pSomePolygon);
static POLYLIST_LEAF *BSP_selectAndRemovePoly(P_POLYLIST_LEAF *pSomePolygon);
static P_BSP_TREE    BSP_CombineTree(P_BSP_TREE TheFrontList, 
				     P_POLYLIST_LEAF TheRoot, 
				     P_BSP_TREE TheBackList);
static POLYLIST_LEAF *BSPSpliceOut( P_POLYLIST_LEAF *APolygonList );
static P_BSP_TREE    BSP_makeTree(P_POLYLIST_LEAF *pThePolygonList);
static int           BSP_pointInFrontOf(POLYLIST_LEAF *pThisPoly, 
					float x, float y, float z);

static void          V_CopyPolylistAttr(P_POLYLIST_LEAF pPgl_orig, 
					P_POLYLIST_LEAF *pPgl_child);
static PolyLocation  BSP_pointInFrontOf2(POLYLIST_LEAF *pThisPoly, 
					 float x, float y, float z);
static PolyLocation  BSP_LocationOfFace(P_POLYLIST_LEAF Pgl_root, 
					P_POLYLIST_LEAF Pgl_leaf );
static int           BSP_SplitPolygon(P_POLYLIST_LEAF pPgl_root, 
				      P_POLYLIST_LEAF pPgl_leaf,
				      P_POLYLIST_LEAF *pPgl_first,
				      P_POLYLIST_LEAF *pPgl_second);

static void          BSPListAdd(P_POLYLIST_LEAF *pThelist, 
				P_POLYLIST_LEAF pThe_New_Entry );
static void          BSP_DisplayRoot(register const P_POLYLIST_LEAF pThe_root);
static void          BSP_DisplayBSP( register const P_BSP_TREE pThe_BSP_tree);
static void          V_UpdateTextLines(int C_lines_added, 
				       PP_POLYGONLINE ppPgt_new_string, 
				       POLYGONLINE Pgt_old_string );
static void          BSP_Shade( PP_BSP_TREE ppBSP_node );
static void          BSP_ClearBSP( P_BSP_TREE pBSP_root_node );
static void          BSP_ClickedOnObject( P_BSP_TREE pThe_BSP_tree);



/***********************************************************************
 * Copies *some* of the attributes from one polygon (the parent)
 * to another (the child)
 */

static void 
V_CopyPolylistAttr( P_POLYLIST_LEAF pPgl_orig, P_POLYLIST_LEAF *pPgl_child )
{   
     int i;

     (*pPgl_child)->F_type = pPgl_orig->F_type; 
     (*pPgl_child)->GSpace_Object_ID = pPgl_orig->GSpace_Object_ID; 

     /* start with no vertices */
     (*pPgl_child)->cVer_numv = 0;

     (*pPgl_child)->Surface_norm_x = pPgl_orig->Surface_norm_x;
     (*pPgl_child)->Surface_norm_y = pPgl_orig->Surface_norm_y;
     (*pPgl_child)->Surface_norm_z = pPgl_orig->Surface_norm_z;

     (*pPgl_child)->R_lastx = pPgl_orig->R_lastx;
     (*pPgl_child)->R_lasty = pPgl_orig->R_lasty;
     (*pPgl_child)->R_lastz = pPgl_orig->R_lastz;

     (*pPgl_child)->F_color = pPgl_orig->F_color;
     (*pPgl_child)->N_fill = pPgl_orig->N_fill;
     (*pPgl_child)->B_split = TRUE;
     (*pPgl_child)->F_Backfacing = pPgl_orig->F_Backfacing;
     (*pPgl_child)->F_text_color = pPgl_orig->F_text_color;
     
     /* if there is no text on the parent then the child has no text 
	if the IS text on the parent then it will be stuck onto the child
	in the BSPsplit function */
     if (pPgl_orig->B_text == FALSE ) {
	  (*pPgl_child)->cPgt_lines = 0;
	  (*pPgl_child)->B_text = FALSE; 
	  (*pPgl_child)->pPgt_string = (P_POLYGONLINE) (NULL);           
     }
     (*pPgl_child)->B_texture  = pPgl_orig->B_texture;
     
     if (TRUE == pPgl_orig->B_split )
     {
	  (*pPgl_child)->cVer_orig_numv = pPgl_orig->cVer_orig_numv; 
	  
	  (*pPgl_child)->arVer_original = 
	       (Vector *) Cvallocate( pPgl_orig->cVer_orig_numv, sizeof( Vector ) ); 
	  
	  
	  for ( i = 0; i < pPgl_orig->cVer_orig_numv ; i++ )
	  {
	       (*pPgl_child)->arVer_original [i][V_X] = 
		    pPgl_orig->arVer_original [i][V_X];
	       (*pPgl_child)->arVer_original [i][V_Y] =
		    pPgl_orig->arVer_original [i][V_Y];
	       (*pPgl_child)->arVer_original [i][V_Z] =
		    pPgl_orig->arVer_original [i][V_Z];
	       (*pPgl_child)->arVer_original [i][V_W] =
		    pPgl_orig->arVer_original [i][V_W];
	       
	  }
     }
     else
     {
	  (*pPgl_child)->cVer_orig_numv = pPgl_orig->cVer_numv;
	  
	  (*pPgl_child)->arVer_original = 
	       (Vector *) Cvallocate( pPgl_orig->cVer_numv, sizeof( Vector ) );
	  
	  for ( i = 0; i < pPgl_orig->cVer_numv ; i++ )
	  {
	       (*pPgl_child)->arVer_original [i][V_X] = 
		    pPgl_orig->rVer_polygon [i][V_X];
	       (*pPgl_child)->arVer_original [i][V_Y] = 
		    pPgl_orig->rVer_polygon [i][V_Y];
	       (*pPgl_child)->arVer_original [i][V_Z] = 
		    pPgl_orig->rVer_polygon [i][V_Z];
	       (*pPgl_child)->arVer_original [i][V_W] =
		    pPgl_orig->rVer_polygon [i][V_W];
	  }
	  
	  
	  /*
	   * Wrap make a connection from the last point to the first
	   *
	   * I don't think we really want to do this since it appears that it
	   * results in bogus vertices being introduced into the polygon. I
	   * saw this as lines drawing from the origin at some view angles for
	   * the kiosk.
	   * So... its commented out for now -mpm
	   *
	   (*pPgl_child)->arVer_original [pPgl_orig->cVer_numv][V_X] =
	   pPgl_orig->rVer_polygon [0][V_X];
	   (*pPgl_child)->arVer_original [pPgl_orig->cVer_numv][V_Y] =
	   pPgl_orig->rVer_polygon [0][V_Y];
	   (*pPgl_child)->arVer_original [pPgl_orig->cVer_numv][V_Z] =
	   pPgl_orig->rVer_polygon [0][V_Z];
	   (*pPgl_child)->arVer_original [pPgl_orig->cVer_numv][V_W] =
	   pPgl_orig->rVer_polygon [0][V_W];
	   */
	  
     }

     (*pPgl_child)->pPolylist_next = NULL;
     (*pPgl_child)->pPolylist_previous = NULL;
}

/**********************************************************************
 * Generate a new identifier for the scene...
 * 
 * Returns -1 if there's no room...
 */

int genscene(void) {
     int i;

     if (scenenum == -1) {
	  for (i=0; i < MAXSCENES; i++)
	       Scenes[scenenum] = NULL;
     }	  

     for (i = 0; i < MAXSCENES; i++) {
	  if (Scenes[i] == NULL) {
	       Scenes[i] = (BspTree*) Cvallocate(1, sizeof(BspTree));
	       Scenes[i]->gF_shaded_already = FALSE;

	       Scenes[i]->pPolyList_head = NULL;
	       Scenes[i]->pBSP_tree_root = NULL;

	       scenenum = i;
	       return(i);

	  }
     }
     return(-1);
}

/***********************************************************************
 * prepare for a new scene
 */
void bgnscene(int sceneid)
{
     Matrix mat;

     /*
      * make sure there is an identity matrix at the top of the 
      * transformation matrix stack
      * we don't want to do BSP in a perspective space!
      */
     identmatrix(mat);
     loadmatrix(mat);

     /* initialize the polygon list */
     Scenes[sceneid]->pPolyList_head = NULL;
     /* initialize the tree */
     Scenes[sceneid]->pBSP_tree_root = NULL;
	
     /* Reset the gopherspace id #'s */
     vdevice.CurrentGSpaceID = 0;
}




/*
 * Add a polygon to the list of polygons the BSP tree should
 * be constructed from
 */
void V_AddPolygonList( P_POLYGON_ENTRY pPolylist_newleaf ) 
{
     P_POLYLIST_LEAF pPolylist_tmp;
     int         	irVer_i;
     int         	i;
     
     pPolylist_tmp = (POLYLIST_LEAF *) Cvallocate(1, sizeof( POLYLIST_LEAF ));
     
     POLYLsetType(pPolylist_tmp, POLYLgetType(pPolylist_newleaf));
     POLYLsetGspaceID(pPolylist_tmp, vdevice.CurrentGSpaceID);
     POLYLsetTextColor(pPolylist_tmp, 0);
     POLYLsetNumVertices(pPolylist_tmp, POLYLgetNumVertices(pPolylist_newleaf));
     
     ((POLYLIST_LEAF *) pPolylist_tmp)->rVer_polygon = 
	  (Vector *) Cvallocate( pPolylist_tmp->cVer_numv, sizeof( Vector ) ); 
     
     for (irVer_i = 0; irVer_i < POLYLgetNumVertices(pPolylist_tmp); irVer_i++)
     {
	  pPolylist_tmp->rVer_polygon [irVer_i][V_X] = 
	       pPolylist_newleaf->rVer_polygon [irVer_i][V_X]; 
	  pPolylist_tmp->rVer_polygon [irVer_i][V_Y] = 
	       pPolylist_newleaf->rVer_polygon [irVer_i][V_Y];
	  pPolylist_tmp->rVer_polygon [irVer_i][V_Z] = 
	       pPolylist_newleaf->rVer_polygon [irVer_i][V_Z];
	  pPolylist_tmp->rVer_polygon [irVer_i][V_W] =
	       pPolylist_newleaf->rVer_polygon [irVer_i][V_W];
     }
     
     pPolylist_tmp->cVer_orig_numv  = pPolylist_newleaf->cVer_numv;
     ((POLYLIST_LEAF *) pPolylist_tmp)->arVer_original = 
	  (Vector *) Cvallocate( pPolylist_newleaf->cVer_numv, sizeof( Vector ) );
     
     for ( irVer_i = 0; irVer_i < pPolylist_tmp->cVer_orig_numv ; irVer_i++ )
     {
	  pPolylist_tmp->arVer_original [irVer_i][V_X] =
	       pPolylist_newleaf->rVer_polygon [irVer_i][V_X];
	  pPolylist_tmp->arVer_original [irVer_i][V_Y] =
	       pPolylist_newleaf->rVer_polygon [irVer_i][V_Y];
	  pPolylist_tmp->arVer_original [irVer_i][V_Z] =
	       pPolylist_newleaf->rVer_polygon [irVer_i][V_Z];
	  pPolylist_tmp->arVer_original [irVer_i][V_W] =
	       pPolylist_newleaf->rVer_polygon [irVer_i][V_W];
     }
     
     
     if ( pPolylist_tmp->cVer_numv >= 2 ) {
	  float R_ax, R_ay, R_az;
	  float R_bx, R_by, R_bz;
	  float R_nx, R_ny, R_nz;

	  float denominator;
	  
	  /*
	   * calculate Normal vector to the plane
	   */
	  R_ax = pPolylist_newleaf->rVer_polygon [0][V_X] - 
	       pPolylist_newleaf->rVer_polygon [1][V_X];
	  R_ay = pPolylist_newleaf->rVer_polygon [0][V_Y] - 
	       pPolylist_newleaf->rVer_polygon [1][V_Y];
	  R_az = pPolylist_newleaf->rVer_polygon [0][V_Z] - 
	       pPolylist_newleaf->rVer_polygon [1][V_Z];
	  
	  R_bx = pPolylist_newleaf->rVer_polygon [2][V_X] - 
	       pPolylist_newleaf->rVer_polygon [1][V_X];
	  R_by = pPolylist_newleaf->rVer_polygon [2][V_Y] - 
	       pPolylist_newleaf->rVer_polygon [1][V_Y];
	  R_bz = pPolylist_newleaf->rVer_polygon [2][V_Z] - 
	       pPolylist_newleaf->rVer_polygon [1][V_Z];
	  
	  R_nx = -((R_ay * R_bz) - (R_az * R_by));
	  R_ny = (R_ax * R_bz) - (R_az * R_bx);
	  R_nz = -((R_ax * R_by) - (R_ay * R_bx));
	  
	  /*
	   * Normalize the vector
	   */  
	  denominator = sqrt((SQ((R_nx)) + SQ((R_ny)) +  SQ((R_nz))));
	  if ( denominator != 0.0 ) {
	       R_nx = R_nx / denominator;
	       R_ny = R_ny / denominator;
	       R_nz = R_nz / denominator;
	       if ( R_nx == -0 )  R_nx = 0.0;
	       if ( R_ny == -0 )  R_ny = 0.0;
	       if ( R_nz == -0 )  R_nz = 0.0;
	  } else {
	       R_nx = 0.0;
	       R_ny = 0.0;
	       R_nz = 0.0;
	  }
         
	  pPolylist_tmp->Surface_norm_x = R_nx;
	  pPolylist_tmp->Surface_norm_y = R_ny;
	  pPolylist_tmp->Surface_norm_z = R_nz;
    
     }

     pPolylist_tmp->R_lastx = pPolylist_newleaf->R_lastx;
     pPolylist_tmp->R_lasty = pPolylist_newleaf->R_lasty;
     pPolylist_tmp->R_lastz = pPolylist_newleaf->R_lastz;
     pPolylist_tmp->F_color = pPolylist_newleaf->F_color;
     pPolylist_tmp->N_fill = pPolylist_newleaf->N_fill;
     pPolylist_tmp->F_Backfacing = pPolylist_newleaf->F_Backfacing;
     pPolylist_tmp->B_split = pPolylist_newleaf->B_split;

     if ( FALSE == pPolylist_newleaf->B_text ) {
	  pPolylist_tmp->cPgt_lines = 0;
	  ((POLYLIST_LEAF *) pPolylist_tmp)->pPgt_string = (P_POLYGONLINE) (NULL);
     } else {
	  pPolylist_tmp->F_text_color = pPolylist_newleaf->F_text_color;
	  pPolylist_tmp->B_text = pPolylist_newleaf->B_text;
	  pPolylist_tmp->cPgt_lines = pPolylist_newleaf->cPgt_lines;
	  
	  ((POLYLIST_LEAF *) pPolylist_tmp)->pPgt_string = (P_POLYGONLINE) 
	       Cvallocate( pPolylist_newleaf->cPgt_lines, sizeof(POLYGONLINE) );
	  
	  for( i=0; i < pPolylist_newleaf->cPgt_lines; i++ )
	  {
	       pPolylist_tmp->pPgt_string [i].R_move [V_X] = 
                    pPolylist_newleaf->pPgt_string [i].R_move [V_X];
	       pPolylist_tmp->pPgt_string [i].R_move [V_Y] = 
                    pPolylist_newleaf->pPgt_string [i].R_move [V_Y];
	       pPolylist_tmp->pPgt_string [i].R_move [V_Z] =
                    pPolylist_newleaf->pPgt_string [i].R_move [V_Z];
	       
	       pPolylist_tmp->pPgt_string [i].R_draw [V_X] = 
                    pPolylist_newleaf->pPgt_string [i].R_draw [V_X];
	       pPolylist_tmp->pPgt_string [i].R_draw [V_Y] = 
                    pPolylist_newleaf->pPgt_string [i].R_draw [V_Y];
	       pPolylist_tmp->pPgt_string [i].R_draw [V_Z] =
                    pPolylist_newleaf->pPgt_string [i].R_draw [V_Z];
	  }
     }
     
     if ( TRUE == pPolylist_newleaf->B_texture )
     {
	  pPolylist_tmp->B_texture = pPolylist_newleaf->B_texture;
     } else {
	  pPolylist_tmp->B_texture = FALSE;
	  
     }
    

     if ( NULL == Scenes[scenenum]->pPolyList_head )
     {
	  pPolylist_tmp->pPolylist_next = NULL;
	  pPolylist_tmp->pPolylist_previous = NULL;
	  Scenes[scenenum]->pPolyList_head = pPolylist_tmp;
     }
     else
     {
	  /* insert the new element at the head of the list */
	  pPolylist_tmp->pPolylist_previous = NULL;
	  pPolylist_tmp->pPolylist_next = Scenes[scenenum]->pPolyList_head;
	  Scenes[scenenum]->pPolyList_head->pPolylist_previous = pPolylist_tmp;
	  Scenes[scenenum]->pPolyList_head = pPolylist_tmp;
     }
}





static POLYLIST_LEAF *DUMB_BSP_selectAndRemovePoly( P_POLYLIST_LEAF *pSomePolygon )
{
     /*
      * really brain-dead: we are passed the head of the list and we
      * use it as the best polygon (it isn't). Later we will make this
      * smarter so that we use some heuristic to pick a polygon that
      * won't fragment things much 
      */

     P_POLYLIST_LEAF currentItem;
               
     if ( *pSomePolygon == NULL ) {
	  return( NULL );
     } else {
	  currentItem = (P_POLYLIST_LEAF) *pSomePolygon;
	  *pSomePolygon = currentItem->pPolylist_next;
	  if ( *pSomePolygon != NULL ) {
	       (*pSomePolygon)->pPolylist_previous = currentItem->pPolylist_previous;
	  }
	  
	  /* update the next and previous links in the current item */
	  currentItem->pPolylist_previous = NULL;
	  currentItem->pPolylist_next = NULL;
	  
	  return( currentItem );
     }
}





static POLYLIST_LEAF *BSP_selectAndRemovePoly( P_POLYLIST_LEAF *pSomePolygon )
{
     P_POLYLIST_LEAF pCurrentItem;
     P_POLYLIST_LEAF pCheckItem;
     P_POLYLIST_LEAF pBestItem;
     int splitcount;
     int bestcount = 32767;
     
     if ( *pSomePolygon == NULL ) {
	  return( NULL );
     }
     pCurrentItem = (P_POLYLIST_LEAF) *pSomePolygon;
     pCheckItem = pBestItem = pCurrentItem;
     do {
	  splitcount = 0;
	  do {        
	       if (pCheckItem != pCurrentItem ) {
		    if (SPLIT == BSP_LocationOfFace(pCurrentItem,
						    pCheckItem))
		    {
			 splitcount++;
		    }
		    
	       }
	  } while ( ( pCheckItem = pCheckItem->pPolylist_next) );
	  if ( splitcount < bestcount ) {
	       pBestItem = pCurrentItem;
	       bestcount = splitcount;
	       /* if there aren't any splits we don't need to
		  look any further */
	       if ( splitcount == 0 ) 
		    break; 
	  }                   
	  pCheckItem = (P_POLYLIST_LEAF) *pSomePolygon;   
     } while ( (pCurrentItem = pCurrentItem->pPolylist_next) );
     
     /* pBestItem now points to the best choice, so update the list */
     
     if ( pBestItem == (P_POLYLIST_LEAF) *pSomePolygon ) {
	  /* the best item is also the head of the list */
	  *pSomePolygon = pBestItem->pPolylist_next;
	  if ( *pSomePolygon != NULL ) {
	       (*pSomePolygon)->pPolylist_previous = 
		    pBestItem->pPolylist_previous;
	  }       
     } else {
	  /* splice out the item we are going to return */
	  if ( pBestItem->pPolylist_previous != NULL ) {
	       (pBestItem->pPolylist_previous)->pPolylist_next = 
		    pBestItem->pPolylist_next;
	  }
	  if ( pBestItem->pPolylist_next != NULL ) {      
	       (pBestItem->pPolylist_next)->pPolylist_previous = 
		    pBestItem->pPolylist_previous;
	  }
     }
     pBestItem->pPolylist_previous = NULL;
     pBestItem->pPolylist_next = NULL;
     return( pBestItem );
     
}





static P_BSP_TREE BSP_CombineTree(P_BSP_TREE TheFrontList, 
                                  P_POLYLIST_LEAF TheRoot, 
                                  P_BSP_TREE TheBackList)
{
     P_BSP_TREE aNode;
     
     aNode = (BSP_NODE *) Cvallocate( 1, sizeof ( BSP_NODE ) );
     
     aNode->root = TheRoot;
     aNode->frontChild = TheFrontList;
     aNode->backChild = TheBackList;
     return( aNode );
}




static int
BSP_pointInFrontOf(POLYLIST_LEAF *pThisPoly, float x, float y, float z)
{
     /* Make a line from a vertex back to the point x,y,z and form the
      * dot product of that line and the surface normal for the
      * polygon.  We translate the test point to the coordinate system
      * of the polygon so that we do the comparison in the right
      * coordinates 
      */

     register Vector *poly = pThisPoly->rVer_polygon;
     
     if (( ( pThisPoly->Surface_norm_x * 
	    ( x - poly[1][V_X] )) +  
	  ( pThisPoly->Surface_norm_y * 
	   ( y - poly[1][V_Y] )) +
	  ( pThisPoly->Surface_norm_z * 
	   ( z - poly[1][V_Z] )) ) > 0.0)
	  return(TRUE);
     else
	  return( FALSE );
}



static PolyLocation BSP_pointInFrontOf2(POLYLIST_LEAF *pThisPoly, 
					float x, float y, float z)
{
     register Vector *poly = pThisPoly->rVer_polygon;
     register float dotprod;
     
     /* Make a line from a vertex back to the point x,y,z and form the
      * dot product of that line and the surface normal for the polygon.
      * We translate the test point to the coordinate system of the polygon
      * so that we do the comparison in the right coordinates
      */

     dotprod = ( ( pThisPoly->Surface_norm_x * 
		  ( x - poly[1][V_X] )) +  
                ( pThisPoly->Surface_norm_y * 
		 ( y - poly[1][V_Y] )) +
                ( pThisPoly->Surface_norm_z * 
		 ( z - poly[1][V_Z] )) );

     if ( dotprod > EPSILON2 )
	  return( IN_FRONT );
     else if ( dotprod < -EPSILON2 ) 
	  return( IN_BACK );
     else    
	  return( IN_NEITHER );
}





static PolyLocation BSP_LocationOfFace(P_POLYLIST_LEAF Pgl_root, 
				       P_POLYLIST_LEAF Pgl_leaf )
{
     int F_infront = 0;
     int F_inback = 0;
     int i;
     PolyLocation F_where;
     
     /*
      * For each vertex in the leaf polygon determine if the point
      * is in front or back of the root polygon
      */
     for( i = 0; i < Pgl_leaf->cVer_numv ; i++ )
     {   
	  F_where = BSP_pointInFrontOf2( Pgl_root, 
					Pgl_leaf->rVer_polygon [i][V_X],
					Pgl_leaf->rVer_polygon [i][V_Y],
					Pgl_leaf->rVer_polygon [i][V_Z] );
	  
	  if ( IN_FRONT == F_where )
	       F_infront++;
	  if ( IN_BACK == F_where )
	       F_inback++; 
     }

     /*
      * We can now determine the orientation of the leaf polygon in relation
      * to the root polygon
      */
     if ( 0 == F_infront )
     {
	  return(IN_BACK );
     }
     if ( 0 == F_inback )
     {
	  return(IN_FRONT );
     }
     return( SPLIT );
     
}



/*
 * Given a root polygon and a leaf polygon we split the leaf polygon
 * in 2 - based on where it is intersected by the plane created by the
 * root polygon.  We then take each of the polygons created by the
 * split and add them to the list of front polygons, or to the list of
 * back polygons.  The value returned by this function tells us if the
 * first polygon is supposed to go in the frontlist 
*/

static int BSP_SplitPolygon(P_POLYLIST_LEAF pPgl_root, 
			    P_POLYLIST_LEAF pPgl_leaf,
			    P_POLYLIST_LEAF *pPgl_first,
			    P_POLYLIST_LEAF *pPgl_second)
{    
     float   rR_plane[4];
     float   rR_point_of_int [4];
     
     int     B_add2First = TRUE;
     int     i, j;
     int     curr_index;
     int     start_index;
     int     B_return_value;
     int     cPgt_first = 0;
     int     cPgt_second = 0;
     
     P_POLYLIST_LEAF pFirst;
     P_POLYLIST_LEAF pSecond;
     
     /*
      * these variables are temp, their values will 
      * eventually be copied into our structure - when we know
      * how much memory to allocate for them.
      */
     Vector			rVer_first [MAXVERTS], rVer_second [MAXVERTS];
     P_POLYGONLINE	aPgt_first_str, aPgt_sec_str;
     
     
     
     /*
      * Since we KNOW that two polygons will come out of all this, we
      * copy the attributes of the polygon we will split to each of 
      * its two children
      */
     
     pFirst = *pPgl_first; 
     pSecond = *pPgl_second; 
     V_CopyPolylistAttr( pPgl_leaf, &pFirst );
     V_CopyPolylistAttr( pPgl_leaf, &pSecond ); 
     

     /*
      * Given the root polygon, can we construct a plane from it? The
      * answer is yes, but we might as well check for good luck
      */
     if ( FALSE == find_plane_3points( pPgl_root->rVer_polygon[0], 
                                      pPgl_root->rVer_polygon[1], 
                                      pPgl_root->rVer_polygon[2], rR_plane ) )
     {
	  verror("BSP: Can't construct plane from root polygon.\n");
     }


     /*
      * For each line segement in the leaf polygon we see if it is 
      * intersected by the plane created by the root polygon.
      *
      * A nice little trick here, if we scan the leaf polygon
      * clockwise, the two resulting polygon will also be  
      * described in a clockwise order 
      */

     pFirst->cVer_numv = 1;
     pSecond->cVer_numv = 0;
     B_add2First = TRUE;
     for ( j = 0; j < 4; j++ )
     {
	  rVer_first [0][j] = pPgl_leaf->rVer_polygon [0][j];
     }
     start_index = 0;
     curr_index = 1;
     
     for ( i = 0; i < pPgl_leaf->cVer_numv; i++ )
     {
	  
	  /*
	   * If the plane and the line segment DON'T intersect then we
	   * add the endpoint of the line to whichever of the two
	   * polygons we have been adding points to.
	   *
	   * If the plane and the line segment DO intersect then we add the
	   * intersection point to the polygon we have been adding to, switch
	   * which polygon we are adding to, and add the intersection point and
	   * the line endpint to the other polygon 
	   */
	  if (FALSE == 
	      plane_intersect_line( rR_plane, 
				   pPgl_leaf->rVer_polygon[start_index], 
				   pPgl_leaf->rVer_polygon[curr_index], 
				   rR_point_of_int ) )
	  {
	       if (B_add2First)
	       {
		    for ( j = 0; j < 4; j++ )
		    {
			 rVer_first [pFirst->cVer_numv][j] =
			      pPgl_leaf->rVer_polygon [curr_index][j];
		    }
		    pFirst->cVer_numv++;
	       }
	       else
	       {
		    for ( j = 0; j < 4; j++ )
		    {
			 rVer_second [pSecond->cVer_numv][j] =
			      pPgl_leaf->rVer_polygon [curr_index][j];
		    }
		    pSecond->cVer_numv++;
	       }           
	  }
	  else
	  {
	       if (B_add2First)
	       {
		    for ( j = 0; j < 3; j++ )
		    {
			 rVer_first[pFirst->cVer_numv][j] = rR_point_of_int[j];
		    }
		    rVer_first [pFirst->cVer_numv][3] = 1.0;
		    pFirst->cVer_numv++;
		    
		    for ( j = 0; j < 3; j++ )
		    {
			 rVer_second[pSecond->cVer_numv][j] = rR_point_of_int[j];
		    }
		    rVer_second [pSecond->cVer_numv][3] = 1.0;
		    pSecond->cVer_numv++;
		    
		    for ( j = 0; j < 4; j++ )
		    {
			 rVer_second [pSecond->cVer_numv][j] =
			      pPgl_leaf->rVer_polygon [curr_index][j];
		    }
		    pSecond->cVer_numv++;
		    B_add2First = FALSE;
	       }
	       else
	       {
		    for ( j = 0; j < 3; j++ )
		    {
			 rVer_second [pSecond->cVer_numv][j] = rR_point_of_int [j];
		    }
		    rVer_second [pSecond->cVer_numv][3] = 1.0;
		    pSecond->cVer_numv++;
		    
		    for ( j = 0; j < 3; j++ )   
		    {  
			 rVer_first [pFirst->cVer_numv][j] = rR_point_of_int [j];   
		    }  
		    rVer_first [pFirst->cVer_numv][3] = 1.0;
		    pFirst->cVer_numv++; 
		    
		    for ( j = 0; j < 4; j++ )
		    {
			 rVer_first [pFirst->cVer_numv][j] =
			      pPgl_leaf->rVer_polygon [curr_index][j];
		    }
		    pFirst->cVer_numv++;
		    B_add2First = TRUE;
	       }
	  }
	  
	  curr_index++;
	  start_index++;
	  
	  /* 
	   * wrap the index around to 0 when we are on the last face
	   * of the polygon 
	   */
	  if ( curr_index == pPgl_leaf->cVer_numv ) {
	       curr_index = 0;
	  }
     }


     /*
      * Now that we have created two polygons, out of the one, we must
      * remember to add another point in each of the 2nd polygon's array
      * of verteci.
     */ 
     for ( j = 0; j < 4; j++ )
     {
	  rVer_second [pSecond->cVer_numv][j] = rVer_second [0][j];
     }
     pSecond->cVer_numv++;
     
     /*
      * It is sometimes possible that the 1st polygon ends up looking
      * like a polygon with the following vertex list: A B C D A A
      * If this is the case we need to make it look like this: A B C D A
      *
      * Because we are using arrays to store the values we can just decrease
      * our count of vertex by 1
      */
     if ((rVer_first [pFirst->cVer_numv-2][V_X] == 
	  rVer_first [pFirst->cVer_numv-1][V_X]) &&

	 ( rVer_first [pFirst->cVer_numv-2][V_Y] ==
	  rVer_first [pFirst->cVer_numv-1][V_Y]) &&
	 
	 ( rVer_first [pFirst->cVer_numv-2][V_Z] ==
	  rVer_first [pFirst->cVer_numv-1][V_Z]) &&
	 
	 ( rVer_first [pFirst->cVer_numv-2][V_W] ==
	  rVer_first [pFirst->cVer_numv-1][V_W]) )
     {
	  pFirst->cVer_numv = pFirst->cVer_numv - 1;
     }


     /*
      * Allocate only the space we need and copy the data from the
      * temp variables into real variables
      */
     ((POLYLIST_LEAF *) pFirst)->rVer_polygon = (Vector *) 
	  Cvallocate( pFirst->cVer_numv, sizeof( Vector ) ); 
     for ( i=0; i < pFirst->cVer_numv; i++ )
     {
	  pFirst->rVer_polygon [i][V_X] = rVer_first [i][V_X];
	  pFirst->rVer_polygon [i][V_Y] = rVer_first [i][V_Y];  
	  pFirst->rVer_polygon [i][V_Z] = rVer_first [i][V_Z];  
	  pFirst->rVer_polygon [i][V_W] = rVer_first [i][V_W];  
     }
     
     ((POLYLIST_LEAF *) pSecond)->rVer_polygon = (Vector *) 
	  Cvallocate( pSecond->cVer_numv, sizeof( Vector ) ); 
     for ( i=0; i < pSecond->cVer_numv; i++ )        
     {
	  pSecond->rVer_polygon [i][V_X] = rVer_second [i][V_X];  
	  pSecond->rVer_polygon [i][V_Y] = rVer_second [i][V_Y];  
	  pSecond->rVer_polygon [i][V_Z] = rVer_second [i][V_Z];  
	  pSecond->rVer_polygon [i][V_W] = rVer_second [i][V_W];  
     }
     
     
     
     /*
      * figure out if the first polygon is in
      * front or not and return that value
      */
     
     /*
      * the result of this gets used a lot, we can reduce the number
      * of compares to B_return_value by assiging a pointer to
      * the polygon in the front and another pointer to the polygon
      * in the back. We will then preform all operations on those
      *
      * But first get things working this monster function partioned to
      * sizes that can be read
      */
     B_return_value = BSP_pointInFrontOf( pPgl_root, 
					 pFirst->rVer_polygon[0][V_X],
					 pFirst->rVer_polygon[0][V_Y],
					 pFirst->rVer_polygon[0][V_Z] );
     

     
     /*
      * Now that the polygon has been split in 2, lets see if it
      * had any text associated with it. If it did, then we need
      * to split the text line segments across the 2 polygons.
      * If it didn't have text make sure we don't have unset variables...
      */
     
     if ( FALSE == pPgl_leaf->B_text ) {
	  pFirst->cPgt_lines = 0;
	  pFirst->B_text = FALSE; 
	  pFirst->pPgt_string = (P_POLYGONLINE) (NULL);           
	  pSecond->cPgt_lines = 0;
	  pSecond->B_text = FALSE; 
	  pSecond->pPgt_string = (P_POLYGONLINE) (NULL);           
     } else {
	  /* party down and split the text */
	  
	  int             i;
	  int             F_where_a = 0;
	  int             F_where_b = 0;
	  
	  /*
	   * Before we can party down we have to allocate the space to
	   * hold the new lines of text. A good guess for the size is
	   * the current size i.e. pPgl_leaf->cPgt_lines we later get
	   * clever and do a realloc after we know the size
           */
	  aPgt_first_str = (P_POLYGONLINE)
	       Cvallocate( pPgl_leaf->cPgt_lines, sizeof(POLYGONLINE) );
	  aPgt_sec_str = (P_POLYGONLINE) 
	       Cvallocate( pPgl_leaf->cPgt_lines, sizeof(POLYGONLINE) );
	  
	  
	  /*
	   * For each line that goes into the text we decide where to place it,
	   * the front polygon or the back polygon
	   *
	   * This is the algorithm:
	   *   - check each end point and see if they are in front or back of the
	   *      splitting plane
	   *   - they are are equal to each other they are on the same side of
	   *      the plane
	   *          - if one of the two end points is in front of the splitting
	   *              plane then the entire line is in front of the splitting
	   *              plane, otherwise the entire line is in back of the plane
	   *   - since they aren't on the same side of the plane we need to split
	   *      the line.
	   *          - once we split the line we check if the first point in
	   *              the original line was in front of or in back of the plane
	   *          - we add the split line segments to the correct polygon list 
	   *
	   */
	  for( i=0; i < pPgl_leaf->cPgt_lines; i++ ) {
	       F_where_a = BSP_pointInFrontOf( pPgl_root, 
					      pPgl_leaf->pPgt_string [i].R_move [V_X],
					      -pPgl_leaf->pPgt_string [i].R_move [V_Y],
					      pPgl_leaf->pPgt_string [i].R_move [V_Z] );

	       F_where_b = BSP_pointInFrontOf( pPgl_root,
					      pPgl_leaf->pPgt_string [i].R_draw [V_X],
					      -pPgl_leaf->pPgt_string [i].R_draw [V_Y],
					      pPgl_leaf->pPgt_string [i].R_draw [V_Z] );


	       if ( F_where_a == F_where_b )
	       {

		    if ( TRUE == F_where_a )
		    {
			 if ( TRUE == B_return_value )
			 {
			      aPgt_first_str[cPgt_first] = pPgl_leaf->pPgt_string[i];
			      cPgt_first++;
			 }
			 else
			 {
			      aPgt_sec_str[cPgt_second] = pPgl_leaf->pPgt_string[i];
			      cPgt_second++;
			 }
		    }
		    else
		    {
			 if ( TRUE == B_return_value )
			 {
			      aPgt_sec_str [cPgt_second] = pPgl_leaf->pPgt_string [i];
			      cPgt_second++;  
			 }
			 else
			 {
			      aPgt_first_str[cPgt_first] = pPgl_leaf->pPgt_string [i];
			      cPgt_first++;
			 }
		    }

	       }
	       else
	       {
		    float       rR_text_start [3], rR_text_end [3];
		    float       rR_int_result [3];
		    POLYGONLINE Pgt_line_1, Pgt_line_2;

		    rR_text_start [V_X] = pPgl_leaf->pPgt_string [i].R_move [V_X];
		    rR_text_start [V_Y] = -pPgl_leaf->pPgt_string [i].R_move [V_Y];
		    rR_text_start [V_Z] = pPgl_leaf->pPgt_string [i].R_move [V_Z];    
		    rR_text_end [V_X] = pPgl_leaf->pPgt_string [i].R_draw [V_X];
		    rR_text_end [V_Y] = -pPgl_leaf->pPgt_string [i].R_draw [V_Y]; 
		    rR_text_end [V_Z] = pPgl_leaf->pPgt_string [i].R_draw [V_Z]; 


		    plane_intersect_line( rR_plane, rR_text_start,
					 rR_text_end, rR_int_result );

		    Pgt_line_1.R_move [V_X] = pPgl_leaf->pPgt_string[i].R_move[V_X];
		    Pgt_line_1.R_move [V_Y] = pPgl_leaf->pPgt_string[i].R_move[V_Y];
		    Pgt_line_1.R_move [V_Z] = pPgl_leaf->pPgt_string[i].R_move[V_Z];

		    Pgt_line_1.R_draw [V_X] = rR_int_result [V_X];
		    Pgt_line_1.R_draw [V_Y] = -rR_int_result [V_Y];
		    Pgt_line_1.R_draw [V_Z] = rR_int_result [V_Z];

		    Pgt_line_2.R_move [V_X] = rR_int_result [V_X];
		    Pgt_line_2.R_move [V_Y] = -rR_int_result [V_Y];
		    Pgt_line_2.R_move [V_Z] = rR_int_result [V_Z];

		    Pgt_line_2.R_draw [V_X] = pPgl_leaf->pPgt_string[i].R_draw[V_X];
		    Pgt_line_2.R_draw [V_Y] = pPgl_leaf->pPgt_string[i].R_draw[V_Y];
		    Pgt_line_2.R_draw [V_Z] = pPgl_leaf->pPgt_string[i].R_draw[V_Z];

		    if ( TRUE == F_where_a )
		    {
			 if ( TRUE == B_return_value )
			 {
			      aPgt_first_str [cPgt_first] = Pgt_line_1;
			      cPgt_first++;

			      aPgt_sec_str [cPgt_second] = Pgt_line_2;
			      cPgt_second++;
			 }
			 else
			 {
			      aPgt_sec_str [cPgt_second] = Pgt_line_1;
			      cPgt_second++;

			      aPgt_first_str [cPgt_first] = Pgt_line_2;
			      cPgt_first++;
			 }
		    }
		    else
		    {
			 if ( TRUE == B_return_value )
			 {
			      aPgt_first_str [cPgt_first] = Pgt_line_2; 
			      cPgt_first++;

			      aPgt_sec_str [cPgt_second] = Pgt_line_1;
			      cPgt_second++;
			 }
			 else
			 {
			      aPgt_sec_str [cPgt_second] = Pgt_line_1;
			      cPgt_second++;

			      aPgt_first_str [cPgt_first] = Pgt_line_2;
			      cPgt_first++;
			 }
		    }
	       }
	  }
	  
	  /*
	   * Update the number of text lines as well as the flags telling
	   * us if a polygon has text associated with it
	   */
	  pFirst->cPgt_lines = cPgt_first;
	  pSecond->cPgt_lines = cPgt_second;
	  

	  if (cPgt_first) {
	       pFirst->pPgt_string = (P_POLYGONLINE)
		    Cvallocate( cPgt_first, sizeof(POLYGONLINE) );
	       for( i = 0; i < cPgt_first; i++ )
	       {
		    pFirst->pPgt_string [i] = aPgt_first_str [i];
	       }
	  }
	  WorldBFree( (char *) aPgt_first_str );
	  

	  if (cPgt_second) {
	       pSecond->pPgt_string = (P_POLYGONLINE)
		    Cvallocate( cPgt_second, sizeof(POLYGONLINE) );
	       for( i = 0; i < cPgt_second; i++ )
	       {
		    pSecond->pPgt_string [i] = aPgt_sec_str [i];    
	       }
	  }
	  WorldBFree( (char *) aPgt_sec_str );
	  
	  /* 
	   * for debugging can set texts two different colors
	   *    pFirst->F_text_color = BLUE;
	   *    pSecond->F_text_color = RED;
	   */
	  
	  if (0 != pFirst->cPgt_lines)
	       pFirst->B_text = TRUE;
	  else
	       pFirst->B_text = FALSE;
	  
	  if (0 != pSecond->cPgt_lines)
	       pSecond->B_text = TRUE;
	  else
	       pSecond->B_text = FALSE;
	  
     }
     
     if ( pPgl_leaf->B_text == TRUE ) {
	  WorldBFree( (char *) pPgl_leaf->pPgt_string );
     }
     WorldBFree( (char *) pPgl_leaf->rVer_polygon );
     WorldBFree( (char *) pPgl_leaf->arVer_original );
     WorldBFree( (char *) pPgl_leaf );
     /*
      * Tell us which of the two polygon is in front, and which is in back
      */
     return( B_return_value ); 
}




static void BSPListAdd( P_POLYLIST_LEAF *pThelist,
		       P_POLYLIST_LEAF pThe_New_Entry )
{
     if ( *pThelist == NULL )
     {
	  pThe_New_Entry->pPolylist_next = NULL;
	  pThe_New_Entry->pPolylist_previous = NULL;
	  *pThelist = pThe_New_Entry;
     }
     else
     {
	  /* insert the new element at the head of the list */
	  pThe_New_Entry->pPolylist_previous = NULL;
	  pThe_New_Entry->pPolylist_next = *pThelist;
	  (*pThelist)->pPolylist_previous = pThe_New_Entry;
	  (*pThelist) = pThe_New_Entry;
     }
     
}





static POLYLIST_LEAF *BSPSpliceOut( P_POLYLIST_LEAF *APolygonList )
{
     P_POLYLIST_LEAF ptemp;
    
     ptemp = *APolygonList;	/* we want to return the head of the list */
    
     *APolygonList = ptemp->pPolylist_next;
     if ( *APolygonList != NULL ) {
	  (*APolygonList)->pPolylist_previous = ptemp->pPolylist_previous;
     }
    
     /* null the links since we spliced it out... */
     ptemp->pPolylist_previous = NULL;
     ptemp->pPolylist_next = NULL;
    
     return( ptemp );

}




/*
 * 
 * from Foley & van Dam page 678
 *
 */
static P_BSP_TREE BSP_makeTree(P_POLYLIST_LEAF *pThePolygonList)
{
     P_POLYLIST_LEAF pRoot;
     P_POLYLIST_LEAF pA_leaf;
     P_POLYLIST_LEAF pBacklist;
     P_POLYLIST_LEAF pFrontlist;
    
     P_POLYLIST_LEAF pPgl_One;
     P_POLYLIST_LEAF pPgl_Two;

     PolyLocation where;
    
     if ( *pThePolygonList == NULL )
	  return( NULL );
     else {
	  pRoot = BSP_selectAndRemovePoly( (PP_POLYLIST_LEAF) pThePolygonList );
	  pBacklist = pFrontlist = NULL;
        
	  while( *pThePolygonList != NULL ) {
	       /* look at the location of the face at the head of the list */
	       pA_leaf = BSPSpliceOut( (PP_POLYLIST_LEAF) pThePolygonList ); 
	       where = BSP_LocationOfFace( pRoot, pA_leaf );  
	       switch ( where )  {
	       case IN_FRONT: {
                    BSPListAdd( &pFrontlist, pA_leaf );
                    break;
	       }
                
	       case IN_BACK: {
                    BSPListAdd( &pBacklist, pA_leaf );          
                    break;
	       }
                
	       case SPLIT: {                   
                    pPgl_One = 
			 (P_POLYLIST_LEAF) Cvallocate(1,sizeof( POLYLIST_LEAF ) );
                    pPgl_Two =  
			 (P_POLYLIST_LEAF) Cvallocate(1,sizeof( POLYLIST_LEAF ) );

                    if (BSP_SplitPolygon( pRoot, pA_leaf, &pPgl_One, &pPgl_Two )) 
                    {
			 BSPListAdd( &pFrontlist, pPgl_One );
			 BSPListAdd( &pBacklist, pPgl_Two ); 
                    } 
                    else 
                    {
			 BSPListAdd( &pFrontlist, pPgl_Two );
			 BSPListAdd( &pBacklist, pPgl_One );
                    }
                    break;
	       }
                
	       }
	  }

	  return(BSP_CombineTree( BSP_makeTree( &pFrontlist ), pRoot,  
				 BSP_makeTree( &pBacklist ) ));

     }
}


/*----------------------------------------------------*/

/*
 * Construct the BSP tree
 */
void endscene(int sceneid)
{
     Scenes[sceneid]->pBSP_tree_root = 
	  BSP_makeTree(&Scenes[sceneid]->pPolyList_head );
}




/*
 * Display the contents of the BSP tree
 *
 */
void callscene(int sceneid)
{
     float   rTranPt[4];
     float   rR_tmp_p [4];
     float   invMat [4][4];
     
     
     /*
      * Invert the topmost matrix and apply it to the view point
      */
     rR_tmp_p[V_X] = 0.0;
     rR_tmp_p[V_Y] = 0.0;
     rR_tmp_p[V_Z] = 0.0;
     rR_tmp_p[V_W] = 1.0;
     minv4_affine( invMat, vdevice.transmat->m);
     QMULTVECTOR_AFFINE( rTranPt, rR_tmp_p, invMat );
     
     rEyeTransX = rTranPt[V_X];
     rEyeTransY = rTranPt[V_Y];
     rEyeTransZ = rTranPt[V_Z];
     
     
     if ( FALSE == Scenes[sceneid]->gF_shaded_already )
     {
	  BSP_Shade( &Scenes[sceneid]->pBSP_tree_root );
	  Scenes[sceneid]->gF_shaded_already = TRUE;
     }
     
     BSP_DisplayBSP( Scenes[sceneid]->pBSP_tree_root );
}


static void BSP_DisplayRoot(register const P_POLYLIST_LEAF pThe_root )
{
     switch ( pThe_root->F_type ) {

     case POLYF:
     case RECTF: 
	  vdevice.attr->a.color = pThe_root->F_color;
	  REALSETCOLOR(vdevice.attr->a.color);
	  
	  vdevice.fill = pThe_root->N_fill;
	  
	  dopoly( pThe_root );
	  
	  vdevice.cpW[V_X] = pThe_root->R_lastx; 
	  vdevice.cpW[V_Y] = pThe_root->R_lasty; 
	  vdevice.cpW[V_Z] = pThe_root->R_lastz; 			
	  break;
	       
     default:
	  break;
     }
     
}           


static void BSP_DisplayBSP( register const P_BSP_TREE pThe_BSP_tree )
{
     if ( pThe_BSP_tree != NULL ) 
     {
	  if ( BSP_pointInFrontOf(pThe_BSP_tree->root, rEyeTransX,
				  rEyeTransY, rEyeTransZ) )
	  {
	       /* viewer is in front of root; 
		  display back, root, child */ 
	       BSP_DisplayBSP( pThe_BSP_tree->backChild );
	       /*  BSP_DisplayRoot( pThe_BSP_tree->root ); */
	       dopoly( pThe_BSP_tree->root );
	       BSP_DisplayBSP( pThe_BSP_tree->frontChild );
	       
	  } else {  
	       /* viewer is in back of root;
		  display front, root, child */
	       BSP_DisplayBSP( pThe_BSP_tree->frontChild );
/*	       BSP_DisplayRoot( pThe_BSP_tree->root );  */
	       dopoly( pThe_BSP_tree->root );
	       BSP_DisplayBSP( pThe_BSP_tree->backChild );     
	  }
     }
}




static void BSP_ClickedOnObject(P_BSP_TREE pThe_BSP_tree)
{
     /* 
      * note that we walk the tree in reverse order of a display walk
      * since we want to improve the chance of finding the frontmost 
      * visible polygon that contains the point early in the search
      */	
     if ( ( pThe_BSP_tree != NULL ) )
     {
	  if ((BSP_pointInFrontOf(pThe_BSP_tree->root, rEyeTransX, rEyeTransY, rEyeTransZ))
	      && ( vdevice.ClickedGopherspaceObject == 0 ) ) {
	       /* viewer is in front of root; check front, root, back */
	       BSP_ClickedOnObject( pThe_BSP_tree->frontChild );
	       
	       if ( vdevice.ClickedGopherspaceObject != 0 ) return;   
	       
	       if ( ( pThe_BSP_tree->root->GSpace_Object_ID > 0 ) &&
		   ( isPointInPoly( ClickedScreenX, ClickedScreenY, 
						pThe_BSP_tree->root )) ) 
	       {
		    vdevice.ClickedGopherspaceObject = 
			 pThe_BSP_tree->root->GSpace_Object_ID;
		    return;
	       }
 			
	       BSP_ClickedOnObject( pThe_BSP_tree->backChild ); 
	       
	  } else {  
	       /* viewer is in back of root; check back, root, front */
	       BSP_ClickedOnObject( pThe_BSP_tree->backChild );
	       
	       if ( vdevice.ClickedGopherspaceObject != 0 ) return;   
	       
	       if ( ( pThe_BSP_tree->root->GSpace_Object_ID > 0 ) &&
		   ( isPointInPoly(ClickedScreenX, ClickedScreenY, 
				   pThe_BSP_tree->root )) ) 
	       {
		    vdevice.ClickedGopherspaceObject = 
			 pThe_BSP_tree->root->GSpace_Object_ID;
		    return;
	       }
	       
	       BSP_ClickedOnObject( pThe_BSP_tree->frontChild );
	       
	  }
     } 
}





/*
 * did they click on an object or not?
 * set the value of vdevice.ClickedGopherspaceObject
 */
void sceneclick(int sceneid, register const int ScreenX,
			  register const int ScreenY )
{	
     float   rTranPt[4];
     float   rR_tmp_p [4];
     float   invMat [4][4];
     
     /* Gopherspace object IDs are all greter than 0 */
     vdevice.ClickedGopherspaceObject = 0; 
     
     if (Scenes[sceneid]->pBSP_tree_root != NULL ) 
     {
	  /*
	   * Invert the topmost matrix and apply it to the point clicked on
	   */
	  /* change this to screen coords next */
	  rR_tmp_p[V_X] = 0.0; 
	  rR_tmp_p[V_Y] = 0.0;
	  rR_tmp_p[V_Z] = 0.0;
	  rR_tmp_p[V_W] = 1.0;
	  minv4_affine( invMat, vdevice.transmat->m);
	  QMULTVECTOR_AFFINE( rTranPt, rR_tmp_p, invMat );	    
	  rEyeTransX = rTranPt[V_X];
	  rEyeTransY = rTranPt[V_Y];
	  rEyeTransZ = rTranPt[V_Z];
	  
	  ClickedScreenX = ScreenX;
	  ClickedScreenY = ScreenY;
	  
	  BSP_ClickedOnObject(Scenes[sceneid]->pBSP_tree_root );
     }
}


/*
 * return an ID for a new gopherspace object
 * this needs to be called BEFORE we start
 * calling the polygons for the object
 * so BSF can keep track of which polygons belong to 
 * which gopherspace item
 */
int V_StartNewGopherObject( void )
{
	return( vdevice.CurrentGSpaceID++ );
}




/*
 * Add linesegments to a polygon's list of linesegments that go
 * into drawing text on the screen
 */

static void V_UpdateTextLines(int C_lines_added, 
			      PP_POLYGONLINE ppPgt_new_string, 
			      POLYGONLINE Pgt_old_string )
{

     if ( 0 == C_lines_added )
     {
	  *ppPgt_new_string = 
	       (P_POLYGONLINE) Cvallocate( 1, sizeof(POLYGONLINE));
     }
     else
     {
	  *ppPgt_new_string = (P_POLYGONLINE) realloc( *ppPgt_new_string, 
						      sizeof(POLYGONLINE) * 1);
     }
     
     if (NULL == *ppPgt_new_string)
     {
	  printf( "Out of memory\n\n" );
	  exit(0);
     }
     
     (*ppPgt_new_string)[C_lines_added].R_move [V_X] = 
	  Pgt_old_string.R_move [V_X]; 
     (*ppPgt_new_string)[C_lines_added].R_move [V_Y] = 
	  Pgt_old_string.R_move [V_Y];
     (*ppPgt_new_string)[C_lines_added].R_move [V_Z] = 
	  Pgt_old_string.R_move [V_Z];
     
     (*ppPgt_new_string)[C_lines_added].R_draw [V_X] = 
	  Pgt_old_string.R_draw [V_X];
     (*ppPgt_new_string)[C_lines_added].R_draw [V_Y] = 
	  Pgt_old_string.R_draw [V_Y];
     (*ppPgt_new_string)[C_lines_added].R_draw [V_Z] = 
	  Pgt_old_string.R_draw [V_Z];

}




static void 
BSP_Shade( PP_BSP_TREE ppBSP_node )
{
     if ( NULL == *ppBSP_node )
     {
	  return;
     }
     else
     {   
	  BSP_Shade( &((*ppBSP_node)->frontChild) );
	  BSP_Shade( &((*ppBSP_node)->backChild) );
	  
	  (*ppBSP_node)->root->F_shaded_color = 
	       N_FlatShading( (*ppBSP_node)->root, 
			     (*ppBSP_node)->root->arVer_original, 
			     0, NULL, NULL, d_FROM_BSP );
     }
}
	




/*
 * The following two routines clear the BSP tree. The second is
 * to be called by applications, the first is internal to this library.
 */

static void BSP_ClearBSP( P_BSP_TREE pBSP_root_node )
{

    if ( pBSP_root_node == NULL )
        return;
    else
    {
        BSP_ClearBSP( pBSP_root_node->frontChild );
        BSP_ClearBSP( pBSP_root_node->backChild );
	
	if (pBSP_root_node->root->rVer_polygon != NULL)
	     WorldBFree( (char *) pBSP_root_node->root->rVer_polygon );

	if (pBSP_root_node->root->arVer_original != NULL)
	     WorldBFree( (char *) pBSP_root_node->root->arVer_original );

	if (pBSP_root_node->root->pPgt_string != NULL) 
	     WorldBFree( (char *) pBSP_root_node->root->pPgt_string );

        if (pBSP_root_node->root != NULL)
	     WorldBFree( (char *) pBSP_root_node->root );

	if (pBSP_root_node != NULL)
	     WorldBFree( (char *) pBSP_root_node );
    }
}




void delscene(int sceneid)
{
     /*
      * Unset some global variables
      */
     Scenes[sceneid]->gF_shaded_already = FALSE;
     
     BSP_ClearBSP( Scenes[sceneid]->pBSP_tree_root );
     /*if (Scenes[sceneid]->pBSP_tree_root != NULL)
	  WorldBFree( (char *) Scenes[sceneid]->pBSP_tree_root );*/
     if (Scenes[sceneid]->pPolyList_head != NULL)
	  WorldBFree( (char *) Scenes[sceneid]->pPolyList_head );
     if (Scenes[sceneid] != NULL)
	  WorldBFree( (char *) Scenes[sceneid]);
     
     Scenes[sceneid] = NULL;
     
     scenenum = -1;
}


