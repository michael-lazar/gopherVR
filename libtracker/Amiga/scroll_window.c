/* amiga/scroll_window.c 
	vi:ts=3 sw=3:
 */

/* $Id: scroll_window.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $
 * $Log: scroll_window.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:23  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.3  1995/02/14  16:51:22  espie
 * *** empty log message ***
 *
 * Revision 1.2  1995/02/13  22:05:42  espie
 * The display takes more space taken into account.
 *
 * Revision 1.1  1995/01/13  13:31:35  espie
 * Initial revision
 *
 * Revision 1.4  1994/01/09  04:49:18  Espie
 * Stupid: forgot that a window font can very well be proportional.
 * It's easier to rely on GfxBase->DefaultFont after all.
 * Uncentralized event handling using event management functions.
 * Handle own's clipping without Layers.
 * Use dynamic colours/masks according to current screen.
 * Added forbid_reopen to avoid dangling window.
 *
 */

/* The line scroller implemented as a window */

#include <assert.h>

#include <intuition/screens.h>
#include <graphics/text.h>
#include <graphics/gfxmacros.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <graphics/gfxbase.h>

#include "defs.h"
#include "extern.h"
#include "amiga/amiga.h"
#include "prefs.h"

ID("$Id: scroll_window.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $")

LOCAL void init_scroller(void);
LOCAL void (*INIT)(void) = init_scroller;
LOCAL int forbid_reopen = FALSE;
LOCAL void handle_scroller(GENERIC nothing);

XT struct IntuitionBase *IntuitionBase;
XT struct GfxBase *GfxBase;

LOCAL struct Screen *pubscreen;

/* our window */
LOCAL struct Window *scroll_win = 0;
LOCAL struct TextFont *font;
   

/* to build up write masks */
LOCAL ULONG text, highlight, background;


LOCAL void close_scroller()	
   {
   if (scroll_win)
      {
      remove_signal_handler(scroll_win->UserPort->mp_SigBit);
      CloseWindow(scroll_win);
      }
   scroll_win = 0;               /* and get ready for reopen */
   }

LOCAL void really_close_scroller()
   {
   forbid_reopen = TRUE;
   close_scroller();
   }

/* graphic niceties: draw vertical lines at the right separation points
 * all the way between y1 and the bottom
 */
LOCAL void separation_lines(int y1)
   {
   int x, xs, i, c;
   
   xs = font->tf_XSize;
   
   SetWrMsk(scroll_win->RPort, background | highlight);  
   SetAPen(scroll_win->RPort, highlight);   
   x = scroll_win->BorderLeft;
   for (i = 1; i < 5; i++)
      {
      if (i == 3)
         x+= xs;
      else
         x += 14 * xs;     /* 14 chars per column */
      c = x - xs/2;
      if (c >= scroll_win->Width - scroll_win->BorderRight -1)
         break;
      Move(scroll_win->RPort, x - xs/2, y1);
      Draw(scroll_win->RPort, x - xs/2, scroll_win->Height - scroll_win->BorderBottom - 1);
      }
   }
   
LOCAL void init_scroller()
   {
   struct DrawInfo *dr;
   
   at_end(really_close_scroller);
      /* default text, background, etc */
   text = 1;
   background = 0;
   highlight = 7;
   pubscreen = obtain_pubscreen();
   dr = GetScreenDrawInfo(pubscreen);
   if (dr)
      {
      if (dr->dri_Version >= 1)  /* for V 2.04 */
         {
         text = dr->dri_Pens[TEXTPEN];
         background = dr->dri_Pens[BACKGROUNDPEN];
         highlight = dr->dri_Pens[SHINEPEN];
         }
      FreeScreenDrawInfo(pubscreen, dr);
      }
   }
   

LOCAL void open_scroller()
   {
   INIT_ONCE;
   font = GfxBase->DefaultFont;
   scroll_win = OpenWindowTags(NULL,
      WA_Title, "Scroll",
      WA_DepthGadget, TRUE,
      WA_SmartRefresh, TRUE,
      WA_DragBar, TRUE,
      WA_SizeGadget, TRUE,
      WA_CloseGadget, TRUE,
      WA_InnerWidth, font->tf_XSize * 60, /* 60 = 14 * 4 + some margin */
      WA_InnerHeight, font->tf_YSize * 15,
      WA_MinWidth, 15 * font->tf_XSize,
      WA_MaxWidth, ~0,           /* need some room for the size gadget */
      WA_MinHeight, 2 * pubscreen->WBorTop,   
      WA_MaxHeight, ~0,
      WA_AutoAdjust, TRUE,
      WA_NoCareRefresh, TRUE,
      WA_PubScreen, pubscreen,
      WA_IDCMP, IDCMP_NEWSIZE | IDCMP_CLOSEWINDOW,
      TAG_END);

   if (!scroll_win)
      end_all(0);
      
   SetFont(scroll_win->RPort, font);
   install_signal_handler(scroll_win->UserPort->mp_SigBit, handle_scroller, 0);
   separation_lines(scroll_win->BorderTop);
   }



LOCAL void handle_scroller(GENERIC nothing)   
   {
   struct IntuiMessage *msg;
   while (scroll_win && (msg = GetMsg(scroll_win->UserPort)))
      switch(msg->Class)
         {
      case IDCMP_NEWSIZE:
         ReplyMsg(msg);
         separation_lines(scroll_win->BorderTop);  /* redraw lines where applicable */
         break;
      case IDCMP_SIZEVERIFY:
         ReplyMsg(msg);
         break;
      case IDCMP_CLOSEWINDOW:
         ReplyMsg(msg);
         close_scroller();
         set_pref_scalar(PREF_SHOW, FALSE);
         break;
      default:
         ReplyMsg(msg);
         }
   }

void add_scroller(char *s)
   {
   int max_length;

   if (forbid_reopen)
      return;
   if (!scroll_win)
      open_scroller();
      
      /* critical section for size changes */
   ModifyIDCMP(scroll_win, scroll_win->IDCMPFlags | IDCMP_SIZEVERIFY);
   handle_scroller(0);   /* handle size problems first */

   if (!scroll_win)     /* window may have closed on us... */
      return;

      /* add the characters */
   SetDrMd(scroll_win->RPort, JAM1);
   SetWrMsk(scroll_win->RPort, background | text);
   SetAPen(scroll_win->RPort, text);
   Move(scroll_win->RPort, scroll_win->BorderLeft, 
      scroll_win->Height - scroll_win->BorderBottom 
    - font->tf_YSize + font->tf_Baseline);
   max_length = scroll_win->Width - scroll_win->BorderLeft - scroll_win->BorderRight;
   max_length /= font->tf_XSize;
   if (max_length > 14 * 4)
      max_length = 14 * 4;    /* line length = 14 * 4 */
   Text(scroll_win->RPort, s, max_length);
   
      /* add the separation lines */
   separation_lines(scroll_win->Height - scroll_win->BorderBottom 
    - font->tf_YSize);
      
      /* and scroll *JUST* the characters */
   SetWrMsk(scroll_win->RPort, 1);
   ScrollRaster(scroll_win->RPort, 0, font->tf_YSize, 
                  scroll_win->BorderLeft, scroll_win->BorderTop, 
                  scroll_win->Width - scroll_win->BorderRight - 1,
                  scroll_win->Height - scroll_win->BorderBottom - 1);
   
      /* end of critical section: allow resize again */
   ModifyIDCMP(scroll_win, scroll_win->IDCMPFlags & ~IDCMP_SIZEVERIFY);
   handle_scroller(0); 
   }
