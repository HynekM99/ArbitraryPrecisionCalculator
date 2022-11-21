#include <stdio.h>
#include "array_list.h"

static int list_increase_capacity(array_list *list) {
    unsigned int *new_values;

    if (!list) {
        return 0;
    }

    list->capacity += SIZE_INCREMENT;
    new_values = (unsigned int *)realloc(list->values, sizeof(unsigned int) * list->capacity);

    if (!new_values) {
        return 0;
    }

    list->values = new_values;
    return 1;
}

array_list *create_list(const size_t init_capacity) {
    size_t i;
    array_list *list;
    unsigned int *init_values;
    
    if (init_capacity <= 0) {
        return NULL;
    }

    list = (array_list *)malloc(sizeof(array_list));
    init_values = (unsigned int *)malloc(sizeof(unsigned int) * init_capacity);

    if (!list || !init_values) {
        free(list);
        free(init_values);
        return NULL;
    }

    for (i = 0; i < init_capacity; i++) {
        init_values[i] = 0;
    }

    list->values = init_values;
    list->capacity = init_capacity;
    list->size = 0;

    return list;
}

void list_add(array_list *list, const unsigned int value) {
    size_t new_size;

    if (!list) {
        return;
    }

    new_size = list->size + 1;    

    if (new_size > list->capacity) {
        if (!list_increase_capacity(list)) {
            return;
        }
    }
   
    list->values[list->size] = value;
    list->size++;
}

unsigned int *list_get(const array_list *list, const size_t index) {
    if (!list || !list->values) {
        return NULL;
    }
    if (index >= list->size) {
        return NULL;
    }
    return &(list->values[index]);
}

void list_free(array_list **list) {
    if(!list || !*list) {
        return;
    }

    free((*list)->values);
    free(*list);
    *list = NULL;
}