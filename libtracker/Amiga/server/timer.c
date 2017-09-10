/* amiga/server/timer.c */

/* $Id: timer.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: timer.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:40  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.5  1995/02/13  22:08:26  Espie
 * No boolean. Changed include path.
 *
 * Revision 1.4  1994/01/07  15:58:17  Espie
 * Semantics of TIME_WAIT has changed:
 * we now input delays and the server computes what's needed.
 * Makes Pause feasible !
 *
 * Revision 1.3  1994/01/05  14:56:02  Espie
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/05  04:35:23  Espie
 * Suppressed old debug messages.
 *
 * Revision 1.1  1994/01/04  15:45:37  Espie
 * Initial revision
 *
 */

#include <exec/nodes.h>
#include <exec/memory.h>
#include <devices/timer.h>
#include <proto/exec.h>
#include <proto/timer.h>

#ifdef EXTERNAL
#include <stdio.h>
#endif

#include "defs.h"
#include "amiga/amiga.h"
#include "amiga/server/server.h"

ID("$Id: timer.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $")
LOCAL struct MsgPort *tport = 0;
LOCAL struct timerequest *tr = 0;
LOCAL int timer_opened = FALSE;
LOCAL struct Library *TimerBase = 0;

/* the reference time at which the sound should play */
LOCAL struct EClockVal play_time;


/* auto adjusting facilities */

/* quantum = EClockFreq >> FRACTION
 * Initially, time_lag = quantum << INITIAL
 */
#define FRACTION 7
#define INITIAL 3
/* the time lag between the current time and the time at which
 * we want to play
 */
LOCAL ULONG time_lag;
/* value to adjust it by each time we miss */
LOCAL ULONG quantum;

/* check the time it is: have we got enough time left ? */
LOCAL void check_time()
   {
   struct EClockVal current_time;
   struct ext_message *msg;
   unsigned long freq;

   freq = ReadEClock(&current_time);
   
      /* compare time_play against current_time */
   if (play_time.ev_hi < current_time.ev_hi ||
      (play_time.ev_hi == current_time.ev_hi && play_time.ev_lo < current_time.ev_lo) )
      {
         /* we've fallen behind -> adjust play_time */
      play_time.ev_hi = current_time.ev_hi;      
      play_time.ev_lo = current_time.ev_lo + time_lag;
      if (play_time.ev_lo < time_lag)
         play_time.ev_hi++;
         /* adjust delay */
      time_lag += quantum;
      }
   }

struct MsgPort *open_timer()
   {
   int fail;

   tport = CreateMsgPort();
   if (!tport)
      return 0;
   tr = CreateIORequest(tport, sizeof(struct timerequest));
   if (!tr)
      return 0;
   fail = OpenDevice(TIMERNAME, UNIT_WAITECLOCK, (struct IORequest *)tr, 0);
   if (fail)
      return 0;
   else
      {
      timer_opened = TRUE;
      TimerBase = (struct Library *)tr->tr_node.io_Device;
      return tport;
      }
   }

void close_timer(void)
   {
   if (timer_opened)
      {
      if (!CheckIO((struct IORequest *)tr))
         {
         AbortIO((struct IORequest *)tr);
         WaitIO((struct IORequest *)tr);
         }
      CloseDevice((struct IORequest *)tr);
      }
   if (tr)
      DeleteIORequest(tr);
   if (tport)
      DeleteMsgPort(tport);
   }
   
void handle_timer(struct List *events, int signaled)
   {
   LOCAL int not_waiting = TRUE;
   struct ext_message *msg;

   if (signaled)
      {
      while(GetMsg(tport))
         {
         do_events(events);
         not_waiting = TRUE;
         }
      }

         /* if there is no timer request pending and some messages waiting, 
          * we post one ! */
   if (not_waiting && (msg = RemHead(events)))
      {        /* if there is a time request, we use it */
      if (msg->type == TYPE_WAIT)   
         {
         play_time.ev_lo += msg->data.time.low;
         if (play_time.ev_lo < msg->data.time.low)
            play_time.ev_hi++;
         play_time.ev_hi += msg->data.time.high;
         ReplyMsg(msg);
         }
      else     /* else we fake one (delay 0) */
         AddHead(events, msg);
      check_time();
      tr->tr_node.io_Command = TR_ADDREQUEST;
      tr->tr_time.tv_secs = play_time.ev_hi;
      tr->tr_time.tv_micro = play_time.ev_lo;
      SendIO((struct IORequest *)tr);
      not_waiting = FALSE;
      }
   }   

