#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

// https://adventofcode.com/2025/day/3

const char *fileName = "input";

struct Line {
	char *bank;
	struct Line *next; // linked list
};

void*
err_malloc(size_t size, char *err)
{
	void *ret = malloc(size);
	if (ret == NULL) {
		perror(err);
		exit(1);
	}
	return ret;
}

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

	char buf[1024] = {0};
	while (fgets(buf, 1024, file)) {
		if (ptr->bank != NULL)
			ptr = ptr->next;
		ptr->bank = err_malloc(strlen(buf)+1, "string malloc in parse loop fail");
		strncpy(ptr->bank, buf, strlen(buf)-1); // drop \n
		ptr->next = err_malloc(sizeof(struct Line), "struct malloc in parse loop fail");
	}

	free(ptr->next);
	ptr->next = NULL;

	return head;
}

uint64_t
analyzeLine(struct Line* line)
{
	char *batteries[2] = {NULL};

	char *bank = line->bank;

	for (char *c = bank; c < bank + strlen(bank) - 1; c++)
		if (batteries[0] == NULL || *c > *batteries[0])
			batteries[0] = c;

	for (char *c = batteries[0]+1; c < bank + strlen(bank); c++)
		if (batteries[1] == NULL || *c > *batteries[1])
			batteries[1] = c;

	//printf("%d\n", (*batteries[0]-'0') * 10 + (*batteries[1]-'0'));

	return (*batteries[0]-'0') * 10 + (*batteries[1]-'0');
}

int
main()
{
	uint64_t sum = 0;
	struct Line *input = parse(fileName);

	for (struct Line *ptr = input; ptr != NULL; ptr = ptr->next)
		sum += analyzeLine(ptr);

	printf("The sum equals %ld\n", sum);
	return 0;
}
