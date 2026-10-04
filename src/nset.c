#include "nset.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>

static size_t nset_hash(long key)
{
	return key;
}

static inline size_t nset_index(nset* set, long key)
{
	size_t hash = nset_hash(key);
	size_t index = hash % set->size;
	return index;
}

static struct nset_node* nset_insert_node(nset* set, long key)
{
	size_t index = nset_index(set, key);

	// Check collision
	if (set->nodes[index].key != KEY_NULL)
	{
		// Check the same key
		if (set->nodes[index].key == key)
		{
			return &set->nodes[index];
		}
		// Collision
		else
		{
			struct nset_node* node = (struct nset_node*)malloc(sizeof(struct nset_node));
			node->key = key;
			node->nodes = NULL;
			node->removed = FALSE;

			list* nodes = set->nodes[index].nodes;
			if (nodes == NULL)
			{
				nodes = list_new();
				set->nodes[index].nodes = nodes;
			}

			list_push(nodes, node);
			list_push(set->iterator, node);
			set->node_count++;
			return node;
		}
	}

	set->nodes[index].key = key;
	set->nodes[index].removed = FALSE;
	set->node_count++;
	list_push(set->iterator, &set->nodes[index]);
	return &set->nodes[index];
}

static struct nset_node* nset_find_list(list* nodes, long key)
{
	for (list_first(nodes); !list_end(nodes); list_next(nodes))
	{
		struct nset_node* node = (struct nset_node*)list_get(nodes);
		if (node == NULL || node->key == KEY_NULL)
			return NULL;

		if (node->key == key)
			return node;
	}

	return NULL;
}

static void nset_rehash(nset* set, size_t size)
{
	struct nset_node* old_nodes = set->nodes;

	if (size == 0)
		set->size += INCR_SIZE;
	else
		set->size = size;

	set->nodes = calloc(set->size, sizeof(struct nset_node));
	set->node_count = 0;

	// Clear iterator list
	for (list_first(set->iterator); !list_end(set->iterator); list_next(set->iterator))
	{
		struct nset_node* node = (struct nset_node*)list_get(set->iterator);
		if (node->removed)
			list_remove(set->iterator, TRUE);
	}
	list_clear(set->iterator, FALSE);

	for (size_t i = 0; i < set->size; ++i)
		set->nodes[i].key = KEY_NULL;

	for (size_t i = 0; i < set->count; ++i)
	{
		list* nodes = old_nodes[i].nodes;
		if (nodes != NULL)
		{
			for (list_first(nodes); !list_end(nodes); list_next(nodes))
			{
				struct nset_node* node = (struct nset_node*)list_get(nodes);
				if (node->key == KEY_NULL)
					continue;

				nset_insert_node(set, node->key);
			}

			list_delete(nodes, TRUE);
		}

		if (old_nodes[i].key == KEY_NULL)
			continue;

		nset_insert_node(set, old_nodes[i].key);
	}

	free(old_nodes);
}

nset* nset_new()
{
	nset* set = (nset*)calloc(1, sizeof(nset));
	nset_init(set);
	return set;
}

void nset_reserve(nset* set, size_t size)
{
	set->size = size;
	nset_rehash(set, size);
}

void nset_init(nset* set)
{
	set->count = 0;
	set->node_count = 0;
	set->size = INCR_SIZE;

	if (set->iterator == NULL)
		set->iterator = list_new();
	else
		list_clear(set->iterator, TRUE);

	if (set->nodes != NULL)
		free(set->nodes);

	set->nodes = calloc(set->size, sizeof(struct nset_node));

	for (size_t i = 0; i < set->size; ++i)
		set->nodes[i].key = KEY_NULL;
}

void nset_clear(nset* set)
{
	if (set->nodes != NULL)
	{
		for (size_t i = 0; i < set->size; ++i)
		{
			list* nodes = set->nodes[i].nodes;
			if (nodes != NULL)
			{
				list_delete(nodes, TRUE);
				set->nodes[i].nodes = NULL;
			}
		}

		free(set->nodes);
		set->nodes = NULL;
	}

	set->size = 0;
	set->count = 0;
	set->node_count = 0;

	if (set->iterator != NULL)
		list_clear(set->iterator, FALSE);
}

void nset_delete(nset* set)
{
	nset_clear(set);
	list_delete(set->iterator, TRUE);
	free(set);
	set = NULL;
}

void nset_insert(nset* set, long key)
{
	nset_insert_node(set, key);
	set->count++;

	if (set->count >= set->size)
		nset_rehash(set, 0);
}

void nset_remove(nset* set, long key)
{
	size_t index = nset_index(set, key);

	list* nodes = set->nodes[index].nodes;
	if (nodes != NULL)
	{
		struct nset_node* node = nset_find_list(nodes, key);
		if (node != NULL)
		{
			// Don't free the nset_node, this will be removed in iterator list
			list_remove(nodes, FALSE);
			node->removed = TRUE;
			set->node_count--;
			return;
		}
	}

	if (set->nodes[index].key == key)
	{
		set->nodes[index].key = KEY_NULL;
		set->nodes[index].removed = TRUE;
		set->count--;
		set->node_count--;
	}
}

BOOL nset_find(nset* set, long key)
{
	size_t index = nset_index(set, key);

	if (set->nodes[index].key != KEY_NULL &&
		set->nodes[index].key == key)
		return TRUE;

	list* nodes = set->nodes[index].nodes;
	if (nodes != NULL)
	{
		struct nset_node* node = nset_find_list(nodes, key);
		if (node != NULL)
			return TRUE;
	}

	return FALSE;
}

size_t nset_size(nset* set)
{
	return set->node_count;
}

BOOL nset_empty(nset* set)
{
	return set->node_count == 0;
}

BOOL nset_first(nset* set)
{
	return list_first(set->iterator);
}

BOOL nset_last(nset* set)
{
	return list_last(set->iterator);
}

BOOL nset_next(nset* set)
{
	return list_next(set->iterator);
}

BOOL nset_prev(nset* set)
{
	return list_prev(set->iterator);
}

long nset_get(nset* set)
{
	struct nset_node* node = list_get(set->iterator);
	if (node == NULL)
		return FALSE;

	while (node->removed)
	{
		list_next(set->iterator);
		node = list_get(set->iterator);
	}

	return node->key;
}

void nset_for_each(nset* set, nset_get_value get_value)
{
	for (size_t i = 0; i < set->size; ++i)
	{
		if (set->nodes[i].key != KEY_NULL)
			get_value(set->nodes[i].key);

		list* nodes = set->nodes[i].nodes;
		if (nodes != NULL)
		{
			for (list_first(nodes); !list_end(nodes); list_next(nodes))
			{
				struct nset_node* node = (struct nset_node*)list_get(nodes);
				if (node != NULL && node->key != KEY_NULL)
					get_value(node->key);
			}
		}
	}
}

void nset_append(nset* set, nset* src)
{
	nset_first(src);
	do
	{
		long key = nset_get(src);
		if (key != KEY_NULL)
			nset_insert(set, key);
	}
	while (nset_next(src));
}

void nset_append_list(nset* set, nlist* src)
{
	for (nlist_first(src); !nlist_end(src); nlist_next(src))
	{
		long key = nlist_get(src);
		if (key != KEY_NULL)
			nset_insert(set, key);
	}
}

void nset_remove_list(nset* set, nlist* src)
{
	for (nlist_first(src); !nlist_end(src); nlist_next(src))
	{
		long key = nlist_get(src);
		if (key != KEY_NULL)
			nset_remove(set, key);
	}
}

void nset_print(nset*set)
{
	for (list_first(set->iterator); !list_end(set->iterator); list_next(set->iterator))
	{
		struct nset_node* node = (struct nset_node*)list_get(set->iterator);
		if (node->removed || node->key == KEY_NULL)
			continue;

		printf("%ld\n", node->key);
	}
}

void nset_print_ip(nset* set)
{
	for (list_first(set->iterator); !list_end(set->iterator); list_next(set->iterator))
	{
		struct nset_node* node = (struct nset_node*)list_get(set->iterator);
		if (node->removed || node->key == KEY_NULL)
			continue;

		unsigned int ip = (unsigned int)node->key;
		char s_ip[16] = {0};
		sprintf(s_ip, "%u.%u.%u.%u"
			, (ip) & 0xff
			, (ip >> 8) & 0xff
			, (ip >> 16) & 0xff
			, (ip >> 24) & 0xff);
		printf("%s\n", s_ip);
	}
}
