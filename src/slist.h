#ifndef SLIST_H
#define SLIST_H

#include "types.h"
#include <stddef.h>

typedef struct slist_node slist_node;
typedef struct slist slist;

slist* slist_new(void);
void slist_init(slist* list);
void slist_push(slist* list, char* data);
void slist_push_copy(slist* list, const char* data);
BOOL slist_find(slist* list, const char* data);
BOOL slist_find_part(slist* list, const char* data);
BOOL slist_first(slist* list);
BOOL slist_last(slist* list);
BOOL slist_next(slist* list);
BOOL slist_prev(slist* list);
BOOL slist_end(slist* list);
char* slist_get(slist* list);
void slist_remove(slist* list, BOOL clean);
size_t slist_size(slist* list);
BOOL slist_empty(slist* list);
void slist_clear(slist* list, BOOL clean);
void slist_delete(slist* list, BOOL clean);
void slist_append(slist* dest, slist* src);
void slist_append_unique(slist* dest, slist* src);
void slist_remove_list(slist* dest, slist* src);
void slist_print(slist* list);


#endif // SLIST_H
