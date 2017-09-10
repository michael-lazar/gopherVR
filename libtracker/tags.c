/* tags.c 
	vi:ts=3 sw=3:
 */

/* $Id: tags.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $
 * $Log: tags.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.11  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 */

#include "defs.h"

#include "tags.h"

ID("$Id: tags.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")

#if 0
unsigned long tags_length(t)
struct tag *t;
   {
   unsigned long i;

   i = 0;

   for (;;)
      {
      switch(t->type)
         {
      case TAG_END:
         return i;
      case TAG_IGNORE:
         t++;
         break;
      case TAG_SKIP:
         t += t->data.scalar;
         break;
      case TAG_SUB:
         i += tags_length(t->data.pointer);
         break;
      case TAG_JUMP:
         i += tags_length(t->data.pointer);
         return i;
      default:
         i++;
         }
      }
   }

LOCAL struct tag *copy_scan(dest, orig)
struct tag *dest;
struct tag *orig;
   {
   switch (orig->type)
      {
   case TAG_END:
      return dest;
   case TAG_IGNORE:
      ++orig;
      break;
   case TAG_SKIP:
      orig += orig->data.scalar;
      break;
   case TAG_SUB:
      dest = copy_scan(dest, orig->data.pointer);
      break;
   case TAG_JUMP:
      orig = orig->data.pointer;
      break;
   default:
      *dest++ = *orig++;
      }
   return copy_scan(dest, orig);
   }

struct tag *tags_copy(t)
struct tag *t;
   {
   unsigned long i;
   struct tag *c, *e;
   
   i = tags_length(t);
   c = (struct tag *)malloc(sizeof(struct tag) * (i+1));
   e = copy_scan(c, t);
   e->type = TAG_END;
   return c;
   }

struct tag *alloc_tags(l)
unsigned long l;
   {
   struct tag *c;
   unsigned long i;
   
   c = ( struct tag *) malloc(sizeof(struct tag) * (l + 1));
   if (!c)
      return 0;
   for (i = 0; i < l; i++)
      c[i].type = TAG_IGNORE;
   c[l].type = TAG_END;
   return c;
   }

#endif
/* WARNING: TAG_SUB is not supported */
struct tag *get_tag(t)
struct tag *t;
   {
   forever
      {
      switch (t->type)
         {
      case TAG_END:
         return 0;
      case TAG_IGNORE:
         t++;
         return get_tag(t);
      case TAG_SKIP:
         t += t->data.scalar;
         break;
      case TAG_JUMP:
         t = t->data.pointer;
         break;
      default:
         return t;
         }
      }
   }
      
