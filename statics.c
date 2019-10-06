#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int uint;

uint* copy(uint *list, uint len);
byte sort(uint *list, uint len);

byte statics(uint *list, uint len, uint *max, uint *min, uint *mean, uint *median, double *variation) {
	uint *newList = copy(list, len);
	if (!newList) {
		return 1;
	}
	sort(newList, len);
	*max = *(newList + len-1);
	*min = *newList;
	*median = (*(newList + (len+1)/2) + *(newList + len/2) )/2;

	register uint sum = 0;
	for (register uint i = 0; i < len; i++) {
		sum += *(newList + i);
	}
	sum /= len;
	*mean = sum;

	sum = 0;
	for (register uint i = 0; i < len; i++) {
		register int tmp = *(newList + i) - *mean;
		sum += tmp * tmp;
	}
	*variation = ((double) sum) / ((double) len);

	free(newList);
	return 0;
}

uint* copy(uint *list, uint len) {
	uint *copy = (uint *) malloc(sizeof(*copy)*len);
	if (!copy) {
		return (uint *) NULL;
	}
	for (register uint i = 0; i < len; i++) {
		*(copy + i) = *(list + i);
	}
	return copy;
}

byte sort(uint *list, uint len) {
	for (register uint i = 1; i < len; i++) {
		for (register uint j = i; j > 0; j--) {
			if (*(list + j) >= *(list + j-1)) {
				break;
			}
			register uint tmp = *(list + j);
			*(list + j) = *(list + j-1);
			*(list + j-1) = tmp;
		}
	}
	return 0;
}
