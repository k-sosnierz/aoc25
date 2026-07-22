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

struct Machine {
	uint32_t targetState;
	uint32_t *buttons;
	int noButtons;
};

struct TreeNode {
	struct TreeNode *parent;
	struct TreeNode **children; // machine->noButtons size

	struct Machine *machine;
	uint32_t machineState;
	struct IntArray buttonsPressed;
};

struct NodeQueue {
	struct TreeNode **nodes;
	int count;
};

struct Answer {
	int presses;
	bool finished;
};

bool
intFollowsIntArray(struct IntArray array, int a)
{
	bool ret = true;

	for (int i=0; i<array.n; i++)
		if (array.data[i] >= a)
			ret = false;

	return ret;
}

struct TreeNode *
dequeue(struct NodeQueue* queue)
{
	struct TreeNode* ret = NULL;
	if (queue->count != 0) {
		ret = queue->nodes[0];
		queue->count--;
		memmove(queue->nodes, queue->nodes+1, queue->count*sizeof(struct TreeNode *));
		queue->nodes = realloc(queue->nodes, queue->count*sizeof(struct TreeNode *));
	}
	
	return ret;
}

void
enqueue(struct NodeQueue* queue, struct TreeNode *node)
{
	queue->count++;
	queue->nodes = realloc(queue->nodes, queue->count*sizeof(struct TreeNode *));
	queue->nodes[queue->count-1] = node;
}

struct Input {
	struct Machine *machines;
	int noMachines;
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

struct Input
parse(const char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		perror("fopen failed");
		exit(1);
	}

	struct Input input = {0};
	
	char buf[256] = {0};

	// read points line by line, set bounds, trim the alloc
	while (fgets(buf, 256, file)) {
		input.noMachines++;
		input.machines = realloc(input.machines,
			sizeof(struct Machine)*input.noMachines);
		struct Machine *m = &input.machines[input.noMachines-1];
		*m = (struct Machine){0};

		// get target state
		for (int i=1; buf[i] != ']'; i++) {
			if (buf[i] == '#')
				m->targetState += (1<<(i-1));
		}

		// parse the button data
		// stateSize <= 10, so all the numbers here are single digit
		char *c = buf;
		while (*c != '{') {
			switch (*c) {
				case '(':
					m->noButtons++;
					m->buttons = realloc(m->buttons,
						sizeof(uint32_t)*(m->noButtons));
					m->buttons[m->noButtons-1] = 0;
					break;
					
				default:
					if (*c - '0' < 10 && *c - '0' >= 0) // is digit
						m->buttons[m->noButtons-1] |= 1<<(*c-'0');
					break;
			}
			c++;
		}
	}

	return input;
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

struct TreeNode *
makeChildNode(struct TreeNode *parent, int n)
{
	struct TreeNode *ret = err_malloc(sizeof(struct TreeNode),
		"alloc struct node in makeChildNode fail");
	
	ret->parent = parent;
	ret->children = err_malloc(sizeof(struct TreeNode *)*(parent->machine->noButtons),
		"alloc children array in makeChildNode fail");

	ret->machine = parent->machine;
	ret->machineState = parent->machineState;

	ret->buttonsPressed = appendIntArray(parent->buttonsPressed, n);

	ret->machineState ^= ret->machine->buttons[n];

	return ret;
}

struct Answer
checkNode(struct TreeNode *node)
{
	struct Answer ret = {
		.presses = node->buttonsPressed.n,
		.finished = (node->machineState == node->machine->targetState)
	};

	return ret;
}

int
getFewestPressesForMachine(struct Machine *machine)
{
	// breadth first search
	struct NodeQueue queue = {0};

	struct TreeNode head = {0};
	head.machine = machine;
	head.children = malloc(sizeof(struct TreeNode *)*machine->noButtons);
	enqueue(&queue, &head);

	struct Answer answer = {0, false};
	while (answer.finished == false) {
		struct TreeNode *node = dequeue(&queue);
		for (int i=0; i<machine->noButtons; i++) {
			if (intFollowsIntArray(node->buttonsPressed, i)) {
				node->children[i] = makeChildNode(node, i);
				enqueue(&queue, node->children[i]);
			}
		}
		answer = checkNode(node);
	}

	free(queue.nodes);

	return answer.presses;
}

int
main()
{
	struct Input input = parse(fileName);

	int presses = 0;

	for (int i=0; i < input.noMachines; i++) {
		int p = getFewestPressesForMachine(&input.machines[i]);
		presses += p;
	}

	printf("Presses needed: %d\n", presses);

	return 0;
}
