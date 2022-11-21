#include <stdlib.h>

#ifndef ARRAY_LIST
#define ARRAY_LIST
#define SIZE_INCREMENT 10

typedef struct _array_list {
    unsigned int *values;
    size_t size;
    size_t capacity;
} array_list;

array_list *create_list(const size_t init_capacity);

void list_add(array_list *list, const unsigned int value);

unsigned int *list_get(const array_list *list, const size_t index);

void list_free(array_list **list);

#endif