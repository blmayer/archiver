/*
 * The Archiver utility
 *
 * Use this program to store all your files in a single file, and an index. It
 * is intended to simplify the way you look for personal documents by providing
 * a simple interface.
 *
 * The intention is to use the common parts in files to compress the archive,
 * so space can be saved, but this is experimental, I don't know if it is effi-
 * cient.
 *
 * Author: Brian Mayer
 * Email: bleemayer@gmail.com
 */

#include "archiver.h"

void printHelp()
{
	puts("Archiver, an utility to store your files");
	puts("");
	puts("Usage: archiver [command] FILE...");
	puts("");
	puts("Examples:");
	puts("  archive doc.txt          # Add file doc.txt to your archive");
	puts("");
	puts("Available commands are:");
	puts("  -h, --help, help:        show this help");
	puts("  -l, list:                list all files in the archive");
}

int main(int argc, char *argv[])
{
	/* Get home dir to fill variables */
	char *homeDir = getenv("HOME");
	ARCHIVE = strcat(homeDir, "/archive");

	char temp[strlen(ARCHIVE) + 5];
	strcpy(temp, ARCHIVE);
	INDEXFILE = strcat(temp, ".idx");

	/* At least one file or command must be passed */
	if (argc < 2) {
		printHelp();
		return 0;
	}

	/* Command line argument parsing */
	while (*++argv != NULL) {
		switch (strcmp("--arger", *argv)) {
		case -7:
		case -59:
			printHelp();
			return 0;
		case -63:
		case -11:
			return readIndex();
		default:
			/* Can be a file name */
			archiveFile(*argv);
			break;
		}
	}

	return 0;
}
