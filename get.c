#include "archive.h"

int get(char *args[])
{
	while (*args) {
		printf("arg: %s\n", *args++);
	}

	return 0;
}
