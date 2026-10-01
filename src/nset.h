#ifndef NSET_H
#define NSET_H

#include "list.h"

#include <stdio.h>

typedef void (*nset_get_value)(long);

#define INCR_SIZE 16

struct nset_node
{
	long key;
	list* nodes;
	BOOL removed;
};

typedef struct nset
{
	size_t size;
	size_t count;
	size_t node_count;
	struct nset_node* nodes;
	list* iterator;
} nset;

nset* nset_new(void);
void nset_reserve(nset* set, size_t size);
void nset_init(nset* set);
void nset_clear(nset* set);
void nset_delete(nset* set);
void nset_insert(nset* set, long key);
void nset_remove(nset* set, long key);
BOOL nset_find(nset* set, long key);
size_t nset_size(nset* set);
BOOL nset_first(nset* set);
BOOL nset_last(nset* set);
BOOL nset_next(nset* set);
BOOL nset_prev(nset* set);
long nset_get(nset* set);
void nset_for_each(nset* set, nset_get_value get_value);

#endif // NSET_H
