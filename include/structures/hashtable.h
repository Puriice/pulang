#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_HASH_TABLE_SIZE 16

typedef struct node
{
	uint64_t hash;
	char* key;
	void* data;
	bool is_occupied;
} HashNode;


typedef struct
{
	HashNode *nodes;
	size_t capacity;
	size_t count;
} HashTable;


HashTable* create_table(size_t capacity);

uint64_t hash(const uint64_t key);

void* get_from_table(HashTable *table, const char *key);

bool insert_to_table(HashTable *table, char *key, void* data, size_t data_size);

bool replace_in_table(HashTable *table, char *key, void* data, size_t data_size);

bool delete_in_table(HashTable *table, char *key);

void free_table(HashTable *table);

void print_table(HashTable *table);