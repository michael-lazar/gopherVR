/* amiga/popen.c 
	vi:ts=3 sw=3:
 */

/* $Id: popen.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: popen.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.8  1995/02/14  16:51:22  espie
 * *** empty log message ***
 *
 * Revision 1.7  1995/01/13  13:31:35  espie
 * *** empty log message ***
 *
 * Revision 1.5  1994/01/07  15:08:54  Espie
 * Maybe correct code now...
 */


#include <proto/dos.h>
#include <proto/exec.h>
#include <exec/tasks.h>
#include <dos/dostags.h>
#include "defs.h"
ID("$Id: popen.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $")

/*
###   CSupport/popen
###
###   NAME
###      popen/pclose -- Unix-like pipes
###
###   STATUS
###      Experimental
###      does not work with csh !
###
 */
FILE *popen(char *command, char *mode)
   {
   static char pname[25];
   struct Task *me = FindTask(0);
   static count = 0;
   
   count++;
   
      /* guarantees a unique pipe name ! */
   sprintf(pname, "pipe:tr_%lx_%d", me, count);
   
   if (strcmp(mode, "r") == 0)
      /* open pipe for reading */
      {
      FILE *reader;
      BPTR writer, null;

      writer = Open(pname, MODE_NEWFILE);
      reader = fopen(pname, "r");
      null = Open("NIL:", MODE_NEWFILE);
      if (SystemTags(command, SYS_Input, null, 
         SYS_Output, writer, SYS_Asynch, TRUE, 
         NP_StackSize, me->tc_SPUpper - me->tc_SPLower,
         TAG_END) == -1)
         {
         Close(null);
         Close(writer);
         fclose(reader);
         return NULL;
         }
      else
         return reader;
      }
   else if (strcmp(mode, "w") == 0)
      /* open pipe for writing */
      {
      FILE *writer;
      BPTR reader, null;
      
      writer = fopen(pname, "w");
      reader = Open(pname, MODE_OLDFILE);
      null = Open("NIL:", MODE_NEWFILE);
      if (SystemTags(command, SYS_Input, reader, 
         SYS_Output, null, SYS_Asynch, TRUE, 
         NP_StackSize, me->tc_SPUpper - me->tc_SPLower, 
         TAG_END) == -1)
         {
         Close(null);
         Close(reader);
         fclose(writer);
         return NULL;
         }
      else
         return writer;
      }
   else
      return NULL;
   }

/* for us, pclose is just fclose.
 * But we have to insure the file is empty first
 */
void pclose(FILE *f)
   {
   while (fgetc(f) != EOF)
      ;
   fclose(f);
   }

