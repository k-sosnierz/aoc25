#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
	uint32_t width; // number of pipelines
	uint32_t length; // numbers per pipeline
	uint64_t *operands; // grouped by pipeline
	enum Operator *operators;
};

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


	// fastest way to get the length: count newlines -1
	char* c = buf;
	while (c = strchr(c+1, '\n')) {
		ret.length++;
	}
	ret.length--;

	// fastest way to get the width: count the + and *
	// in the last line
	c = buf;
	for (int i = 0; i < ret.length; i++) {
		c = strchr(buf,'\n')+1;
	}
	while (*c != 0) {
		if (*c == '*' || *c == '+')
			ret.width++;
		c++;
	}

	//printf("%d\n", ret.width);
	//printf("%d\n", ret.length);

	// allocate memory
	ret.operators = err_malloc(sizeof(enum Operator) * ret.width, "alloc input operators array fail");
	ret.operands = err_malloc(sizeof(uint64_t)*ret.width*ret.length, "alloc input operands array fail");

	// fill operands
	c = buf;
	for (int i = 0; i<ret.length; i++) {
		for (int j = 0; j<ret.width; j++) {
			// blocks of operands grouped by pipeline
			ret.operands[ret.length*j+i] = strtoull(c, &c, 10);
		}
		c = strchr(c, '\n') + 1;
	}

	// fill operators
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

	return ret;
}

uint64_t
evaluate(struct Input *input, int j)
{
	uint64_t ret = 0;
	if (input->operators[j] == multiply)
		ret = 1;

	for (int i = 0; i < input->length; i++) {
		uint64_t val = input->operands[j*input->length+i];
		if (input->operators[j] == sum)
			ret += val;
		else
			ret *= val;
	}

	//printf("%lu\n", ret);

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
