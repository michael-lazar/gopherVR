/* wb2cli.c */

#include <exec/exec.h>
#include <exec/ports.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <workbench/startup.h>
#include <proto/exec.h>
#include <proto/dos.h>

/* Undocumented DOS structure */

struct PathEntry
   {
   BPTR pe_Next;
   BPTR pe_Lock;
   };

/* to use local DOSBase */
#define DOSBase dosbase


/* WB2CLI, inspired by Mike Sinz idea */

LONG __asm WB2CLI(
   register __a0 struct WBStartup *wbmsg, 
   register __d0 ULONG defaultstack, 
   register __a1 struct DosLibrary *dosbase)
   {
   struct Process *this_task, *wbtask;
   struct CommandLineInterface *this_cli, *wbcli;
   struct MsgPort *wbport;
   LONG *this_path_pointer;
   struct PathEntry *wbentry, *new_entry;
   
      /* get the current task */
   this_task = (struct Process *)FindTask(0);

/* was:  this_task = (struct Process *)AbsExecBase->ThisTask; */

      /* if there is already a CLI, or we don't have a WBStartup, exit gracefully */
   this_cli = BADDR(this_task->pr_CLI);
   if (this_cli || !wbmsg)
      goto end;

      /*** 
       *** Add a new cli structure
       ***/

   this_cli = (struct CommandLineInterface *)AllocDosObject(DOS_CLI, NULL);
   if (!this_cli)
      goto end;
      /* link it into the current task (free it at end) */
   this_task->pr_CLI = MKBADDR(this_cli);
   this_task->pr_Flags |= PRF_FREECLI;
      /* set up default stack */
   this_cli->cli_DefaultStack = (defaultstack + 3) >>2;


   Forbid();
   
      /***
       ***     Find the workbench process
       ***/
      
      /* we check all the cases that could go bad */
   wbport = wbmsg->sm_Message.mn_ReplyPort;
   if (!wbport || wbport->mp_Flags & PF_ACTION)
      goto permit;
   wbtask = wbport->mp_SigTask;
   if (wbtask->pr_Task.tc_Node.ln_Type != NT_PROCESS || !wbtask->pr_CLI)
      goto permit;
   wbcli = BADDR(wbtask->pr_CLI);
      
      /* clone prompt if any */
   if (wbcli->cli_Prompt)
      SetPrompt(BADDR(wbcli->cli_Prompt + 1));

      /***
       ***     Clone the path
       ***/
       
      /* this strange way to go, together with MEMF_CLEAR,
       * ensures we don't have to take action in case anything goes bad
       */
   this_path_pointer = &(this_cli->cli_CommandDir);
   for (wbentry = BADDR(wbcli->cli_CommandDir); wbentry; wbentry = BADDR(wbentry->pe_Next))
      {
         /* note we HAVE to use AllocVec, since the path won't be freed by us */
      new_entry = AllocVec(sizeof(struct PathEntry), MEMF_CLEAR | MEMF_PUBLIC);
      if (!wbentry)
         goto permit;
      new_entry->pe_Lock = DupLock(wbentry->pe_Lock);
      if (!new_entry)
         {
         FreeVec(new_entry);
         goto permit;
         }
      *this_path_pointer = MKBADDR(new_entry);
      this_path_pointer = &(new_entry->pe_Next);
      }
      
permit:
   Permit();

end:
   return (LONG)this_cli;
   }
