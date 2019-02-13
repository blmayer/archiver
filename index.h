#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include <stdlib.h>

/* External variables */
extern char *INDEXFILE;
extern FILE *idx;

/* item defines a file in the index */
struct item {
	char *fileName;
	long *lengths;
};

/* Load all file entries from index file */
int readIndex();

/* Write an array of index files to index file */
int writeIndex(struct item *index);

/* Add or update one item in the index file */
int addToIndex(struct item item);

#endif
