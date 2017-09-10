void ParseGeomInit(void);	/* Get ready to start geom parsing */
int  ParseGeomLine(char *line);	/* Process one line, -1 if an error.. */
int  ParseGeomDone(void);	/* Returns obj for callobj() */
