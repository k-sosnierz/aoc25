#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


const char* fileName = "input.txt";

struct Node {
	int value;
	int occurrences;
	struct Node *next;
};

void
addValue(int value, struct Node** list)
{
	while (*list != nullptr) {
		if ((*list)->value == value) {
			(*list)->occurrences++;
			return;
		}
		list = &((*list)->next);
	}

	*list = malloc(sizeof(struct Node));
	if (*list != nullptr) {
		(*list)->value = value;
		(*list)->occurrences = 1;
		(*list)->next = nullptr;
	}
}

int
main()
{
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		perror(fileName);
		exit(1);
	}

	int total = 0;

	struct Node *l = nullptr;
	struct Node *r = nullptr;

	char line[20];
	while (fgets(line, 20, file)) {
		int lv, rv;
		sscanf(line, "%d %d\n", &lv, &rv);
		addValue(lv, &l);
		addValue(rv, &r);
	}

	if (l == nullptr || r == nullptr) {
		perror("data not read");
		exit(1);
	}

	for (struct Node *lnode = l; lnode != nullptr; lnode = lnode->next) {
		for (struct Node *rnode = r; rnode != nullptr; rnode = rnode->next) {
			if (lnode->value == rnode->value) {
				total += lnode->value * lnode->occurrences * rnode->occurrences;
				break;
			}
		}
	}

	printf("total: %d\n", total);

	return 0;
}
