/**
 * @file linked_list.c
 * @author František Pártl (fpartl@kiv.zcu.cz)
 * @brief Soubor s cvičnou implementací jednosměrně zřetězeného spojového seznamu.
 * @version 1.0
 * @date 2020-09-03
 * 
 * 
 * Program můžete zkontrolovat pomocí nástroje Valgrind následovně:
 *     gcc list.c -o list -Wall -Wextra -pedantic -ansi -g
 *     valgrind --leak-check=full ./list
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"

linked_list *create_list() {
    linked_list *new = (linked_list *)malloc(sizeof(linked_list));
    if (!new) {
        return NULL;
    }

    new->head = NULL;
    new->tail = NULL;

    return new;
}

int list_add(linked_list *list, const node_type value) {

}

int list_insert(linked_list *list, const node_type value) {
    list_node *new;
    
    if (!list) {
        return 0;
    }

    new = (list_node *)malloc(sizeof(list_node));
    if (!new) {
        return 0;
    }

    new->value = value;
    new->next = list->head;

    if (list->head) {
        list->head->prev = new;
    }
    list->head = new;

    return 1;
}

void list_print(const linked_list *list) {
    list_node *current;
    
    if (!list) {
        return;
    }

    current = list->head;

    while (current) {
        printf("%li,", current->value);
        current = current->next;
    }
}

void list_free(linked_list **list) {
    list_node *current;
    list_node *next;

    if (!list) {
        return;
    }

    current = (*list)->head;

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }

    free(*list);
    *list = NULL;
}
