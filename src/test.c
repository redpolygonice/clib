#include "smap.h"
#include "nmap.h"
#include "slist.h"
#include "nlist.h"
#include "sset.h"
#include "nset.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void smap_print_value(char* key, void* value)
{
	printf("Data %s %s\n", key, (char*)value);
}

void test_smap(void)
{
	smap* map = smap_new();
	smap_reserve(map, 10);
	const int count = 10;
	const int key_size = 100;

	// Insert
	for (int i = 0; i < count; ++i)
	{
		char* key = (char*)malloc(key_size);
		char* value = (char*)malloc(key_size);
		sprintf(key, "key%d", i + 1);
		sprintf(value, "value%d", i + 1);
		smap_insert(map, key, value);
		printf("Insert %s %s\n", key, value);
	}

	// Insert copy
	int copy_count = count * 2;
	for (int i = count; i < copy_count; ++i)
	{
		char key[key_size];
		char value[key_size];
		sprintf(key, "key%d", i + 1);
		sprintf(value, "value%d", i + 1);
		smap_insert_copy(map, key, value, sizeof(value));
		printf("Insert copy %s %s\n", key, value);
	}

	// Print content
	//printf("Content:\n");
	//smap_for_each(map, smap_print_value);

	// Remove
	printf("Remove key1\n");
	smap_remove(map,"key1", TRUE);
	printf("Remove key3\n");
	smap_remove(map,"key3", TRUE);
	printf("Remove key4\n");
	smap_remove(map,"key4", TRUE);
	printf("Remove key10\n");
	smap_remove(map, "key10", TRUE);

	// Print content
	printf("Content:\n");
	smap_first(map);
	do
	{
		smap_kv kv = smap_get(map);
		printf("Data %s %s\n", kv.key, (char*)kv.value);
	}
	while (smap_next(map));

	// Find
	char key[key_size];
	for (int i = 0; i < copy_count; ++i)
	{
		sprintf(key, "key%d", i + 1);
		char* find_value = smap_find(map, key);
		printf("Find %s %s\n", key, find_value);
	}

	printf("Map size %lu\n", smap_size(map));
	printf("Clear map\n");
	smap_clear(map, TRUE);
	printf("Map size %lu\n", smap_size(map));

	printf("Delete map\n");
	smap_delete(map, TRUE);
}

void nmap_print_value(long key, void* value)
{
	printf("Data %ld %s\n", key, (char*)value);
}

void test_nmap(void)
{
	nmap* map = nmap_new();
	nmap_reserve(map, 10);
	const int count = 10;
	const int value_size = 100;

	// Insert
	for (long i = 0; i < count; ++i)
	{
		char* value = (char*)malloc(value_size);
		sprintf(value, "value%ld", i + 1);
		nmap_insert(map, i + 1, value);
		printf("Insert %ld %s\n", i + 1, value);
	}

	// Insert copy
	int copy_count = count * 2;
	for (long i = count; i < copy_count; ++i)
	{
		char value[value_size];
		sprintf(value, "value%ld", i + 1);
		nmap_insert_copy(map, i + 1, value, sizeof(value));
		printf("Insert copy %ld %s\n", i + 1, value);
	}

	// Print content
	//printf("Content:\n");
	//nmap_for_each(map, nmap_print_value);

	// Remove
	printf("Remove key 1\n");
	nmap_remove(map, 1, TRUE);
	printf("Remove key 3\n");
	nmap_remove(map, 3, TRUE);
	printf("Remove key 4\n");
	nmap_remove(map, 4, TRUE);
	printf("Remove key 10\n");
	nmap_remove(map, 10, TRUE);

	// Print content
	printf("Content:\n");
	nmap_first(map);
	do
	{
		nmap_kv kv = nmap_get(map);
		printf("Data %ld %s\n", kv.key, (char*)kv.value);
	}
	while (nmap_next(map));

	// Find
	for (long i = 0; i < copy_count; ++i)
	{
		char* find_value = nmap_find(map, i + 1);
		printf("Find %ld %s\n", i + 1, find_value);
	}

	printf("Map size %lu\n", nmap_size(map));
	printf("Clear map\n");
	nmap_clear(map, TRUE);
	printf("Map size %lu\n", nmap_size(map));

	printf("Delete map\n");
	nmap_delete(map, TRUE);
}

void test_slist(void)
{
	slist* list = slist_new();
	const int count = 10;
	const int str_size = 100;

	// Push
	for (int i = 0; i < count; ++i)
	{
		char* str = (char*)malloc(str_size);
		sprintf(str, "string%d", i + 1);
		slist_push(list, str);
		printf("Push %s\n", str);
	}

	// Print content
	printf("Content:\n");
	slist_first(list);
	do
	{
		char* data = slist_get(list);
		printf("%s\n", data);
	}
	while (slist_next(list));

	// Clear
	printf("Clear list\n");
	slist_clear(list, TRUE);

	// Push copy
	for (int i = 0; i < count; ++i)
	{
		char str[str_size];
		sprintf(str, "string%d", i + 1);
		slist_push_copy(list, str);
		printf("Push copy %s\n", str);
	}

	// Print content
	printf("Content:\n");
	for (slist_first(list); !slist_end(list); slist_next(list))
	{
		char* data = slist_get(list);
		printf("%s\n", data);
	}

	// Find
	for (int i = 0; i < count; ++i)
	{
		char str[str_size];
		sprintf(str, "string%d", i + 1);
		BOOL result = slist_find(list, str);
		printf("Found %s %d\n", str, result);
	}

	// Remove string
	slist_first(list);
	slist_next(list);
	slist_remove(list, TRUE);
	printf("Remove next the first string\n");
	printf("Remove string5\n");
	if (slist_find(list, "string5"))
		slist_remove(list, TRUE);
	printf("Count %d\n", slist_count(list));

	// Reverse content
	printf("Reverse Content:\n");
	if (slist_last(list))
	{
		do
		{
			char* data = slist_get(list);
			printf("%s\n", data);
		}
		while (slist_prev(list));
	}

	// Clear list
	printf("Clear list\n");
	slist_clear(list, TRUE);
	printf("Count %d\n", slist_count(list));

	// Delete list
	printf("Delete list\n");
	slist_delete(list, TRUE);
}

void test_nlist(void)
{
	nlist* list = nlist_new();
	const int count = 10;
	const int str_size = 100;

	// Push
	for (int i = 1; i <= count; ++i)
	{
		nlist_push(list, i);
		printf("Push %d\n", i);
	}

	// Print content
	printf("Content:\n");
	nlist_first(list);
	do
	{
		unsigned int data = nlist_get(list);
		printf("%d\n", data);
	}
	while (nlist_next(list));

	// Find
	for (int i = 1; i <= count; ++i)
	{
		BOOL result = nlist_find(list, i);
		printf("Found %d %d\n", i, result);
	}

	// Remove string
	nlist_first(list);
	nlist_next(list);
	nlist_remove(list);
	printf("Remove next the first element\n");
	printf("Remove 5\n");
	if (nlist_find(list, 5))
		nlist_remove(list);
	printf("Count %d\n", nlist_count(list));

	// Reverse content
	printf("Reverse Content:\n");
	if (nlist_last(list))
	{
		do
		{
			unsigned int data = nlist_get(list);
			printf("%d\n", data);
		}
		while (nlist_prev(list));
	}

	// Clear list
	printf("Clear list\n");
	nlist_clear(list);
	printf("Count %d\n", nlist_count(list));

	// Delete list
	printf("Delete list\n");
	nlist_delete(list);
}

void sset_print_value(char* value)
{
	printf("Data %s\n", value);
}

void test_sset(void)
{
	sset* set = sset_new();
	sset_reserve(set, 10);
	const int count = 10;
	const int key_size = 100;

	// Insert
	for (int i = 0; i < count; ++i)
	{
		char* value = (char*)malloc(key_size);
		sprintf(value, "value%d", i + 1);
		sset_insert(set, value);
		printf("Insert %s\n", value);
	}

	// Insert copy
	int copy_count = count * 2;
	for (int i = count; i < copy_count; ++i)
	{
		char value[key_size];
		sprintf(value, "value%d", i + 1);
		sset_insert_copy(set, value);
		printf("Insert copy %s\n", value);
	}

	// Print content
	//sset_for_each(set, sset_print_value);

	// Remove
	printf("Remove value1\n");
	sset_remove(set, "value1", TRUE);
	printf("Remove value3\n");
	sset_remove(set, "value3", TRUE);
	printf("Remove value4\n");
	sset_remove(set, "value4", TRUE);
	printf("Remove value10\n");
	sset_remove(set, "value10", TRUE);

	// Print content
	sset_first(set);
	do
	{
		char* value = sset_get(set);
		printf("Data %s\n", value);
	}
	while (sset_next(set));

	// Find
	char value[key_size];
	for (int i = 0; i < copy_count; ++i)
	{
		sprintf(value, "value%d", i + 1);
		BOOL result = sset_find(set, value);
		printf("Found %s %d\n", value, result);
	}

	printf("Set size %lu\n", sset_size(set));
	printf("Clear set\n");
	sset_clear(set, TRUE);
	printf("Set size %lu\n", sset_size(set));

	printf("Delete set\n");
	sset_delete(set, TRUE);
}

void nset_print_value(long value)
{
	printf("Data %ld\n", value);
}

void test_nset(void)
{
	nset* set = nset_new();
	nset_reserve(set, 10);
	const int count = 10;
	const int key_size = 100;

	// Insert
	for (int i = 1; i <= count; ++i)
	{
		nset_insert(set, i);
		printf("Insert value %d\n", i);
	}

	// Print content
	//nset_for_each(set, nset_print_value);

	// Remove
	printf("Remove value 1\n");
	nset_remove(set, 1);
	printf("Remove value 3\n");
	nset_remove(set, 3);
	printf("Remove value 4\n");
	nset_remove(set, 4);
	printf("Remove value 10\n");
	nset_remove(set, 10);

	// Print content
	nset_first(set);
	do
	{
		long value = nset_get(set);
		printf("Data %ld\n", value);
	}
	while (nset_next(set));


	// Find
	for (int i = 1; i <= count; ++i)
	{
		BOOL result = nset_find(set, i);
		printf("Found value %d %d\n", i, result);
	}

	printf("Set size %lu\n", nset_size(set));
	printf("Clear set\n");
	nset_clear(set);
	printf("Set size %lu\n", nset_size(set));

	printf("Delete set\n");
	nset_delete(set);
}

int main()
{
	//test_smap();
	//test_nmap();
	//test_slist();
	//test_nlist();
	//test_sset();
	//test_nset();
	return 0;
}
