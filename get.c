#include "archiver.h"

int readArchive(long addr, long len)
{
	arch = fopen(ARCHIVE, "rb");
	if (!arch) {
		puts("Error opening archive file");
		return -1;
	}

	/* Read archive at addr for len bytes */
	fseek(arch, addr, SEEK_SET);
	while (len--) {
		printf("%c", getc(arch));
	}

	fclose(arch);
	return 0;
}

int get(char *args[])
{
	while (*args) {
		printf("arg: %s\n", *args++);
	}

	return 0;
}
