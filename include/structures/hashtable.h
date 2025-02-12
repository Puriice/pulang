#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_HASH_TABLE_SIZE 16

typedef struct node
{
	uint64_t hash;
	size_t key_len;
	bool is_occupied;
	void* key;
	void* data;
} HashNode;


typedef struct
{
	HashNode *nodes;
	size_t capacity;
	size_t count;
} HashTable;

HashTable* create_table(size_t capacity);

uint64_t hash(const void *key, size_t len);

void* get_from_table(HashTable *table, const void *key, size_t key_len);

bool insert_to_table(HashTable *table, const void *key, size_t key_len, void* data, size_t data_len);

bool replace_in_table(HashTable *table, const void *key, size_t key_len, void* data, size_t data_len);

bool delete_in_table(HashTable *table, const void *key, size_t key_len);

void free_table(HashTable *table);

void print_table(HashTable *table);