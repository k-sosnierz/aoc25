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
	uint64_t ids[1024];
	int noIds;
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

int
compareUint64_t(const void *lp, const void *rp)
{
	uint64_t l = *((uint64_t*) lp), r = *((uint64_t*) rp);
	if (l < r)
		return -1;
	if (l > r)
		return 1;
	return 0;
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
			break; // IDs follow
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

	// ids
	while (fgets(buf, 64, file)) {
		ret.ids[ret.noIds] = strtoull(buf, NULL, 10);
		ret.noIds++;
	}

	qsort(ret.ids, ret.noIds, sizeof(uint64_t), compareUint64_t);

	return ret;
}

int
checkID(uint64_t id, struct Range range)
{
	if (id < range.start)
		return -1;
	if (id > range.end)
		return 1;
	return 0;
}

int
main()
{
	uint64_t fresh = 0;
	struct Input input = parse(fileName);

	for (int i = 0, j = 0; i < input.noIds && j < input.noRanges; i++) {
		while (checkID(input.ids[i], input.ranges[j]) == 1)
			j++;
		if (j >= input.noRanges)
			break;
		if (checkID(input.ids[i], input.ranges[j]) == 0)
			fresh++;
	}

	printf("The number of fresh ingredient IDs: %ld\n", fresh);
	return 0;
}
