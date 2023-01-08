#include <stdlib.h>
#include "multiple_precision_type.h"

/**
 * \brief Přidá zadaný počet nulových segmentů do instance mpt.
 * \param value Ukazatel na instanci mpt.
 * \param segments_to_add Počet segmentů k přidání.
 * \return int 1 jestli se přidání podařilo, 0 pokud ne.
 */
static int mpt_add_segments_(mpt value, const size_t segments_to_add) {
    size_t i, zero = 0;

    if (segments_to_add == 0) {
        return 0;
    }

    for (i = 0; i < segments_to_add; ++i) {
        if (!vector_push_back(value.list, &zero)) {
            return 0;
        }
    }

    return 1;
}

/**
 * \brief Vrátí index segmentu s bitem na at-té pozici.
 * \param value Ukazatel na instanci mpt.
 * \param at Pozice bitu, jehož segment chceme najít.
 * \return size_t Index segmentu s bitem na at-té pozici.
 */
static size_t mpt_get_segment_index_(const mpt value, const size_t at) {
    return at / mpt_bits_in_segment(value);
}

/**
 * \brief Vrátí index bitu v segmentu s bitem na at-té pozici.
 * \param value Ukazatel na instanci mpt.
 * \param at Pozice bitu, jehož pozici v jemu příslušném segmentu chceme najít.
 * \return size_t Index bitu v segmentu s bitem na at-té pozici.
 */
static size_t mpt_bit_pos_in_segment_(const mpt value, const size_t at) {
    return at % mpt_bits_in_segment(value);
}

int mpt_init(mpt *value, const char init_value) {
    char *default_segment;

    if (!value) {
        return 0;
    }

    value->list = NULL;
    if (!(value->list = vector_allocate(sizeof(char), NULL))) {
        return 0;
    }

    if (!mpt_add_segments_(*value, 1)) {
        return 0;
    }
    
    if (!(default_segment = mpt_get_segment_ptr(*value, 0))) {
        return 0;
    }
    *default_segment = init_value;

    return 1;
}

mpt *mpt_allocate(const char init_value) {
    mpt *new = (mpt *)malloc(sizeof(mpt));
    if (!new) {
        return NULL;
    }

    new->list = NULL;

    if (!mpt_init(new, init_value)) {
        mpt_deallocate(new);
    }

    return new;
}

mpt mpt_clone(const mpt orig) {
    mpt new;
    mpt_init(&new, 0);

    vector_deallocate(&new.list);
    if (!(new.list = vector_clone(orig.list))) {
        mpt_deallocate(&new);
    }

    return new;
}

void mpt_replace(mpt *to_replace, mpt *replace_with) {
    if (!to_replace || !replace_with) {
        return;
    }
    mpt_deallocate(to_replace);
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

char *mpt_get_segment_ptr(const mpt value, const size_t at) {
    return (char *)vector_at(value.list, at);
}

char mpt_get_segment(const mpt value, const size_t at) {
    if (at >= mpt_segment_count(value)) {
        return mpt_get_msb(value) * ~0;
    }

    return *mpt_get_segment_ptr(value, at);
}

int mpt_set_bit_to(mpt value, const size_t at, const int bit_set) {
    size_t segment_pos, bit_pos, to_add;
    char *segment;

    segment_pos = mpt_get_segment_index_(value, at);
    bit_pos = mpt_bit_pos_in_segment_(value, at);

    if (segment_pos >= mpt_segment_count(value)) {
        to_add = segment_pos - mpt_segment_count(value) + 1;
        if (!mpt_add_segments_(value, to_add)) {
            return 0;
        }
    }

    segment = mpt_get_segment_ptr(value, segment_pos);

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
    char *segment;
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
    char *segment = NULL;
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

int mpt_optimize(mpt target) {
    char segments_to_remove, *segment;
    size_t i, last_segment;
    int msb;

    #define EXIT_IF(v) \
        if (v) { \
            return 0; \
        }

    msb = mpt_get_msb(target);
    segments_to_remove = msb * 0xff;

    for (i = 0; i < mpt_segment_count(target); ++i) {
        last_segment = mpt_segment_count(target) - i - 1;
        EXIT_IF(!(segment = mpt_get_segment_ptr(target, last_segment)));

        if (*segment != segments_to_remove) {
            break;
        }
    }

    /* V případě samých nul, nebo samých jedniček nechceme odstranit všechny segmenty */
    if (i == mpt_segment_count(target)) {
        --i;
    }

    EXIT_IF(!vector_remove(target.list, i));

    if (mpt_get_msb(target) != msb) {
        mpt_add_segments_(target, 1);
        EXIT_IF(!(segment = mpt_get_segment_ptr(target, mpt_segment_count(target) - 1)));
        *segment = segments_to_remove;
    }

    EXIT_IF(!vector_realloc(target.list, mpt_segment_count(target)));

    return 1;

    #undef EXIT_IF
}

void mpt_deallocate(mpt *value) {
    if (!value) {
        return;
    }

    vector_deallocate(&(value->list));
}
