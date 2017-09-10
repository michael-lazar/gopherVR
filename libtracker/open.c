/* open.c 
	vi:ts=3 sw=3:
 */

/* Magic open file: path lookup and transparent decompression */

/* $Id: open.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $ 
 * $Log: open.c,v $
 * Revision 1.1.1.1  2002/01/18 16:34:21  lindner
 * Slightly modded sources for gophervr...
 *
 * Revision 1.1.1.1  1995/06/19  22:48:38  lindner
 * Initial GopherVR combined source modules
 *
 * Revision 4.15  1995/02/21  21:13:16  espie
 * Cleaned up source. Moved minor pieces of code around.
 *
 * Revision 4.14  1995/02/21  17:54:32  espie
 * Internal problem: buggy RCS. Fixed logs.
 *
 * Revision 4.9  1995/02/01  20:41:45  espie
 * Added color.
 *
 * Revision 4.8  1995/02/01  16:39:04  espie
 * Cleaned up rewind stuff.
 * Fixed serious bug with read_file.
 *
 * Revision 4.7  1995/01/30  18:15:01  espie
 * Implemented buffer for open_file and such !
 *
 *
 * Revision 4.2  1994/07/04  14:00:06  espie
 * External compression methods.
 * A little more abstract, should work better
 * Better amiga patterns.
 * Amiga support.
 * Added gzip/shorten.
 * restore stty.
 *
 * Revision 1.5  1992/11/01  13:10:06  espie
 * Cleaned up path handler, and some more bugs.
 * Check for size now.
 * Added path support. Transparent interface. We look up through the file
 * list, which is small anyway.
 */

#include "defs.h"

#include <ctype.h>

#include "extern.h"

ID("$Id: open.c,v 1.1.1.1 2002/01/18 16:34:21 lindner Exp $")


/* forward declarations */
LOCAL struct exfile *do_open(struct exfile *file, char *fname, char *path);



/***
 *** 	Stuff for compression methods 
 ***/

/* Max buffer length for reading compression methods */
#define MAX_LENGTH 90


/* automaton */
#define END_OF_LINE 0
#define BEGIN_OF_LINE 1
#define IN_SPEC 2
#define BEGIN_OF_COMMAND 3
#define IN_COMMAND 4


LOCAL void init_compression P((void));
LOCAL void (*INIT)P((void)) = init_compression;
extern int error;



/*** extended file structure:
 ***		designed to be able to rewind pipes on a small length	(BUFSIZE) 
 ***		in order to be able to retry lots of formats for cheap
 ***/
#define BUFSIZE 15000
struct exfile
   {
   FILE *handle;							/* the real Mc Coy */
	unsigned char buffer[BUFSIZE];	/* we buffer only the file beginning */
	int length;								/* the length read in the buffer */
	int pos;									/* current pos in the buffer */

		/* OO methods */
   void (*close)P((struct exfile *f));	
   void (*rewind)P((struct exfile *f));
	int (*read)P((void *ptr, int size, int nitems, struct exfile *f));
   int (*getcar)P((struct exfile *f));
   int (*tell)P((struct exfile *f));

		/* kludge to reopen file */
	char *name;
	char *path;
   };

/***
 *** 	 the methods for buffered files 
 ***/
LOCAL int do_getchar(f)
struct exfile *f;
   {
   int c;

	if (f->pos < BUFSIZE)
		{
		if (f->pos >= f->length)
			{
			error = FILE_TOO_SHORT;
			return EOF;
			}
		else
			return f->buffer[f->pos++];
		}
   if ((c = fgetc(f->handle)) == EOF)
      error = FILE_TOO_SHORT;
   else
      f->pos++;
   return c;
   }

LOCAL int do_read(p, s, n, f)
void *p;
int s, n;
struct exfile *f;
	{
	int total = s * n;
	if (f->pos < BUFSIZE)
		{
		int remaining = f->length - f->pos;
		if (remaining >= total)
			{
			memcpy(p, &(f->buffer[f->pos]), total);
			f->pos += total;
			return n;
			}
		else
			{
			memcpy(p, &(f->buffer[f->pos]), remaining);
			total = remaining + 
					fread((char *)p+remaining, 1, total - remaining, f->handle);
			f->pos += total;
			return total/s;
			}
		}
	else
		{
		total = fread(p, s, n, f->handle);
		f->pos += total * s;
		return total;
		}
	}


LOCAL void do_rewind(f)
struct exfile *f;
	{
	if (f->pos <= f->length)
		f->pos = 0;
	else
		{
		(*f->close)(f);
		f = do_open(f, f->name, f->path);
		}
	}

LOCAL int do_tell(f)
struct exfile *f;
   {
   return f->pos;
   }

LOCAL void do_pclose(f)
struct exfile *f;
   {
   pclose(f->handle);
   }

LOCAL void do_fclose(f)
struct exfile *f;
   {
   fclose(f->handle);
   }

LOCAL struct exfile *init_buffered(f)
struct exfile *f;
	{
	f->length = fread(f->buffer, 1, BUFSIZE, f->handle);
	f->getcar = do_getchar;
	f->tell = do_tell;
	f->read = do_read;
	f->rewind = do_rewind;
	f->pos = 0;
	return f;
	}
  

/***
 ***	Compression methods database handling
 ***/

/* compression methods we do know about.
 * Important restriction: for the time being, the output
 * must be a single module.
 */
struct compression_method
   {
   struct compression_method *next;
   char *extension;
   char *command;
   };

LOCAL struct compression_method *read_method(f)
FILE *f;
   {
   static char buffer[MAX_LENGTH + 1];
   
   while (fgets(buffer, MAX_LENGTH, f))
      {
      int state = BEGIN_OF_LINE;
      int start, i;
      char *spec = NULL, *command = NULL;
      
      for (i = 0; state != END_OF_LINE; i++)
         {
         switch(state)
            {
         case BEGIN_OF_LINE:
            switch(buffer[i])
               {
            case ' ':
            case '\t':
               break;
            case 0:
            case '\n':
            case '#':
               state = END_OF_LINE;
               break;
            default:
               start = i;
               state = IN_SPEC;
               }
            break;
         case IN_SPEC:
            switch(buffer[i])
               {
            case ' ':
            case '\t':
               spec = malloc(i - start + 1);
               strncpy(spec, buffer + start, i - start);
               spec[i - start] = 0;
               state = BEGIN_OF_COMMAND;
               break;
            case 0:
            case '\n':
               state = END_OF_LINE;
               break;
            default:
               break;
               }
            break;
         case BEGIN_OF_COMMAND:
            switch (buffer[i])
               {
            case ' ':
            case '\t':
               break;
            case 0:
            case '\n':
               state = END_OF_LINE;
               free(spec);
               break;
            default:
               state = IN_COMMAND;
               start = i;
               }
            break;
         case IN_COMMAND:
            switch (buffer[i])
               {
            case 0:
            case '\n':
               command = malloc(i - start + 1);
               strncpy(command, buffer + start, i - start);
               command[i-start] = 0;
               state = END_OF_LINE;
            default:
               break;
               }
            }
         }      
      if (command && spec)
         {
         struct compression_method *new;
         
         new = malloc(sizeof(struct compression_method));
         new->next = 0;
         new->extension = spec;
         new->command = command;
         return new;
         }
      }
   return 0;
   }
      

LOCAL struct compression_method **read_methods(previous, f)
struct compression_method **previous;
FILE *f;
   {
   struct compression_method *method;
   
   if (f)
      {
      while (method = read_method(f))
         {
         *previous = method;
         previous = &(method->next);
         }
      fclose(f);
      }
   return previous;
   }
      

LOCAL struct compression_method *comp_list;

LOCAL void init_compression()
   {
   char *fname;
   FILE *f;
   struct compression_method **previous;

   f = 0;
   fname = getenv("TRACKER_COMPRESSION");
   if (fname)
      f = fopen(fname, "r");
   if (!f)
      {
      fname = COMPRESSION_FILE;
      f = fopen(fname, "r");
      }
   previous = read_methods(&comp_list, f);
   }
      
/* Handling extensions.
 */
LOCAL int check_ext(s, ext)
char *s, *ext;
   {
   int ext_len, s_len;
   char *c;

   ext_len = strlen(ext);
   s_len = strlen(s);
   if (s_len < ext_len)
      return FALSE;
   for (c = s + s_len - ext_len; *c; c++, ext++)
      if (tolower(*c) != tolower(*ext))
         return FALSE;
   return TRUE;
   }

LOCAL int exist_file(fname)
char *fname;
   {
   FILE *temp;

   temp = fopen(fname, READ_ONLY);
   if (temp)
      {
      fclose(temp);
      return TRUE;
      }
   else
      return FALSE;
   }

#ifndef MAXPATHLEN
#define MAXPATHLEN 350
#endif

/* note that find_file returns a STATIC value */
LOCAL char *find_file(fname, path)
char *fname;
char *path;
   {
   char *sep;
   static char buffer[MAXPATHLEN];
   int len;

      /* first, check the current directory */
   if (exist_file(fname))
      return fname;
   while(path)
      {
      sep = strchr(path, ':');
      if (sep)
         len = sep - path;
      else
         len = strlen(path);
      if (len < MAXPATHLEN)
         {
         strncpy(buffer, path, len);
         buffer[len] = '/';
         if (len + strlen(fname) < MAXPATHLEN - 5)
            {
            strcpy(buffer + len + 1, fname);
            puts(buffer);
            if (exist_file(buffer))
               return buffer;
            }
         }
      if (sep)
         path = sep + 1;
      else
			return NULL;
      }
	return NULL;
   }

/* opening a file is bigger than it seems (much bigger) */
LOCAL struct exfile *do_open(file, fname, path)
struct exfile *file;
char *fname;
char *path;
   {
   struct compression_method *comp;

   INIT_ONCE;
    
	file->name = fname;
	file->path = path;

   fname = find_file(fname, path);
   if (!fname)
      goto not_opened;
         /* check for extension */
   for (comp = comp_list; comp; comp = comp->next)
      if (check_ext(fname, comp->extension))
         {
         char *pipe;
         
         pipe = malloc(strlen(comp->command) + strlen(fname) + 25);
         if (!pipe)
            goto not_opened;

         sprintf(pipe, comp->command, fname);
         file->close = do_pclose;
         file->handle = popen(pipe, READ_ONLY);
         free(pipe);
         if (file->handle)
            return init_buffered(file);
         else
            goto not_opened;
         }
   file->close = do_fclose;
   if (file->handle = fopen(fname, READ_ONLY))
      return init_buffered(file);

not_opened:
   free(file);
   return NULL;
   }




/***
 ***	The stubs that call the actual methods
 ***/


int getc_file(f)
struct exfile *f;
   {
   return (*f->getcar)(f);
   }

int read_file(p, s, n, f)
void *p;
int s, n;
struct exfile *f;
	{
	return (*f->read)(p, s, n, f);
	}

int tell_file(f)
struct exfile *f;
   {
   return (*f->tell)(f);
   }

void rewind_file(f)
struct exfile *f;
	{
	(*f->rewind)(f);
	}

struct exfile *open_file(fname, mode, path)
char *fname;
char *mode; /* right now, only mode "r" is supported */
char *path; 
	{
	struct exfile *new;

   if (mode[0] != 'r' || mode[1] != 0)
      return NULL;
	new = (struct exfile *)malloc(sizeof(struct exfile));
   if (new)
		return do_open(new, fname, path);
	else
      return NULL;
	}


void close_file(file)
struct exfile *file;
   {
	if (file)
		{
		(*file->close)(file);
		free(file);
		}
   }

