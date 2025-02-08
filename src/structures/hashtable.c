#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "structures/hashtable.h"

#define FNV_OFFSET_BASIS 0xcbf29ce484222325
#define FNV_PRIME 0x100000001b3

#define LOAD_FACTOR_THRESHOLD 0.7

#define INDEX(HASH, I) ((uint64_t) (HASH + I + (I * I)/2 )) % table->capacity

HashTable* create_table(size_t capacity) {
	if (capacity == 0) capacity = DEFAULT_HASH_TABLE_SIZE;
	else {
		// Ensure the size is the power of 2
		capacity = (size_t) (pow(ceil(sqrt(capacity)), 2));
	}

	HashTable *table = (HashTable*) malloc(sizeof(HashTable));

	if (table == NULL) {
		perror("Can't allocate memory for hash table");
		return NULL;
	}

	table->capacity = capacity;
	table->count = 0;
	table->nodes = (HashNode*) malloc(capacity * sizeof(HashNode));

	if (table->nodes == NULL) {
		perror("Can't allocate memory for hash nodes");
		free(table);
		return NULL;
	}

	for (size_t i = 0; i < capacity; i++)
	{
		table->nodes[i].key = NULL;
		table->nodes[i].data = NULL;
		table->nodes[i].is_occupied = false;
	}
	

	return table;
}

bool rehash_table(HashTable* table) {
	HashNode* old_nodes = table->nodes;
	table->nodes = (HashNode*) malloc(table->capacity * sizeof(HashNode));

	if (table->nodes == NULL) {
		perror("Can't allocate memory for rehashing the table");
		table->nodes = old_nodes;
		return false;
	}


	for (size_t i = 0; i < table->capacity; i++) {
		if (!old_nodes[i].is_occupied) continue;

		uint64_t index;
		size_t j = 0;

		do {
			index = INDEX(old_nodes[i].hash, j);
			j++;
		} while (table->nodes[index].is_occupied);

		memcpy(&table->nodes[index], &old_nodes[i], sizeof(HashNode));
	}

	free(old_nodes);
	return true;
}

bool incease_table_size(HashTable* table) {
	size_t new_capacity = (size_t) (pow(ceil(sqrt(table->capacity * 1.5)), 2));
	HashNode* new_alloc = (HashNode*) realloc(table->nodes, new_capacity * sizeof(HashNode));

	if (new_alloc == NULL) {
		perror("Can't allocate memory for new size");
		return false;
	}

	table->nodes = new_alloc;

	for (size_t i = table->capacity; i < new_capacity; i++)
	{
		table->nodes[i].key = NULL;
		table->nodes[i].data = NULL;
		table->nodes[i].is_occupied = false;
	}

	table->capacity = new_capacity;

	rehash_table(table);
	return true;
}

// hashing using FNV-1a algorithm
// TODO: Change to SipHash algorithm
// REASONS: Security concern: Hash flooding DDOS
uint64_t hash(const uint64_t key) {
	uint64_t hash = FNV_OFFSET_BASIS;

	for (size_t i = 0; i < 8; i++)
	{
		uint8_t key_byte = key >> (8 * i) & 0xFF;
		
		hash = hash ^ key_byte;
		hash = hash * FNV_PRIME;
	}
	
	return hash;
}

uint64_t str_to_key(const char *s_key) {
	uint64_t sum = 0;
	for (int i = 0; s_key[i] != '\0'; i++) {  // Loop through each character
        sum += (int) s_key[i];
    }

	return sum;
}

uint8_t get_node(HashTable *table, const char *key, HashNode** output) {
	uint64_t ui_key = str_to_key(key);
	uint64_t hash_value = hash(ui_key);
	uint64_t index;
	uint8_t i = 0;
	
	do {
		index = INDEX(hash_value, i);
		i++;
	} while (table->nodes[index].is_occupied && strcmp(table->nodes[index].key, key) != 0);

	if (!table->nodes[index].is_occupied || strcmp(table->nodes[index].key, key) != 0) {
		*output = NULL;
		return i;
	}

	*output = &table->nodes[index];
	return i;
}

void* get_from_table(HashTable *table, const char *key) {
	HashNode* node = NULL;
	get_node(table, key, &node);

	if (node == NULL) return NULL;

	return node->data;
}

bool insert_to_table(HashTable *table, char *key, void *data, size_t data_size) {
	uint64_t ui_key = str_to_key(key);
	uint64_t hash_value = hash(ui_key);
	uint64_t index;
	uint8_t i = 0;

	if (table->count/table->capacity > LOAD_FACTOR_THRESHOLD) {
		bool is_success = incease_table_size(table);

		if (!is_success && table->count == table->capacity) return false;
	}

	do {
		index = INDEX(hash_value, i);
		printf("index: %llu, i: %u\n", index, i);
		i++;
	} while (table->nodes[index].is_occupied);

	table->nodes[index].hash = hash_value;
	table->nodes[index].key = strdup(key);
	table->nodes[index].data = malloc(data_size);
	memcpy(table->nodes[index].data, data, data_size);
	table->nodes[index].is_occupied = true;
	table->count++;

	return true;
}

bool replace_in_table(HashTable *table, char *key, void *data, size_t data_size) {
	HashNode *node = NULL; 
	uint16_t index = get_node(table, key, &node);
	uint64_t ui_key = str_to_key(key);
	uint64_t hash_value = hash(ui_key);

	if (node == NULL) {
		table->nodes[index].hash = hash_value;
		
		table->nodes[index].key = strdup(key);

		table->nodes[index].data = malloc(data_size);
		memcpy(table->nodes[index].data, data, data_size);

		table->nodes[index].is_occupied = true;
		table->count++;

		return true;
	}

	if (node->is_occupied) {
		free(node->key);
		free(node->data);
	}

	node->hash = hash_value;
	node->key = strdup(key);
	node->data = malloc(data_size);
	memcpy(node->data, data, data_size);
	node->is_occupied = true;

	return true;
}

bool delete_in_table(HashTable *table, char *key) {
	HashNode* node = NULL;
	get_node(table, key, &node);

	if (node == NULL) return false;

	free(node->key);
	free(node->data);
	node->is_occupied = false;

	table->count--;

	return true;
}

void free_table(HashTable *table) {
    if (!table) return; 

    for (size_t i = 0; i < table->capacity; i++) {
        if (table->nodes[i].is_occupied) { 
            free(table->nodes[i].key);  
            free(table->nodes[i].data); 
        }
    }

    free(table->nodes);
    free(table);
}

void print_table(HashTable *table) {
	printf("Capacity: %zu/%zu\n", table->count, table->capacity);
	
	for (size_t i = 0; i < table->capacity; i++)
	{
		if (!table->nodes[i].is_occupied) continue;
		printf("| %zu |\t%s\t|\t%s\t|\n", i, table->nodes[i].key, (char*) table->nodes[i].data);
	}
}