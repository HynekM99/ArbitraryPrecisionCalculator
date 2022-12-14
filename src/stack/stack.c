#include "stack.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void *stack_at_(const stack *s, const size_t at) {
    return (char *)s->items + at * s->item_size;
} 

stack *stack_create(const size_t size, const size_t item_size) {
    stack *new;
    
    if (size == 0 || item_size == 0) {
        return NULL;
    }

    new = (stack *)malloc(sizeof(stack));
    if (!new) {
        return NULL;
    }

    new->items = malloc(size * item_size);
    if (!new->items) {
        free(new);
        return NULL;
    }

    new->size = size;
    new->item_size = item_size;
    new->sp = 0;

    return new;
}

int stack_push(stack *s, const void *item) {
    if (!s || !item || s->sp >= s->size) {
        return 0;
    }

    memcpy(stack_at_(s, s->sp), item, s->item_size);

    ++s->sp;
    return 1;
}

int stack_pop(stack *s, void *item) {
    if (stack_head(s, item)) {
        --s->sp;
        return 1;
    }
    else {
        return 0;
    }
}

int stack_head(const stack *s, void *item) {
    if (stack_item_count(s) == 0 || !item) {
        return 0;
    }

    memcpy(item, stack_at_(s, s->sp - 1), s->item_size);
    return 1;
}

size_t stack_item_count(const stack *s) {
    if (!s) {
        return 0;
    }
    
    return s->sp;
}

void stack_free(stack **s) {
    if (!s || !*s) {
        return;
    }

    free((*s)->items);
    free(*s);
    *s = NULL;
}