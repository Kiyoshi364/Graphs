#include <stdio.h>
#include <stdlib.h>

#include "graphLib.c"
#include "path.c"
#include "statics.c"

typedef unsigned char byte;
#define ruint register uint

#define MAIN_VERBOSE 1
#define VERBOSE_LOADING 0
#define VERBOSE_DISTANCE 0
#define VERBOSE_CLUSTERS 0

byte loadGraph(graph_t *g, char *filename, uint * links_count);

byte getDegrees(graph_t *g, uint **list);
uint* getDistances(graph_t *g, graph_t ***clusters, uint *clusterLen);
uint countTriangles(graph_t *g);

void printGraph(graph_t *g);
void printList(uint *list, uint len);

byte write(char *filename, uint *list, uint len);
byte writeCluster(char *filename, graph_t **clusters, uint len);

#define FILENAME "facebook.txt"
#define DIRECTIONAL_LINKS 1

int main() {
	graph_t *g = newGraph();
	uint links = 0;
	char *filename = FILENAME;

	if (loadGraph(g, filename, &links)) {
		printf("%s could not be open", FILENAME);
	}
#if MAIN_VERBOSE == 1
	printf("Graph Loading: Done!\n%u Nodes Loaded.\n%u Links Loaded.\n\n", g->size, links);
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


	uint *distList, clusterLen = 0;
	graph_t **clusters;
	distList = getDistances(g, &clusters, &clusterLen);
	if (clusterLen) {
#if MAIN_VERBOSE == 1
		printf("Cluster Calculation: Done!\n");
#endif // MAIN_VERBOSE == 1
		writeCluster("cluster_" FILENAME, clusters, clusterLen);
#if MAIN_VERBOSE == 1
		printf("Cluster Writing: Done!\n\n");
#endif // MAIN_VERBOSE == 1
		for (ruint i = 0; i < clusterLen; i++) {
			graph_t *g = *(clusters + i);
			free(g->nodes);
			free(g);
		}
		free(clusters);
	}

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

byte loadGraph(graph_t *g, char *filename, uint *links_count) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		return 1;
	}

	uint links = 0;
	while (!feof(f)) {
		uint first_id, second_id, sum = 0;
		int c;
		while ((c = fgetc(f)) != EOF && c != '\n') {
			if (c == ' ' || c == '\t') {
				first_id = sum;
				sum = 0;
			} else if (c >= '0' && c <= '9') {
				sum = sum*10 + c - '0';
			}
		}
		second_id = sum;
		//fscanf(f, "%u[ \t]%u\n", &first_id, &second_id);
#if VERBOSE_LOADING == 1
		printf("LOADING: fid = %u sid = %u g->size=%u\n", first_id, second_id, g->size);
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
		links += 1;
		// Comment next line if the file has directional links
#if DIRECTIONAL_LINKS == 1
		linkNode(n2, n1);
		links += 1;
#endif // DIRECTIONAL_LINKS == 1
#if VERBOSE_LOADING == 1
		printf("LOADING: nodelink: %u %u\n", first_id, second_id);
#endif // VERBOSE_LOADING == 1
	}
	fclose(f);
	*links_count = links;
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

uint* getDistances(graph_t *g, graph_t ***clusters, uint *clusterLen) {
	uint *distancesList = (uint *) malloc(sizeof(*distancesList)*g->size);
	if (!distancesList) {
		return (uint *) NULL;
	}

	uint start = 0, node_count;
	byte ToDoList[g->size];
	for (ruint i = 0; i < g->size; i++) {
		ToDoList[i] = 0;
	}
	while (start < g->size) {
		node_count = 0;
		qpath_t *path = newQPath();
		qpath_t *next_path = newQPath();
		byte visitedList[g->size];
		for (ruint i = 0; i < g->size; i++) {
			visitedList[i] = 0;
		}

		enq(path, getNode(g, start));
		visitedList[start] = 1;
		node_count += 1;

		uint steps = 0, maxSteps = 0xFFFFFFFF;
		while(path->end || next_path->end) {
			if (!path->end) {
				if (maxSteps < steps) {
					steps = maxSteps;
					break;
				}
				register qpath_t *tmp = path;
				path = next_path;
				next_path = tmp;
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
					enq(next_path, side);
					node_count += 1;
					if (side->id < start) {
						ruint distance = *(distancesList + side->id) + steps;
						if (maxSteps < distance) {
							maxSteps = distance;
						}
					}
				}
			}
		}
		if (!start) {
			if (node_count != g->size) {
				printf("IT IS NOT A COMPLETE GRAPH\n");
#if VERBOSE_CLUSTERS == 1
				printf("New Cluster:\n");
#endif // VERBOSE_CLUSTERS == 1
				*clusters = (graph_t **) malloc(sizeof(**clusters));
				**clusters = newGraph();
				for (ruint i = 0; i < g->size; i++) {
					if (visitedList[i]) {
						ToDoList[i] = 1;
						addNode(**clusters, *(g->nodes + i));
#if VERBOSE_CLUSTERS == 1
						printf("%u ", i);
#endif // VERBOSE_CLUSTERS == 1
					}
				}
				*clusterLen = 1;
#if VERBOSE_CLUSTERS == 1
				printf("\n\n");
#endif // VERBOSE_CLUSTERS == 1
			}
		} else if (ToDoList[0] && !ToDoList[start]) {
#if VERBOSE_CLUSTERS == 1
			printf("New Cluster:\n");
#endif // VERBOSE_CLUSTERS == 1
			*clusters = (graph_t **) realloc(*clusters, sizeof(**clusters)*(*clusterLen + 1));
			*(*clusters + *clusterLen) = newGraph();
			for (ruint i = 0; i < g->size; i++) {
				if (visitedList[i]) {
					ToDoList[i] = 1;
					addNode(*(*clusters + *clusterLen), *(g->nodes + i));
#if VERBOSE_CLUSTERS == 1
					printf("%u ", i);
#endif // VERBOSE_CLUSTERS == 1
				}
			}
			*clusterLen += 1;
#if VERBOSE_CLUSTERS == 1
			printf("\n\n");
#endif // VERBOSE_CLUSTERS == 1
			/*// if visitedList is not complete printf
			for (ruint i = 0; i < g->size; i++) {
				if (!visitedList[i]) {
					for (ruint ii = 0; ii < i; ii++) {
					}
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
			}*/
		}
#if VERBOSE_DISTANCE == 1
		if (!(start%10)) {
			printf("%u%c", start, (start-1)/10%10?0:10);
		}
#endif // VERBOSE_DISTANCE == 1
		*(distancesList + start) = steps;
		start += 1;
	}
#if VERBOSE_DISTANCE == 1
	printf("\n");
#endif // VERBOSE_DISTANCE == 1
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
		fprintf(f, "#max=%u, min=%u, mean=%lf, median=%u, variation=%lf\n", max, min, mean, median, variation);
	}
	for (ruint i = 0; i < len; i++) {
		fprintf(f, "%u\n", *(list + i));
	}
	fclose(f);
	return 0;
}

byte writeCluster(char *filename, graph_t **clusters, uint len) {
	FILE *f = fopen(filename, "w");
	if (!f) {
		return 1;
	}

	uint *list = (uint *) malloc(sizeof(*list)*len);
	if (!list) {
		fclose(f);
		return 2;
	}
	for (ruint i = 0; i < len; i++) {
		graph_t *g = *(clusters + i);
		*(list + i) = g->size;
	}

	uint max, min, median;
       	double mean, variation;
	if (statics(list, len, &max, &min, &mean, &median, &variation)) {
		printf("ERROR %s", filename);
	} else {
		fprintf(f, "#max=%u, min=%u, mean=%lf, median=%u, variation=%lf\n", max, min, mean, median, variation);
	}
	for (ruint i = 0; i < len; i++) {
		fprintf(f, "#cluster %u (%u):\n", i, *(list + i));
		graph_t *g = *(clusters + i);
		for (ruint j = 0; j < g->size; j++) {
			node_t *node = *(g->nodes + j);
			fprintf(f, "%u%c", node->id, (j+1)-g->size?32:10);
		}
		fprintf(f, "\n");
	}

	free(list);
	fclose(f);
	return 0;
}
