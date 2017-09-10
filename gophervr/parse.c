#include "vogl.h"
#include <stdio.h>
#include "parse_geom.h"
#include "parse_nff.h"

/**********************************************************************
 * General Purpose Parsing routines.
 * 
 * This file abstracts most 3D file format parsing.
 * 
 * To add a new file type, write three functions:
 *        Parse{type}Init()      Initializes data structures
 *        Parse{type}Line(line)  Process One Line of read-only data
 *        Parse{type}Done()      Returns a long for callobj() calls
 *
 * Then add the MIME media-type and the above three functions to the
 * "Parsers" array..
 */

/**********************************************************************
 * General Utiity routines...
 */

char *ScanWhiteSpace(char *line) {
     while (*line == ' ' || *line == '\t' || *line == '\n' || line == '\0')
	  line++;
     return(line);
}

ScanTokens(char* line, char **tokens) {
     ;
}

/*
 * A comment is a line that is empty or starts with a circumflex
 */

int ScanComments(char *line) {
     line = ScanWhiteSpace(line);
     if (*line == '#' || *line == '\0')
	  return(1);
     else
	  return(0);
}

/*
 * Read an integer from the input stream, returns a pointer to the next
 * character in the stream.
 */

char *ScanInt(char *line, int *value) {
     if (line == NULL)
	  return(NULL);
     line = ScanWhiteSpace(line);

     *value = atoi(line);

     while (*line != ' ' && *line != '\t' && *line != '\n' && *line != '\0')
	  line++;
     return(line);
}

char *ScanFloat(char *line, int *value) {
     if (line == NULL)
	  return(NULL);
     line = ScanWhiteSpace(line);

     *value = atof(line);

     while (*line != ' ' && *line != '\t' && *line != '\n' && *line != '\0')
	  line++;
     return(line);
}


/**********************************************************************
 * A structure that defines how we parse file types
 */

typedef struct parsetype_struct {
     char *FileType;
     void (*InitFunc)(void);
     int  (*LineFunc)(char *);
     int  (*DoneFunc)(void);
} ParseType;

ParseType Parsers[3] = {
   {"world/geom",  ParseGeomInit, ParseGeomLine, ParseGeomDone},
   {"world/nff-spd", ParseNFFInit, ParseNFFLine, ParseNFFDone},
   NULL
   };



/**********************************************************************
 * Read from a file and run a Parser
 */

int
ParseGeomFile(FILE *f) 
{
     char buf[512];
     int result;

     ParseGeomInit();
     while (fgets(buf, sizeof(buf), f)) {
	  result = ParseGeomLine(buf);
	  if (result < 0)
	       return(-1);
     }
     return(ParseGeomDone());
}

			
/**********************************************************************
 * Try to run a specific parser...  Use table to look up correct functions
 */

int
ParseFile(FILE *f, char *filetype)
{
     int i;
     char buf[512];
     int result;

     for (i=0; ; i++) {
	  if (Parsers[i].FileType == NULL)
	       return -1;	/* Couldn't find filetype */
	  if (strcasecmp(Parsers[i].FileType, filetype) == 0) 
	       break;
     }

     /* Okay, we know the type of file, now lets do it.. */

     Parsers[i].InitFunc();
     while (fgets(buf, sizeof(buf), f)) {
	  result = Parsers[i].LineFunc(buf);
	  if (result < 0)
	       return(-1);
     }
     return(Parsers[i].DoneFunc());
}

/**********************************************************************
 * Parse an array of character strings, useful for embedded objects
 */

int
ParseArray(char **lines, char *filetype)
{
     int i;
     int result;
     int lnum = 0;

     for (i=0; ; i++) {
	  if (Parsers[i].FileType == NULL)
	       return -1;	/* Couldn't find filetype */
	  if (strcasecmp(Parsers[i].FileType, filetype) == 0) 
	       break;
     }

     /* Okay, we know the type of file, now lets do it.. */

     Parsers[i].InitFunc();
     while (lines[lnum] != NULL) {
	  result = Parsers[i].LineFunc(lines[lnum++]);
	  if (result < 0)
	       return(-1);
     }
     return(Parsers[i].DoneFunc());
}
