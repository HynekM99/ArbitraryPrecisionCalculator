#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
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

static size_t mpt_get_segment_(const mpt *mpv, const size_t bit) {
    return bit / (mpv->list->item_size * 8);
}

static size_t mpt_bit_pos_in_segment_(const mpt *mpv, const size_t bit) {
    return bit % (mpv->list->item_size * 8);
}

mpt *create_mpt(const char init_value) {
    mpt *new = (mpt *)malloc(sizeof(mpt));
    if (!new) {
        return NULL;
    }

    new->list = NULL;

    if (!init_mpt(new, init_value)) {
        mpt_free(&new);
    }

    return new;
}

int init_mpt(mpt *mpv, const char init_value) {
    char def = 0;
    size_t i;
    if (!mpv) {
        return 0;
    }

    vector_deallocate(&(mpv->list));
    mpv->list = vector_allocate(sizeof(char), NULL);
    if (!mpv->list) {
        return 0;
    }

    vector_push_back(mpv->list, &def);

    for (i = 0; i < sizeof(init_value) * 8; ++i) {
        if (!mpt_set_bit_to(mpv, i, ((init_value >> i) & 1) == 1)) {
            return 0;
        }
    }

    return 1;
}

mpt *copy_mpt(const mpt *orig) {
    size_t i;
    mpt *new = NULL;
    if (!orig) {
        return NULL;
    }

    new = create_mpt(0);
    if (!new) {
        return NULL;
    }

    for (i = 0; i < mpt_bit_count(orig); ++i) {
        if (!mpt_set_bit_to(new, i, mpt_get_bit(orig, i))) {
            mpt_free(&new);
            return NULL;
        }
    }

    return new;
}

size_t mpt_bit_count(const mpt *mpv) {
    return mpv ? vector_count(mpv->list) * mpv->list->item_size * 8 : 0;
}

int mpt_set_bit_to(mpt *mpv, const size_t bit, const int bit_set) {
    char *segment;
    size_t segment_pos, bit_pos, to_add;

    segment_pos = mpt_get_segment_(mpv, bit);
    bit_pos = mpt_bit_pos_in_segment_(mpv, bit);

    if (segment_pos >= vector_count(mpv->list)) {
        to_add = segment_pos - vector_count(mpv->list) + 1;
        if (!mpt_add_segments_(mpv, to_add)) {
            return 0;
        }
    }

    segment = (char *)vector_at(mpv->list, segment_pos);

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
    segment = (char *)vector_at(mpv->list, mpt_get_segment_(mpv, bit));
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
    size_t i;
    if (!mpv) {
        return 0;
    }
    if (mpt_get_msb(mpv) == 1) {
        return 0;
    }
    for (i = 0; i < mpt_bit_count(mpv); ++i) {
        if (mpt_get_bit(mpv, i) == 1) {
            return 0;
        }
    }
    return 1;
}

int mpt_is_negative(const mpt *mpv) {
    if (!mpv) {
        return 0;
    }
    return mpt_get_msb(mpv) == 1;
}

mpt *mpt_optimize(const mpt *orig) {
    char segments_to_remove, *segment;
    int msb;
    size_t i, last_segment, bits;
    mpt *new = NULL;

    #define EXIT_IF_NOT(v) \
        if (!(v)) { \
            mpt_free(&new); \
            return NULL; \
        }

    EXIT_IF_NOT(orig);

    new = create_mpt(0);
    EXIT_IF_NOT(new);

    msb = mpt_get_msb(orig);
    segments_to_remove = msb * ~0;

    for (i = 0; i < vector_count(orig->list); ++i) {
        last_segment = vector_count(orig->list) - i - 1;
        segment = (char *)vector_at(orig->list, last_segment);
        EXIT_IF_NOT(segment);

        if (*segment != segments_to_remove) {
            break;
        }
    }

    for (i = 0; i < (last_segment + 1) * orig->list->item_size * 8; ++i) {
        EXIT_IF_NOT(mpt_set_bit_to(new, i, mpt_get_bit(orig, i)));
    }

    if (mpt_get_msb(new) != msb) {
        bits = mpt_bit_count(new);
        for (i = bits; i < bits + new->list->item_size * 8; ++i) {
            EXIT_IF_NOT(mpt_set_bit_to(new, i, msb));
        }
    }

    return new;
}

void mpt_free(mpt **mpv) {
    if (!mpv || !*mpv) {
        return;
    }

    vector_deallocate(&((*mpv)->list));
    free(*mpv);
    *mpv = NULL;
}
