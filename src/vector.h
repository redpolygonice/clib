#ifndef VECTOR_H
#define VECTOR_H

#include "types.h"

#include <stdlib.h>

#define INCR_SIZE 16

typedef struct vector_node vector_node;
typedef struct vector vector;

vector* vector_new(void);
void vector_init(vector* the_vector);
void vector_reserve(vector* the_vector, size_t size);
void vector_resize(vector* the_vector, size_t size);
void vector_push(vector* the_vector, void* data);
void vector_push_copy(vector* the_vector, void* data, size_t size);
BOOL vector_find(vector* the_vector, const void* data, size_t size);
void* vector_at(vector* the_vector, size_t index);
void vector_remove_at(vector* the_vector, size_t index, BOOL clean);
BOOL vector_first(vector* the_vector);
BOOL vector_last(vector*the_vector);
BOOL vector_next(vector* the_vector);
BOOL vector_prev(vector* the_vector);
BOOL vector_end(vector* the_vector);
void* vector_get(vector* the_vector);
void vector_remove(vector* the_vector, BOOL clean);
size_t vector_size(vector* the_vector);
BOOL vector_empty(vector* the_vector);
void vector_clear(vector* the_vector, BOOL clean);
void vector_delete(vector* the_vector, BOOL clean);
void vector_append(vector* dest, vector* src);
void vector_print(vector* the_vector);


#endif // VECTOR_H
