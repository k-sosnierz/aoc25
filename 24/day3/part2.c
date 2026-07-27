#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/mman.h>


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

bool
substringAt(char *substr, char *buffer, int index, int size)
{
	if (index + strlen(substr) > size)
		return false;

	if (strncmp(&buffer[index], substr, strlen(substr)) == 0)
		return true;
	else
		return false;
}

int
main()
{
	// let's copy the file to memory, so we can reuse the logic from part1
	// for interpreting muls, and check do() & don't() with string compares
	
	char *buffer;
	FILE* file = fopen(fileName, "rb");
	if (file == NULL) {
		perror(fileName);
		exit(1);
	}
	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	buffer = malloc(fileSize);
	fseek(file, 0, SEEK_SET);
	size_t bytesRead = fread(buffer, 1, fileSize, file);
	fclose(file);

	int total = 0;

	struct MulState state = {};
	bool mulsEnabled = true;

	int c;
	for (int i=0; i<fileSize; i++) {
		if (substringAt("do()", buffer, i, fileSize)) {
			i += strlen("do()"); 
			mulsEnabled = true;
		}
		if (substringAt("don't()", buffer, i, fileSize)) {
			i += strlen("don't()");
			mulsEnabled = false;
		}
		c = buffer[i];
		//printf("read %c\n", c);
		if (mulsEnabled)
			total += advance(&state, c);
	}

	printf("Sum of multiplications: %d\n", total);

	return 0;
}
