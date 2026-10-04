#ifndef NMAP_H
#define NMAP_H

#include "list.h"

#include <stdio.h>

typedef void (*nmap_get_value)(long, void*);

#define INCR_SIZE 16

struct nmap_node
{
	long key;
	void* value;
	list* nodes;
	BOOL removed;
};

typedef struct nmap_kv
{
	long key;
	void* value;
} nmap_kv;

typedef struct nmap
{
	size_t size;
	size_t count;
	size_t node_count;
	struct nmap_node* nodes;
	list* iterator;
} nmap;

nmap* nmap_new(void);
void nmap_reserve(nmap* map, size_t size);
void nmap_init(nmap* map);
void nmap_clear(nmap* map, BOOL delete);
void nmap_delete(nmap* map, BOOL delete);
void nmap_insert(nmap* map, long key, void* value);
void nmap_insert_copy(nmap* map, long key, void* value, size_t size);
void nmap_remove(nmap* map, long key, BOOL delete);
void* nmap_find(nmap* map, long key);
size_t nmap_size(nmap* map);
BOOL nmap_empty(nmap* map);
BOOL nmap_first(nmap* map);
BOOL nmap_last(nmap* map);
BOOL nmap_next(nmap* map);
BOOL nmap_prev(nmap* map);
nmap_kv nmap_get(nmap* map);
void nmap_for_each(nmap* map, nmap_get_value get_value);
void nmap_print(nmap* map);

#endif // NMAP_H
