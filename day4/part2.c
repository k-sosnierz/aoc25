#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// https://adventofcode.com/2025/day/4

const char *fileName = "input";

void*
err_malloc(size_t size, char *err)
{
	void *ret = calloc(size, 1);
	if (ret == NULL) {
		perror(err);
		exit(1);
	}
	return ret;
}

struct Line {
	char s[256]; // max 256 chars per line (the actual input only has 135, the example has 10)
	uint8_t neighbours[256]; 
	struct Line *next;
};

// returns linked list 
struct Line*
parse(const char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		perror("fopen failed");
		exit(1);
	}

	struct Line *ptr = err_malloc(sizeof(struct Line), "malloc in parse init fail");
	struct Line *head = ptr;

	while (fgets(ptr->s, sizeof(ptr->s), file)) {
		ptr->s[strlen(ptr->s)-1] = 0; // drop newline
		ptr->next = err_malloc(sizeof(struct Line), "struct malloc in parse loop fail");
		ptr = ptr->next;
	}

	for (ptr = head; ptr->next->next != NULL; ptr = ptr->next) {}
	ptr->next = NULL; // abandon last, unused node

	return head;
}

void
printLine(struct Line* line)
{
	// debug
	for (int i = 0; i < strlen(line->s); i++) {
		if (line->s[i] == '.') {
			printf(".");
		} else {
			if (line->neighbours[i] < 4)
				printf("x");
			else
				printf("@");
		}
	}
	printf("\n");
}

void
analyzeLine(struct Line* line)
{
	struct Line* line2 = line->next;

	for (int i = 0; i < strlen(line->s); i++) {
		// line: increment neighbours[i] for each '@' neighbouring s[i] in the X axis
		if (i > 0 && line->s[i-1] == '@')
			line->neighbours[i]++;
		if (i+1 < strlen(line->s) && line->s[i+1] == '@')
			line->neighbours[i]++;

		// line, line2: increment neighbours[i] for each '@' located at s[i-1], s[i], s[i+1]
		if (line2 != NULL) {
			if (line->s[i] == '@') {
				if (i > 0)
					line2->neighbours[i-1]++;
				line2->neighbours[i]++;
				if (i+1 < strlen(line->s))
					line2->neighbours[i+1]++;
			}
			if (line2->s[i] == '@') {
				if (i > 0)
					line->neighbours[i-1]++;
				line->neighbours[i]++;
				if (i+1 < strlen(line->s))
					line->neighbours[i+1]++;
			}
		}
	}
}

uint8_t
removeRemovable(struct Line* line)
{
	uint8_t count = 0;
	for (int i = 0; i < strlen(line->s); i++) {
		if (line->s[i] == '@' && line->neighbours[i]<4) {
			line->s[i] = '.';
			count++;
		}
	}

	return count;
}

int
main()
{
	struct Line *input = parse(fileName);

	uint64_t totalRemovable = 0;
	uint64_t removedNow;

	do {
		for (struct Line *ptr = input; ptr != NULL; ptr = ptr->next)
			memset(ptr->neighbours, 0, sizeof(ptr->neighbours));

		removedNow = 0;
		for (struct Line *ptr = input; ptr != NULL; ptr = ptr->next) {
			analyzeLine(ptr);
			removedNow += removeRemovable(ptr);
		}

		totalRemovable += removedNow;
	} while (removedNow != 0);

	printf("Up to %ld rolls of paper can be removed.\n", totalRemovable);
	return 0;
}
