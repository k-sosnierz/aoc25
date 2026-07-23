#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


const char* fileName = "input.txt";

// comparison function for qsort
int
compare(const void* aptr, const void* bptr)
{
	int a = *(int*)aptr;
	int b = *(int*)bptr;
	if (a < b)
		return -1;
	if (a == b)
		return 0;
	if (a > b)
		return 1;
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

	int size = 0;
	int* l = nullptr;
	int* r = nullptr;

	char line[20];
	while (fgets(line, 20, file)) {
		size++;
		l = realloc(l, size*sizeof(int));
		r = realloc(r, size*sizeof(int));
		sscanf(line, "%d %d\n", &l[size-1], &r[size-1]);
	}

	qsort(l, size, sizeof(int), compare);
	qsort(r, size, sizeof(int), compare);

	for (int i=0; i<size; i++) {
		printf("%d, %d: %d\n", l[i], r[i], abs(l[i] - r[i]));
		total += abs(l[i] - r[i]);
	}

	printf("total: %d\n", total);

	return 0;
}
