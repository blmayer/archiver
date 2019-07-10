#include "archiver.h"

int add(struct node *root, char *key, char *data)
{
	struct node *curr = root;
	printf("curr addr: \t%p\n", curr);

	while (*key) {
		printf("processing key %c\n", *key);
		printf("curr %p\n", curr);

		if (!curr->next[*key - 65]) {
			puts("not curr");
			struct node *new = malloc(sizeof(struct node));
			printf("new addr: %p\n", new);

			curr->next[*key - 65] = new;
		}

		curr = curr->next[*key - 65];
		printf("new curr: %p\n\n", curr);
		key++;
	}

	curr->content = data;
	return 1;
}

char *get(struct node *root, char *key)
{
	struct node *curr = root;
	while (*key) {
		printf("curr %p\n", curr);
		curr = (struct node *)&curr->next[*key++ - 65];
		if (!curr) {
			return NULL;
		}
	}
	return curr->content;
}
