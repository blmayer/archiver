#include "archiver.h"
#include <unistd.h>

/*
 * Index file format
 *
 * This index is a text file using the format:
 * path/to/file1\0numOfAddrs[2]addr1len1addr2len2...
 * path/to/file1\0numOfAddrs[2]addr1len1addr2len2...
 * where addr is the position in the archive file and each len is how many
 * bytes to read from that position.  numOfAddrs is a short counting how many
 * long values follow (always even: pairs of addr, len).
 *
 * When a file shares sequences with existing archive content, multiple
 * (addr, len) pairs point at those sequences instead of storing the bytes
 * again.
 */

int readIndex()
{
	idx = fopen(INDEXFILE, "rb");
	if (!idx) {
		puts("Error opening index file");
		return -1;
	}

	for (;;) {
		int c = fgetc(idx);
		if (c < 0)
			break;

		/* Print the file name */
		while (c > 0) {
			printf("%c", c);
			c = fgetc(idx);
		}
		if (c < 0)
			break;

		short n_longs;
		if (fread(&n_longs, sizeof(short), 1, idx) != 1)
			break;

		long total = 0;
		for (short i = 0; i < n_longs; i += 2) {
			long addr, len;
			if (fread(&addr, sizeof(long), 1, idx) != 1)
				break;
			if (fread(&len, sizeof(long), 1, idx) != 1)
				break;
			total += len;
		}
		printf("\tlength: %ld bytes (%d segments)\n", total,
		       n_longs / 2);
	}

	fclose(idx);
	return 0;
}

/* Return 1 if stored path matches the user argument. */
static int name_matches(const char *stored, const char *arg)
{
	if (strcmp(stored, arg) == 0)
		return 1;

	/* Allow get with a realpath if the file still exists on disk. */
	char *rp = realpath(arg, NULL);
	if (rp) {
		int ok = strcmp(stored, rp) == 0;
		free(rp);
		if (ok)
			return 1;
	}

	/* Allow matching by suffix (e.g. basename). */
	size_t sl = strlen(stored);
	size_t al = strlen(arg);
	if (al > 0 && sl >= al && strcmp(stored + sl - al, arg) == 0) {
		if (sl == al || stored[sl - al - 1] == '/')
			return 1;
	}
	return 0;
}

int grepIndex(char *name)
{
	idx = fopen(INDEXFILE, "rb");
	if (!idx) {
		puts("Error opening index file");
		return -1;
	}

	char fname[4096];
	int found = 0;

	while (!found) {
		/* Read null-terminated file name */
		size_t fi = 0;
		int c;
		while ((c = fgetc(idx)) > 0) {
			if (fi + 1 < sizeof(fname))
				fname[fi++] = (char)c;
		}
		if (c < 0)
			break;
		fname[fi] = '\0';

		short n_longs;
		if (fread(&n_longs, sizeof(short), 1, idx) != 1)
			break;

		if (name_matches(fname, name)) {
			for (short i = 0; i < n_longs; i += 2) {
				long addr, len;
				if (fread(&addr, sizeof(long), 1, idx) != 1)
					break;
				if (fread(&len, sizeof(long), 1, idx) != 1)
					break;
				readArchive(addr, len);
			}
			found = 1;
			break;
		}

		/* Skip this entry's address pairs */
		fseek(idx, (long)n_longs * (long)sizeof(long), SEEK_CUR);
	}

	fclose(idx);
	return found ? 0 : -1;
}

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

	/* Write the number of long values (addr/len pairs) */
	short n = (short)item.n_longs;
	fwrite(&n, sizeof(short), 1, idx);

	printf("segments: %d\n", n / 2);
	for (int i = 0; i < item.n_longs; i += 2) {
		printf("  addr=%ld len=%ld\n", item.lengths[i],
		       item.lengths[i + 1]);
		fwrite(&item.lengths[i], sizeof(long), 1, idx);
		fwrite(&item.lengths[i + 1], sizeof(long), 1, idx);
	}

	fclose(idx);
	return 0;
}
