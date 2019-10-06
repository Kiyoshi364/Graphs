#ifndef __PATHLIST__
#define __PATHLIST__

#include <stdlib.h>

#define ruint register uint
typedef unsigned char byte;
typedef unsigned int uint;

typedef struct _queue {
	node_t **set;
	uint start, end, len;
} qpath_t;

qpath_t* newQPath() {
	qpath_t *path = (qpath_t *) malloc(sizeof(*path));
	if (!path) {
		return (qpath_t *) NULL;
	}
	path->start = 0;
	path->end = 0;
	path->len = 5;
	path->set = (node_t **) malloc(sizeof(*path->set)*path->len);
	if (!path->set) {
		free(path);
		return (qpath_t *) NULL;
	}
	return path;
}

byte enq(qpath_t *path, node_t *n) {
	if (!path) {
		return 1;
	}
	if (!n) {
		return 2;
	}
	if (path->end >= path->len) {
		path->len += 5;
		path->set = (node_t **) realloc(path->set, sizeof(*path->set)*path->len);
		if (!path->set) {
			return 3;
		}
	}
	*(path->set + path->end) = n;
	path->end += 1;
	return 0;
}

node_t* deq(qpath_t *path) {
	if (!path) {
		return (node_t *) NULL;
	}
	if (path->start == path->end) {
		return (node_t *) NULL;
	}
	node_t *n = *(path->set);
	for (ruint i = 0; i < path->end-1; i++) {
		*(path->set + i) = *(path->set + i+1);
	}
	path->end -= 1;
	return n;
}

#endif // __PATHLIST__
