/* amiga/server/sound.c */

/* internal operations for the sound server itself */

/* $Id: sound.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: sound.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:40  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.13  1995/02/13  22:08:26  Espie
 * No boolean. Changed include path.
 *
 * Revision 1.12  1994/01/08  20:26:45  Espie
 * Added pause gadget.
 *
 * Revision 1.11  1994/01/07  15:12:26  Espie
 * *** empty log message ***
 *
 * Revision 1.10  1994/01/07  01:01:49  Espie
 * Id.
 *
 * Revision 1.9  1994/01/05  16:11:43  Espie
 * Made TYPE_xxxSYNC explicit.
 *
 * Revision 1.8  1994/01/05  14:56:02  Espie
 * *** empty log message ***
 *
 * Revision 1.7  1994/01/05  04:35:23  Espie
 * Found spurious bug: I had to track through the various
 * possibilities of CMD_WRITE to finally realize I was losing my
 * messages... So when the queue was empty, I could not allocate
 * any channel...
 * Needed external program capability back in order though.
 * Problem was coming from data faronly, which is very incompatible
 * with printf.
 *
 * Note that now, we only care about reallocating channels when we're
 * doing a CMD_WRITE. doing it for ADCMD_PERVOL or CMD_FLUSH is pointless
 * since our old data have been completely forgotten in between...
 *
 * Some tests and masking are probably unnecessary. On the other hand, the
 * error code MUST be checked correctly. Maybe I'll try and make the
 * CMD_WRITE part clearer.
 *
 * Revision 1.6  1994/01/05  02:03:14  Espie
 * Mostly correct check for losing audio channels.
 *
 * Revision 1.5  1994/01/04  22:05:35  Espie
 * Cosmetic changes, mostly.
 *
 * Revision 1.4  1994/01/04  20:46:23  Espie
 * Plainly working version with all checks...
 *
 * Revision 1.3  1994/01/04  20:33:20  Espie
 * Mostly working version
 *
 * Revision 1.2  1994/01/04  19:13:21  Espie
 * Corrected most of the audio allocation/free muck-up.
 *
 * Revision 1.1  1994/01/04  15:45:37  Espie
 * Initial revision
 *
 */

#include <hardware/cia.h>
#include <hardware/intbits.h>
#include <hardware/dmabits.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <devices/audio.h>
#include <proto/exec.h>
#include <stddef.h>

#include <stdio.h>


#include "defs.h"
#include "amiga/amiga.h"
#include "amiga/server/server.h"

ID("$Id: sound.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $")
extern volatile struct CIA __far ciaa;

UBYTE saved_filter, current_filter;

/* we allow for lots more audio requests than we need */
#define QUEUE_LENGTH 50


LOCAL struct ext_audio
   {
   struct MinNode node;
   struct IOAudio request;
   }
/* for sending immediate commands */
 *immediate = 0;
/* the specific request that gets used for closing/opening */
struct IOAudio *req = 0;

/* the audio queue */
struct MinList queue;

/* keep track of progress */
struct MinList pending;

LOCAL struct MsgPort *aport = 0;
/* in order to clean up afterwards: */
LOCAL int audio_opened = FALSE;

void send_immediate(ULONG command, int mask);
LOCAL void get_requests(void);

/* mask for allocation */
LOCAL UBYTE whichchannel[1];
/* mask of owned channels */
LOCAL UBYTE owned;

/* allocate_channels(mask): sends a request to the audio.device for
 * the channels described by mask
 */
LOCAL void allocate_channels(UBYTE mask)
   { 
   whichchannel[0] = mask & ~owned;
   req->ioa_Request.io_Command = ADCMD_ALLOCATE;
   req->ioa_Data = whichchannel;
   req->ioa_Length = sizeof(whichchannel);
   req->ioa_Request.io_Flags = ADIOF_NOWAIT | IOF_QUICK;
   BeginIO(req);
   WaitIO(req);
   if (!req->ioa_Request.io_Error)     /* update owned channels mask */
      {
      owned |= (int)req->ioa_Request.io_Unit;
#ifdef EXTERNAL
      printf("Allocated: %d (%d) = %d\n", (int)mask, (int)req->ioa_Request.io_Unit, (int)owned);
      fflush(stdout);
#endif
      }
#ifdef EXTERNAL
   else
      printf("Failed: %d (%d) = %d\n", req->ioa_Request.io_Error, mask, owned);
#endif
   ciaa.ciapra = (ciaa.ciapra & ~ CIAF_LED) | current_filter;
   }

/* free_channels(): give back all channels.
 */ 
LOCAL void free_channels(void)
   {
   if (req->ioa_AllocKey)
      {
      req->ioa_Request.io_Command = ADCMD_FREE;
      req->ioa_Request.io_Unit = owned;
      BeginIO(req);
      WaitIO(req);
#ifdef EXTERNAL
      printf("Freed !\n");
      fflush(stdout);
#endif
      owned = 0;
      }
   }

LOCAL struct ext_audio *create_request(void)
   {
   struct ext_audio *new;
         /* CHECK: no test for failed allocation */
   new = AllocMem(sizeof(struct ext_audio), MEMF_CLEAR|MEMF_PUBLIC);
   if (req && new)
      new->request = *req;
   return new;
   }

/* actual = create_queue(n): creates a queue of n audio requests,
 *    returns the actual number
 */   
LOCAL int create_queue(int n)
   {
   int i;
   struct ext_audio *new;

   for (i = 0; i < n; i++)
      {
      new = create_request();
      if (!new)
         return i;
      AddTail(&queue, new);
      }  
   return i;
   }

/* get back available requests */
LOCAL void get_requests(void)
   {
   struct IOAudio *back;

   while(back = GetMsg(aport))
      {
      struct ext_audio *full;
      full = (struct ext_audio *)
         (((UBYTE *)back) - offsetof(struct ext_audio, request));
      Remove(full);           /* no longer pending */
      AddTail(&queue, full);  /* ... and available */
      }
   }

/* send immediate command.
 * WARNING: only use send_immediate with commands which
 * are GUARANTEED to be synchronous if IOF_QUICK is set
 */
void send_immediate(ULONG command, int mask)
   {
   immediate->request.ioa_Request.io_Command = command;
   immediate->request.ioa_Request.io_Flags = IOF_QUICK;
   immediate->request.ioa_Request.io_Unit = (void *)(mask & owned);
   immediate->request.ioa_AllocKey = req->ioa_AllocKey;
   BeginIO(&immediate->request);
   return;     
   }

void reset_audio(void)
   {
   send_immediate(CMD_FLUSH, 15);
   get_requests();
   free_channels();
   }

/* obtain_audio(): allocate all the structures we will need to
 * play with the audio device
 */ 
LOCAL void obtain_audio(void)
   {
   BYTE fail;
   
   aport = CreateMsgPort();
   if (!aport)
      return;
   req = CreateIORequest(aport, sizeof(struct IOAudio));
   if (!req)
      return;
   req->ioa_AllocKey = 0;
   req->ioa_Request.io_Message.mn_Node.ln_Pri = 0;
      /* Note that OpenDevice returns 0 on success
       */
   fail = OpenDevice("audio.device", 0L, (struct IORequest *)req, 0L);
   if (!fail)
      audio_opened = TRUE;
   }

struct MsgPort *start_audio(void)
   {
   NewList(&queue);     /* initialize these now for correct cleanup */
   NewList(&pending);

   owned = 0;           /* we own no channel right now */
   obtain_audio();
   if (!audio_opened || create_queue(QUEUE_LENGTH) < QUEUE_LENGTH)
      return 0;
   immediate = RemHead(&queue);
   saved_filter = ciaa.ciapra & CIAF_LED;
   current_filter = CIAF_LED;
   return aport;  
   }  

void end_audio()
   {
   struct ext_audio *sweep, *next;

   if (immediate)       /* get immediate back in queue for freeing it */
      AddTail(&queue, immediate);
   if (req)
      free_channels();
   get_requests();
      /* REMOVE ALL REQUESTS FROM THE PENDING LIST */
   SCANLIST(sweep, next, &pending, struct ext_audio *)
      {
      AbortIO(&sweep->request);
      WaitIO(&sweep->request);
      }
      /* Now we can close safely */
   if (audio_opened)
      CloseDevice((struct IORequest *)req);
   if (req)
      DeleteIORequest(req);
   if (aport)
      DeletePort(aport);

   SCANLIST(sweep, next, &queue, struct ext_audio *)
      FreeMem(sweep, sizeof(struct ext_audio));
   ciaa.ciapra = (ciaa.ciapra & ~CIAF_LED) | saved_filter;
   }


/* Perform musical events pertaining to the current time */
void do_events(struct List *e)
   {
   struct ext_message *msg, *msg2;
   
   while (msg = RemHead(e))
      {
      switch (msg->type)
         {
      case TYPE_WAIT:         /* next time to wait for: finished */
         AddHead(e, msg);
         return;
      case TYPE_FLUSH_CHANNEL:
         send_immediate(CMD_FLUSH, msg->data.info.channel_mask);
         break;
      case TYPE_SETUP:        /* setup is a two message command */
         msg2 = RemHead(e);
         
         if (msg2)
            {
            struct ext_audio *new;
            
            new = RemHead(&queue);
            if (new)
               {
               new->request.ioa_Request.io_Command = CMD_WRITE;
               if (msg2->data.info.pitch)
                  {
                  new->request.ioa_Request.io_Flags = ADIOF_PERVOL | IOF_QUICK;
                  new->request.ioa_Period = msg2->data.info.pitch;
                  new->request.ioa_Volume = msg2->data.info.volume;
                  }
               else
                  new->request.ioa_Request.io_Flags = IOF_QUICK;
               if ((owned & msg2->data.info.channel_mask) == 0)
                  allocate_channels(msg2->data.info.channel_mask);
               new->request.ioa_Request.io_Unit = msg2->data.info.channel_mask & owned;
               new->request.ioa_Cycles = msg2->data.info.cycle;
               new->request.ioa_Data = msg->data.sample.start;
               new->request.ioa_Length= msg->data.sample.length;
               new->request.ioa_AllocKey = req->ioa_AllocKey;
               if (new->request.ioa_Request.io_Unit)
                  {
                  BeginIO(&new->request);
                  if (new->request.ioa_Request.io_Flags & IOF_QUICK)
                     {
                     AddTail(&queue, new);
                     if (new->request.ioa_Request.io_Error)
                        {
                        owned &=~ msg2->data.info.channel_mask;
#ifdef EXTERNAL
                        printf("Lost: %d (%d) = %d\n", new->request.ioa_Request.io_Error, msg2->data.info.channel_mask, owned);
                        fflush(stdout);
#endif
                        }
                     }
                  else
                     AddTail(&pending, new);
                  }
               else
                  AddTail(&queue, new);
               }
#ifdef EXTERNAL
            else
               printf("No new\n");
#endif
            ReplyMsg(msg2);
            }        
         break;
      case TYPE_CHANGE:
         immediate->request.ioa_Period = msg->data.info.pitch;    
         immediate->request.ioa_Volume = msg->data.info.volume;
         send_immediate(ADCMD_PERVOL, msg->data.info.channel_mask);
         break;
      case TYPE_COMM:
      case TYPE_SYNC:
      case TYPE_SYNC_DO:
         /* nothing to do there */
         break;
      default:
         /* not implemented yet */
         break;
         }
      ReplyMsg(msg);
      get_requests();
      }
   }

   
void handle_audio(struct List *l, int signaled)
   {
   get_requests();
   }
