char *ScanInt(char *line, int *value);
int   ScanComments(char *line);
int   ParseGeomFile(FILE *f);
int   ParseFile(FILE *f, char *filetype);
int   ParseArray(char **lines, char *filetype);
