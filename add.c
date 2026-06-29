#include "archiver.h"

/*
 * Paged longest common sequence search.
 *
 * At each position in the new file we look for the longest run of bytes that
 * already appears in the archive.  Matches shorter than MIN_MATCH are ignored
 * (storing a pointer would not save space).  Both the file and the archive are
 * read in PAGE_SIZE windows so neither needs to fit in memory.
 */

/* Append one (addr, len) pair to the growing lengths array. */
static int push_pair(long **lengths, int *n_longs, long addr, long len)
{
	long *p = realloc(*lengths, (*n_longs + 2) * sizeof(long));
	if (!p) {
		puts("Error: out of memory");
		return -1;
	}
	*lengths = p;
	(*lengths)[(*n_longs)++] = addr;
	(*lengths)[(*n_longs)++] = len;
	return 0;
}

/*
 * Find the longest sequence in the archive [0, arch_len) that matches the
 * file starting at file_pos.  Returns 1 and fills *out_addr / *out_len when
 * a match of at least MIN_MATCH bytes is found.
 */
static int find_longest_match(FILE *file, long file_pos, long file_len,
			      FILE *arch, long arch_len, long *out_addr,
			      long *out_len)
{
	unsigned char file_buf[PAGE_SIZE];
	unsigned char arch_buf[PAGE_SIZE];
	long best_len = 0;
	long best_addr = 0;
	long file_left = file_len - file_pos;

	if (file_left < MIN_MATCH || arch_len < MIN_MATCH)
		return 0;

	/* First page of the file from the search position (more loaded on demand). */
	fseek(file, file_pos, SEEK_SET);
	size_t file_have = fread(file_buf, 1, PAGE_SIZE, file);
	if (file_have < MIN_MATCH)
		return 0;

	/*
	 * Walk the archive in pages.  Overlap by MIN_MATCH - 1 so a sequence
	 * that straddles a page boundary is still found as a starting point.
	 */
	for (long apos = 0; apos + MIN_MATCH <= arch_len;) {
		fseek(arch, apos, SEEK_SET);
		size_t n = fread(arch_buf, 1, PAGE_SIZE, arch);
		if (n > (size_t)(arch_len - apos))
			n = (size_t)(arch_len - apos);
		if (n < MIN_MATCH)
			break;

		for (size_t i = 0; i + MIN_MATCH <= n; i++) {
			if (memcmp(arch_buf + i, file_buf, MIN_MATCH) != 0)
				continue;

			/* Extend within the buffers we already have. */
			long match_len = MIN_MATCH;
			while (i + (size_t)match_len < n &&
			       (size_t)match_len < file_have &&
			       arch_buf[i + match_len] == file_buf[match_len])
				match_len++;

			/* Extend past current pages with single-byte compares. */
			while (apos + (long)i + match_len < arch_len &&
			       match_len < file_left) {
				unsigned char ac, fc;
				fseek(arch, apos + (long)i + match_len,
				      SEEK_SET);
				if (fread(&ac, 1, 1, arch) != 1)
					break;
				if ((size_t)match_len < file_have) {
					fc = file_buf[match_len];
				} else {
					fseek(file, file_pos + match_len,
					      SEEK_SET);
					if (fread(&fc, 1, 1, file) != 1)
						break;
				}
				if (ac != fc)
					break;
				match_len++;
			}

			if (match_len > best_len) {
				best_len = match_len;
				best_addr = apos + (long)i;
			}
		}

		if (n < PAGE_SIZE)
			break;
		/* Step forward, keeping MIN_MATCH - 1 bytes of overlap. */
		apos += (long)PAGE_SIZE - (MIN_MATCH - 1);
	}

	if (best_len >= MIN_MATCH) {
		*out_addr = best_addr;
		*out_len = best_len;
		return 1;
	}
	return 0;
}

/* Copy [file_pos, file_pos + len) from file to the end of arch. */
static long append_literal(FILE *file, long file_pos, long len, FILE *arch)
{
	unsigned char buf[PAGE_SIZE];
	long written = 0;

	fseek(arch, 0, SEEK_END);
	long addr = ftell(arch);
	fseek(file, file_pos, SEEK_SET);

	while (written < len) {
		size_t chunk = (size_t)(len - written);
		if (chunk > PAGE_SIZE)
			chunk = PAGE_SIZE;
		size_t n = fread(buf, 1, chunk, file);
		if (n == 0)
			break;
		if (fwrite(buf, 1, n, arch) != n) {
			puts("Error writing to archive file");
			exit(-2);
		}
		written += (long)n;
	}

	return addr;
}

int archiveFile(char *filePath)
{
	arch = fopen(ARCHIVE, "a+b");
	if (!arch) {
		puts("Error opening archive file");
		exit(-1);
	}

	FILE *file = fopen(filePath, "rb");
	if (!file) {
		printf("Error: File \"%s\" not found.\n", filePath);
		exit(-1);
	}

	/* Size of archive content we may point at (grows as we append literals). */
	fseek(arch, 0, SEEK_END);
	long arch_len = ftell(arch);

	fseek(file, 0, SEEK_END);
	long file_len = ftell(file);
	fseek(file, 0, SEEK_SET);

	long *lengths = NULL;
	int n_longs = 0;
	long pos = 0;

	/* Start of a pending unmatched run in the file (-1 if none). */
	long lit_pos = -1;
	long lit_len = 0;

	while (pos < file_len) {
		long m_addr = 0, m_len = 0;

		if (find_longest_match(file, pos, file_len, arch, arch_len,
				       &m_addr, &m_len)) {
			/* Flush any pending new bytes into the archive first. */
			if (lit_len > 0) {
				long addr = append_literal(file, lit_pos,
							   lit_len, arch);
				if (push_pair(&lengths, &n_longs, addr,
					      lit_len) < 0)
					exit(-1);
				/* New data is now searchable for later matches. */
				arch_len = ftell(arch);
				lit_pos = -1;
				lit_len = 0;
			}

			if (push_pair(&lengths, &n_longs, m_addr, m_len) < 0)
				exit(-1);
			pos += m_len;
		} else {
			/* No useful common sequence here — treat as new bytes. */
			if (lit_pos < 0) {
				lit_pos = pos;
				lit_len = 0;
			}
			lit_len++;
			pos++;
		}
	}

	/* Flush trailing literals. */
	if (lit_len > 0) {
		long addr = append_literal(file, lit_pos, lit_len, arch);
		if (push_pair(&lengths, &n_longs, addr, lit_len) < 0)
			exit(-1);
	}

	fclose(file);
	fclose(arch);

	char *realPath = realpath(filePath, NULL);
	if (!realPath) {
		puts("Error resolving file path");
		exit(-1);
	}
	printf("Real path: %s\n", realPath);

	struct item newFile = {realPath, lengths, n_longs};
	addToIndex(newFile);

	free(lengths);
	free(realPath);
	return 0;
}
