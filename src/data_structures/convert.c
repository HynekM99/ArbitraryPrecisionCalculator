#include "convert.h"

stack *vector_to_stack(vector_type **v) {
    size_t i = 0;
    stack *s = NULL;
    if (!v || !*v) {
        return NULL;
    }

    s = stack_create(vector_count(*v), (*v)->item_size);
    if (!s) {
        return NULL;
    }

    for (i = 0; i < vector_count(*v); ++i) {
        if (!stack_push(s, vector_at(*v, vector_count(*v) - i - 1))) {
            stack_free(&s);
            return NULL;
        }
    }

    (*v)->deallocator = NULL;
    vector_deallocate(v);
    return s;
}