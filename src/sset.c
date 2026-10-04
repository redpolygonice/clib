#include "sset.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

static size_t sset_hash(const char* key)
{
	size_t code = 0;
	static const size_t number = 31;

	size_t len = strlen(key);
	for (size_t i = 0; i < len; ++i)
		code += (int)key[i] * pow(number, i);

	return code;
}

static inline size_t sset_index(sset* set, const char* key)
{
	size_t hash = sset_hash(key);
	size_t index = hash % set->size;
	return index;
}

static struct sset_node* sset_insert_node(sset* set, char* key)
{
	size_t index = sset_index(set, key);

	// Check collision
	if (set->nodes[index].key != NULL)
	{
		// Check the same key
		if (strcmp(set->nodes[index].key, key) == 0)
		{
			return &set->nodes[index];
		}
		// Collision
		else
		{
			struct sset_node* node = (struct sset_node*)malloc(sizeof(struct sset_node));
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

static struct sset_node* sset_find_list(list* nodes, const char* key)
{
	for (list_first(nodes); !list_end(nodes); list_next(nodes))
	{
		struct sset_node* node = (struct sset_node*)list_get(nodes);
		if (node == NULL || node->key == NULL)
			return NULL;

		if (strcmp(node->key, key) == 0)
			return node;
	}

	return NULL;
}

static void sset_rehash(sset* set, size_t size)
{
	struct sset_node* old_nodes = set->nodes;

	if (size == 0)
		set->size += INCR_SIZE;
	else
		set->size = size;

	set->nodes = calloc(set->size, sizeof(struct sset_node));
	set->node_count = 0;

	// Clear iterator list
	for (list_first(set->iterator); !list_end(set->iterator); list_next(set->iterator))
	{
		struct sset_node* node = (struct sset_node*)list_get(set->iterator);
		if (node->removed)
			list_remove(set->iterator, TRUE);
	}
	list_clear(set->iterator, FALSE);

	// Create new node list with new size
	for (size_t i = 0; i < set->count; ++i)
	{
		list* nodes = old_nodes[i].nodes;
		if (nodes != NULL)
		{
			for (list_first(nodes); !list_end(nodes); list_next(nodes))
			{
				struct sset_node* node = (struct sset_node*)list_get(nodes);
				if (node->key == NULL)
					continue;

				sset_insert_node(set, node->key);
			}

			list_delete(nodes, TRUE);
		}

		if (old_nodes[i].key == NULL)
			continue;

		sset_insert_node(set, old_nodes[i].key);
	}

	free(old_nodes);
}

sset* sset_new()
{
	sset* set = (sset*)calloc(1, sizeof(sset));
	sset_init(set);
	return set;
}

void sset_reserve(sset* set, size_t size)
{
	set->size = size;
	sset_rehash(set, size);
}

void sset_init(sset* set)
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

	set->nodes = calloc(set->size, sizeof(struct sset_node));
}

void sset_clear(sset* set, BOOL delete)
{
	if (set->nodes != NULL && set->node_count > 0)
	{
		for (size_t i = 0; i < set->size; ++i)
		{
			list* nodes = set->nodes[i].nodes;
			if (nodes != NULL)
			{
				if (delete)
				{
					for (list_first(nodes); !list_end(nodes); list_next(nodes))
					{
						struct sset_node* node = (struct sset_node*)list_get(nodes);
						if (node->key != NULL)
						{
							free(node->key);
							node->key = NULL;
						}
					}
				}

				list_delete(nodes, TRUE);
				set->nodes[i].nodes = NULL;
			}

			if (delete && set->nodes[i].key != NULL)
			{
				free(set->nodes[i].key);
				set->nodes[i].key = NULL;
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

void sset_delete(sset* set, BOOL delete)
{
	sset_clear(set, delete);
	list_delete(set->iterator, TRUE);
	free(set);
	set = NULL;
}

void sset_insert(sset* set, char* key)
{
	sset_insert_node(set, key);
	set->count++;

	if (set->count >= set->size)
		sset_rehash(set, 0);
}

void sset_insert_copy(sset* set, const char* key)
{
	char* copy = strdup(key);
	sset_insert(set, copy);
}

void sset_remove(sset* set, const char* key, BOOL delete)
{
	size_t index = sset_index(set, key);

	list* nodes = set->nodes[index].nodes;
	if (nodes != NULL)
	{
		struct sset_node* node = sset_find_list(nodes, key);
		if (node != NULL)
		{
			if (delete && node->key != NULL)
			{
				free(node->key);
				node->key = NULL;
			}

			// Don't free the sset_node, this will be removed in iterator list
			list_remove(nodes, FALSE);
			node->removed = TRUE;
			set->node_count--;
			return;
		}
	}

	if (strcmp(set->nodes[index].key, key) == 0)
	{
		if (delete && set->nodes[index].key != NULL)
			free(set->nodes[index].key);

		set->nodes[index].key = NULL;
		set->nodes[index].removed = TRUE;
		set->count--;
		set->node_count--;
	}
}

BOOL sset_find(sset* set, const char* key)
{
	size_t index = sset_index(set, key);

	if (set->nodes[index].key != NULL &&
		strcmp(set->nodes[index].key, key) == 0)
		return TRUE;

	list* nodes = set->nodes[index].nodes;
	if (nodes != NULL)
	{
		struct sset_node* node = sset_find_list(nodes, key);
		if (node != NULL)
			return TRUE;
	}

	return FALSE;
}

void sset_append(sset* set, sset* src)
{
	sset_first(src);
	do
	{
		char* key = sset_get(src);
		if (key != NULL)
			sset_insert(set, key);
	}
	while (sset_next(src));
}

void sset_append_list(sset* set, slist* src)
{
	for (slist_first(src); !slist_end(src); slist_next(src))
	{
		char* key = slist_get(src);
		if (key != NULL)
			sset_insert(set, key);
	}
}

size_t sset_size(sset* set)
{
	return set->node_count;
}

BOOL sset_empty(sset*set)
{
	return set->node_count == 0;
}

BOOL sset_first(sset* set)
{
	return list_first(set->iterator);
}

BOOL sset_last(sset* set)
{
	return list_last(set->iterator);
}

BOOL sset_next(sset* set)
{
	return list_next(set->iterator);
}

BOOL sset_prev(sset*set)
{
	return list_prev(set->iterator);
}

char* sset_get(sset* set)
{
	struct sset_node* node = list_get(set->iterator);
	if (node == NULL)
		return FALSE;

	while (node->removed)
	{
		list_next(set->iterator);
		node = list_get(set->iterator);
	}

	return node->key;
}

void sset_for_each(sset* set, sset_get_value get_value)
{
	for (size_t i = 0; i < set->size; ++i)
	{
		if (set->nodes[i].key != NULL)
			get_value(set->nodes[i].key);

		list* nodes = set->nodes[i].nodes;
		if (nodes != NULL)
		{
			for (list_first(nodes); !list_end(nodes); list_next(nodes))
			{
				struct sset_node* node = (struct sset_node*)list_get(nodes);
				if (node != NULL && node->key != NULL)
					get_value(node->key);
			}
		}
	}
}

void sset_print(sset*set)
{
	for (list_first(set->iterator); !list_end(set->iterator); list_next(set->iterator))
	{
		struct sset_node* node = (struct sset_node*)list_get(set->iterator);
		if (node->removed || node->key == NULL)
			continue;

		printf("%s\n", node->key);
	}
}
