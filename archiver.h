#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* External variables */
extern char *INDEXFILE;
extern char *ARCHIVE;
extern FILE *idx;
extern FILE *arch;

/* item defines a file in the index */
struct item {
	char *fileName;
	long *lengths;
};

/* Function signatures */

int archiveFile(char *file);

int writeToArchive(char *file);

/* Load all file entries from index file */
int readIndex();

/* Write an array of index files to index file */
int writeIndex(struct item *index);

/* Add or update one item in the index file */
int addToIndex(struct item item);

#endif
