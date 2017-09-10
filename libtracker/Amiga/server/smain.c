/* amiga/server/main.c */

/* sound server task management */

/* $Id: smain.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: smain.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:40  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.8  1995/02/13  22:08:26  Espie
 * No boolean. Changed include path.
 *
 * Revision 1.7  1994/01/08  20:26:45  Espie
 * Added pause gadget.
 *
 * Revision 1.6  1994/01/08  04:05:32  Espie
 * Added geta4 for local data model.
 *
 * Revision 1.5  1994/01/07  15:58:17  Espie
 * Semantics of TIME_WAIT has changed:
 * we now input delays and the server computes what's needed.
 * Makes Pause feasible !
 *
 * Revision 1.4  1994/01/05  14:56:02  Espie
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/05  04:35:23  Espie
 * Suppressed old debug messages.
 *
 * Revision 1.2  1994/01/04  19:13:21  Espie
 * Almost nothing.
 *
 * Revision 1.1  1994/01/04  15:45:37  Espie
 * Initial revision
 *
 */
#include <exec/nodes.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <devices/audio.h>
#ifdef EXTERNAL
#include <stdio.h>
#endif

#include "defs.h"
#include "amiga/amiga.h"
#include "amiga/server/server.h"

ID("$Id: smain.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $")

LOCAL struct MsgPort *port = 0;
LOCAL struct MinList event_list;

void close_all()
   {
   end_audio();
   close_timer();
   if (port)
      {
      struct Message *msg;
      
      while (msg = GetMsg(port))
         ReplyMsg(msg);
      while (msg = RemHead(&event_list))
         ReplyMsg(msg);
      if (port->mp_Node.ln_Name)
         RemPort(port);
      DeleteMsgPort(port);
      }
   }

void subtask(struct ext_message *msg)
   {
   struct MsgPort *aport, *tport;
   struct ext_message *msg2;
   int paused = FALSE;

   geta4();
   msg->data.comm.task = FindTask(0);
   msg->data.comm.port = 0;
   if (!port)
      port = CreateMsgPort();
   NewList(&event_list);
   aport = start_audio();
   tport = open_timer();

   if (port && aport && tport)
      {
      msg->data.comm.port = port;
      ReplyMsg(msg);
      }
   else
      {
      close_all();
      Forbid();
      ReplyMsg(msg);
#ifdef EXTERNAL
      return;
#else
      Wait(0L);
#endif
      }

   forever
      {
      ULONG mask;
      
      mask = Wait(1<<port->mp_SigBit | 1<<tport->mp_SigBit | 1<<aport->mp_SigBit);
         /* deal with messages */
      if (mask & 1<<port->mp_SigBit)
         {

         while (msg = (struct ext_message *)GetMsg(port))
            {
            switch(msg->type)
               {
            case TYPE_DIE:
#ifdef EXTERNAL
               puts("Dying");
#endif
               close_all();
               Forbid();
               msg->data.comm.task = FindTask(0);
               ReplyMsg(msg);
#ifdef EXTERNAL
               puts("Dead");
               return;
#else
               Wait(0);
#endif
            case TYPE_FLUSH_BUFFER:
               reset_audio();
               ReplyMsg(msg);
               while(msg = RemHead(&event_list))
                  {
                  ReplyMsg(msg);
                  }
               break;
            case TYPE_PAUSE:
               paused = TRUE;
               ReplyMsg(msg);
               send_immediate(CMD_STOP, 15);
               break;
            case TYPE_UNPAUSE:
               paused = FALSE;
               send_immediate(CMD_START, 15);
               handle_timer(&event_list, TRUE);
               ReplyMsg(msg);
               break;
               /* bandwidth optimization: compress multiple WAIT messages 
                * and reply them right away
                */
            case TYPE_WAIT:
               if (msg2 = RemTail(&event_list))
                  {
                  if (msg2->type == TYPE_WAIT)
                     {
                     msg->data.time.low += msg2->data.time.low;
                     if (msg->data.time.low < msg2->data.time.low)
                        msg->data.time.high++;
                     msg->data.time.high += msg2->data.time.high;
                     ReplyMsg(msg2);
                     }
                  else
                     AddTail(&event_list, msg2);
                  }
               /* FALL THROUGH */
            default:
               AddTail(&event_list, msg);
               }
            }
         }
      if (!paused)
         handle_timer(&event_list, mask & 1<<tport->mp_SigBit);
      handle_audio(&event_list, mask & 1<<aport->mp_SigBit);
      }
   }

#ifdef EXTERNAL
main()
   {
   port = CreateMsgPort();
   port->mp_Node.ln_Name = PUBLIC_PORT_NAME;
   port->mp_Node.ln_Pri = 0;
   AddPort(port);
   WaitPort(port);
   subtask(GetMsg(port));
   Permit();
   }
#endif
