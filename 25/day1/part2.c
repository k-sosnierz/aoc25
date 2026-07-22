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
				int prevDialPosition = dialPosition;

				// each rotation of 100 in either direction meets a 0 once and gets us back to the same position
				if (turnAmount >= dialSize) {
					timesConditionFulfilled += turnAmount / dialSize;
					turnAmount = turnAmount % dialSize;
				}

				// now turnAmount is guaranteed to be less than dialSize

				if (turnLeft)
					turnAmount *= -1;
				dialPosition += turnAmount;

				if (dialPosition < 0) {
					dialPosition += dialSize;
					if (prevDialPosition != 0)
						timesConditionFulfilled++;
				} else if (dialPosition >= dialSize) {
					dialPosition -= dialSize;
					timesConditionFulfilled++;
				} else if (turnAmount != 0 && dialPosition == 0) {
					timesConditionFulfilled++;
				}

				turnLeft = false;
				turnAmount = 0;

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
	printf("Times the dial was ever set to 0: %d\n", timesConditionFulfilled);
	return 0;
}
