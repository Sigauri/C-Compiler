#ifndef HT_H
#define HT_H
#include "stdlib.h"	

#define DEFAULT_HT_SIZE 8
#define DEFAULT_MAX_LOAD_FACTOR 0.75
#define DEFAULT_MIN_LOAD_FACTOR 0.25


#define KEY_TYPE_INT 0 
#define KEY_TYPE_STR 1

struct node
{
	void *value;
	
	void *key;
	unsigned long hash;

	struct node *next;
};



// This will need a LOT of storage. Remove things we don't need
struct hash_table
{

	float max_load_factor;

	float min_load_factor;

	// Amount of elements currently being in use
	unsigned int in_use;

	// Amount of collisions
	unsigned int collisions;

	// Size of hash table's array
	unsigned long ht_size;

	// Pointer to an array of nodes
	struct node **ht_arr;

	// Pointer to hash function for strings
	unsigned long (*ht_hash_str)(unsigned char *key);

	// Pointer to hash function for int values
	unsigned long (*ht_hash_int)(long long key);
};


// The interface


#define ht_insert_int(ht, value, key)					\
	do 													\
	{													\
		unsigned long hash = (*ht)->ht_hash_int(key);		\
		ht_insert(ht, value, key, hash);				\
	} while (0);

#define ht_insert_str(ht, value, key)					\
	do 													\
	{													\
		unsigned long hash = (*ht)->ht_hash_str(key);		\
		ht_insert(ht, value, key, hash);				\
	} while (0);


// Creates a hashtable
struct hash_table *ht_create(unsigned int ht_size);

// Destroy hashtable
void ht_destroy(struct hash_table **ht);


// Not really sure if this has to be here.
// Recompute the hash values, copy ht_dest to ht_src.
// If rm is 1 - free ht_src
int ht_copy(struct hash_table **ptr_ht_dest, struct hash_table **ptr_ht_src, int rm);

// Insert value into ht (hash computed from key, hash function choosed from key_type)
struct node *ht_insert(struct hash_table **ptr_ht, void *value, void *key, unsigned long hash);

// Remove a node with key value equal to key
int ht_remove(struct hash_table **ht, void *value, unsigned long hash);
void *ht_find(struct hash_table *ht, void *key, unsigned long hash);

#ifdef DEBUG_INFO
	#define ht_state_print(ht)										\
	{																\
		printf("\n\n%s%zu\n", "In Use: ", ht->in_use);				\
		printf("%s%zu\n", "Collisions: ", ht->collisions);			\
		printf("%s%zu\n", "HT Size: ", ht->ht_size);				\
		printf("%s%zu\n", "ht_hash_str: ", ht->ht_hash_str);		\
		printf("%s%zu\n\n", "ht_hash_int: ", ht->ht_hash_int);		\
	}	
#endif

#endif 