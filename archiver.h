#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* External variables */
const char *INDEXFILE;
const char *ARCHIVE;
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

/* node is part of the key table */
struct node {
	char *content;
	struct node *next[57];
};

/* Function signatures */

/* Add a file to archive */
int archiveFile(char *file);

/* Load all file entries from index file */
int readIndex();

/* Write an array of index files to index file */
int writeIndex(struct index index);

/* Add or update one item in the index file */
int addToIndex(struct item item);

/* Read a file's content */
int grepIndex(char *arg);

int readArchive(long addr, long len);

int add(struct node *root, char *key, char *data);

char *get(struct node *root, char *key);

#endif
