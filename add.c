#include "add.h"

int writeToArchive(char *filePath)
{
	/* Check if file exists */
	FILE *file = fopen(filePath, "r");
	if (!file) {
		printf("Error: File \"%s\" not found.\n", filePath);
		exit(-1);
	}

	/*
	 * The fun will start here in the future:
	 * Search the archive for each chunk in the file.
	 *
	 * But now we only append the content :(
	 */

	/* Read some bytes of file into buffer */
	unsigned char buffer[512];
	long addr = ftell(arch);
	int bytes;

	do {
		bytes = fread(buffer, 1, 512, file);
		if (fwrite(buffer, bytes, 1, arch) < 0) {
			puts("Error writing to archive file");
			exit(-2);
		}
		puts("Wrote to archive");
	} while (bytes == 512);

	long file_len = ftell(arch) - addr;

	/* Resolve paths */
	char *realPath = realpath(filePath, NULL);

	// TODO: Write addToIndex function to add an item to the index correctly
	// addToIndex();

	fclose(arch);
	fclose(file);

	return 0;
}

int archiveFile(char *file)
{
	/* Check if needed files exists */
	// TODO: Let user choose where to store the archive
	arch = fopen(ARCHIVE, "a+");
	if (!arch) {
		puts("Error opening archive file");
		exit(-1);
	}

	int res = writeToArchive(file);
	if (res) {
		puts("Error writing file");
	}
	// TODO: Search in index file and check if file already exists
	// TODO: Add file to archive

	return 0;
}
