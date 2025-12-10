#include <stdint.h>
#include <stdbool.h>

// https://adventofcode.com/2025/day/7

struct Point {
	uint64_t c[3]; // coords X, Y, Z
};

struct JunctionBox {
	struct Point loc;
	int id;

	// for circuit linked lists
	struct JunctionBox *prev;
	struct JunctionBox *next;
};

struct Box {
	struct Point lower;  // 0, 0, 0
	struct Point upper; // x, y, z
};

struct Octree {
	struct Box bound;
	struct Point mid;
	// see getOctant
	struct Octree *children[8];
	struct JunctionBox *junctionBox;
	bool isLeaf;
};

struct State {
	struct Octree tree;
	struct JunctionBox *junctionBoxes;
	int noJunctionBoxes;
	struct JunctionBox **circuits;
};

struct Connection {
	struct JunctionBox *a;
	struct JunctionBox *b;
	uint64_t distanceSq;
};
