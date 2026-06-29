#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Page size for streaming archive / file I/O (must not require full file in RAM) */
#define PAGE_SIZE 4096

/* Minimum common sequence length worth pointing at instead of storing again */
#define MIN_MATCH 32

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

/*
 * item defines a file in the index.
 * lengths holds n_longs values: addr0, len0, addr1, len1, ...
 * (pairs of archive offset + length for each sequence used by the file)
 */
struct item {
	char *fileName;
	long *lengths;
	int n_longs;
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

/* Add a key-value pair to a map */
int add(struct node *root, char *key, char *data);

/* Retrieve the value from map with a key */
char *get(struct node *root, char *key);

#endif
