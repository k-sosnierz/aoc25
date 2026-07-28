#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


const char* fileName = "input.txt";

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

	for (int x = 1; x < noCols-1; x++) {
		for (int y = 1; y < noRows-1; y++) {
			if (grid[y*noCols+x] == 'A') {
				char tl = grid[(y-1)*noCols+x-1];
				char tr = grid[(y-1)*noCols+x+1];
				char bl = grid[(y+1)*noCols+x-1];
				char br = grid[(y+1)*noCols+x+1];
				if (! ((tl=='M' && br=='S') || (tl=='S' && br=='M')))
					continue;
				if (! ((bl=='M' && tr=='S') || (bl=='S' && tr=='M')))
					continue;
				xmasesFound++;
			}
		}
	}

	printf("found %d XMASes\n", xmasesFound);

	return 0;
}
