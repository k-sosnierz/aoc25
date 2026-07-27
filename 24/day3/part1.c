#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


const char* fileName = "input.txt";

struct MulState {
	int step; // 0: intro, 1: 1st number, 2: 2nd number
	char arg1[3];
	char arg2[3]; 
	int introStatus; //0: not given, 1: m, 2: u, 3: l, 4: (
	int arg1status; // 0: not given, 1-3: number of digits, >3: too long
	int arg2status; //
};

// returns the mul result if mul is parsed, 0 otherwise
int
advance(struct MulState *state, int c)
{
	// step 0: intro
	if (state->step == 0) {
		const char intro[4] = "mul(";
		if (intro[state->introStatus] == c) {
			state->introStatus++;
			if (state->introStatus == 4)
				state->step++;
		} else {
			*state = (struct MulState){}; // discard progress
		}
		return 0;
	}

	// step 1: number
	if (state->step == 1) {
		if (isdigit(c) && state->arg1status < 3) {
			state->arg1[state->arg1status] = c;
			state->arg1status++;
		}
		else if (c == ',' && state->arg1status > 0 ) {
			state->step++;
		}
		else {
			*state = (struct MulState){}; // discard progress
		}
		return 0;
	}

	// step 2: 2nd number
	if (state->step == 2) {
		if (isdigit(c) && state->arg2status < 3) {
			state->arg2[state->arg2status] = c;
			state->arg2status++;
		}
		else if (c == ')' && state->arg2status > 0 ) { // complete
			int num1 = 0;
			for (int i=0; i<state->arg1status; i++) {
				num1 *= 10;
				num1 += state->arg1[i] - '0';
			}
			int num2 = 0;
			for (int i=0; i<state->arg2status; i++) {
				num2 *= 10;
				num2 += state->arg2[i] - '0';
			}
			printf("Got %d from %d*%d\n", num1*num2, num1, num2);
			*state = (struct MulState){}; // discard progress
			return num1*num2;
		}
		else {
			*state = (struct MulState){}; // discard progress
		}
		return 0;
	}
}

int
main()
{
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		perror(fileName);
		exit(1);
	}

	int total = 0;

	// we're looking for "mul(" num "," num ")"
	struct MulState state = {};

	int c;
	while ((c = fgetc(file)) && c != EOF) {
		//printf("read %c\n", c);
		total += advance(&state, c);
	}

	printf("Sum of multiplications: %d\n", total);

	return 0;
}
