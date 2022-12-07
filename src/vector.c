#include <stdlib.h>
#include <string.h>

#include "vector.h"

/**
 * \brief Pracuje stejně jako funkce vector_at (viz vector.h), ale neprovádí kontrolu rozsahu přistupovaného vektoru.
 *        Funkce je deklarovaná jako statická, tj. její viditelnost je omezena na její objektový soubor.
 *        Vytvořili jsme tedy takové typově bezpečné makro. Jak jsme viděli na druhém cvičení, optimalizátor funkci
 *        celkem jistě inlinuje, takže se nemusíme bát drahého funkčního volání.
 * \param v Přistupovaný vektor.
 * \param at Index prvku ve vektoru.
 * \return void* Ukazatel na at-tý prvek ve vektoru *v.
 */
static void *vector_at_(const vector_type *v, const size_t at) {
    return (char *)v->data + (at * v->item_size);
}

vector_type *vector_allocate(const size_t item_size, const vec_it_dealloc_type deallocator) {
    vector_type *new;

    if (item_size == 0) {
        return NULL;
    }

    new = (vector_type *)malloc(sizeof(vector_type));
    if (!new) {
        return NULL;
    }

    if (!vector_init(new, item_size, deallocator)) {
        return NULL;
    }

    return new;
}

int vector_init(vector_type *v, const size_t item_size, const vec_it_dealloc_type deallocator) {
    if (!v || item_size == 0) {
        return 0;
    }

    v->count = 0;
    v->capacity = 0;
    v->data = NULL;
    v->item_size = item_size;
    v->deallocator = deallocator;

    if (!vector_realloc(v, VECTOR_INIT_SIZE)) {
        return 0;
    }

    ((char *)v->data)[0] = 0;

    return 1;
}

void vector_deinit(vector_type *v) {
    size_t i;

    if (!v) {
        return;
    }

    if (v->deallocator) {
        for (i = 0; i < vector_count(v); ++i) {
            v->deallocator(vector_at_(v, i));
        }
    }

    if (v->data) {
        free(v->data);
    }
}

void vector_deallocate(vector_type **v) {
    if (!v || !*v) {
        return;
    }

    vector_deinit(*v);

    free(*v);
    *v = NULL;
}

vector_type *vector_copy(const vector_type *src) {
    vector_type *dest = NULL;
    if (!src) {
        return NULL;
    }

    dest = vector_allocate(src->item_size, src->deallocator);
    if (!dest) {
        return NULL;
    }

    if (!vector_realloc(dest, src->capacity)) {
        vector_deallocate(&dest);
        return NULL;
    }

    memcpy(dest->data, src->data, vector_count(src));
    dest->count = src->count;

    return dest;
}

size_t vector_capacity(const vector_type *v) {
    return v ? v->capacity : 0; 
}

size_t vector_count(const vector_type *v) {
    return v ? v->count : 0;
}

int vector_isempty(const vector_type *v)  {
    return vector_count(v) == 0;
}

int vector_realloc(vector_type *v, const size_t capacity) {
    void *data_temp;

    if (!v) {
        return 0;
    }

    if (v->deallocator && capacity < vector_count(v)) {
        return 0;
    }

    data_temp = realloc(v->data, capacity * v->item_size);
    if (!data_temp) {
        return 0;
    }

    v->data = data_temp;
    v->capacity = capacity;

    return 1;
}

void *vector_at(const vector_type *v, const size_t at) {
    if (at > vector_count(v) - 1) {
        return NULL;
    }

    return vector_at_(v, at);
}

int vector_remove(vector_type *v, size_t count) {
    size_t i, index, vec_count;
    if (!v || count == 0) {
        return 0;
    }

    vec_count = vector_count(v);

    if (count > vec_count) {
        count = vec_count;
    }

    for (i = 0; i < count; ++i) {
        index = vec_count - i - 1;
        if (v->deallocator) {
            v->deallocator(vector_at_(v, index));
        }
    }

    v->count -= count;
    return 1;
}

int vector_push_back(vector_type *v, const void *item) {
    if (!v || !item) {
        return 0;
    }

    if (vector_count(v) >= vector_capacity(v)) {
        if (!vector_realloc(v, v->capacity * VECTOR_SIZE_MULT)) {
            return 0;
        }
    }

    memcpy(vector_at_(v, v->count), item, v->item_size);
    v->count++;

    return 1;
}

void *vector_giveup(vector_type *v) {
    void *data;

    if (vector_isempty(v)) {
        return NULL;
    }

    data = v->data;

    vector_init(v, v->item_size, v->deallocator);

    return data;
}
