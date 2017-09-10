/* requester.c 
	vi:ts=3 sw=3:
 */

/* $Id: requester.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: requester.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.7  1995/02/14  16:51:22  espie
 * *** empty log message ***
 *
 * Revision 1.6  1995/01/13  13:31:35  espie
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/09  04:49:18  Espie
 * File requester !
 */

#include <libraries/reqtools.h>
#include <proto/reqtools.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>

#include "defs.h"
#include "extern.h"
#include "amiga/amiga.h"

ID("$Id: requester.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $")
LOCAL void init_requester(void);

LOCAL struct ReqToolsBase *ReqToolsBase = 0;

LOCAL void (*INIT)(void) = init_requester;

LOCAL struct rtFileRequester *req = 0;
LOCAL struct rtHandlerInfo *myhandler = 0;



LOCAL void close_requester()
   {
   if (myhandler)
      rtReqHandler(myhandler, 0, RTRH_EndRequest, REQ_CANCEL, TAG_END);
   if (req)
      rtFreeRequest(req);
   if (ReqToolsBase)
      CloseLibrary(ReqToolsBase);
   myhandler = 0;
   req = 0;
   ReqToolsBase = 0;
   }

LOCAL void init_requester()
   {
   ReqToolsBase = OpenLibrary("reqtools.library", 38);
   if (!ReqToolsBase)
      {
      requested_file(0);
      return;
      }
   at_end(close_requester);
   req = rtAllocRequestA(RT_FILEREQ, NULL);
   }

LOCAL char fname[108];

void handle_requester(ULONG received)
   {   
   struct amiganame *new;
   int total;
   struct rtFileList *flist, *sweep;
   struct MinList temp;
   int result;

   if ( (result = rtReqHandler(myhandler, received, TAG_END)) == CALL_HANDLER)
      return;
   remove_req_handler();
   myhandler = 0;
   flist = (struct rtFileList *)result;

   if (flist)
      {
      NewList(&temp);      /* use temp as a `place holder' */
      sweep = flist;
      while(sweep)
         {
         total = strlen(req->Dir) + strlen(sweep->Name) + 2;
         new = malloc(sizeof(struct amiganame) + total);
         if (!new)
            continue;
         AddPart(strcpy(new->s, req->Dir), sweep->Name, total);
         new->i = TRUE;
         AddTail(&temp, new);
         sweep = sweep->Next;
         }
      rtFreeFileList(flist);
      
      if (temp.mlh_TailPred != &temp)
         {
         /* unscrew the first element of temp out and loop the elements */

         new = temp.mlh_Head;
         new->n.mln_Pred = temp.mlh_TailPred;
         temp.mlh_TailPred->mln_Succ = new;
   
         requested_file(new);
         }
      else
         requested_file(0);
      }
   else
      requested_file(0);
   }
   
void launch_requester()
   {
   INIT_ONCE;
   
   if (myhandler || !req)
      return;
   if (CALL_HANDLER != (int)rtFileRequest(req, fname, "Load song", 
      RTFI_Flags, FREQF_MULTISELECT,
      RT_ReqHandler, &myhandler, TAG_END))
      {
      myhandler = 0;
      return;
      }
   install_req_handler(myhandler->WaitMask, handle_requester);
   if (myhandler->DoNotWait)
      handle_requester(0);
   }

