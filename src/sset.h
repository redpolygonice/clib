#ifndef SSET_H
#define SSET_H

#include "slist.h"

#include <stdio.h>

#define SSET_INCR_SIZE 16

typedef void (*sset_get_value)(char*);

typedef struct sset_node sset_node;
typedef struct sset sset;

sset* sset_new(void);
void sset_reserve(sset* set, size_t size);
void sset_init(sset* set);
void sset_clear(sset* set, BOOL clean);
void sset_delete(sset* set, BOOL clean);
void sset_insert(sset* set, char* key);
void sset_insert_copy(sset* set, const char* key);
void sset_remove(sset* set, const char* key, BOOL clean);
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
