#ifndef __GRAPHLIB__
#define __GRAPHLIB__

#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int uint;

typedef struct _node {
	uint id, size, len;
	struct _node **next;
} node_t;

typedef struct graph {
	uint size, len;
	node_t **nodes;
} graph_t;

node_t* newNode() {
	static uint id = 0;
	node_t *n = (node_t *) malloc(sizeof(*n));
	if (!n) {
		return (node_t *) NULL;
	}
	n->id = id;
	n->size = 0;
	n->len = 1;
	n->next = (node_t **) malloc(sizeof(*n->next)*n->len);
	if (!n->next) {
		free(n);
		return (node_t *) NULL;
	}
	id += 1;
	return n;
}

byte linkNode(node_t *n, node_t *friend) {
	if (!n) {
		return 1;
	}
	if (!friend) {
		return 2;
	}
	if (n->size >= n->len) {
		n->len += 1;
		n->next = (node_t **) realloc(n->next, sizeof(*n->next)*n->len);
		if (!n->next) {
			return 3;
		}
	}
	*(n->next + n->size) = friend;
	n->size += 1;
	return 0;
}

graph_t* newGraph() {
	graph_t *g = (graph_t *) malloc(sizeof(*g));
	if (!g) {
		return (graph_t *) NULL;
	}
	g->size = 0;
	g->len = 5;
	g->nodes = (node_t **) malloc(sizeof(*g->nodes)*g->len);
	if (!g->nodes) {
		return (graph_t *) NULL;
	}
	return g;
}

byte addNode(graph_t *g, node_t *n) {
	if (!g) {
		return 1;
	}
	if (!n) {
		return 2;
	}
	if (g->size >= g->len) {
		g->len += 1;
		g->nodes = (node_t **) realloc(g->nodes, sizeof(*g->nodes)*g->len);
		if (!g->nodes) {
			return 3;
		}
	}
	*(g->nodes + g->size) = n;
	g->size += 1;
	return 0;
}

node_t* getNode(graph_t *g, uint id) {
	if (id < g->size) {
		return *(g->nodes + id);
	}
	return (node_t *) NULL;
}
#endif // __GRAPHLIB__
