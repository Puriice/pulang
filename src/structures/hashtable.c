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

	return rehash_table(table);
}

// hashing using FNV-1a algorithm
// TODO: Change to SipHash algorithm
// REASONS: Security concern: Hash flooding DDOS
uint64_t hash(const void *data, size_t len) {
	uint64_t hash = FNV_OFFSET_BASIS;
	const uint8_t *bytes = (const uint8_t*) data;

	for (size_t i = 0; i < len; i++)
	{
		hash = hash ^ bytes[i];
		hash = hash * FNV_PRIME;
	}
	
	return hash;
}

/**
 * @brief Get the hash node from table
 * 
 * @param table 
 * @param key 
 * @param key_len 
 * @param output 
 * @return * uint8_t If the key is found, the index of hash node in the table is returned. Otherwise, the last search index is returned.
 * 
 */
uint8_t get_node(HashTable *table, const void *key, size_t key_len, HashNode** output) {
	uint64_t hash_value = hash(key, key_len);
	uint64_t index;
	uint8_t i = 0;
	
	do {
		index = INDEX(hash_value, i);
		i++;
	} while (table->nodes[index].is_occupied && (table->nodes[index].key_len != key_len || memcmp(table->nodes[index].key, key, key_len) != 0) && i < table->capacity);

	if (!table->nodes[index].is_occupied || table->nodes[index].key_len != key_len || memcmp(table->nodes[index].key, key, key_len) != 0) {
		*output = NULL;
		return index;
	}

	*output = &table->nodes[index];
	return index;
}

void* get_from_table(HashTable *table, const void *key, size_t key_len) {
	HashNode* node = NULL;
	get_node(table, key, key_len, &node);

	if (node == NULL) return NULL;

	return node->data;
}

bool insert_to_table(HashTable *table, const void *key, size_t key_len, void *data, size_t data_len) {
	uint64_t hash_value = hash(key, key_len);
	uint64_t index;
	uint8_t i = 0;

	if ((float)table->count/table->capacity > LOAD_FACTOR_THRESHOLD) {
		bool is_success = incease_table_size(table);

		if (!is_success && table->count == table->capacity) return false;
	}

	do {
		index = INDEX(hash_value, i);
		// printf("index: %llu, i: %u\n", index, i);
		i++;
	} while (table->nodes[index].is_occupied);

	table->nodes[index].key = malloc(key_len);
	table->nodes[index].data = malloc(data_len);

	table->nodes[index].hash = hash_value;
	table->nodes[index].key_len = key_len;
	memcpy(table->nodes[index].key, key, key_len);
	memcpy(table->nodes[index].data, data, data_len);
	table->nodes[index].is_occupied = true;
	table->count++;

	return true;
}

bool replace_in_table(HashTable *table, const void *key, size_t key_len, void *data, size_t data_len) {
	HashNode *node = NULL; 
	uint16_t index = get_node(table, key, key_len, &node);
	uint64_t hash_value = hash(key, key_len);

	// key is not in the node
	if (node == NULL) {
		// The hashmap is full, increase the size and insert.
		if (table->nodes[index].is_occupied) {
			return insert_to_table(table, key, key_len, data, data_len);
		}

		table->nodes[index].key = malloc(key_len);
		table->nodes[index].data = malloc(data_len);

		table->nodes[index].hash = hash_value;
		table->nodes[index].key_len = key_len;
		memcpy(table->nodes[index].key, key, key_len);
		memcpy(table->nodes[index].data, data, data_len);

		table->nodes[index].is_occupied = true;
		table->count++;

		return true;
	}

	free(node->key);
	free(node->data);

	node->key = malloc(key_len);
	node->data = malloc(data_len);

	node->hash = hash_value;
	node->key_len = key_len;
	memcpy(node->key, key, key_len);
	memcpy(node->data, data, data_len);
	node->is_occupied = true;

	return true;
}

bool delete_in_table(HashTable *table, const void *key, size_t key_len) {
	HashNode* node = NULL;
	get_node(table, key, key_len, &node);

	if (node == NULL) return false;

	free(node->key);
	free(node->data);
	node->key = NULL;
	node->data = NULL;
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
		printf("| %zu |\t%s\t|\t%s\t|\n", i, (char*) table->nodes[i].key, (char*) table->nodes[i].data);
	}
}