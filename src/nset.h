#ifndef NSET_H
#define NSET_H

#include "list.h"
#include "nlist.h"

#include <stdio.h>

typedef void (*nset_get_value)(long);

#define NSET_INCR_SIZE 16

typedef struct nset_node nset_node;
typedef struct nset nset;

nset* nset_new(void);
void nset_reserve(nset* set, size_t size);
void nset_init(nset* set);
void nset_clear(nset* set);
void nset_delete(nset* set);
void nset_insert(nset* set, long key);
void nset_remove(nset* set, long key);
BOOL nset_find(nset* set, long key);
void nset_append(nset* set, nset* src);
void nset_append_list(nset* set, nlist* src);
void nset_remove_list(nset* set, nlist* src);
size_t nset_size(nset* set);
BOOL nset_empty(nset* set);
BOOL nset_first(nset* set);
BOOL nset_last(nset* set);
BOOL nset_next(nset* set);
BOOL nset_prev(nset* set);
long nset_get(nset* set);
void nset_for_each(nset* set, nset_get_value get_value);
void nset_print(nset* set);
void nset_print_ip(nset* set);

#endif // NSET_H
