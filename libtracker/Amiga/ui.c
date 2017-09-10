/* amiga/ui.c 
	vi:ts=3 sw=3:
 */

/* $Id: ui.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $
 * $Log: ui.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:23  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.22  1995/02/14  16:51:22  espie
 * *** empty log message ***
 *
 * Revision 1.21  1995/02/13  22:05:42  espie
 * Changed version number.
 *
 * Revision 1.20  1995/01/13  13:31:35  espie
 * *** empty log message ***
 *
 * Revision 1.20  1994/06/22  21:54:12  Espie
 * Split across other files.
 * File requester !
 * Added pause gadget.
 * Uncentralized event handling using event management functions.
 * Changed name to ui_win, added Show gadget.
 * better coding for gadgets.
 * Nasty bug with info: did not close the file properly.
 * Fully working asynchronous interface.
 * User feedback.
 * Added missing autoinit
 * Removed some typecasts.
 * info facility.
 * scroll post-synchronized output.
 * notice.
 * Cursor handling.
 * Used of discard_buffer for premature ending.
 * Mostly working.
 * Just dies with a guru.
 * Plus timing problems at start.
 */

#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <intuition/intuitionbase.h>

#include "defs.h"
#include "version.h"
#include "extern.h"
#include "amiga/amiga.h"
#include "tags.h"
#include "prefs.h"

ID("$Id: ui.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $")

XT unsigned int inhibit_output;

LOCAL void init_ui(void);
LOCAL void do_set_current(VALUE current);
LOCAL void handle_ui_window(GENERIC nothing);

LOCAL void (*INIT)(void) = init_ui;


/* These two variables memorize where we actually are in
 * the current song
 */
LOCAL int current_pattern;

#define SMALL_DELAY 3         /* in seconds */

/* And these when stuff last changed */
LOCAL ULONG pattern_change_seconds, pattern_change_micros,
song_change_seconds, song_change_micros;


/* The basic user interface (a simple window) */

struct IntuitionBase *IntuitionBase = 0;
struct GfxBase *GfxBase = 0;
LOCAL struct Library *GadtoolsBase = 0;
LOCAL struct Window *ui_win;

void set_busy_pointer(BOOL maybe)
   {
   if (IntuitionBase->LibNode.lib_Version < 39)
      return;
   if (ui_win)
      SetWindowPointer(ui_win, WA_BusyPointer, maybe, WA_PointerDelay, TRUE, TAG_DONE, 0);
   }

/* for asynchronous easy requests */
LOCAL struct Window *notice_win = 0;

   /* for computing length */
LOCAL struct IntuiText it;

#define SHIFT 4
#define SPACEX 10
#define SPACEY 4

#define WINDOW_TITLE "Experiment IV "        /* the space in case the font is italic */

LOCAL struct NewGadget template =
   {
   0, 0,
   0, 0,
   NULL,
   0,
   0,       /* gadget ID */
   0,
   NULL,
   NULL
   };

LOCAL struct NewMenu menu_template[] =
   {
   {NM_TITLE,  "Project",        0, 0, 0, 0},
      {NM_ITEM,   "Load song...",   0, 0, 0, (void *)4},
      {NM_ITEM,   "About...",       0, 0, 0, (void *)1},
      {NM_ITEM,   "Quit",           0, 0, 0, (void *)2},
   {NM_TITLE,  "Settings",       0, 0, 0, 0},
         /* WARNING: check item_number if you add menu entries */
      {NM_ITEM,   "PAL",            0, CHECKIT, 6, (void *)50},
      {NM_ITEM,   "NTSC",           0, CHECKIT, 5, (void *)60},
      {NM_ITEM,   "Custom",         0, CHECKIT, 3, (void *)3},
   {NM_END,    0,                0, 0, 0, 0}
   };

LOCAL struct Menu *menu = 0;

LOCAL APTR vi;
LOCAL struct Screen *pub = 0;
LOCAL struct Gadget *glist, *title_gad, *pattern_gad, *total_gad;

/* we precisely have seven gadgets */
#define MAX_GADGET 6

/* all labelled with strings */
LOCAL char *label[MAX_GADGET + 1] =
   {
   "|<",
   "<<",
   "//",
   ">>", 
   ">|",
   "?",
   "000",
   };

#define G_NEXT 4
#define G_RESTART_PREVIOUS 0
#define G_REWIND 1
#define G_FF 3
#define G_SHOW 5
#define G_PAUSE 2
LOCAL struct ext_message *restart_msg = NULL;

LOCAL void cleanup_ui()
   {
   if (restart_msg)
      {
      send(restart_msg, TYPE_UNPAUSE);
      restart_msg = 0;
      }
   if (ui_win)
      {
      remove_signal_handler(ui_win->UserPort->mp_SigBit);
      SetWindowTitles(ui_win, 0, 0);
      ClearMenuStrip(ui_win);
      CloseWindow(ui_win);
      ui_win = 0;
      }
   if (menu)
      FreeMenus(menu);
   if (glist)
      FreeGadgets(glist);
   if (vi)
      FreeVisualInfo(vi);
   if (pub)
      UnlockPubScreen(NULL, pub);
   while (notice_win)
      await_events();
   if (GfxBase)
      CloseLibrary(GfxBase);
   if (IntuitionBase)
      CloseLibrary(IntuitionBase);
   if (GadtoolsBase)
      CloseLibrary(GadtoolsBase);
   }
   

LOCAL void init_ui(void)
   {
   struct Gadget *gad;
   int i;
   int max_width;
   int width, height;
   UWORD zoom[4];

   at_end(cleanup_ui);
   IntuitionBase = OpenLibrary("intuition.library", 37);
   if (!IntuitionBase)
      end_all("No Intuition");
   GadtoolsBase = OpenLibrary("gadtools.library", 37);
   if (!GadtoolsBase)
      end_all("No gadtools");
   GfxBase = OpenLibrary("graphics.library", 37);
   if (!GfxBase)
      end_all("No graphics");
   pub = LockPubScreen(NULL);
   if (!pub)
      end_all("No pubscreen");
   vi = GetVisualInfo(pub, TAG_END);
   if (!vi)
      end_all("No VI");
      {
      int item_number;

      switch(get_pref_scalar(PREF_SPEED))
         {
      case 50:
         item_number = 5; break;
      case 60:
         item_number = 6; break;
      default:
         item_number = 7; break;
         }
      menu_template[item_number].nm_Flags |= CHECKED;
      }
   menu = CreateMenus(menu_template, GTMN_FrontPen, BARDETAILPEN, TAG_END);
   if (!menu)
      end_all("No menus");
   if (!LayoutMenus(menu, vi, TAG_END))
      end_all("Menus badly formed");
      
      /* now to create the gadgets */
      /* Compute max width/height according to the font */
   it.ITextFont = pub->Font;
   template.ng_TextAttr = pub->Font;
   
   max_width = 0;
   for (i = 0; i < MAX_GADGET + 1; i++)
      {
      it.IText = label[i];  
      width = IntuiTextLength(&it);
      if (width > max_width)
         max_width = width;
      }
    
   max_width += SPACEX;
   template.ng_Width = max_width;     
   template.ng_Height = pub->Font->ta_YSize + SPACEY;

      /* set up Top/Left Edge of initial gadget according to Wbar */
   template.ng_TopEdge = pub->WBorTop + 1 + 2 * template.ng_Height + SHIFT;
   template.ng_LeftEdge = pub->WBorLeft + SHIFT; 

   gad = CreateContext(&glist);
   if (!gad)
      end_all("No context");
   template.ng_VisualInfo = vi;
      /* lay out gadgets */
   for (i = 0; i < MAX_GADGET; i++)
      {
      template.ng_GadgetText = label[i];
      gad = CreateGadget(BUTTON_KIND, gad, &template, TAG_END);
      if (!gad)
         end_all("Bad gadget");
      template.ng_LeftEdge += template.ng_Width + SHIFT;
      template.ng_GadgetID++;
      }
   width = template.ng_LeftEdge + pub->WBorRight;
   height = template.ng_TopEdge + template.ng_Height + pub->WBorBottom + SHIFT;
 
      /* zoom box */
   zoom[0] = ~0;
   zoom[1] = ~0;
   zoom[2] = width;
   zoom[3] = pub->WBorTop + 1 + pub->Font->ta_YSize;

      /* title gadget */     
   template.ng_GadgetText = "";
   template.ng_Width = width - SHIFT - pub->WBorLeft - pub->WBorRight;
   template.ng_Width -= 2 * max_width;
   template.ng_TopEdge = pub->WBorTop + 1 + template.ng_Height + SHIFT;
   template.ng_LeftEdge = SHIFT + pub->WBorLeft;
   title_gad = gad = CreateGadget(TEXT_KIND, gad, &template, TAG_END);
   if (!gad)
      end_all("Bad gadget");
      
      /* pattern gadget */
   template.ng_LeftEdge += template.ng_Width + SHIFT;
   template.ng_Width = max_width;
   pattern_gad = gad = CreateGadget(NUMBER_KIND, gad, &template, TAG_END);
   if (!gad)
      end_all("Bad gadget");

      /* total pattern */
   template.ng_GadgetText = "/";
   template.ng_LeftEdge += template.ng_Width + SHIFT;
   total_gad = gad = CreateGadget(NUMBER_KIND, gad, &template, TAG_END);
   if (!gad)
      end_all("Bad gadget");
   ui_win = OpenWindowTags(NULL, 
      WA_Title, WINDOW_TITLE,
      WA_Width, width,
      WA_Height, height,
      WA_MinWidth, zoom[2],
      WA_MaxWidth, width,
      WA_MinHeight, zoom[3],
      WA_MaxHeight, height,
      WA_AutoAdjust, TRUE,
      WA_MouseQueue, 35,   /* we can't always answer messages */
      WA_DepthGadget, TRUE,
      WA_CloseGadget, TRUE,
      WA_DragBar, TRUE,
      WA_Zoom, zoom,
      WA_Gadgets, glist,
      WA_IDCMP, IDCMP_CLOSEWINDOW | BUTTONIDCMP | TEXTIDCMP | 
                IDCMP_REFRESHWINDOW | IDCMP_MENUPICK,
      WA_NewLookMenus, TRUE,
      WA_PubScreen, pub,
      TAG_DONE, 0);
   if (!ui_win)
      end_all("No window");
   GT_RefreshWindow(ui_win, NULL);  
   SetMenuStrip(ui_win, menu);

   install_signal_handler(ui_win->UserPort->mp_SigBit, handle_ui_window, 0);
   
   /* build up scroll buffer stuff */
   }

struct Screen *obtain_pubscreen(void)
   {
   INIT_ONCE;
   
   return pub;
   }

/* Max number of input messages we can remember */
#define MAX_INPUT 20
LOCAL struct tag result[MAX_INPUT +1];
LOCAL int i = 0;

LOCAL void handle_ui_window(GENERIC nothing)
   {
   struct IntuiMessage *msg;
   UWORD number;
   struct MenuItem *item;
   int id;
   VALUE temp;
   
   while((msg = GT_GetIMsg(ui_win->UserPort)) && i < MAX_INPUT)
      switch(msg->Class)
         {
      case IDCMP_CLOSEWINDOW:
         GT_ReplyIMsg(msg);
         set_break();
         break;
      case IDCMP_MENUPICK:
         number = msg->Code;
         while (number != MENUNULL)
            {
            item = ItemAddress(menu, msg->Code);
            switch((int)GTMENUITEM_USERDATA(item))
               {
            case 1:
               notice(
"Tracker "VERSION"\n\
      by Marc Espie (Marc.Espie@ens.fr)\n\n\
This is a giftware program\n\
If you want, you can send me some money\n\
My address is:\n\
      Espie Marc\n\
      60 rue du 4 septembre\n\
      87100 Limoges\n\
      France\n\n\
For the most recent version:\n\
      ftp Aminet or nic.funet.fi");
               break;
            case 2:
               item = 0;
               set_break();
               break;
            case 4:
               launch_requester();
               break;
            case 50:
            case 60:
               result[i].type = UI_SET_BPM;
               result[i++].data.scalar = (int)GTMENUITEM_USERDATA(item);
               set_pref_scalar(PREF_SPEED, (int)GTMENUITEM_USERDATA(item));
               break;
            default:
               break;
               }
            number = item->NextSelect;
            }
         GT_ReplyIMsg(msg);
         break;
      case IDCMP_REFRESHWINDOW:
         GT_ReplyIMsg(msg);
         GT_BeginRefresh(ui_win);
         GT_EndRefresh(ui_win, TRUE);
         break;
      case IDCMP_GADGETUP:
         id = ((struct Gadget *)msg->IAddress)->GadgetID;
         switch(id)
            {
         case G_NEXT:
            result[i++].type = UI_NEXT_SONG;
            break;
         case G_RESTART_PREVIOUS:
            if (msg->Seconds < song_change_seconds + SMALL_DELAY ||
            	(msg->Seconds == song_change_seconds + SMALL_DELAY && 
                msg->Micros <= song_change_micros) )
                {
                result[i++].type = UI_PREVIOUS_SONG;
                break;
                }
            else
               {
               result[i++].type = UI_RESTART;
               song_change_seconds = msg->Seconds;
               song_change_micros = msg->Micros;
               }
            break;
         case G_REWIND:
            result[i].type = UI_JUMP_TO_PATTERN;
            result[i].data.scalar = current_pattern;
            if (msg->Seconds < pattern_change_seconds + SMALL_DELAY ||
            	(msg->Seconds == pattern_change_seconds + SMALL_DELAY && 
                msg->Micros <= pattern_change_micros) )
            	result[i].data.scalar--;
            	   /* give some immediate feedback to the user */
            temp.scalar = result[i].data.scalar;
            do_set_current(temp);
				i++;
            break;
         case G_FF:
            result[i].type = UI_JUMP_TO_PATTERN;
            result[i].data.scalar = current_pattern + 1;
                  /* give some immediate feedback to the user */
            temp.scalar = result[i].data.scalar;
            do_set_current(temp);
            i++;
            break;
         case G_SHOW:
            set_pref_scalar(PREF_SHOW, TRUE);
            break;
         case G_PAUSE:
            if (restart_msg)
               {
               send(restart_msg, TYPE_UNPAUSE);
               restart_msg = 0;
               }
            else
               {
               struct ext_message *msg;
               
               msg = obtain_message();
               restart_msg = obtain_message();
               send(msg, TYPE_PAUSE);
               }
            }
         GT_ReplyIMsg(msg);
         break;
      default:
         GT_ReplyIMsg(msg);
         }
   }


void requested_file(struct amiganame *name)
   {
   result[i].data.pointer = name;
   result[i++].type = UI_LOAD_SONG;
   }

   
struct tag *get_ui()
   {

   INIT_ONCE

   if (checkbrk())
      result[i++].type = UI_QUIT;
   
   result[i].type = TAG_END;
   
   i = 0;
   return result;
   }


void song_title(char *s)
   {
   static char title[25];

   INIT_ONCE;

   strncpy(title, s, 25);
   if (ui_win)
      GT_SetGadgetAttrs(title_gad, ui_win, 0, GTTX_Text, title, TAG_END);
   /* stamp the time we changed the song */
   CurrentTime(&song_change_seconds, &song_change_micros);
   }

void status(char *s)   
   {
   INIT_ONCE;
   
   SetWindowTitles(ui_win, s ? s : WINDOW_TITLE, -1);
   }

/* hook to change current pattern */
LOCAL void do_set_current(VALUE p)
   {
   if (!inhibit_output)
      {
      INIT_ONCE;
      if (ui_win)
         GT_SetGadgetAttrs(pattern_gad, ui_win, 0, GTNM_Number, p.scalar, TAG_END);
      }
   current_pattern = p.scalar;
   /* stamp the time we changed the pattern */
   CurrentTime(&pattern_change_seconds, &pattern_change_micros);
   }

/* hook to change current pattern total */
LOCAL void do_set_total(VALUE p)
	{
	INIT_ONCE;
   if (ui_win)
      GT_SetGadgetAttrs(total_gad, ui_win, 0, GTNM_Number, p.scalar, TAG_END);
	}

void display_pattern(int current, int total, int real)
   {
   struct ext_message *msg;

   INIT_ONCE 

   msg = obtain_message();
	msg->data.hook.func = do_set_total;
   msg->data.hook.p.scalar = total;
   send(msg, TYPE_SYNC_DO);

   msg = obtain_message();
	msg->data.hook.func = do_set_current;
   msg->data.hook.p.scalar = current;
   send(msg, TYPE_SYNC_DO);
   }




/***
 ***
 ***	notice() pseudo-system call.
 ***	mostly used to report errors
 ***
 ***	The only difficulty comes from the fact
 ***	that we may be called under any kind of environment
 ***
 ***/

#ifdef USE_ARQ
#include "arq.h"
/* arq 1.78 doesn't notice BuildEasyRequest().
   I need to contact Martin Laubach about it
       FidoNet: 2:310/3.14 
       Usenet:  mjl@alison.at (home) 
                               mjl@auto.tuwien.ac.at (work) 
                {cbmvax!cbmehq,mcsun!tuvie}!cbmvie!alison!mjl 

       Peter, the graphics and animation wizard, can be reached
     2:310/42 in FidoNet.  
 */

LOCAL struct ExtEasyStruct es =
   {
   0,
   0,
   ARQ_ID_INFO,
   0,
   ARQ_MAGIC,
   0, 0, 0,
   sizeof(struct EasyStruct),
   0,
   "Notice\xa0",
   NULL,
   "Proceed"
   };
LOCAL struct EasyStruct *esp = & (es.Easy);
#else
LOCAL struct EasyStruct es = 
   {
   sizeof(struct EasyStruct),
   0,
   "Notice",
   NULL,
   "Proceed"
   };
LOCAL struct EasyStruct *esp = &es;
#endif


void handle_notice(struct Window *w)
   {
   if (SysReqHandler(w, 0, FALSE) != -2)
      {
      remove_signal_handler(w->UserPort->mp_SigBit);
      FreeSysRequest(w);
      notice_win = 0;
      }
   }

void notice(char *s)
   {
   INIT_ONCE;
   
   if (!IntuitionBase)
      {
      fprintf(stderr, s);
      fputc('\n', stderr);
      }
   else
      {
         /* wait for previous notice to go away */
      while (notice_win)
         await_events();
      
      esp->es_TextFormat = s;
      notice_win = BuildEasyRequest(0, esp, NULL, NULL);
      install_signal_handler(notice_win->UserPort->mp_SigBit, handle_notice, notice_win);
      }
   }

