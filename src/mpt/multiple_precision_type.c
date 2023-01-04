#include <stdlib.h>
#include "multiple_precision_type.h"

static int mpt_add_segments_(mpt *mpv, const size_t segments_to_add) {
    size_t i, def = 0;

    if (!mpv || segments_to_add == 0) {
        return 0;
    }

    for (i = 0; i < segments_to_add; ++i) {
        if (!vector_push_back(mpv->list, &def)) {
            return 0;
        }
    }

    return 1;
}

static size_t mpt_get_segment_index_(const mpt *mpv, const size_t bit) {
    return bit / mpt_bits_in_segment(mpv);
}

static size_t mpt_bit_pos_in_segment_(const mpt *mpv, const size_t bit) {
    return bit % mpt_bits_in_segment(mpv);
}

static int mpt_init_(mpt *mpv, const char init_value) {
    char *default_segment;
    
    if (!mpv) {
        return 0;
    }

    vector_deallocate(&mpv->list);
    if (!(mpv->list = vector_allocate(sizeof(char), NULL))) {
        return 0;
    }

    if (!mpt_add_segments_(mpv, 1)) {
        return 0;
    }
    
    if (!(default_segment = mpt_get_segment_ptr(mpv, 0))) {
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

    if (!mpt_init_(new, init_value)) {
        mpt_deallocate(&new);
    }

    return new;
}

mpt *mpt_clone(const mpt *orig) {
    mpt *new = NULL;
    if (!orig) {
        return NULL;
    }

    if (!(new = mpt_allocate(0))) {
        return NULL;
    }

    vector_deallocate(&new->list);
    if (!(new->list = vector_clone(orig->list))) {
        mpt_deallocate(&new);
    }

    return new;
}

void mpt_replace(mpt **to_replace, mpt **replace_with) {
    if (!to_replace || !replace_with) {
        return;
    }
    mpt_deallocate(to_replace);
    *to_replace = *replace_with;
    *replace_with = NULL;
}

size_t mpt_bits_in_segment(const mpt *mpv) {
    return mpv ? mpv->list->item_size * BITS_IN_BYTE : 0;
}

size_t mpt_segment_count(const mpt *mpv) {
    return mpv ? vector_count(mpv->list) : 0;
}

size_t mpt_bit_count(const mpt *mpv) {
    return mpt_segment_count(mpv) * mpt_bits_in_segment(mpv);
}

char *mpt_get_segment_ptr(const mpt *mpv, const size_t index) {
    return mpv ? (char *)vector_at(mpv->list, index) : NULL;
}

char mpt_get_segment(const mpt *mpv, const size_t index) {
    if (index >= vector_count(mpv->list)) {
        return mpt_get_msb(mpv) * 0xff;
    }

    return *mpt_get_segment_ptr(mpv, index);
}

int mpt_set_bit_to(mpt *mpv, const size_t bit, const int bit_set) {
    size_t segment_pos, bit_pos, to_add;
    char *segment;

    if (!mpv) {
        return 0;
    }

    segment_pos = mpt_get_segment_index_(mpv, bit);
    bit_pos = mpt_bit_pos_in_segment_(mpv, bit);

    if (segment_pos >= vector_count(mpv->list)) {
        to_add = segment_pos - vector_count(mpv->list) + 1;
        if (!mpt_add_segments_(mpv, to_add)) {
            return 0;
        }
    }

    segment = mpt_get_segment_ptr(mpv, segment_pos);

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

int mpt_get_bit(const mpt *mpv, const size_t bit) {
    char *segment;
    size_t bit_pos;

    if (bit >= mpt_bit_count(mpv)) {
        return mpt_get_msb(mpv);
    }
    segment = mpt_get_segment_ptr(mpv, mpt_get_segment_index_(mpv, bit));
    bit_pos = mpt_bit_pos_in_segment_(mpv, bit);

    return (*segment >> bit_pos) & 1;
}

int mpt_get_msb(const mpt *mpv) {
    return mpt_get_bit(mpv, mpt_bit_count(mpv) - 1);
}

size_t mpt_get_mssb_pos(const mpt *mpv) {
    size_t i, bit_pos, bit_count;
    bit_count = mpt_bit_count(mpv);

    for (i = 0; i < bit_count; ++i) {
        bit_pos = bit_count - i - 1;
        if (mpt_get_bit(mpv, bit_pos)) {
            return bit_pos;
        }
    }

    return ~0;
}

int mpt_is_zero(const mpt *mpv) {
    char *segment = NULL;
    size_t i;
    if (!mpv) {
        return 0;
    }

    for (i = 0; i < vector_count(mpv->list); ++i) {
        segment = mpt_get_segment_ptr(mpv, i);
        if (!segment || *segment != 0) {
            return 0;
        }
    }

    return 1;
}

int mpt_is_negative(const mpt *mpv) {
    return mpv ? mpt_get_msb(mpv) : 0;
}

int mpt_is_odd(const mpt *mpv) {
    return mpv ? mpt_get_bit(mpv, 0) : 0;
}

mpt *mpt_optimize(const mpt *orig) {
    char segments_to_remove, *segment;
    size_t i, last_segment;
    mpt *new = NULL;
    int msb;

    #define EXIT_IF(v) \
        if (v) { \
            mpt_deallocate(&new); \
            return NULL; \
        }

    EXIT_IF(!orig);

    EXIT_IF(!(new = mpt_clone(orig)));

    msb = mpt_get_msb(orig);
    segments_to_remove = msb * 0xff;

    for (i = 0; i < vector_count(orig->list); ++i) {
        last_segment = vector_count(orig->list) - i - 1;
        EXIT_IF(!(segment = mpt_get_segment_ptr(orig, last_segment)));

        if (*segment != segments_to_remove) {
            break;
        }
    }

    /* Nechceme odstranit všechny segmenty */
    if (i == vector_count(orig->list)) {
        --i;
    }

    EXIT_IF(!vector_remove(new->list, i));

    if (mpt_get_msb(new) != msb) {
        mpt_add_segments_(new, 1);
        EXIT_IF(!(segment = mpt_get_segment_ptr(new, vector_count(new->list) - 1)));
        *segment = segments_to_remove;
    }

    EXIT_IF(!vector_realloc(new->list, vector_count(new->list)));

    return new;

    #undef EXIT_IF
}

void mpt_deallocate(mpt **mpv) {
    if (!mpv || !*mpv) {
        return;
    }

    vector_deallocate(&((*mpv)->list));
    free(*mpv);
    *mpv = NULL;
}
