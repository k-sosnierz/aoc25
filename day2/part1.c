#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

// https://adventofcode.com/2025/day/2

const char *fileName = "input";

// 32-bit ints wouldn't be large enough
struct Range {
	uint64_t start;
	uint64_t end;
	struct Range *next; // linked list
};

bool
isPrime(uint64_t in)
{
	for (int i = 2; i*i < in; i++)
		if (in%i == 0)
			return false;
	return true;
}

// number of decimal digits
uint64_t
getNumLength(uint64_t in)
{
	return floor(log10(in)) + 1;
}

uint64_t
ipow(uint64_t in, int exp)
{
    uint64_t ret = 1;
    for (;;)
    {
        if (exp & 1)
            ret *= in;
        exp >>= 1;
        if (!exp)
            break;
        in *= in;
    }
    return ret;
}

uint64_t
repeatNum(uint64_t in, int n)
{
	uint64_t ret = 0;
	for (int i = 0; i < n; i++)
		ret = ret * ipow(10, getNumLength(in)) + in;
	return ret;
}

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
struct Range*
parse(const char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		perror("fopen failed");
		exit(1);
	}

	struct Range *ptr = err_malloc(sizeof(struct Range), "malloc in parse init failed");
	uint64_t *number = &ptr->start;
	struct Range *head = ptr;

	char c;
	while (c = fgetc(file)) {
		switch (c) {
			case EOF:
				break;
			case '-':
				number = &ptr->end;
				break;
			case ',':
				ptr->next = err_malloc(sizeof(struct Range), "malloc in parse loop failed");
				ptr = ptr->next;
				number = &ptr->start;
				break;
			default:
				if (c - '0' >= 0 && c - '0' < 10)
					*number = *number * 10 + (c-'0');
				break;
		}
		if (c == EOF) break;
	}

	return head;
}

uint64_t
analyzeRange(struct Range* range)
{
	uint64_t sum = 0;

	uint64_t start = range->start;
	uint64_t end = range->end;

	int startLen = getNumLength(start);
	int endLen = getNumLength(end);

	for (int l = startLen; l <= endLen; l++) {
		if (l%2 != 0) // num doesn't split evenly in two
			continue;
		// check 100100...999999 for every 100...999
		for (int i = ipow(10, l/2 - 1); i<ipow(10, l/2); i++) {
			uint64_t num = repeatNum(i, 2);
			if (num < start) {
				i = start / ipow(10, l/2);
				num = repeatNum(i, 2);
			}
			if (num > end) {
				break;
			}
			if (num >= start) {
				sum += num;
			}
		}
	}

	//printf("S: %ld\nE: %ld\nSum: %ld\n\n", start, end, sum);

	return sum;
}

int
main()
{
	uint64_t sum = 0;
	struct Range *range = parse(fileName);

	for (struct Range *ptr = range; ptr != NULL; ptr = ptr->next)
		sum += analyzeRange(ptr);

	printf("The sum equals %ld\n", sum);
	return 0;
}
