#ifndef LIST_H
#define LIST_H

#include "types.h"

#include <stdlib.h>

typedef struct list_node
{
	void* data;
	struct list_node* next;
	struct list_node* prev;
} list_node;

typedef struct list
{
	list_node* first;
	list_node* last;
	list_node* curr;
} list;

list* list_new(void);
void list_init(list* list);
void list_push(list* list, void* data);
void list_push_copy(list* list, void* data, size_t size);
BOOL list_find(list* list, const void* data, size_t size);
BOOL list_first(list* list);
BOOL list_last(list*list);
BOOL list_next(list* list);
BOOL list_prev(list*list);
BOOL list_end(list* list);
void* list_get(list*  list);
void list_remove(list* list, BOOL delete);
size_t list_size(list* list);
BOOL list_empty(list*list);
void list_clear(list* list, BOOL delete);
void list_delete(list* list, BOOL delete);
void list_append(list* dest, list* src);
void list_print(list* list);


#endif // LIST_H
