#include "archiver.h"

/*
 * Index file format
 *
 * This index is a text file using the format:
 * path/to/file1\0numOfAddrs[2]addr1len1addr2len2...
 * path/to/file1\0numOfAddrs[2]addr1len1addr2len2...
 * where addr is the position in the archive file
 */

int readIndex()
{
	idx = fopen(INDEXFILE, "rb");
	if (!idx) {
		puts("Error opening index file");
		return -1;
	}

	/* Read the whole index */
	short fileLen;
	char c;
	for (;;) {
		/* Print the file name */
		while ((c = fgetc(idx)) != 0) {
			printf("%c", c);
		}
		puts("");

		/* Discard first and second values */
		fseek(idx, 6, SEEK_CUR);

		/* Read now to get length */
		fread(&fileLen, sizeof(long), 1, idx);
		printf(" length: %d bytes\n", fileLen);
	}

	/* Close all files */
	fclose(idx);

	return 0;
}

int grepIndex(char *name)
{
	idx = fopen(INDEXFILE, "rb");
	if (!idx) {
		puts("Error opening index file");
		return -1;
	}

	/* Read the whole index searching for name */
	long addr, len = 0;
	char c;
	for (;;) {
		/* Check if the file name matches */
		while ((c = fgetc(idx)) == *name++) {
			if (c == 0) {
				/* File was found, print its contents */
				fseek(idx, 2, SEEK_CUR);
				fread(&addr, sizeof(long), 1, idx);
				fread(&len, sizeof(long), 1, idx);

				// TODO: Read archive at given add and print
			}
		}

		/* Break if reached EOF */
		if (c < 0) {
			break;
		}

		/* jump to next file */
		while (fgetc(idx) > 0) {
			;
		}
		fseek(idx, 10, SEEK_CUR);
	}

	/* Close all files */
	fclose(idx);

	return 0;
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
// 			fwrite((index + i)->lengths + n, sizeof(long), 1,
// idx);
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

	/* Write the file name */
	fwrite(item.fileName, strlen(item.fileName) + 1, 1, idx);

	/* Write the number of addresses */
	// For now it's only 2
	// short len = 0;
	// for (long *ind = item.lengths; ind != NULL; ind++) {
	// 	len++;
	// }
	short temp = 2;
	fwrite(&temp, sizeof(short), 1, idx);
	printf("addrs lens: %ld %ld\n", item.lengths[0], item.lengths[1]);

	/* Write address intervals */
	// For now there are just 2
	// for (int n = 0; n < len; n++) {
	// 	fwrite(item.lengths + n, sizeof(long), 1, idx);
	// }
	fwrite(&item.lengths[0], sizeof(long), 1, idx);
	fwrite(&item.lengths[1], sizeof(long), 1, idx);

	fclose(idx);

	return 0;
}
