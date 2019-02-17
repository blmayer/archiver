#include "archiver.h"

int archiveFile(char *filePath)
{
	/* Check if needed files exists */
	// TODO: Let user choose where to store the archive
	arch = fopen(ARCHIVE, "a+b");

	if (!arch) {
		puts("Error opening archive file");
		exit(-1);
	}

	/* Check if filePath exists */
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
	fseek(arch, 0, SEEK_END);
	long addrs[2] = {ftell(arch), 0};
	int bytes;

	do {
		bytes = fread(buffer, 1, 512, file);
		if (fwrite(buffer, bytes, 1, arch) < 0) {
			puts("Error writing to archive file");
			exit(-2);
		}
		puts("Wrote to archive");
	} while (bytes == 512);

	fclose(file);
	addrs[1] = ftell(arch) - addrs[0];
	fclose(arch);

	/* Resolve paths */
	char *realPath = realpath(filePath, NULL);
	printf("Real path: %s\n", realPath);

	/* Update the index file */
	struct item newFile = {realPath, addrs};
	addToIndex(newFile);

	return 0;
}
