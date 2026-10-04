#include "smap.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

static size_t smap_hash(const char* key)
{
	size_t code = 0;
	static const size_t number = 31;

	size_t len = strlen(key);
	for (size_t i = 0; i < len; ++i)
		code += (int)key[i] * pow(number, i);

	return code;
}

static inline size_t smap_index(smap* map, const char* key)
{
	size_t hash = smap_hash(key);
	size_t index = hash % map->size;
	return index;
}

static struct smap_node* smap_insert_node(smap* map, char* key, void* value)
{
	size_t index = smap_index(map, key);

	// Check collision
	if (map->nodes[index].key != NULL)
	{
		// Check the same key
		if (strcmp(map->nodes[index].key, key) == 0)
		{
			map->nodes[index].value = value;
			return &map->nodes[index];
		}
		// Collision
		else
		{
			struct smap_node* node = (struct smap_node*)malloc(sizeof(struct smap_node));
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

static struct smap_node* smap_find_list(list* nodes, const char* key)
{
	for (list_first(nodes); !list_end(nodes); list_next(nodes))
	{
		struct smap_node* node = (struct smap_node*)list_get(nodes);
		if (node == NULL || node->key == NULL)
			return NULL;

		if (strcmp(node->key, key) == 0)
			return node;
	}

	return NULL;
}

static void smap_rehash(smap* map, size_t size)
{
	struct smap_node* old_nodes = map->nodes;

	if (size == 0)
		map->size += INCR_SIZE;
	else
		map->size = size;

	map->nodes = calloc(map->size, sizeof(struct smap_node));
	map->node_count = 0;

	// Clear iterator list
	for (list_first(map->iterator); !list_end(map->iterator); list_next(map->iterator))
	{
		struct smap_node* node = (struct smap_node*)list_get(map->iterator);
		if (node->removed)
			list_remove(map->iterator, TRUE);
	}
	list_clear(map->iterator, FALSE);

	// Create new node list with new size
	for (size_t i = 0; i < map->count; ++i)
	{
		list* nodes = old_nodes[i].nodes;
		if (nodes != NULL)
		{
			for (list_first(nodes); !list_end(nodes); list_next(nodes))
			{
				struct smap_node* node = (struct smap_node*)list_get(nodes);
				if (node->key == NULL)
					continue;

				smap_insert_node(map, node->key, node->value);
			}

			list_delete(nodes, TRUE);
		}

		if (old_nodes[i].key == NULL)
			continue;

		smap_insert_node(map, old_nodes[i].key, old_nodes[i].value);
	}

	free(old_nodes);
}

smap* smap_new()
{
	smap* map = (smap*)calloc(1, sizeof(smap));
	smap_init(map);
	return map;
}

void smap_reserve(smap* map, size_t size)
{
	map->size = size;
	smap_rehash(map, size);
}

void smap_init(smap* map)
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

	map->nodes = calloc(map->size, sizeof(struct smap_node));
}

void smap_clear(smap* map, BOOL delete)
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
						struct smap_node* node = (struct smap_node*)list_get(nodes);
						if (node->key != NULL)
						{
							free(node->key);
							node->key = NULL;
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

			if (delete && map->nodes[i].key != NULL)
			{
				free(map->nodes[i].key);
				map->nodes[i].key = NULL;
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

void smap_delete(smap* map, BOOL delete)
{
	smap_clear(map, delete);
	list_delete(map->iterator, TRUE);
	free(map);
	map = NULL;
}

void smap_insert(smap* map, char* key, void* value)
{
	smap_insert_node(map, key, value);
	map->count++;

	if (map->count >= map->size)
		smap_rehash(map, 0);
}

void smap_insert_copy(smap* map, const char* key, const void* value, size_t size)
{
	char* key_copy = strdup(key);
	void* copy = malloc(size);
	memcpy(copy, value, size);
	smap_insert(map, key_copy, copy);
}

void smap_remove(smap* map, const char*key, BOOL delete)
{
	size_t index = smap_index(map, key);

	list* nodes = map->nodes[index].nodes;
	if (nodes != NULL)
	{
		struct smap_node* node = smap_find_list(nodes, key);
		if (node != NULL)
		{
			if (delete && node->key != NULL)
			{
				free(node->key);
				node->key = NULL;
			}

			if (delete && node->value != NULL)
			{
				free(node->value);
				node->value = NULL;
			}

			// Don't free the smap_node, this will be removed in iterator list
			list_remove(nodes, FALSE);
			node->removed = TRUE;
			map->node_count--;
			return;
		}
	}

	if (strcmp(map->nodes[index].key, key) == 0)
	{
		if (delete && map->nodes[index].key != NULL)
			free(map->nodes[index].key);

		if (delete && map->nodes[index].value != NULL)
			free(map->nodes[index].value);

		map->nodes[index].key = NULL;
		map->nodes[index].value = NULL;
		map->nodes[index].removed = TRUE;
		map->count--;
		map->node_count--;
	}
}

void* smap_find(smap* map, const char* key)
{
	size_t index = smap_index(map, key);

	list* nodes = map->nodes[index].nodes;
	if (nodes == NULL)
		return map->nodes[index].value;
	else
	{
		// First compare the base node
		if (map->nodes[index].key != NULL &&
			strcmp(map->nodes[index].key, key) == 0)
			return map->nodes[index].value;

		// Find node in list
		struct smap_node* node = smap_find_list(nodes, key);
		if (node != NULL)
			return node->value;
	}

	return NULL;
}

size_t smap_size(smap* map)
{
	return map->node_count;
}

BOOL smap_empty(smap*map)
{
	return map->node_count == 0;
}

BOOL smap_first(smap* map)
{
	return list_first(map->iterator);
}

BOOL smap_last(smap* map)
{
	return list_last(map->iterator);
}

BOOL smap_next(smap* map)
{
	return list_next(map->iterator);
}

BOOL smap_prev(smap* map)
{
	return list_prev(map->iterator);
}

smap_kv smap_get(smap* map)
{
	smap_kv kv = {0};
	struct smap_node* node = list_get(map->iterator);
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

void smap_for_each(smap* map, smap_get_value get_value)
{
	for (size_t i = 0; i < map->size; ++i)
	{
		if (map->nodes[i].key != NULL)
			get_value(map->nodes[i].key, map->nodes[i].value);

		list* nodes = map->nodes[i].nodes;
		if (nodes != NULL)
		{
			for (list_first(nodes); !list_end(nodes); list_next(nodes))
			{
				struct smap_node* node = (struct smap_node*)list_get(nodes);
				if (node != NULL && node->key != NULL)
					get_value(node->key, node->value);
			}
		}
	}
}

void smap_print(smap* map)
{
	for (list_first(map->iterator); !list_end(map->iterator); list_next(map->iterator))
	{
		struct smap_node* node = (struct smap_node*)list_get(map->iterator);
		if (node->key == NULL)
			continue;

		printf("%s %s\n", node->key, (char*)node->value);
	}
}
