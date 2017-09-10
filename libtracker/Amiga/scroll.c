/* amiga/scroll.c
	vi:ts=3 sw=3:
 */

/* $Id: scroll.c,v 1.1.1.1 2002/01/18 16:34:23 lindner Exp $
 * $Log: scroll.c,v $
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
 * Added one space to buffer.
 *
 * Revision 1.1  1995/01/13  13:31:35  espie
 * Initial revision
 *
 */

#include <proto/exec.h>

#include "defs.h"
#include "extern.h"
#include "amiga/amiga.h"
#include "prefs.h"
ID("$Id")

XT unsigned int inhibit_output;

LOCAL void init_scroll(void);
LOCAL void (*INIT)(void) = init_scroll;

/* the special structure to display lines 
 *	
 */
LOCAL struct MinList scrolls;

struct scroll_line
   {
   struct MinNode node;
   char buffer[80];
   };

LOCAL void init_scroll()
   {
   NewList(&scrolls);
   }

/***
 ***
 ***	Scrolling line handling: 
 ***		note this is totally asynchronous and uses the TYPE_DO_SYNC
 ***		message for synchronizing with songs that are really played
 ***/

LOCAL struct scroll_line *scroll_buffer = 0;

char *new_scroll(void)
   {
   char *s;
   	/* need some temporary storage in case everything is full */
	LOCAL char buffer[80];

   INIT_ONCE;
                           /* check for a scroll line available */
   scroll_buffer = RemHead(&scrolls);
   if (!scroll_buffer)     /* none available ? allocate one on the fly */
      scroll_buffer = malloc(sizeof(struct scroll_line));
   if (scroll_buffer)
      s = scroll_buffer->buffer;
   else                    /* still none ? use static buffer */
      s = buffer;
   strcpy(s, "                                                        ");
   return s;
   }

/* The actual hook that does all the printing */
LOCAL void do_scroll(VALUE p)  
   {
   struct scroll_line *s = p.pointer;
	
	if (inhibit_output == 0 && get_pref_scalar(PREF_SHOW))
		{
		add_scroller(s->buffer);
      }
	AddTail(&scrolls, s);
   }

void scroll()
   {
   struct ext_message *msg;
   
   if (scroll_buffer)		/* did we obtain a scroll line ? */
      {                    /* then set up to scroll it */
      msg = obtain_message();
      msg->data.hook.func = do_scroll;
      msg->data.hook.p.pointer = scroll_buffer;
      send(msg, TYPE_SYNC_DO);
      }
   scroll_buffer = 0;
   }

