#include <stdlib.h>

#ifndef LINKED_LIST
#define LINKED_LIST

typedef size_t node_type; 

typedef struct _list_node {
    node_type value;
    struct _list_node *next;
    struct _list_node *prev;
} list_node;

typedef struct _linked_list {
    list_node *head;
    list_node *tail;
} linked_list;

linked_list *create_list();

int list_add(linked_list *list, const node_type value);

int list_insert(linked_list *list, const node_type value);

void list_print(const linked_list *list);

void list_free(linked_list **list);

#endif