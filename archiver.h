#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* External variables */
char *INDEXFILE;
char *ARCHIVE;
FILE *idx;
FILE *arch;

/* index is the structure in the index file */
struct index {
	int len;
	struct item *items;
};

/* item defines a file in the index */
struct item {
	char *fileName;
	long *lengths;
};

/* Function signatures */

int archiveFile(char *file);

/* Load all file entries from index file */
int readIndex();

/* Write an array of index files to index file */
// int writeIndex(struct item index[]);
int writeIndex(struct index index);

/* Add or update one item in the index file */
int addToIndex(struct item item);

#endif
