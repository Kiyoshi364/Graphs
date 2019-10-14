#include <stdio.h>
#include <stdlib.h>

#include "statics.c"

typedef unsigned char byte;
#define ruint register uint

byte loadList(char *filename, uint **list, uint *len);
byte createEmpiricDistribuition(uint *list, uint len, uint **newList, uint **countList, uint *newLen);
byte writeEmpiric(char *filename, uint *list, uint *count, uint len);

void printList(uint *list, uint len);
void print2List(uint *list1, uint *list2, uint len);

#define FILENAME "degree_facebook.txt"

int main() {

	uint *list, len;
	if (loadList(FILENAME, &list, &len)) {
		printf("%s could not be open", FILENAME);
	}
	//printList(list, len);

	quicksort(list, len);
	sort(list, len);

	//printList(list, len);

	uint *newList, *countList, newLen;
	if(createEmpiricDistribuition(list, len, &newList, &countList, &newLen)) {
		printf("Empiric Distribuition Error!\n");
		return 1;
	}

	writeEmpiric("empiricD_" FILENAME, newList, countList, newLen);

	//print2List(newList, countList, newLen);

	return 0;
}

byte loadList(char *filename, uint **list, uint *len) {
	FILE *f = fopen(filename, "r");
	if (!f) {
		return 1;
	}
	*len = 5;
	*list = (uint *) malloc(sizeof(**list)**len);
	if (!*list) {
		fclose(f);
		return 2;
	}

	ruint i = 0;
	register int sum = 0;
	while (!feof(f)) {
		int c = fgetc(f);
		if (c == '#') {
			while ((c = fgetc(f)) != EOF && c != '\n');
			//printf("Skip\n");
		} else if (c == ' ' || c == '\t' || c == '\n') {
			if (i >= *len) {
				*len += 5;
				*list = (uint *) realloc(*list, sizeof(**list)**len);
				if (!*list) {
					fclose(f);
					return 3;
				}
			}
			*(*list + i) = sum;
			i += 1;
			sum = 0;
		} else if (c >= '0' && c <= '9') {
			sum = sum*10 + c - '0';
		}
	}
	*len = i;

	fclose(f);
	return 0;
}

byte createEmpiricDistribuition(uint *list, uint len, uint **newList, uint **countList, uint *newLen) {
	*newLen = 5;
	*newList = (uint *) malloc(sizeof(**newList)**newLen);
	if (!*newList) {
		return 1;
	}
	*countList = (uint *) malloc(sizeof(**countList)**newLen);
	if (!*countList) {
		return 2;
	}

	ruint index = 0, last = *list;
	**newList = *list;
	**countList = 0;
	for (ruint i = 0; i < len; i++) {
		if (*(list + i) == last) {
			*(*countList + index) += 1;
		} else {
			index += 1;
			if (index >= *newLen) {
				*newLen += 5;
				*newList = (uint *) realloc(*newList, sizeof(**newList)**newLen);
				if (!*newList) {
					return 3;
				}
				*countList = (uint *) realloc(*countList, sizeof(**countList)**newLen);
				if (!*countList) {
					return 4;
				}
			}
			*(*newList + index) = *(list + i);
			*(*countList + index) = 1;
			last = *(list + i);
		}
	}
	*newLen = index+1;

	return 0;
}

byte writeEmpiric(char *filename, uint *list, uint *count, uint len) {
	FILE *f = fopen(filename, "w");
	if (!f) {
		return 1;
	}

	uint maxCount = 0;
	for (ruint i = 0; i < len; i++) {
		maxCount += *(count + i);
	}

	fprintf(f, "#K\t>K\t>K%%\t=K\t=K%%\n");

	ruint i = 0, index = 0, leftover = maxCount;
	for (; index < len; i++) {
		ruint value = *(list + index);

		if (value > i) {
			fprintf(f, "%u\t%u\t%lf\t%u\t%lf\n", i, leftover, ((double) leftover/(double) maxCount), 0, (double) 0);
		} else if (value < i) {
			printf("WRITING ERROR: index=%u value=%u i=%u\n", index, value, i);
			fprintf(f, "#WRITING ERROR: index=%u value=%u i=%u\n", index, value, i);
			fclose(f);
			return 2;
		} else {
			fprintf(f, "%u\t%u\t%lf\t%u\t%lf\n", i, leftover, ((double) leftover/(double) maxCount), *(count + index), ((double) *(count + index)/(double) maxCount));
			leftover -= *(count + index);
			index += 1;
		}
	}
	fprintf(f, "%u\t%u\t%lf\t%u\t%lf\n", i, leftover, ((double) leftover/(double) maxCount), 0, (double) 0);

	fclose(f);
	return 0;
}

void printList(uint *list, uint len) {
	for (ruint i = 0; i < len; i++) {
		printf("%u: %u\n", i, *(list + i));
	}
}

void print2List(uint *list1, uint *list2, uint len) {
	for (ruint i = 0; i < len; i++) {
		printf("%u: %u\t%u\n", i, *(list1 + i), *(list2 + i));
	}
}
