/* events.c 
	vi:ts=3 sw=3:
 */
/* global events handler mechanism */

/* $Id: events.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: events.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.4  1995/01/13  13:31:35  espie
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/09  04:49:18  Espie
 * File requester !
 */

#include <proto/exec.h>
#include <dos/dos.h>
#include "defs.h"
#include "extern.h"
#include "amiga/amiga.h"

ID("$Id: events.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $")

LOCAL void init_events(void);
LOCAL void (*INIT)(void) = init_events;

#define NUMBER_SIGNALS 32

LOCAL struct event_manager
   {
   ULONG    mask;       /* mask of signals we might wait for */
   ULONG    req_mask;   /* special requester handler */
   void     (*req_f)P((ULONG received));

   struct hook
      {
      void (*f)P((GENERIC data));
      GENERIC data;
      } hook[NUMBER_SIGNALS];
   } manager;


void install_signal_handler(int signal, void (*f)P((GENERIC data)), GENERIC data)
   {
   ULONG sigmask = 1 << signal;
   
   INIT_ONCE;
   
   if (f)
      {
      manager.mask |= sigmask;
      manager.hook[signal].f = f;
      manager.hook[signal].data = data;
      }
   else
      {
      manager.mask &= ~signal;
      manager.hook[signal].f = 0;
      }
   }

void remove_signal_handler(int signal)
   {
   install_signal_handler(signal, 0, 0);
   }

/* special case for reqtools.library ! */
void install_req_handler(ULONG mask, void (*req_f)P((ULONG received)))
   {
   INIT_ONCE;

   if (req_f)
      {
      manager.req_mask = mask;
      manager.req_f = req_f;
      }
   else
      {
      manager.req_mask = 0;
      manager.req_f = 0;
      }
   }

void remove_req_handler(void)
   {
   install_req_handler(0, 0);
   }

LOCAL void handle_events(ULONG received)
   {
   int i; 
   ULONG mask;

   if ((received & manager.req_mask) && manager.req_f)
      (*manager.req_f)(received & manager.req_mask);
	for (i = 0, mask = 1; i < NUMBER_SIGNALS; i++, mask <<= 1)
		if ((received & mask) && manager.hook[i].f)
		   (*manager.hook[i].f)(manager.hook[i].data);
   }

void await_events()
   {
   ULONG received;
   
   INIT_ONCE;
   received = Wait(manager.mask | manager.req_mask);
   handle_events(received);
   }

void check_events()
   {
   ULONG received;

   INIT_ONCE;
   received = SetSignal(0, 0) & (manager.mask | manager.req_mask);
      /* Clear signals received */
   SetSignal(0, received);
   handle_events(received);
   }


LOCAL int ask_break = FALSE;

void set_break()
   {
   ask_break = TRUE;
   }

LOCAL void handle_ctrl_c(GENERIC nothing)
   {
   set_break();
   }

LOCAL void init_events()
   {
   int i;

   manager.mask = 0;
   manager.req_mask = 0;
   manager.req_f = 0;
   for (i = 0; i < NUMBER_SIGNALS; i++)
      manager.hook[i].f = 0;
      
   install_signal_handler(SIGBREAKB_CTRL_C, handle_ctrl_c, 0);
   }
   
int checkbrk()
   {
   INIT_ONCE;

   check_events();
   return ask_break;
   }

