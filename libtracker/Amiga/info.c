/* amiga/info.c
	vi:ts=3 sw=3:
 */

/* $Id: info.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $
 * $Log: info.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:24  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:39  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 1.2  1995/02/14  16:51:22  espie
 * *** empty log message ***
 *
 * Revision 1.1  1995/01/13  13:31:35  espie
 * Initial revision
 *
 */

#include <proto/exec.h>

#include "defs.h"
#include "extern.h"
#include "amiga/amiga.h"

ID("$Id: info.c,v 1.1.1.1 2002/01/18 16:34:24 lindner Exp $")


/***
 ***
 ***		Info window handling
 ***
 ***/

/* We chose the easy way: outputting everything in a console window
 * on the fly. An interesting improvement would be to buffer everything
 * and open the window with the right size afterwards
 */
struct handle
   {
   FILE *file;
   int linecount;
   int maxlength;
   int currentlength;
   };

#define H(h, field)  ( ((struct handle *)h)->field )

void *begin_info(char *title)
   {
   struct handle *new;
   
   char buffer[50];
   
   new = malloc(sizeof(struct handle));
   if (!new)
      return 0;
   sprintf(buffer, "CON:////%s/auto/close/wait", title);
   new->file=fopen(buffer, "w");
   if (!new->file)
      {
      free(new);
      return 0;
      }
   new->linecount = 0;
   new->maxlength = 0;
   new->currentlength = 0;
   return new;
   }

void infos(void *handle, char *s)
   {
   if (handle)
      {
      fprintf( H(handle,file), s);
      H(handle, currentlength) += strlen(s);
      }
   }

void info(void *handle, char *line)
   {
   infos(handle, line);
   if (handle)
      {
      fputc('\n', H(handle, file));
      if ( H(handle, currentlength) > H(handle, maxlength) )
         H(handle, maxlength) = H(handle, currentlength);
      H(handle, linecount)++;
      }
   }

void end_info(void *handle)
   {
   if (handle)
      {
      
      fclose(H(handle, file));
      free(handle);
      }
   }

