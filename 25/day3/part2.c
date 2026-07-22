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


#define BAT_NUM 12
uint64_t
analyzeLine(struct Line* line)
{
	uint64_t sum = 0;

	char *batteries[BAT_NUM] = {NULL}; // msb (leftmost) to lsb (rightmost)

	char *bank = line->bank;

	for (int i = 0; i < BAT_NUM; i++)
		for (char *c = bank; c <= bank + strlen(bank) - BAT_NUM + i; c++)
			if (batteries[i] == NULL || *c > *batteries[i])
				if (i == 0 || batteries[i-1] < c)
					batteries[i] = c;

	for (int i = 0; i < BAT_NUM; i++)
		sum = sum*10 + (*batteries[i]-'0');

	//printf("%ld\n", sum);

	return sum;
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
