#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// https://adventofcode.com/2025/day/1

const char* fileName = "input";
const int dialSize = 100; // 0 to 99

int
main()
{
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		perror(fileName);
		exit(1);
	}

	int dialPosition = 50;

	bool turnLeft = false;
	int turnAmount = 0;

	int timesConditionFulfilled = 0;

	char c;
	while (c = fgetc(file)) {
		switch(c) {
			case EOF:
			case '\n':
				if (turnLeft)
					turnAmount *= -1;
				dialPosition += turnAmount;

				while (dialPosition < 0)
					dialPosition += dialSize;
				while (dialPosition >= dialSize)
					dialPosition -= dialSize;

				turnLeft = false;
				turnAmount = 0;

				if (dialPosition == 0)
					timesConditionFulfilled++;

				break;

			case 'L':
				turnLeft = true;
				break;

			case 'R':
				turnLeft = false;
				break;

			default: // digits or junk;
				if (c - '0' < 0 || c - '0' > 9)
					break; // let's ignore junk, the spec doesn't tell us what to do with it
				turnAmount = 10*turnAmount + (c - '0');
				break;
		}


		if (c == EOF) break;
	}
	printf("Times the dial was set to 0 after a rotation: %d\n", timesConditionFulfilled);
	return 0;
}
