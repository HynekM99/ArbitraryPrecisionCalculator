#include <stdlib.h>
#include "multiple_precision_type.h"

/**
 * \brief Přidá zadaný počet nulových segmentů do instance mpt.
 * \param value Ukazatel na instanci mpt.
 * \param segments_to_add Počet segmentů k přidání.
 * \return int 1 jestli se přidání podařilo, 0 pokud ne.
 */
static int mpt_add_segments_(mpt *value, const size_t segments_to_add) {
    size_t i;
    int zero = 0;
    if (!value || segments_to_add == 0) {
        return 0;
    }

    for (i = 0; i < segments_to_add; ++i) {
        if (!vector_push_back(value->list, &zero)) {
            return 0;
        }
    }

    return 1;
}

/**
 * \brief Vrátí index segmentu s bitem na at-té pozici.
 * \param value Instance mpt.
 * \param at Pozice bitu, jehož segment chceme najít.
 * \return size_t Index segmentu s bitem na at-té pozici.
 */
static size_t mpt_get_segment_index_(const mpt value, const size_t at) {
    return at / mpt_bits_in_segment(value);
}

/**
 * \brief Vrátí index bitu v segmentu s bitem na at-té pozici.
 * \param value Instance mpt.
 * \param at Pozice bitu, jehož pozici v jemu příslušném segmentu chceme najít.
 * \return size_t Index bitu v segmentu s bitem na at-té pozici.
 */
static size_t mpt_bit_pos_in_segment_(const mpt value, const size_t at) {
    return at % mpt_bits_in_segment(value);
}

int mpt_init(mpt *value, const segment_type init_value) {
    segment_type *default_segment;
    
    if (!value) {
        return 0;
    }

    if (!(value->list = vector_allocate(sizeof(segment_type), NULL))) {
        return 0;
    }

    if (!mpt_add_segments_(value, 1)) {
        mpt_deinit(value);
        return 0;
    }
    
    if (!(default_segment = mpt_get_segment_ptr(*value, 0))) {
        mpt_deinit(value);
        return 0;
    }
    *default_segment = init_value;

    return 1;
}

mpt *mpt_allocate(const segment_type init_value) {
    mpt *new = (mpt *)malloc(sizeof(mpt));
    if (!new) {
        return NULL;
    }

    new->list = NULL;

    if (!mpt_init(new, init_value)) {
        mpt_deallocate(&new);
    }

    return new;
}

int mpt_clone(mpt *dest, const mpt orig) {
    if (!dest) {
        return 0;
    }

    return (dest->list = vector_clone(orig.list)) != NULL;
}

void mpt_replace(mpt *to_replace, mpt *replace_with) {
    if (!to_replace || !replace_with) {
        return;
    }
    mpt_deinit(to_replace);
    *to_replace = *replace_with;
    replace_with->list = NULL;
}

size_t mpt_bits_in_segment(const mpt value) {
    return value.list->item_size * BITS_IN_BYTE;
}

size_t mpt_segment_count(const mpt value) {
    return vector_count(value.list);
}

size_t mpt_bit_count(const mpt value) {
    return mpt_segment_count(value) * mpt_bits_in_segment(value);
}

segment_type *mpt_get_segment_ptr(const mpt value, const size_t at) {
    return (segment_type *)vector_at(value.list, at);
}

segment_type mpt_get_segment(const mpt value, const size_t at) {
    if (at >= mpt_segment_count(value)) {
        return mpt_get_msb(value) * ~0;
    }

    return *mpt_get_segment_ptr(value, at);
}

int mpt_set_bit_to(mpt *value, const size_t at, const int bit_set) {
    size_t segment_pos, bit_pos, to_add;
    segment_type *segment;

    segment_pos = mpt_get_segment_index_(*value, at);
    bit_pos = mpt_bit_pos_in_segment_(*value, at);

    if (segment_pos >= mpt_segment_count(*value)) {
        to_add = segment_pos - mpt_segment_count(*value) + 1;
        if (!mpt_add_segments_(value, to_add)) {
            return 0;
        }
    }

    segment = mpt_get_segment_ptr(*value, segment_pos);

    if (!segment) {
        return 0;
    }

    if (bit_set) {
        *segment |= (1 << bit_pos);
    } else {
        *segment &= ~(1 << bit_pos);
    }

    return 1;
}

int mpt_get_bit(const mpt value, const size_t at) {
    segment_type *segment;
    size_t bit_pos;

    if (at >= mpt_bit_count(value)) {
        return mpt_get_msb(value);
    }
    segment = mpt_get_segment_ptr(value, mpt_get_segment_index_(value, at));
    bit_pos = mpt_bit_pos_in_segment_(value, at);

    return (*segment >> bit_pos) & 1;
}

int mpt_get_msb(const mpt value) {
    return mpt_get_bit(value, mpt_bit_count(value) - 1);
}

int mpt_is_zero(const mpt value) {
    segment_type *segment = NULL;
    size_t i;

    for (i = 0; i < mpt_segment_count(value); ++i) {
        segment = mpt_get_segment_ptr(value, i);
        if (!segment || *segment != 0) {
            return 0;
        }
    }

    return 1;
}

int mpt_is_negative(const mpt value) {
    return mpt_get_msb(value);
}

int mpt_is_odd(const mpt value) {
    return mpt_get_bit(value, 0);
}

int mpt_optimize(mpt *orig) {
    segment_type segments_to_remove, *segment;
    size_t i, last_segment;
    int msb;

    #define EXIT_IF(v) \
        if (v) { \
            return 0; \
        }

    EXIT_IF(!orig);

    msb = mpt_get_msb(*orig);
    segments_to_remove = msb * ~0;

    for (i = 0; i < mpt_segment_count(*orig); ++i) {
        last_segment = mpt_segment_count(*orig) - i - 1;
        EXIT_IF(!(segment = mpt_get_segment_ptr(*orig, last_segment)));

        if (*segment != segments_to_remove) {
            break;
        }
    }

    /* V případě samých nul, nebo samých jedniček nechceme odstranit všechny segmenty */
    if (i == mpt_segment_count(*orig)) {
        --i;
    }

    EXIT_IF(!vector_remove(orig->list, i));

    if (mpt_get_msb(*orig) != msb) {
        mpt_add_segments_(orig, 1);
        EXIT_IF(!(segment = mpt_get_segment_ptr(*orig, mpt_segment_count(*orig) - 1)));
        *segment = segments_to_remove;
    }

    return 1;
    
    #undef EXIT_IF
}

void mpt_deinit(mpt *value) {
    if (!value) {
        return;
    }

    vector_deallocate(&(value->list));
}

void mpt_deallocate(mpt **value) {
    if (!value || !*value) {
        return;
    }

    mpt_deinit(*value);
    free(*value);
    *value = NULL;
}
