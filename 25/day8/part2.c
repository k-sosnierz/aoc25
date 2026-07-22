#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "structs.h"

// https://adventofcode.com/2025/day/8

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

int
intCmp(const void *ap, const void *bp)
{
	int a = *((int*)ap);
	int b = *((int*)bp);
	if (a<b) return -1;
	if (b<a) return 1;
	return 0;
}

static inline
uint64_t
max(uint64_t a, uint64_t b)
{
	if (a>b)
		return a;
	return b;
}

static inline
uint64_t
min(uint64_t a, uint64_t b)
{
	if (a<b)
		return a;
	return b;
}

uint8_t
getOctant(struct Point point, struct Point middle)
{
	uint8_t ret = 0;
	if (point.c[0] > middle.c[0])
		ret += 4;
	if (point.c[1] > middle.c[1])
		ret += 2;
	if (point.c[2] > middle.c[2])
		ret += 1;
	return ret;
}

uint64_t
ipow(uint64_t in, int exp)
{
	uint64_t ret = 1;
	for (;;)
	{
		if (exp & 1)
			ret *= in;
		exp >>= 1;
		if (!exp)
			break;
		in *= in;
	}
	return ret;
}

uint64_t
distanceSq(struct Point *a, struct Point *b)
{
	// note we don't need to take the square root;
	// we want to compare distances, not find their actual values
	uint64_t ret = 0;
	for (int i=0; i<3; i++)
		ret += ipow(a->c[i] - b->c[i], 2);
	return ret;
}

void
makeSubtree(struct Octree *tree, int octant)
{
	tree->children[octant] = err_malloc(sizeof(struct Octree), 
		"alloc octree in makeSubtree fail");
	struct Octree *subtree = tree->children[octant];

	// 4-7 and 0-3
	if (octant & 4) {
		subtree->bound.lower.c[0] = tree->mid.c[0] + 1;
		subtree->bound.upper.c[0] = tree->bound.upper.c[0];
	} else {
		subtree->bound.lower.c[0] = tree->bound.lower.c[0];
		subtree->bound.upper.c[0] = tree->mid.c[0];
	}

	// 2,3,6,7 and 0,1,4,5
	if (octant & 2) {
		subtree->bound.lower.c[1] = tree->mid.c[1] + 1;
		subtree->bound.upper.c[1] = tree->bound.upper.c[1];
	} else {
		subtree->bound.lower.c[1] = tree->bound.lower.c[1];
		subtree->bound.upper.c[1] = tree->mid.c[1];
	}

	// odd and even
	if (octant & 1) {
		subtree->bound.lower.c[2] = tree->mid.c[2] + 1;
		subtree->bound.upper.c[2] = tree->bound.upper.c[2];
	} else {
		subtree->bound.lower.c[2] = tree->bound.lower.c[2];
		subtree->bound.upper.c[2] = tree->mid.c[2];
	}

	for (int i=0; i<3; i++)
		subtree->mid.c[i] = (
			subtree->bound.lower.c[i] +
			subtree->bound.upper.c[i]
		) / 2;

	subtree->isLeaf = true;
}

void
putJunctionBoxInTree(struct JunctionBox *junctionBox, struct Octree *tree)
{
	if (tree->isLeaf) {
		if (tree->junctionBox == NULL) {
			tree->junctionBox = junctionBox;
		} else {
			tree->isLeaf = false;
			int octant = getOctant(junctionBox->loc, tree->mid);
			if (tree->children[octant] == NULL)
				makeSubtree(tree, octant);
			putJunctionBoxInTree(junctionBox, tree->children[octant]);
			putJunctionBoxInTree(tree->junctionBox, tree);
			tree->junctionBox = NULL;
		}
	} else {
		int octant = getOctant(junctionBox->loc, tree->mid);
		if (tree->children[octant] == NULL)
			makeSubtree(tree, octant);
		putJunctionBoxInTree(junctionBox, tree->children[octant]);
	}
}

static inline
bool
isPointInSphere(struct Point point, struct Point centre, int radius)
{
	return (distanceSq(&point, &centre) < radius*radius);
}

bool
doesBoxTouchSphere(struct Box box, struct Point centre, int radius)
{
	struct Point point;
	for (int i=0; i<3; i++)
		point.c[i] = max(box.lower.c[i], min(centre.c[i], box.upper.c[i]));
	
	return (distanceSq(&point, &centre) < radius*radius);
}

// returns an array of junction box IDs in radius
// but first element is actually the number of them
int *
getBoxesAroundPoint(struct Point centre, int radius, struct Octree *tree)
{

	int* buffer = err_malloc(sizeof(int)*1024,
		"alloc int buffer in getBoxesAroundPoint fail");

	if (tree->isLeaf &&
	isPointInSphere(tree->junctionBox->loc, centre, radius)) {
		buffer[0] = 1;
		buffer[1] = tree->junctionBox->id;
		buffer = realloc(buffer, 2*sizeof(int));
		return buffer;
	}
	
	int* ptr = buffer + 1;
	for (int i=0; i<8; i++) {
		struct Octree *child = tree->children[i];
		if (child && doesBoxTouchSphere(child->bound, centre, radius)) {
			int *partial = getBoxesAroundPoint(centre, radius, child);
			memcpy(ptr, &partial[1], sizeof(int)*partial[0]);
			buffer[0] += partial[0];
			ptr += partial[0];
			free(partial);
		}
	}
	buffer = realloc(buffer, (1+buffer[0])*sizeof(int));
	return buffer;
}

int
connectionCmp(const void *ap, const void *bp)
{
	struct Connection *a = (struct Connection *) ap;
	struct Connection *b = (struct Connection *) bp;

	// push 0 to the end 
	if (a->distanceSq == 0) return 1;
	if (b->distanceSq == 0) return -1;


	if (a->distanceSq < b->distanceSq) return -1;
	if (a->distanceSq > b->distanceSq) return 1;
	return 0;
}

struct Connection *
findShortestConnections(struct State state, int n, int minNeigh)
{
	struct Connection *ret = err_malloc(n*sizeof(struct Connection),
		"alloc ret array in findShortestConnections fail");

	for (int id=0; id<state.noJunctionBoxes; id++) {
		struct Point a = state.junctionBoxes[id].loc;
		int* ids = NULL;
		int radius = 512;
		do {
			free(ids);
			ids = getBoxesAroundPoint(a, radius, &state.tree);
			radius *= 2;
		} while (ids[0] < minNeigh);
		for (int i=0; i<ids[0]; i++) {
			if (id == ids[i+1])
				continue;
			struct Connection newCon = {0};
			newCon.a = &(state.junctionBoxes[min(id, ids[i+1])]);
			newCon.b = &(state.junctionBoxes[max(id, ids[i+1])]);
			newCon.distanceSq = distanceSq(&newCon.a->loc, &newCon.b->loc);

			// filter for duplicates
			bool duplicate = false;
			for (int j=n-1; j>=0; j--)
				if (ret[j].a == newCon.a && ret[j].b == newCon.b)
					duplicate = true;

			// replace largest number (or blank field)
			if (!duplicate) {
			for (int j=n-1; j>=0; j--) {
				if (ret[j].distanceSq == 0 || ret[j].distanceSq > newCon.distanceSq) {
					ret[j] = newCon;
					qsort(ret, n, sizeof(struct Connection), connectionCmp);
					break;
				}
			}
			}
		}
	}

	return ret;
}

struct State
parse(const char *fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		perror("fopen failed");
		exit(1);
	}

	struct State state = {0};
	state.junctionBoxes = err_malloc(sizeof(struct JunctionBox)*4096,
		"alloc state.points array in parse fail");
	struct Point maxCoords = {0};
	
	char buf[32] = {0};

	// read points line by line, set bounds, trim the alloc
	while (fgets(buf, 32, file)) {
		char *number = buf - 1;
		struct JunctionBox *junctionBox =
			&state.junctionBoxes[state.noJunctionBoxes];
		struct Point *point = &junctionBox->loc;
		for (int i=0; i<3; i++) {
			point->c[i] = strtoull(++number, &number, 10);
			if (point->c[i] > maxCoords.c[i])  
				maxCoords.c[i] = point->c[i];
		}
		junctionBox->id = state.noJunctionBoxes;
		state.noJunctionBoxes++;
	}

	state.tree.bound.upper = maxCoords;
	state.tree.mid = maxCoords;
	state.tree.isLeaf = true;
	for (int i=0; i<3; i++)
		state.tree.mid.c[i] /= 2;
	state.junctionBoxes = realloc(state.junctionBoxes, sizeof(struct JunctionBox)*state.noJunctionBoxes);
	state.circuits = err_malloc(sizeof(struct JunctionBox *)*state.noJunctionBoxes,
		"alloc circuits array fail");

	for (int i = 0; i < state.noJunctionBoxes; i++)
		state.circuits[i] = &state.junctionBoxes[i];

	// insert the junction boxes into the tree
	// we assume that no two boxes share the exact same location.
	// this assumption is not frivolous, as it's junction boxes
	// we're talking about, after all.

	for (int i = 0; i < state.noJunctionBoxes; i++)
		putJunctionBoxInTree(&state.junctionBoxes[i], &state.tree);

	return state;
}

void
connect(struct Connection connection, struct State *state)
{
	// first, check if they're not in one circuit already
	int bId = connection.b->id;
	for (struct JunctionBox *ptr = connection.a; ptr != NULL; ptr = ptr->next)
		if (ptr->id == bId)
			return;
	for (struct JunctionBox *ptr = connection.a; ptr != NULL; ptr = ptr->prev)
		if (ptr->id == bId)
			return;

	// alright, they aren't. put b's circuit at the end of a's circuit
	
	struct JunctionBox *bStart = connection.b;
	while (bStart->prev != NULL)
		bStart = bStart->prev;
	int bCircuitId = bStart->id;

	struct JunctionBox *aEnd = connection.a;
	while (aEnd->next != NULL)
		aEnd = aEnd->next;
	
	aEnd->next = bStart;
	bStart->prev = aEnd;
	
	state->circuits[bCircuitId] = NULL;
}

int
getCircuitLength(struct JunctionBox *circuit)
{
	int ret = 0;
	for (struct JunctionBox *ptr = circuit; ptr != NULL; ptr = ptr->next)
		ret++;
	return ret;
}

void
sanityCheck(struct Octree *tree)
{
	if (tree->isLeaf) {
		bool invalid = false;
		for (int i=0; i<3; i++) {
			if (tree->junctionBox->loc.c[i] < tree->bound.lower.c[i] ||
				tree->junctionBox->loc.c[i] > tree->bound.upper.c[i])
				invalid = true;
		}
		if (invalid) {
			printf("Box [%ld, %ld, %ld] in bound: [%ld, %ld, %ld] to [%ld, %ld, %ld]\n",
			tree->junctionBox->loc.c[0], tree->junctionBox->loc.c[1], tree->junctionBox->loc.c[2],
			tree->bound.lower.c[0], tree->bound.lower.c[1], tree->bound.lower.c[2],
			tree->bound.upper.c[0], tree->bound.upper.c[1], tree->bound.upper.c[2]
			);
			exit(1);
		}
		return;
	}

	for (int i=0; i<8; i++) {
		if (tree->children[i])
			sanityCheck(tree->children[i]);
	}
}

int
main()
{
	struct State state = parse(fileName);

	sanityCheck(&state.tree);

	int noConnections = 6000; // basically an upper-bound guess; needs to be higher than the proper answer (what we're looking for), but low enough to compute quickly
	int minNeighbours = 5; // arbitrary, results may be wrong if set too low
	struct Connection *shortest =
		findShortestConnections(state, noConnections, minNeighbours);

	int *circuitLengths = err_malloc(sizeof(int)*state.noJunctionBoxes,
			"alloc circuit lengths array in main fail");

	uint64_t xMultiplied;

	for (int i=0; i<noConnections; i++) {
		connect(shortest[i], &state);
		xMultiplied = (uint64_t) 1 * shortest[i].a->loc.c[0] * shortest[i].b->loc.c[0];

		for (int j=0; j<state.noJunctionBoxes; j++)
			if (getCircuitLength(state.circuits[j]) == state.noJunctionBoxes)
				i = noConnections; // exit loop; goal achieved
	}

	printf("The answer: %lu\n", xMultiplied);

	return 0;
}
