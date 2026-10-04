#ifndef SSET_H
#define SSET_H

#include "list.h"
#include "slist.h"

#include <stdio.h>

#define INCR_SIZE 16

typedef void (*sset_get_value)(char*);

struct sset_node
{
	char* key;
	list* nodes;
	BOOL removed;
};

typedef struct sset
{
	size_t size;
	size_t count;
	size_t node_count;
	struct sset_node* nodes;
	list* iterator;
} sset;

sset* sset_new(void);
void sset_reserve(sset* set, size_t size);
void sset_init(sset* set);
void sset_clear(sset* set, BOOL delete);
void sset_delete(sset* set, BOOL delete);
void sset_insert(sset* set, char* key);
void sset_insert_copy(sset* set, const char* key);
void sset_remove(sset* set, const char* key, BOOL delete);
BOOL sset_find(sset* set, const char* key);
void sset_append(sset* set, sset* src);
void sset_append_list(sset* set, slist* src);
size_t sset_size(sset* set);
BOOL sset_empty(sset* set);
BOOL sset_first(sset* set);
BOOL sset_last(sset* set);
BOOL sset_next(sset* set);
BOOL sset_prev(sset* set);
char* sset_get(sset* set);
void sset_for_each(sset* set, sset_get_value get_value);
void sset_print(sset* set);

#endif // SSET_H
