#include <stdlib.h>
#include <string.h>
#include "vector.h"

/**
 * \brief Pracuje stejně jako funkce vector_at (viz vector.h), ale neprovádí kontrolu rozsahu přistupovaného vektoru.
 *        Funkce je deklarovaná jako statická, tj. její viditelnost je omezena na její objektový soubor.
 *        Vytvořili jsme tedy takové typově bezpečné makro. Jak jsme viděli na druhém cvičení, optimalizátor funkci
 *        celkem jistě inlinuje, takže se nemusíme bát drahého funkčního volání.
 * \param v Ukazatek na vektor.
 * \param at Index prvku ve vektoru.
 * \return void* Ukazatel na at-tý prvek ve vektoru *v.
 */
static void *vector_at_(const vector_type *v, const size_t at) {
    return (char *)v->data + (at * v->item_size);
}

/**
 * \brief Uvolní at-tý prvek vektoru v, pokud má vektor dealokátor. Neprovádí kontrolu rozsahu přistupovaného vektoru.
 * \param v Ukazatel na vektor.
 * \param at Index prvku ve vektoru.
 */
static void deallocate_at_(const vector_type *v, const size_t at) {
    if (v && v->deallocator && at > vector_count(v) - 1) {
        v->deallocator(vector_at_(v, at));
    }
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

    for (i = 0; i < vector_count(v); ++i) {
        deallocate_at_(v, i);
    }

    if (v->data) {
        free(v->data);
    }
}

int vector_clear(vector_type *v) {
    if (!v) {
        return 0;
    }
    vector_deinit(v);
    return vector_init(v, v->item_size, v->deallocator);
}

void vector_deallocate(vector_type **v) {
    if (!v || !*v) {
        return;
    }

    vector_deinit(*v);

    free(*v);
    *v = NULL;
}

vector_type *vector_clone(const vector_type *src) {
    vector_type *dest = NULL;
    
    if (!src) {
        return NULL;
    }

    if (!(dest = vector_allocate(src->item_size, src->deallocator))) {
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
    size_t i;

    if (!v) {
        return 0;
    }

    if (count >= vector_count(v)) {
        return vector_clear(v);
    }

    for (i = 0; i < count; ++i) {
        deallocate_at_(v, vector_count(v) - i - 1);
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
