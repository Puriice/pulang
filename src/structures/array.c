#include "structures/array.h"
#include <string.h>

bool contains(char **arr, size_t nArr, char *target) {
	for (size_t i = 0; i < nArr; i++) {
		if (strcmp(arr[i], target)) continue;

		return true;
	}
	return false;
}