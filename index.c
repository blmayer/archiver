#include "archiver.h"
#include <unistd.h>

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
		do {
			c = fgetc(idx);
			if (c < 0) {
				exit(0);
			}
			printf("%c", c);
		} while (c);

		/* Discard first and second values */
		fseek(idx, 10, SEEK_CUR);

		/* Read now to get length */
		fread(&fileLen, sizeof(long), 1, idx);
		printf("\tlength: %d bytes\n", fileLen);
	}

	exit(0);
}

int grepIndex(char *name)
{
	idx = fopen(INDEXFILE, "rb");
	if (!idx) {
		puts("Error opening index file");
		return -1;
	}

	/* Read the whole index searching for name */
	// FIXME: Loop is not ok
	long addr, len = 0;
	char c;
	while ((c = fgetc(idx)) > -1) {
		/* Check if the file name matches */
		while (c > 0) {
			if (c == *name++) {
				/* File was found, save content location */
				puts(" file found");
				fseek(idx, 2, SEEK_CUR);
				fread(&addr, sizeof(long), 1, idx);
				fread(&len, sizeof(long), 1, idx);
				break;
			} else {
				/* jump to next file */
				while (fgetc(idx) > 0) {
					;
				}
				fseek(idx, 18, SEEK_CUR);
			}

			c = fgetc(idx);
		}
	}

	/* Print file contents if found */
	if (len > 0) {
		readArchive(addr, len);
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
