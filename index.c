#include "index.h"

int readIndex()
{
	idx = fopen(INDEXFILE, "a+b");
	if (!idx) {
		puts("Error opening index file");
		exit(-1);
	}

	/*
	 * Write file to index
	 *
	 * This index is a text file using the format:
	 * numOfFiles[2]
	 * numOfAddrs[2]pathLen[2]path/to/file1addr1len1addr2len2...
	 * numOfAddrs[2]pathLen[2]path/to/file2addr1len1addr2len2...
	 * where addr is the position in the archive file
	 */

	/* Allocate the array of items */
	short entries, num, len;
	fscanf(idx, "%hd\n", &entries);
	struct item *index = malloc(entries * sizeof(struct item));

	/* Read the whole index */
	for (short i = 0; i < entries; i++) {
		/* Read lengths */
		fread(&num, 2, 1, idx);
		fread(&len, 2, 1, idx);

		/* Allocate name and array for intervals */
		struct item indexItem;
		indexItem.fileName = malloc(len + 1);
		indexItem.lengths = malloc(num * sizeof(long));

		/* Read the file name */
		fread(indexItem.fileName, len, 1, idx);

		/* Read address intervals */
		for (int i = 0; i < num; i++) {
			fread(&indexItem.lengths[i], sizeof(long), 1, idx);
		}

		/* Place item in the array */
		index[i] = indexItem;
	}

	/* Close all files */
	fclose(idx);
}

int writeIndex(struct item *index)
{
	return 0;
}

int addToIndex(struct item item)
{
	return 0;
}
