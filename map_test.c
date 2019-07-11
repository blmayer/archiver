#include "archiver.h"

int main(void)
{
	struct node map;
	if (!add(&map, "abc", "blablabla")) {
		puts("ERROR: add did not return 1");
		exit(1);
	}
	puts("PASS: add");

	if (strcmp(get(&map, "abc"), "blablabla")) {
		puts("ERROR: get did not return blablabla");
		exit(1);
	}
	puts("PASS: get");

	exit(0);
}