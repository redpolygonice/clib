#include "nmap.h"

#include <stdlib.h>
#include <string.h>

static size_t nmap_hash(long key)
{
	return key;
}

static inline size_t nmap_index(nmap* map, long key)
{
	size_t hash = nmap_hash(key);
	size_t index = hash % map->size;
	return index;
}

static struct nmap_node* nmap_insert_node(nmap* map, long key, void* value)
{
	size_t index = nmap_index(map, key);

	// Check collision
	if (map->nodes[index].key != KEY_NULL)
	{
		// Check the same key
		if (map->nodes[index].key == key)
		{
			map->nodes[index].value = value;
			return &map->nodes[index];
		}
		// Collision
		else
		{
			struct nmap_node* node = (struct nmap_node*)malloc(sizeof(struct nmap_node));
			node->key = key;
			node->value = value;
			node->nodes = NULL;
			node->removed = FALSE;

			list* nodes = map->nodes[index].nodes;
			if (nodes == NULL)
			{
				nodes = list_new();
				map->nodes[index].nodes = nodes;
			}

			list_push(nodes, node);
			list_push(map->iterator, node);
			map->node_count++;
			return node;
		}
	}

	map->nodes[index].key = key;
	map->nodes[index].value = value;
	map->nodes[index].removed = FALSE;
	map->node_count++;
	list_push(map->iterator, &map->nodes[index]);
	return &map->nodes[index];
}

static struct nmap_node* nmap_find_list(list* nodes, long key)
{
	for (list_first(nodes); !list_end(nodes); list_next(nodes))
	{
		struct nmap_node* node = (struct nmap_node*)list_get(nodes);
		if (node == NULL || node->key == KEY_NULL)
			return NULL;

		if (node->key == key)
			return node;
	}

	return NULL;
}

static void nmap_rehash(nmap* map, size_t size)
{
	struct nmap_node* old_nodes = map->nodes;

	if (size == 0)
		map->size += INCR_SIZE;
	else
		map->size = size;

	map->nodes = calloc(map->size, sizeof(struct nmap_node));
	map->node_count = 0;

	// Clear iterator list
	for (list_first(map->iterator); !list_end(map->iterator); list_next(map->iterator))
	{
		struct nmap_node* node = (struct nmap_node*)list_get(map->iterator);
		if (node->removed)
			list_remove(map->iterator, TRUE);
	}
	list_clear(map->iterator, FALSE);

	// Clear keys
	for (size_t i = 0; i < map->size; ++i)
		map->nodes[i].key = KEY_NULL;

	// Create new node list with new size
	for (size_t i = 0; i < map->count; ++i)
	{
		list* nodes = old_nodes[i].nodes;
		if (nodes != NULL)
		{
			for (list_first(nodes); !list_end(nodes); list_next(nodes))
			{
				struct nmap_node* node = (struct nmap_node*)list_get(nodes);
				if (node->key == KEY_NULL)
					continue;

				nmap_insert_node(map, node->key, node->value);
			}

			list_delete(nodes, TRUE);
		}

		if (old_nodes[i].key == KEY_NULL)
			continue;

		nmap_insert_node(map, old_nodes[i].key, old_nodes[i].value);
	}

	free(old_nodes);
}

nmap* nmap_new()
{
	nmap* map = (nmap*)calloc(1, sizeof(nmap));
	nmap_init(map);
	return map;
}

void nmap_reserve(nmap* map, size_t size)
{
	map->size = size;
	nmap_rehash(map, size);
}

void nmap_init(nmap* map)
{
	map->count = 0;
	map->node_count = 0;
	map->size = INCR_SIZE;

	if (map->iterator == NULL)
		map->iterator = list_new();
	else
		list_clear(map->iterator, TRUE);

	if (map->nodes != NULL)
		free(map->nodes);

	map->nodes = calloc(map->size, sizeof(struct nmap_node));

	for (size_t i = 0; i < map->size; ++i)
		map->nodes[i].key = KEY_NULL;
}

void nmap_clear(nmap* map, BOOL delete)
{
	if (map->nodes != NULL)
	{
		for (size_t i = 0; i < map->size; ++i)
		{
			list* nodes = map->nodes[i].nodes;
			if (nodes != NULL)
			{
				if (delete)
				{
					for (list_first(nodes); !list_end(nodes); list_next(nodes))
					{
						struct nmap_node* node = (struct nmap_node*)list_get(nodes);
						if (node->key != KEY_NULL)
						{
							node->key = KEY_NULL;
						}

						if (node->value != NULL)
						{
							free(node->value);
							node->value = NULL;
						}
					}
				}

				list_delete(nodes, TRUE);
				map->nodes[i].nodes = NULL;
			}

			if (delete && map->nodes[i].value != NULL)
			{
				free(map->nodes[i].value);
				map->nodes[i].value = NULL;
			}
		}

		free(map->nodes);
		map->nodes = NULL;
	}

	map->size = 0;
	map->count = 0;
	map->node_count = 0;

	if (map->iterator != NULL)
		list_clear(map->iterator, FALSE);
}

void nmap_delete(nmap* map, BOOL delete)
{
	nmap_clear(map, delete);
	list_delete(map->iterator, TRUE);
	free(map);
	map = NULL;
}

void nmap_insert(nmap* map, long key, void* value)
{
	nmap_insert_node(map, key, value);
	map->count++;

	if (map->count >= map->size)
		nmap_rehash(map, 0);
}

void nmap_insert_copy(nmap* map, long key, void* value, size_t size)
{
	void* copy = malloc(size);
	memcpy(copy, value, size);
	nmap_insert(map, key, copy);
}

void nmap_remove(nmap* map, long key, BOOL delete)
{
	size_t index = nmap_index(map, key);

	list* nodes = map->nodes[index].nodes;
	if (nodes != NULL)
	{
		struct nmap_node* node = nmap_find_list(nodes, key);
		if (node != NULL)
		{
			if (delete && node->key != KEY_NULL)
			{
				node->key = KEY_NULL;
			}

			if (delete && node->value != NULL)
			{
				free(node->value);
				node->value = NULL;
			}

			// Don't free the nmap_node, this will be removed in iterator list
			list_remove(nodes, FALSE);
			node->removed = TRUE;
			map->node_count--;
			return;
		}
	}

	if (map->nodes[index].key == key)
	{
		if (delete && map->nodes[index].value != NULL)
			free(map->nodes[index].value);

		map->nodes[index].key = KEY_NULL;
		map->nodes[index].value = NULL;
		map->nodes[index].removed = TRUE;
		map->count--;
		map->node_count--;
	}
}

void* nmap_find(nmap* map, long key)
{
	size_t index = nmap_index(map, key);

	list* nodes = map->nodes[index].nodes;
	if (nodes == NULL)
		return map->nodes[index].value;
	else
	{
		// First compare the base node
		if (map->nodes[index].key != KEY_NULL &&
			map->nodes[index].key == key)
			return map->nodes[index].value;

		// Find node in list
		struct nmap_node* node = nmap_find_list(nodes, key);
		if (node != NULL && node->key != KEY_NULL)
			return node->value;
	}

	return NULL;
}

size_t nmap_size(nmap* map)
{
	return map->node_count;
}

BOOL nmap_empty(nmap*map)
{
	return map->node_count == 0;
}

BOOL nmap_first(nmap* map)
{
	return list_first(map->iterator);
}

BOOL nmap_last(nmap* map)
{
	return list_last(map->iterator);
}

BOOL nmap_next(nmap* map)
{
	return list_next(map->iterator);
}

BOOL nmap_prev(nmap* map)
{
	return list_prev(map->iterator);
}

nmap_kv nmap_get(nmap* map)
{
	nmap_kv kv = {0};
	struct nmap_node* node = list_get(map->iterator);
	if (node == NULL)
		return kv;

	while (node->removed)
	{
		list_next(map->iterator);
		node = list_get(map->iterator);
	}

	kv.key = node->key;
	kv.value = node->value;
	return kv;
}

void nmap_for_each(nmap*map, nmap_get_value get_value)
{
	for (size_t i = 0; i < map->size; ++i)
	{
		if (map->nodes[i].key != KEY_NULL)
			get_value(map->nodes[i].key, map->nodes[i].value);

		list* nodes = map->nodes[i].nodes;
		if (nodes != NULL)
		{
			for (list_first(nodes); !list_end(nodes); list_next(nodes))
			{
				struct nmap_node* node = (struct nmap_node*)list_get(nodes);
				if (node != NULL && node->key != KEY_NULL)
					get_value(node->key, node->value);
			}
		}
	}
}

void nmap_print(nmap* map)
{
	for (list_first(map->iterator); !list_end(map->iterator); list_next(map->iterator))
	{
		struct nmap_node* node = (struct nmap_node*)list_get(map->iterator);
		if (node->key == KEY_NULL)
			continue;

		printf("%ld %s\n", node->key, (char*)node->value);
	}
}
