#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// https://adventofcode.com/2025/day/10

const char *fileName = "input";

struct IntArray {
	int n;
	int *data;
};

bool
intInIntArray(int a, struct IntArray array)
{
	bool ret = false;

	for (int i=0; i<array.n; i++)
		if (array.data[i] == a)
			ret = true;

	return ret;
}

struct Device {
	int id; // id == (name[0] * 256 + name[1]) * 256 + name[2]
	struct IntArray outputs;
};

struct Probe {
	int device;
	struct IntArray path; // stores the ids of devices passed
};

struct Queue {
	struct Probe **probes;
	int count;
};

int
deviceCmp(const void *lp, const void *rp)
{
	const struct Device *l = lp;
	const struct Device *r = rp;
	if (l->id < r->id)
		return -1;
	if (l->id > r->id)
		return 1;
	return 0;
}

int
nameToInt(char *c)
{
	return ((256*c[0] + c[1])*256 + c[2])*256;
}

struct Probe *
dequeue(struct Queue* queue)
{
	struct Probe* ret = NULL;
	if (queue->count != 0) {
		ret = queue->probes[0];
		queue->count--;
		memmove(queue->probes, queue->probes+1, queue->count*sizeof(struct Probe *));
		queue->probes = realloc(queue->probes, queue->count*sizeof(struct Probe *));
	}
	
	return ret;
}

void
enqueue(struct Queue* queue, struct Probe *probe)
{
	queue->count++;
	queue->probes = realloc(queue->probes, queue->count*sizeof(struct Probe *));
	queue->probes[queue->count-1] = probe;
}

struct Input {
	struct Device *devices;
	int noDevices;
};

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

struct IntArray
appendIntArray(struct IntArray in, int n)
{
	struct IntArray ret = {
		.n = in.n + 1,
		.data = err_malloc(sizeof(int)*(in.n+1),
			"appendIntArray alloc fail"),
	};
	
	memcpy(ret.data, in.data, sizeof(int)*in.n);
	ret.data[in.n] = n;

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

	struct Input input = {0};
	
	char buf[128] = {0};

	while (fgets(buf, 128, file)) {
		input.noDevices++;
		input.devices = realloc(input.devices,
			sizeof(struct Device)*input.noDevices);
		struct Device *d = &input.devices[input.noDevices-1];
		*d = (struct Device){0};

		d->id = nameToInt(&buf[0]);

		// get outputs
		char *c = strchr(buf, ':') + 1;
		while (*c != '\n') {
			switch (*c) {
				case ' ':
					c++;
					break;
					
				default: // letter
					struct IntArray tmp =
						appendIntArray(d->outputs, nameToInt(c));
					free(d->outputs.data);
					d->outputs = tmp;
					c += 3;
					break;
			}
		}
	}

	qsort(input.devices, input.noDevices, sizeof(struct Device), deviceCmp);

	return input;
}

struct Probe *
makeChildProbe(struct Probe *parent, int next)
{
	struct Probe *ret = err_malloc(sizeof(struct Probe),
		"alloc struct probe in makeChildProbe fail");

	ret->device = next;
	ret->path.n = parent->path.n + 1;
	ret->path.data = err_malloc(sizeof(int)*ret->path.n,
		"alloc probe path data array in makeChildProbe fail");
	memcpy(ret->path.data, parent->path.data, parent->path.n*sizeof(int));
	ret->path.data[parent->path.n] = next;

	return ret;
}

int
getPathsForDeviceTree(struct Input input)
{
	int ret = 0;

	struct Queue queue = {0};

	struct Probe first = {0};
	first.device = nameToInt("you");
	enqueue(&queue, &first);

	int out = nameToInt("out");

	while (queue.count != 0) {
		struct Probe *probe = dequeue(&queue);
		if (probe->device == out) {
			ret++;
			continue;
		}
		//printf("%c%c%c%c\n", probe->device >> 24, probe->device >>16, probe->device >>8, probe->device);
		struct Device key = {probe->device, 0};
		struct Device *device = bsearch(&key, input.devices,
			input.noDevices, sizeof(struct Device), deviceCmp);
		for (int i=0; i < device->outputs.n; i++) {
			int nextDevice = device->outputs.data[i];
			if (!intInIntArray(nextDevice, probe->path)) {
				enqueue(&queue, makeChildProbe(probe, nextDevice));
			}
		}
	}

	free(queue.probes);

	return ret;
}

int
main()
{
	struct Input input = parse(fileName);

	int paths = getPathsForDeviceTree(input);

	printf("Paths: %d\n", paths);

	return 0;
}
