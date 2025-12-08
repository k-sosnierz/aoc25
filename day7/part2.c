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

struct Space {
	bool isSplitter;
	uint64_t waysToReach;
};

struct State {
	int n; // rows
	struct Space *spaces; // n(n+1)/2 elements 
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

		0       ^
		12      ^^
		345     ^.^
		6789    .^^.
	*/

	struct State state = {0};
	state.n = (rows-2)/2;
	state.spaces = err_malloc(sizeof(struct Space)*triangular(state.n),
			"alloc space array in parse fail");

	for (int i = 0; i < state.n; i++) {
		int row = i*2+2;
		char *lineStart = &buf[row*columns + columns/2 - i - 1];
		for (int j = 0; j <= i; j++) {
			char *c = lineStart + j*2;
			if (*c == '^')
				state.spaces[triangular(i)+j].isSplitter = true;
			// Splitter.none is the default value of the array
		}
	}

	state.spaces[0] = (struct Space) {.isSplitter = true, .waysToReach = 1};

	return state;
}

void
step (struct State *state, int i)
{
	struct Space *thisLine = &state->spaces[triangular(i)];
	struct Space *nextLine = &state->spaces[triangular(i+1)];
	struct Space *nextNextLine = NULL;
	if (i + 2 < state->n)
		nextNextLine = &state->spaces[triangular(i+2)];

	for (int j = 0; j <= i; j++) {
		if (thisLine[j].isSplitter) {
			nextLine[j].waysToReach += thisLine[j].waysToReach;
			nextLine[j+1].waysToReach += thisLine[j].waysToReach;
		}
		if (nextNextLine && !thisLine[j].isSplitter) {
			nextNextLine[j+1].waysToReach += thisLine[j].waysToReach;
		}
	}
}

int
main()
{
	struct State state = parse(fileName);

	for (int i = 0; i < state.n - 1; i++)
		step(&state, i);

	uint64_t pathsTotal = 0;

	// count stray beams that pass between the spaces of the last row
	struct Space *prevToLastRow = &state.spaces[triangular(state.n-2)];

	for (int i = 0; i < state.n; i++)
		if (!prevToLastRow[i].isSplitter)
			pathsTotal += prevToLastRow[i].waysToReach;

	// count stray beams and splitters in the last row
	struct Space *lastRow = &state.spaces[triangular(state.n-1)];

	for (int i = 0; i < state.n; i++) {
		if (lastRow[i].isSplitter)
			pathsTotal += 2*lastRow[i].waysToReach;
		else
			pathsTotal += lastRow[i].waysToReach;
	}

	printf("Total timelines: %lu\n", pathsTotal);
	return 0;
}
