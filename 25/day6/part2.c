#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// https://adventofcode.com/2025/day/6

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

enum Operator {
	sum = 0,
	multiply = 1
};

struct Input {
	uint32_t width; // number of operators
	uint32_t length; // number of lines of values
	uint32_t *operands; // assume max 8 per operator? UINT64_MAX for empty lines
	enum Operator *operators;
};

uint32_t
verticalNumber(char* start, int offset, int n)
{
	int ret = 0;
	bool anyDigitFound = false;

	for (int i = 0; i < n; i++) {
		int digit = start[offset*i] - '0';
		if (digit >= 0 && digit <= 9) {
			ret = ret * 10 + digit;
			anyDigitFound = true;
		}
	}

	if (!anyDigitFound)
		return UINT32_MAX;

	return ret;
}

struct Input
parse(const char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		perror("fopen failed");
		exit(1);
	}

	struct Input ret = {0};

	// get file size, alloc buffer, put file in it
	fseek(file, 0L, SEEK_END);
	uint32_t fileSize = ftell(file);
	rewind(file);
	char *buf = err_malloc(fileSize + 1,
			"parse buf alloc fail");
	fread(buf, 1, fileSize, file);

	// get line width
	int lineWidth = strchr(buf, '\n') - buf + 1;

	// get value length in rows
	ret.length = fileSize / lineWidth - 1;

	// fastest way to get the width: count the + and *
	// in the last line
	char *c = buf + lineWidth * ret.length;
	while (*c != 0) {
		if (*c == '*' || *c == '+')
			ret.width++;
		c++;
	}

	//printf("%d\n", ret.width);
	//printf("%d\n", ret.length);

	// allocate memory
	ret.operators = err_malloc(sizeof(enum Operator) * ret.width, "alloc input operators array fail");
	ret.operands = err_malloc(sizeof(uint64_t)*ret.width*8, "alloc input operands array fail");

	// fill operators
	c = buf;
	for (int i = 0; i<ret.width; i++) {
		char *op = strpbrk(c, "+*");
		switch (*op) {
			case '*':
				ret.operators[i] = multiply;
				break;
			case '+':
				ret.operators[i] = sum;
				break;
		};
		c = op + 1;
	}

	// fill operands
	int currentOperator = -1;
	int currentOperand = 0;
	for (int i = 0; i < lineWidth; i++) {
		c = buf + lineWidth*ret.length + i;
		if (*c == '*' || *c == '+') {
			currentOperator++;
			currentOperand = 0;
		}
		int num = verticalNumber(buf+i, lineWidth, ret.length);
		ret.operands[8*currentOperator + currentOperand] = num;
		currentOperand++;
	}

	return ret;
}

uint64_t
evaluate(struct Input *input, int j)
{
	uint64_t ret = 0;
	if (input->operators[j] == multiply)
		ret = 1;

	for (int i = 0; i < 8; i++) {
		uint64_t val = input->operands[j*8+i];
		if (val == UINT32_MAX)
			break;

		if (input->operators[j] == sum)
			ret += val;
		else if (input->operators[j] == multiply)
			ret *= val;
	}


	return ret;
}

int
main()
{
	uint64_t sum = 0;
	struct Input input = parse(fileName);

	for (int i = 0; i < input.width; i++)
		sum += evaluate(&input, i);

	printf("The sum of the results is %ld\n", sum);
	return 0;
}
