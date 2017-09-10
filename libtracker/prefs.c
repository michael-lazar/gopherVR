/* prefs.c 
	vi:ts=3 sw=3:
 */
/* $Id: prefs.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $
 * $Log: prefs.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:22  lindner
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
 * Revision 4.6  1995/02/01  20:41:45  espie
 * Added color.
 *
 */

#include "defs.h"
#include "extern.h"
#include "prefs.h"
#include "tags.h"

ID("$Id: prefs.c,v 1.1.1.1 2002/01/18 16:34:22 lindner Exp $")
LOCAL void init_prefs P((void));

LOCAL void (*INIT)P((void)) = init_prefs;

LOCAL struct tag preferences[NUMBER_PREFS];

LOCAL void init_prefs()
   {
   int i;
   
   for (i = 0; i < NUMBER_PREFS; i++)
      preferences[i].type = BASE_PREFS + i;
   }



VALUE get_pref(index)
int index;
   {
   INIT_ONCE;

   return preferences[index-BASE_PREFS].data;
   }

void set_pref(index, value)
int index;
VALUE value;
   {
   preferences[index-BASE_PREFS].data = value;
   }

void set_pref_scalar(index, value)
int index;
int value;
   {
   VALUE temp;
   
   temp.scalar = value;
   set_pref(index, temp);
   }

int get_pref_scalar(index)
int index;
   {
   return get_pref(index).scalar;
   }

struct tag *get_prefs()
   {
   INIT_ONCE;

   return preferences;
   }
