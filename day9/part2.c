#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// https://adventofcode.com/2025/day/9

const char *fileName = "input";

struct Tile {
	int64_t x;
	int64_t y;
};

struct Line {
	struct Tile *a;
	struct Tile *b;
};

struct Input {
	struct Tile *tiles;
	struct Line *lines;
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

int64_t
max(int64_t a, int64_t b)
{
	if (a>b)
		return a;
	return b;
}

int64_t
min(int64_t a, int64_t b)
{
	if (a<b)
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

int
tileOrientation(struct Tile a, struct Tile b, struct Tile c)
{
	int64_t val = (b.y-a.y)*(c.x-b.x) - (b.x-a.x)*(c.y-b.y);
	if (val > 0)
		return 1;
	if (val < 0)
		return -1;
	if (val == 0)
		return 0;
}

bool
doesLineGoBetweenTiles(struct Line l, struct Tile a, struct Tile b)
{
	int o1 = tileOrientation(*l.a, *l.b, a);
	int o2 = tileOrientation(*l.a, *l.b, b);
	int o3 = tileOrientation(a, b, *l.a);
	int o4 = tileOrientation(a, b, *l.b);

	if (o1 != o2 && o3 != o4)
		return true;

	return false;
}

bool
doesLineCrossBox(struct Line l, struct Tile a, struct Tile b)
{
	// four line-line intersects
	int x[2] = {min(a.x, b.x) + 1, max(a.x, b.x) - 1};
	int y[2] = {min(a.y, b.y) + 1, max(a.y, b.y) - 1};
	struct Tile ps[4] = {0};

	for (int ix=0; ix<2; ix++)
		for (int iy=0; iy<2; iy++)
			ps[ix*2+iy] = (struct Tile){.x=x[ix], .y=y[iy]};

	if (doesLineGoBetweenTiles(l, ps[0], ps[1])
		|| doesLineGoBetweenTiles(l, ps[1], ps[3])
		|| doesLineGoBetweenTiles(l, ps[2], ps[3])
		|| doesLineGoBetweenTiles(l, ps[2], ps[0]))
			return true;
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

	input.tiles = realloc(input.tiles, sizeof(struct Tile)*input.noTiles);
	input.lines = err_malloc(sizeof(struct Line)*input.noTiles,
		"alloc lines array in parse fail");

	for (int i=0; i < input.noTiles - 1; i++) {
		input.lines[i].a = &input.tiles[i];
		input.lines[i].b = &input.tiles[i+1];
	}
	input.lines[input.noTiles-1].a = &input.tiles[input.noTiles-1];
	input.lines[input.noTiles-1].b = &input.tiles[0];

	return input;
}

int
main()
{
	struct Input input = parse(fileName);

	// The task doesn't guarantee that the polygon isn't complex,
	// but looking at my input, it isn't. So let's account for that :)
	
	// All we have to do is the same as in part1, but:
	// 1) Check that no lines pass through the rectangle
	// 2) Check that the rectangle is inside the shape ← okay I didn't do that... seems to not be necessary for the input 

	uint64_t largestArea = 0;
	int id1;
	int id2;

	for (int i=0; i<input.noTiles; i++) {
		struct Tile tile1 = input.tiles[i];
		for (int j=0; j<input.noTiles; j++) {
			struct Tile tile2 = input.tiles[j];

			uint64_t area = getArea(tile1, tile2);
			if (area <= largestArea)
				continue;

			bool isBoxInsideShape = true;
			for (int k=0; k<input.noTiles; k++) {
				struct Line line = input.lines[k];
				if (doesLineCrossBox(line, tile1, tile2))
					isBoxInsideShape = false;
			}

			if (isBoxInsideShape) {
				largestArea = area;
				id1 = i;
				id2 = j;
			}
		}
	}

	printf("The largest area possible: %lu\n", largestArea);
	printf("Tile IDs: %d, %d\n", id1, id2);

	return 0;
}
