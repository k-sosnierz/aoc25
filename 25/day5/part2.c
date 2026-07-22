#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// https://adventofcode.com/2025/day/5

const char *fileName = "input";

struct Range {
	uint64_t start;
	uint64_t end; // set to 0 to invalidate range
};

struct Input {
	struct Range ranges[1024];
	int noRanges;
	//uint64_t ids[1024];
	//int noIds;
};

void*
err_malloc(size_t size, char *err)
{
	void *ret = calloc(1, size);
	if (ret == NULL) {
		perror(err);
		exit(1);
	}
	return ret;
}

int
compareRanges(const void *lp, const void *rp)
{
	struct Range l = *((struct Range*) lp), r = *((struct Range*) rp);
	// put invalid ranges at the end 
	if (l.end == 0)
		return 1;
	if (r.end == 0)
		return -1;

	if (l.start == r.start) {
		if (l.end < r.end)
			return -1;
		else if (l.end > r.end)
			return 1;
		else
			return 0;
	} else {
		if (l.start < r.start)
			return -1;
		else
			return 1;
	}
	assert(0 && "unreachable");
}

struct Input
parse(const char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		perror("fopen failed");
		exit(1);
	}

	struct Input ret = {0};

	char buf[64];

	// ranges
	while (fgets(buf, 64, file)) {
		if (buf[0] == '\n')
			break;
		char *number = buf;
		ret.ranges[ret.noRanges].start = strtoull(number, &number, 10);
		ret.ranges[ret.noRanges].end   = strtoull(number+1, NULL, 10);
		ret.noRanges++;
	}

	// sort, merge overlapping, sort again
	// so unneeded ranges go to the end, update noRanges
	qsort(ret.ranges, ret.noRanges, sizeof(struct Range), compareRanges);
	for (int i = 0; i < ret.noRanges; i++) {
		for (int j = i+1; ret.ranges[j].start <= ret.ranges[i].end + 1; j++) {
			if (ret.ranges[j].end > ret.ranges[i].end)
				ret.ranges[i].end = ret.ranges[j].end;
			ret.ranges[j].end = 0; // not needed, make invalid
		}
	}
	qsort(ret.ranges, ret.noRanges, sizeof(struct Range), compareRanges);
	for (int i = 0;; i++) {
		if (ret.ranges[i].end == 0) {
			ret.noRanges = i;
			break;
		}
	}

	return ret;
}

int
main()
{
	uint64_t fresh = 0;
	struct Input input = parse(fileName);

	for (int j = 0; j < input.noRanges; j++)
		fresh += (input.ranges[j].end - input.ranges[j].start) + 1;

	printf("The number of fresh ingredient IDs according to the ranges: %ld\n", fresh);
	return 0;
}
