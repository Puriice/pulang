#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct graph
{
	struct graph *nodes;
	uint8_t *data;
	size_t size;
} Graph;

bool append(Graph *graph, Graph node);
