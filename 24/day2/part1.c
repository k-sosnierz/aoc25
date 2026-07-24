#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


const char* fileName = "input.txt";

enum Trend {
	UNDEFINED,
	INCREASING,
	DECREASING,
	CONSTANT
};

enum Trend
checkTrend(int before, int after)
{
	if (after > before) 
		return INCREASING;
	if (after < before) 
		return DECREASING;
	if (after == before)
		return CONSTANT;
}


int
main()
{
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		perror(fileName);
		exit(1);
	}

	int noSafeReports = 0;

	char line[30];
	while (fgets(line, 30, file)) {
		bool isSafe = true;
		char* levelStr = strtok(line, " \n");
		int prevLevel = atoi(levelStr);
		enum Trend trend = UNDEFINED;

		while (levelStr = strtok(nullptr, " \n")) {
			int level = atoi(levelStr);

			// Any two adjacent levels differ by at least one and at most three
			int difference = abs(level - prevLevel);
			if (difference < 1 || difference > 3)
				isSafe = false;

			// The levels are either all increasing or all decreasing
			if (trend == UNDEFINED)
				trend = checkTrend(prevLevel, level);
			else if (trend != checkTrend(prevLevel, level))
				isSafe = false;
			prevLevel = level;
		}

		if (isSafe)
			noSafeReports++;
	}

	printf("Number of safe reports: %d\n", noSafeReports);

	return 0;
}
