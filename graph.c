#include <stdio.h>
#include <stdlib.h>

#include "graphLib.c"
#include "path.c"
#include "statics.c"

typedef unsigned char byte;
#define ruint register uint

#define MAIN_VERBOSE 1

#define VERBOSE_LOADING 0
byte loadGraph(graph_t *g, char *filename);

byte getDegrees(graph_t *g, uint **list);
uint* getDistances(graph_t *g);
uint countTriangles(graph_t *g);

void printGraph(graph_t *g);
void printList(uint *list, uint len);

byte write(char *filename, uint *list, uint len);

#define FILENAME "facebook.txt"
int main() {
	graph_t *g = newGraph();
	char *filename = FILENAME;

	loadGraph(g, filename);
#if MAIN_VERBOSE == 1
	printf("Graph Loading: Done!\n\n");
#endif // MAIN_VERBOSE == 1

	//printGraph(g);

	uint *degreeList;
	getDegrees(g, &degreeList);
#if MAIN_VERBOSE == 1
	printf("Degree Calculation: Done!\n");
#endif // MAIN_VERBOSE == 1
	//printList(degreeList, g->size);
	write("degree_" FILENAME, degreeList, g->size);
#if MAIN_VERBOSE == 1
	printf("Degree Writing: Done!\n\n");
#endif // MAIN_VERBOSE == 1
	free(degreeList);


	uint *distList;
	distList = getDistances(g);
#if MAIN_VERBOSE == 1
	printf("Distance Calculation: Done!\n");
#endif // MAIN_VERBOSE == 1
	write("distances_" FILENAME, distList, g->size);
#if MAIN_VERBOSE == 1
	printf("Distance Writing: Done!\n\n");
#endif // MAIN_VERBOSE == 1
	//printList(distList, g->size);
	free(distList);

	/*uint triangleCount;
	triangleCount = countTriangles(g);
	printf("%u\n%u\n", triangleCount, triangleCount*3);
	uint tmp = g->size;
	printf("max:%u\n", tmp*(tmp-1)*(tmp-2)/6);
	*/
	
	return 0;
}

byte loadGraph(graph_t *g, char *filename) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		return 1;
	}

	while (!feof(f)) {
		uint first_id, second_id;
		fscanf(f, "%u[ \t]%u\n", &first_id, &second_id);
#if VERBOSE_LOADING == 1
		printf("LOADING: sid = %u fid = %u g->size=%u\n", second_id, first_id, g->size);
#endif // VERBOSE_LOADING == 1
		while (g->size <= first_id || g->size <= second_id) {
			node_t *n = newNode(n);
			if (!n) {
#if VERBOSE_LOADING == 1
				printf("ERROR_LOADING_ERROR: Couldn't create node\n");
#endif // VERBOSE_LOADING == 1
				return 2;
			}
			if (addNode(g, n)) {
#if VERBOSE_LOADING == 1
				printf("ERROR_LOADING_ERROR: Couldn't add node\n");
#endif // VERBOSE_LOADING == 1
				return 3;
			}
#if VERBOSE_LOADING == 1
	printf("LOADING: newNode (while): n-> id=%u size=%u len=%u p=%p *p=%p\n", n->id, n->size, n->len, n->next, *n->next);
#endif // VERBOSE_LOADING == 1
		}
		node_t *n1 = getNode(g, first_id), *n2 = getNode(g, second_id);
		if (!n1) {
			printf("ERROR_LOADING_ERROR: Empty n1\n");
			return 4;
		}
		if (!n2) {
			printf("ERROR_LOADING_ERROR: Empty n2\n");
			return 5;
		}
		linkNode(n1, n2);
		// Comment next line if the file has directional links
		linkNode(n2, n1);
#if VERBOSE_LOADING == 1
		printf("LOADING: nodelink: %u %u\n", first_id, second_id);
#endif // VERBOSE_LOADING == 1
	}
	fclose(f);
	return 0;
}

byte getDegrees(graph_t *g, uint **list) {
	uint *degreeList = (uint *) malloc(sizeof(*degreeList)*g->size);
	if (!degreeList) {
		return 1;
	}
	for (ruint i = 0; i < g->size; i++) {
		*(degreeList + i) = (*(g->nodes + i))->size;
	}
	*list = degreeList;
	return 0;
}

uint* getDistances(graph_t *g) {
	uint *distancesList = (uint *) malloc(sizeof(*distancesList)*g->size);
	if (!distancesList) {
		return (uint *) NULL;
	}

	uint start = 0;
	while (start < g->size) {
		qpath_t *path = newQPath();
		qpath_t *next_path = newQPath();
		byte visitedList[g->size];
		for (ruint i = 0; i < g->size; i++) {
			visitedList[i] = 0;
		}

		enq(path, getNode(g, start));
		visitedList[start] = 1;

		uint steps = 0, maxSteps = 0xFFFFFFFF;
		while(path->end || next_path->end) {
			if (!path->end) {
				if (maxSteps < steps) {
					steps = maxSteps;
					break;
				}
				while(next_path->end) {
					enq(path, deq(next_path));
				}
				steps += 1;
			}
			node_t *n = deq(path);
			if (!n) {
				printf("ERROR: EMPTY NODE\n");
				return (uint *) NULL;
			}
			for (ruint i = 0; i < n->size; i++) {
				node_t *side = *(n->next + i);
				if (!visitedList[side->id]) {
					visitedList[side->id] = 1;
					if (side->id < start) {
						ruint distance = *(distancesList + side->id) + steps;
						if (maxSteps < distance) {
							maxSteps = distance;
						}
					}
					enq(next_path, side);
				}
			}
		}
		if (!start) {
			// if visitedList is not complete printf
			for (ruint i = 0; i < g->size; i++) {
				if (!visitedList[i]) {
					printf("IT IS NOT A COMPLETE GRAPH\n");
					printf("Disconected Vertices: ");
					for (ruint ii = i; ii < g->size; ii++) {
						if (!visitedList[ii]) {
							printf("%u ", ii);
						}
					}
					printf("\n");
					//printf("Node %u NOT COMPLETE\n", start);
					break;
				}
			}
		}
		*(distancesList + start) = steps;
		start += 1;
	}
	return distancesList;
}

// May not work properly if the graph is has diretional links
uint countTriangles(graph_t *g) {
	uint count = 0;
	for (ruint start = 0; start < g->size; start++) { //For each node
		node_t *n = getNode(g, start);
		for (ruint i = 0; i < n->size; i++) { // For each side1
			node_t *side = *(n->next + i);
			if (side->id < start) {
				continue;
			}
			for (ruint j = 0; j < side->size; j++) { // For each side1's side
				node_t *sideside = *(side->next + j);
				if (sideside->id < start) {
					continue;
				}
				for (ruint k = i+1; k < n->size; k++) { // For each side2
					node_t *side2 = *(n->next + k);
					if (side2->id < start) {
						continue;
					}
					if (sideside == side2) {
						count += 1;
					}
				}
			}
		}
	}
	return count;
}

void printGraph(graph_t *g) {
	for(ruint i = 0; i < g->size; i++) {
		node_t *n = getNode(g, i);
		printf("%u: ", n->id);
		for(ruint j = 0; j < n->size; j++) {
			printf("%u%c", (*(n->next+j))->id, j+1-n->size?32:10);
		}
		putchar(10);
	}
}

void printList(uint *list, uint len) {
	for (ruint i = 0; i < len; i++) {
		printf("%u: %u\n", i, *(list + i));
	}
}

byte write(char *filename, uint *list, uint len) {
	FILE *f = fopen(filename, "w");
	if (!f) {
		return 1;
	}
	uint max, min, median;
       	double mean, variation;
	if (statics(list, len, &max, &min, &mean, &median, &variation)) {
		printf("ERROR %s", filename);
	} else {
	fprintf(f, "max=%u, min=%u, mean=%lf, median=%u, variation=%lf\n", max, min, mean, median, variation);
	}
	for (ruint i = 0; i < len; i++) {
		fprintf(f, "%u\n", *(list + i));
	}
	fclose(f);
	return 0;
}
