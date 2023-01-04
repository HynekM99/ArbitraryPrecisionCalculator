#include "conversion.h"

stack_type *vector_to_stack(vector_type **v) {
    size_t i = 0;
    stack_type *s = NULL;
    if (!v || !*v) {
        return NULL;
    }

    if (!(s = stack_allocate(vector_count(*v), (*v)->item_size, (*v)->deallocator))) {
        return NULL;
    }

    for (i = 0; i < vector_count(*v); ++i) {
        if (!stack_push(s, vector_at(*v, vector_count(*v) - i - 1))) {
            s->deallocator = NULL;
            stack_deallocate(&s);
            return NULL;
        }
    }

    (*v)->deallocator = NULL;
    vector_deallocate(v);
    return s;
}