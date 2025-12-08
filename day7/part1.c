#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// https://adventofcode.com/2025/day/7

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

static const struct {
	uint8_t none;
	uint8_t off;
	uint8_t on;
	uint8_t strayBeam;
} Splitter = {0, 1, 2, 3};

struct State {
	int n; // levels of splitters
	uint8_t *splitters; // n(n+1)/2 elements 
};

// triangular number
static inline
int
triangular(int in)
{
	return in*(in+1)/2;
}

struct State
parse(const char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		perror("fopen failed");
		exit(1);
	}

	// get file size, alloc buffer, put file in it
	fseek(file, 0L, SEEK_END);
	uint32_t fileSize = ftell(file);
	rewind(file);
	char *buf = err_malloc(fileSize + 1,
			"parse buf alloc fail");
	fread(buf, 1, fileSize, file);

	int columns = strchr(buf, '\n') - buf + 1;
	int rows = fileSize / columns;


	/*
		read the input into a contiguous array

		0		^
		12		^^
		345		^.^
		6789	.^^.
	*/

	struct State state = {0};
	state.n = (rows-2)/2;
	state.splitters = err_malloc(sizeof(uint8_t)*triangular(state.n),
			"alloc splitters array in parse fail");

	for (int i = 0; i < state.n; i++) {
		int row = i*2+2;
		char *lineStart = &buf[row*columns + columns/2 - i - 1];
		for (int j = 0; j <= i; j++) {
			char *c = lineStart + j*2;
			if (*c == '^')
				state.splitters[triangular(i)+j] = Splitter.off;
			// Splitter.none is the default value of the array
		}
	}

	state.splitters[0] = Splitter.on;

	return state;
}

int
step (struct State *state, int i)
{
	int activated = 0;

	uint8_t *thisLine = &state->splitters[triangular(i)];
	uint8_t *nextLine = &state->splitters[triangular(i+1)];
	uint8_t *nextNextLine = NULL;
	if (i + 2 < state->n) // for stray beams dropping two lines down, between splitters
		nextNextLine = &state->splitters[triangular(i+2)];

	for (int j = 0; j <= i; j++) {
		if (thisLine[j] == Splitter.on) {
			if (nextLine[j] == Splitter.off) {
				nextLine[j] = Splitter.on;
				activated++;
			}
			if (nextLine[j+1] == Splitter.off) {
				nextLine[j+1] = Splitter.on;
				activated++;
			}
			if (nextLine[j] == Splitter.none) {
				nextLine[j] = Splitter.strayBeam;
			}
			if (nextLine[j+1] == Splitter.none) {
				nextLine[j+1] = Splitter.strayBeam;
			}
		}
		if (nextNextLine && thisLine[j] == Splitter.strayBeam) {
			if (nextNextLine[j+1] == Splitter.off) {
				nextNextLine[j+1] = Splitter.on;
				activated++;
			}
			if (nextNextLine[j+1] == Splitter.none) {
				nextNextLine[j+1] = Splitter.strayBeam;
			}
		}
	}

	return activated;
}

int
main()
{
	int splits = 1; // initial splitter activated
	struct State state = parse(fileName);

	for (int i = 0; i < state.n - 1; i++)
		splits += step(&state, i);

	printf("The beam split %d times\n", splits);
	return 0;
}
