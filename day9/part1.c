#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// https://adventofcode.com/2025/day/9

const char *fileName = "input";

struct Tile {
	uint32_t x;
	uint32_t y;
};

struct Input {
	struct Tile *tiles;
	int noTiles;
};

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

uint64_t
max(uint64_t a, uint64_t b)
{
	if (a>b)
		return a;
	return b;
}

uint64_t
getArea(struct Tile a, struct Tile b)
{
	uint64_t ret = 1;
	ret *= (1 + abs(a.x - b.x));
	ret *= (1 + abs(a.y - b.y));
	return ret;
}

struct Input
parse(const char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		perror("fopen failed");
		exit(1);
	}

	struct Input input = {0};
	input.tiles = err_malloc(sizeof(struct Tile)*4096,
		"alloc tiles array in parse fail");
	
	char buf[32] = {0};

	// read points line by line, set bounds, trim the alloc
	while (fgets(buf, 32, file)) {
		char *number = buf;
		struct Tile *tile = &input.tiles[input.noTiles];
		tile->x = strtoull(number, &number, 10);
		tile->y = strtoull(++number, NULL, 10);
		input.noTiles++;
	}

	return input;
}

int
main()
{
	struct Input input = parse(fileName);


	// a very naive approach, but it only takes several milliseconds to run
	// so i wouldn't feel justified in spending minutes to optimize it :)
	uint64_t largestArea = 0;

	for (int i=0; i<input.noTiles; i++) {
		struct Tile tile1 = input.tiles[i];
		for (int j=0; j<input.noTiles; j++) {
			struct Tile tile2 = input.tiles[j];
			largestArea = max(largestArea, getArea(tile1, tile2));
		}
	}

	printf("The largest area possible: %lu\n", largestArea);

	return 0;
}
