#include <stdio.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("usage: %s entry target\n", argv[0]);
		return -1;
	}

	FILE *entry = fopen(argv[1], "r");

	if (entry == NULL) {
		perror("Can't open file");
	}

	fclose(entry);
	return 0;
}