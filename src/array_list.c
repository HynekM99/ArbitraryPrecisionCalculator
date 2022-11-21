#include <stdio.h>
#include "array_list.h"

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
    size_t i;
    size_t new_size;
    unsigned int *old_values;

    if (!list) {
        return;
    }

    new_size = list->size + 1;    

    if (new_size > list->capacity) {
        list->capacity += SIZE_INCREMENT;
        old_values = list->values;
        list->values = (unsigned int *)malloc(sizeof(unsigned int) * list->capacity);
        
        for (i = 0; i < list->size; i++) {
            list->values[i] = old_values[i];
        }

        free(old_values);
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