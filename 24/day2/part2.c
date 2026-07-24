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

// analyze the report, acting like the element of index `excluded' doesn't exist
// a negative value of `excluded' will result in the whole report being analyzed
bool
isSafe(int* report, int reportSize, int excluded)
{
	// create array
	int* reportCopy;
	int reportCopySize = reportSize;
	if (excluded >= 0 && excluded < reportSize) {
		reportCopy = malloc((reportSize-1)*sizeof(int));
		memcpy(reportCopy, report, excluded*sizeof(int));
		memcpy(reportCopy+excluded, report+excluded+1, (reportSize-excluded-1)*sizeof(int));
		reportCopySize--;
	} else {
		reportCopy = malloc(reportSize*sizeof(int));
		memcpy(reportCopy, report, reportSize*sizeof(int));
	}

	// analyze
	bool ret = true;
	int prevLevel = reportCopy[0];
	enum Trend trend = UNDEFINED;

	for (int i=1; i<reportCopySize; i++) {
		int level = reportCopy[i];

		// Any two adjacent levels differ by at least one and at most three
		int difference = abs(level - prevLevel);
		if (difference < 1 || difference > 3)
			ret = false;
		
		// The levels are either all increasing or all decreasing
		if (trend == UNDEFINED)
			trend = checkTrend(prevLevel, level);
		else if (trend != checkTrend(prevLevel, level))
			ret = false;

		prevLevel = level;
	}

	return ret;
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
		// read report into array
		int reportSize = 0;
		int *report = nullptr;
		char* levelStr = nullptr;

		while (levelStr = strtok((report==nullptr? line : nullptr), " \n")) {
			reportSize++;
			report = realloc(report, sizeof(int)*reportSize);
			report[reportSize-1] = atoi(levelStr);
		}

		for (int excluded=-1; excluded<reportSize; excluded++) {
			if (isSafe(report, reportSize, excluded)) {
				noSafeReports++;
				break;
			}
		}
	}

	printf("Number of safe reports: %d\n", noSafeReports);

	return 0;
}
