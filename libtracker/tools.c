/* tools.c 
	vi:ts=3 sw=3:
 */

/* standard routines for use in tracker. Used to be in main.c
 */

/* $Id: tools.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: tools.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.12  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 */
     

#include "defs.h"
#include "extern.h"
     
ID("$Id: tools.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")


/* v = read_env(name, default): read the scalar value v in the environment, 
 * supply default value.
 */
int read_env(name, def)
char *name;
int def;
   {
   char *var;
   int value;

   var = getenv(name);
   if (!var)
      return def;
   if (sscanf(var, "%d", &value) == 1)
      return value;
   else
      return def;
   }

