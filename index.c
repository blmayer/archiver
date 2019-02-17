#include "archiver.h"

/*
 * Index file format
 *
 * This index is a text file using the format:
 * numOfFiles[4]
 * pathLen[2]path/to/file1numOfAddrs[2]addr1len1addr2len2...
 * pathLen[2]path/to/file1numOfAddrs[2]addr1len1addr2len2...
 * where addr is the position in the archive file
 */

struct index readIndex()
{
	idx = fopen(INDEXFILE, "rb");
	if (!idx) {
		puts("Error opening index file");
		exit(-1);
	}

	/* Allocate the index structure */
	static struct index fileIndex;
	fscanf(idx, "%d\n", &fileIndex.len);

	/* The second element is an array of length len */
	fileIndex.items = malloc(fileIndex.len * sizeof(struct item));

	/* Read the whole index */
	for (int i = 0; i < fileIndex.len; i++) {
		/* Read lengths */
		short num, len;
		fread(&len, 2, 1, idx);
		fread(&num, 2, 1, idx);

		/* Allocate name and array for intervals */
		fileIndex.items[i].fileName = malloc(len + 1);
		fileIndex.items[i].lengths = malloc(num * sizeof(long));

		/* Read the file name */
		fread(fileIndex.items[i].fileName, len, 1, idx);

		/* Read address intervals */
		for (short j = 0; j < num; j++) {
			fread(&fileIndex.items[i].lengths[j], sizeof(long), 1,
			      idx);
		}
	}

	/* Close all files */
	fclose(idx);

	return fileIndex;
}

// int writeIndex(struct item *index)
// {
// 	idx = fopen(INDEXFILE, "wb");
// 	if (!idx) {
// 		puts("Error opening index file");
// 		exit(-1);
// 	}

// 	/* Discover length of index */
// 	short entries = 0, len = 0;
// 	for (struct item *ind = index; ind; ind++) {
// 		entries++;
// 	}

// 	/* Write item length */
// 	fprintf(idx, "%hd\n", entries);

// 	/* Read the whole index */
// 	for (short i = 0; i < entries; i++) {
// 		/* Get list of addresses length */
// 		for (short ind = 0; (index + i)->lengths; ind++) {
// 			len++;
// 		}

// 		fwrite(&len, 2, 1, idx);
// 		int nameLen = strlen((index + 1)->fileName);
// 		fwrite(&nameLen, 2, 1, idx);

// 		/* Write address intervals */
// 		for (int n = 0; n < len; n++) {
// 			fwrite((index + i)->lengths + n, sizeof(long), 1, idx);
// 		}
// 	}

// 	/* Close index file */
// 	fclose(idx);

// 	return entries;
// }

int writeIndex(struct index index)
{
	idx = fopen(INDEXFILE, "wb");
	if (!idx) {
		puts("Error opening index file");
		exit(-1);
	}

	/* Write whole index to file */
	return fwrite(&index, sizeof(struct index), 1, idx);
}

int addToIndex(struct item item)
{
	idx = fopen(INDEXFILE, "ab");
	if (!idx) {
		puts("Error opening index file");
		exit(-1);
	}

	/* Write length of the file name */
	short nameLen = (short)strlen(item.fileName);
	fwrite(&nameLen, 2, 1, idx);

	/* Write the file name */
	fwrite(item.fileName, strlen(item.fileName), 1, idx);

	/* Write the number of addresses */
	// For now it's only 2
	// short len = 0;
	// for (long *ind = item.lengths; ind != NULL; ind++) {
	// 	len++;
	// }
	short temp = 2;
	fwrite(&temp, 2, 1, idx);
	printf("addrs lens: %ld %ld\n", item.lengths[0], item.lengths[1]);

	/* Write address intervals */
	// For now there are just 2
	// for (int n = 0; n < len; n++) {
	// 	fwrite(item.lengths + n, sizeof(long), 1, idx);
	// }
	fwrite(&item.lengths[0], 2, 1, idx);
	fwrite(&item.lengths[1], 2, 1, idx);

	fclose(idx);

	return 0;
}
