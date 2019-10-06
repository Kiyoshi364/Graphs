#include <stdio.h>
#include <stdlib.h>

#define ruint register uint;

typedef unsigned char byte;
typedef unsigned int uint;

byte loadList(char *filename, uint *list, uint *len);

byte printList(uint *list, uint len);

#define FILENAME "facebook.txt"

int main() {
	uint *list, len = 0;
	//loadList("degree_" FILENAME, list, &len);
	FILE *f = fopen("degree_" FILENAME, "r");
	if (!f) {
		return 1;
	}

	uint tam = 10, *list2 = (uint *) malloc(sizeof(*list2)*tam), size = 0;
	if (!list2) {
		return 2;
	}

	while (!feof(f)) {
		uint sum;
		fscanf(f, "%u\n", &sum);
		if (size >= tam) {
			tam += 5;
			list2 = (uint *) realloc(list2, sizeof(*list2)*tam);
	printf("size: %u tam: %u\n", size, tam);
			if (!list2) {
				printf("aaaaaaa\n");
				return 3;
			}
		}
		*(list + size) = sum;
	printf("list: %u size: %u\n", *(list + size), size);
		size += 1, sum = 0;
	}
	fclose(f);

	list = list2;
	len = size;

	printf("Done!");
	printList(list, len);
	printf("Done2!");
	return 0;
}

byte loadList(char *filename, uint *list, uint *len) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		return 1;
	}

	uint tam = 10, *list2 = (uint *) malloc(sizeof(*list2)*tam), size = 0;
	if (!list2) {
		return 2;
	}

	while (!feof(f)) {
		uint sum;
		fscanf(f, "%u\n", &sum);
		if (size >= tam) {
	printf("size: %u tam: %u\n", size, tam);
			tam += 5;
			list2 = (uint *) realloc(list2, sizeof(*list2)*tam);
			if (!list2) {
				return 3;
			}
		}
		*(list + size) = sum;
	printf("list: %u size: %u\n", *(list + size), size);
		size += 1, sum = 0;
	}

	list = list2;
	*len = size;

	return 0;
}

byte printList(uint *list, uint len) {
	if (!list) {
		return 1;
	}
	for (register int i = 0; i < len; i++) {
		printf("%u: %u\n", i, *(list + i));
	}
	return 0;
}
