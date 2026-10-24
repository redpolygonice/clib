#include "vector.h"

#include <stdio.h>
#include <string.h>

struct vector
{
	vector_node* data;
	size_t size;
	size_t capacity;
	size_t iterator;
};

struct vector_node
{
	void* data;
};

static void vector_realloc(vector* the_vector, size_t size)
{
	if (size == 0)
		the_vector->capacity += INCR_SIZE;
	else
		the_vector->capacity = size;

	vector_node* new_address = (vector_node*)realloc(the_vector->data,
		the_vector->capacity * sizeof(vector_node));
	if (new_address == NULL)
	{
		printf("realloc failed!");
		exit(1);
	}

	the_vector->data = new_address;
}

static void vector_remove_index(vector* the_vector, size_t index)
{
	if (index == the_vector->size - 1)
	{
		the_vector->data[index].data = NULL;
		return;
	}

	size_t count = the_vector->size - index - 1;
	vector_node* address = &the_vector->data[index];
	memmove(address, address + 1, count * sizeof(vector_node));
}

vector* vector_new(void)
{
	vector* new_vector = (vector*)malloc(sizeof(vector));
	vector_init(new_vector);
	return new_vector;
}

void vector_init(vector* the_vector)
{
	if (!the_vector)
		return;

	the_vector->size = 0;
	the_vector->capacity = INCR_SIZE;
	the_vector->iterator = 0;
	the_vector->data = (vector_node*)calloc(the_vector->capacity, sizeof(vector_node));
}

void vector_reserve(vector* the_vector, size_t size)
{
	if ((int)size < 0)
		return;

	if (size <= the_vector->capacity)
		return;

	vector_realloc(the_vector, size);
}

void vector_resize(vector* the_vector, size_t size)
{
	if ((int)size < 0)
		return;

	the_vector->size = size;
	vector_realloc(the_vector, size + INCR_SIZE);
}

void vector_push(vector* the_vector, void* data)
{
	if (the_vector->data == NULL || the_vector->size == 0)
		vector_init(the_vector);

	the_vector->data[the_vector->size].data = data;
	the_vector->size++;

	if (the_vector->size >= the_vector->capacity)
		vector_realloc(the_vector, 0);
}

void vector_push_copy(vector* the_vector, void* data, size_t size)
{
	void* copy = malloc(size);
	memcpy(copy, data, size);
	vector_push(the_vector, copy);
}

BOOL vector_find(vector* the_vector, const void* data, size_t size)
{
	for (size_t i = 0; i < the_vector->size; ++i)
	{
		if (memcmp(the_vector->data[i].data, data, size) == 0)
			return TRUE;
	}

	return FALSE;
}

void* vector_at(vector* the_vector, size_t index)
{
	if (index >= the_vector->size)
		return NULL;

	return the_vector->data[index].data;
}

void vector_remove_at(vector* the_vector, size_t index, BOOL clean)
{
	if (index >= the_vector->size)
		return;

	if (clean && the_vector->data[the_vector->iterator].data != NULL)
	{
		free(the_vector->data[index].data);
		the_vector->data[index].data = NULL;
	}

	vector_remove_index(the_vector, index);
	the_vector->size--;
}

BOOL vector_first(vector* the_vector)
{
	if (the_vector->size == 0)
		return FALSE;

	the_vector->iterator = 0;
	return TRUE;
}

BOOL vector_last(vector* the_vector)
{
	if (the_vector->size == 0)
		return FALSE;

	the_vector->iterator = the_vector->size - 1;
	return TRUE;
}

BOOL vector_next(vector* the_vector)
{
	if (the_vector->iterator >= the_vector->size)
		return FALSE;

	the_vector->iterator++;
	return TRUE;
}

BOOL vector_prev(vector* the_vector)
{
	if (the_vector->iterator >= the_vector->size)
		return FALSE;

	the_vector->iterator--;
	return TRUE;
}

BOOL vector_end(vector* the_vector)
{
	return the_vector->iterator >= the_vector->size;
}

void* vector_get(vector* the_vector)
{
	if (the_vector->size == 0)
		return NULL;

	if (the_vector->iterator >= the_vector->size)
		return NULL;

	return the_vector->data[the_vector->iterator].data;
}

void vector_remove(vector* the_vector, BOOL clean)
{
	if (the_vector->iterator >= the_vector->size)
		return;

	if (clean && the_vector->data[the_vector->iterator].data != NULL)
	{
		free(the_vector->data[the_vector->iterator].data);
		the_vector->data[the_vector->iterator].data = NULL;
	}

	vector_remove_index(the_vector, the_vector->iterator);
	the_vector->size--;
}

size_t vector_size(vector* the_vector)
{
	return the_vector->size;
}

BOOL vector_empty(vector* the_vector)
{
	return the_vector->size == 0;
}

void vector_clear(vector* the_vector, BOOL clean)
{
	if (the_vector->data == NULL || the_vector->size == 0)
		return;

	if (clean)
	{
		for (size_t i = 0; i < the_vector->size; ++i)
		{
			vector_node node = the_vector->data[i];
			if (node.data == NULL)
				continue;
			free(node.data);
			node.data = NULL;
		}
	}

	free(the_vector->data);
	the_vector->data = NULL;
	the_vector->size = 0;
	the_vector->capacity = 0;
	the_vector->iterator = 0;
}

void vector_delete(vector* the_vector, BOOL clean)
{
	vector_clear(the_vector, clean);
	free(the_vector);
}

void vector_append(vector* dest, vector* src)
{
	if (src->size == 0)
		return;

	size_t new_size = dest->size + src->size;
	if (new_size > dest->capacity)
		vector_reserve(dest, new_size);

	memcpy(&dest->data[dest->size], src->data, src->size * sizeof(vector_node));
	dest->size = new_size;
}

void vector_print(vector* the_vector)
{
	for (size_t i = 0; i < the_vector->size; ++i)
		printf("%s\n", (char*)the_vector->data[i].data);
}
