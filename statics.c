#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int uint;

uint* copy(uint *list, uint len);
byte quicksort(uint *list, uint len);
byte sort(uint *list, uint len);

byte statics(uint *list, uint len, uint *max, uint *min, double *mean, uint *median, double *variation) {
	uint *newList = copy(list, len);
	if (!newList) {
		return 1;
	}
	quicksort(newList, len);
	printf("Quicksorting: Done!\n");
	sort(newList, len);
	printf("Sorting: Done!\n");

	*max = *(newList + len-1);
	*min = *newList;
	*median = (*(newList + (len+1)/2) + *(newList + len/2) )/2;

	register uint sum = 0;
	for (register uint i = 0; i < len; i++) {
		sum += *(newList + i);
	}
	*mean = ((double) sum) / ((double) len);
	printf("Mean: Done!\n");

	sum = 0;
	for (register uint i = 0; i < len; i++) {
		register int tmp = *(newList + i) - *mean;
		sum += tmp * tmp;
	}
	*variation = ((double) sum) / ((double) len);
	printf("Variation: Done!\n");

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

byte quicksort(uint *list, uint len) {
	if (len <= 1) {
		return 0;
	}
	register uint pivot = 0, end = len - 1;
	while(pivot != end) {
		if (*(list + pivot) >= *(list + pivot+1)) {
			register uint tmp = *(list + pivot);
			*(list + pivot) = *(list + pivot+1);
			*(list + pivot+1) = tmp;
			pivot += 1;
		} else {
			register uint tmp = *(list + pivot+1);
			*(list + pivot+1) = *(list + end);
			*(list + end) = tmp;
			end -= 1;
		}
	}
	return quicksort(list, pivot) || quicksort(list + pivot+1, len - pivot-1);
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
