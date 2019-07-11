#include "archiver.h"

int add(struct node *root, char *key, char *data)
{
	struct node *curr = root;
	while (*key) {
		if (!curr->next[*key - 65]) {
			struct node *new = malloc(sizeof(struct node));
			curr->next[*key - 65] = new;
		}

		curr = curr->next[*key - 65];
		key++;
	}

	/* Free previous data */
	if (curr->content) {
		free(curr->content);
	}

	curr->content = malloc(strlen(data) + 1);
	memcpy(curr->content, data, strlen(data) + 1);

	return 1;
}

char *get(struct node *root, char *key)
{
	struct node *curr = root;
	while (*key) {
		curr = curr->next[*key++ - 65];
		if (!curr) {
			return NULL;
		}
	}
	return curr->content;
}
