#include <inttypes.h>
#include <stdlib.h>
#include "structures/string.h"

bool isInteger(char *str) {
	if (*str == '\0') return false;

	char *endptr = NULL;

	strtol(str, &endptr, 10);
	
	return (*endptr == '\0');
}