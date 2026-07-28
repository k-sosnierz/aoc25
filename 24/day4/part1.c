#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


const char* fileName = "input.txt";

enum Edges {
	LEFT  = 0b0001,
	RIGHT = 0b0010,
	TOP   = 0b0100,
	BOTTOM= 0b1000
};

struct Pos {
	int x;
	int y;
	struct Pos *next;
};

void
enlist(struct Pos **list, int x, int y)
{
	while (*list != nullptr) {
		if ((*list)->x == x && (*list)->y == y)
			return;
		list = &((*list)->next);
	}

	*list = malloc(sizeof(struct Pos));
	if (*list != nullptr) {
		(*list)->x = x;
		(*list)->y = y;
		(*list)->next = nullptr;
	}
}

struct Pos *
positionsFromEdges(int edges, int noCols, int noRows)
{
	struct Pos *list = nullptr;

	if (edges & LEFT) {	
		int x = 0;
		for (int y=0; y<noRows; y++)
			enlist(&list, x, y);
	}

	if (edges & RIGHT) {	
		int x = noCols-1;
		for (int y=0; y<noRows; y++)
			enlist(&list, x, y);
	}

	if (edges & TOP) {	
		int y = 0;
		for (int x=0; x<noCols; x++)
			enlist(&list, x, y);
	}

	if (edges & BOTTOM) {	
		int y = noRows-1;
		for (int x=0; x<noCols; x++)
			enlist(&list, x, y);
	}

	return list;
}

int
main()
{
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		perror(fileName);
		exit(1);
	}

	char line[256] = {};
	fgets(line, 256, file);
	rewind(file);

	int noCols = strcspn(line, "\n");

	char *grid = nullptr;
	int noRows = 0;
	
	while (fgets(line, 256, file)) {
		noRows++;
		grid = realloc(grid, noRows*noCols*sizeof(char));
		memcpy(&grid[(noRows-1)*noCols], line, noCols);
	}

	int xmasesFound = 0;

	// now go through the grid in every direction
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			if (dy == 0 && dx == 0)
				continue;
			int startEdges = 0;
			// dy/dx	-1		0		1
			// -1		BR		B		BL
			// 0		R		ø		L
			// 1		TR		T		TL
			if (dx == -1)
				startEdges |= RIGHT;
			if (dx == 1)
				startEdges |= LEFT;
			if (dy == -1)
				startEdges |= BOTTOM;
			if (dy == 1)
				startEdges |= TOP;
			
			//xmasesFound = 0;
			struct Pos *pos = positionsFromEdges(startEdges, noCols, noRows);
			while (pos != nullptr) {
				char buf[5] = {0};
				int x = pos->x;
				int y = pos->y;
				while (x >= 0 && x < noCols && y >= 0 && y < noRows) {
					char c = grid[y*noCols+x];
					y += dy;
					x += dx;

					if (strlen(buf) < 4) {
						buf[strlen(buf)] = c;
					} else {
						memmove(&buf[0], &buf[1], 3);
						buf[3] = c;
					}

					if (strcmp(buf, "XMAS") == 0)
						xmasesFound++;
				}
				struct Pos *prevPos = pos;
				pos = pos->next;
				free(prevPos);
			}
		}
	}

	printf("found %d XMASes\n", xmasesFound);

	return 0;
}
