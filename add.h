#ifndef ADD_H
#define ADD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* External variables */
extern char *INDEXFILE;
extern char *ARCHIVE;
extern FILE *idx;
extern FILE *arch;

int archiveFile(char *file);

int writeToArchive(char *file);
#endif
