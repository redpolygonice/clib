#ifndef SMAP_H
#define SMAP_H

#include "types.h"

#include <stdio.h>

typedef void (*smap_get_value)(char*, void*);

#define SMAP_INCR_SIZE 16

typedef struct smap_node smap_node;
typedef struct smap smap;

typedef struct smap_kv
{
	char* key;
	void* value;
} smap_kv;

smap* smap_new(void);
void smap_reserve(smap* map, size_t size);
void smap_init(smap* map);
void smap_clear(smap* map, BOOL clean);
void smap_delete(smap* map, BOOL clean);
void smap_insert(smap* map, char* key, void* value);
void smap_insert_copy(smap* map, const char* key, const void* value, size_t size);
void smap_remove(smap* map, const char* key, BOOL clean);
void* smap_find(smap* map, const char* key);
size_t smap_size(smap* map);
BOOL smap_empty(smap* map);
BOOL smap_first(smap* map);
BOOL smap_last(smap* map);
BOOL smap_next(smap* map);
BOOL smap_prev(smap* map);
smap_kv smap_get(smap* map);
void smap_for_each(smap* map, smap_get_value get_value);
void smap_print(smap* map);

#endif // SMAP_H
