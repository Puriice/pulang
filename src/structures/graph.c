#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures/graph.h"

bool append(Graph *graph, Graph node) {
	// resizing the nodes array
	if (sizeof(graph->nodes) <= sizeof(Graph)*graph->size) {
		Graph *temp = (Graph*) realloc(graph->nodes,  sizeof(Graph) * ceil(graph->size * 1.5 + 1));

		if (temp == NULL) {
			perror("Fail to reallocate memory for new node in graph");
			exit(EXIT_FAILURE);
		}

		graph->nodes = temp;
	}

	graph->nodes[graph->size++] = node;

	return true;
}
