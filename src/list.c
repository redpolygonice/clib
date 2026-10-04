#include "list.h"

#include <stdio.h>
#include <string.h>

list* list_new(void)
{
	list* new_list = (list*)malloc(sizeof(list));
	list_init(new_list);
	return new_list;
}

void list_init(list* list)
{
	if (!list)
		return;

	list->first = NULL;
	list->last = NULL;
	list->curr = NULL;
}

void list_push(list* list, void* data)
{
	list_node* node = (list_node*)malloc(sizeof(list_node));
	node->data = data;

	if (list->first == NULL)
	{
		list->first = node;
		node->next = NULL;
		node->prev = NULL;
	}
	else
	{
		node->prev = list->last;
		node->prev->next = node;
		node->next = NULL;
	}

	list->curr = node;
	list->last = node;
}

void list_push_copy(list* list, void* data, size_t size)
{
	void* copy = malloc(size);
	memcpy(copy, data, size);
	list_push(list, copy);
}

BOOL list_find(list* list, const void* data, size_t size)
{
	list_node* node = list->first;
	if (!node)
		return FALSE;

	while (node)
	{
		if (memcmp(node->data, data, size) == 0)
		{
			list->curr = node;
			return TRUE;
		}

		node = node->next;
	}

	return FALSE;
}

BOOL list_first(list* list)
{
	if (!list->first)
		return FALSE;

	list->curr = list->first;
	return TRUE;
}

BOOL list_last(list* list)
{
	if (!list->last)
		return FALSE;

	list->curr = list->last;
	return TRUE;
}

BOOL list_next(list* list)
{
	if (!list->curr || !list->curr->next)
	{
		list->curr = NULL;
		return FALSE;
	}

	list->curr = list->curr->next;
	return TRUE;
}

BOOL list_prev(list* list)
{
	if (!list->curr || !list->curr->prev)
		return FALSE;

	list->curr = list->curr->prev;
	return TRUE;
}

BOOL list_end(list* list)
{
	return list->curr == NULL;
}

void* list_get(list* list)
{
	if (!list->curr)
		return NULL;

	return list->curr->data;
}

void list_remove(list* list, BOOL delete)
{
	if (!list->curr)
		return;

	list_node* curr = list->curr;

	if (list->curr->prev)
	{
		list->curr = list->curr->prev;
		if (curr->next)
		{
			list->curr->next = curr->next;
			curr->next->prev = list->curr;
		}
		else
			list->curr->next = NULL;
	}
	else if (list->curr->next)
	{
		list->curr = list->curr->next;
		if (curr->prev)
			list->curr->prev = curr->prev;
		else
			list->curr->prev = NULL;
	}

	if (curr == list->first && curr == list->last)
	{
		list->first = NULL;
		list->last = NULL;
		list->curr = NULL;
	}
	else
	{
		if (curr == list->first)
			list->first = list->curr;

		if (curr == list->last)
			list->last = list->curr;
	}

	if (delete && curr->data)
		free(curr->data);
	free(curr);
}

size_t list_size(list* list)
{
	list_node* node = list->first;
	if (!node)
		return 0;

	size_t count = 0;
	while (node)
	{
		count++;
		node = node->next;
	}

	return count;
}

BOOL list_empty(list* list)
{
	return list->first == NULL;
}

void list_clear(list* list, BOOL delete)
{
	list_node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		list_node* temp = node->next;
		if (delete && node->data)
			free(node->data);
		free(node);
		node = temp;
	}

	list_init(list);
}

void list_delete(list* list, BOOL delete)
{
	list_clear(list, delete);
	free(list);
	list = NULL;
}

void list_append(list* dest, list* src)
{
	list_node* src_node = src->first;
	if (!src_node)
		return;

	while (src_node)
	{
		list_push(dest, src_node->data);
		src_node = src_node->next;
	}
}

void list_print(list* list)
{
	list_node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		printf("%s\n", (char*)node->data);
		node = node->next;
	}
}
