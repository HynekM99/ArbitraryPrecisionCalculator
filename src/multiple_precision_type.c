#include <stdio.h>
#include <limits.h>
#include "array_list.h"
#include "multiple_precision_type.h"

#define INIT_SEGMENTS 10

static void mpt_add_segments(mpt *mpt, const size_t segments_to_add) {
    size_t i;

    if (!mpt || segments_to_add == 0) {
        return;
    }

    for (i = 0; i < segments_to_add; ++i) {
        list_add(mpt->list, 0);
        mpt->bits += sizeof(unsigned int) * 8;
    }
}

static int mpt_prepare_for_bit_operation(mpt *mpt, const size_t bit, size_t *segment, size_t *bit_pos) {
    size_t to_add;

    if (!mpt) {
        return 0;
    }
    
    *segment = bit / (sizeof(unsigned int) * 8);
    *bit_pos  = bit % (sizeof(unsigned int) * 8);
    
    if (*segment >= mpt->list->size) {
        to_add = *segment - mpt->list->size + 1;
        mpt_add_segments(mpt, to_add);
    }

    return 1;
}

static size_t mpt_get_mssb_pos(const mpt *mpt) {
    size_t i, bit_pos;

    if (!mpt) {
        return -1;
    }

    for (i = 0; i < mpt->bits; ++i) {
        bit_pos = mpt->bits - i - 1;
        if (mpt_get_bit(mpt, bit_pos)) {
            return bit_pos;
        }
    }

    return -1;
}

static int mpt_get_msb(const mpt *mpt) {
    return mpt_get_bit(mpt, mpt->bits - 1);
}

static int init_mpt(mpt *mpt) {
    if (!mpt) {
        return 0;
    }

    mpt->list = create_list(INIT_SEGMENTS);
    if (!mpt->list) {
        return 0;
    }

    list_add(mpt->list, 0);
    mpt->bits = sizeof(unsigned int) * 8;
    return 1;
}

mpt *create_mpt() {
    mpt *new = (mpt *)malloc(sizeof(mpt));
    if (!new) {
        return NULL;
    }

    if (!init_mpt(new)) {
        mpt_free(&new);
        return NULL;
    }

    return new;
}

void mpt_set_bit(mpt *mpt, const size_t bit) {
    size_t segment, bit_pos;

    if (mpt_prepare_for_bit_operation(mpt, bit, &segment, &bit_pos)) {
        mpt->list->values[segment] |= (1 << bit_pos);
    }
}

void mpt_reset_bit(mpt *mpt, const size_t bit) {
    size_t segment, bit_pos;

    if (mpt_prepare_for_bit_operation(mpt, bit, &segment, &bit_pos)) {
        mpt->list->values[segment] &= ~(1 << bit_pos);
    }
}

int mpt_get_bit(const mpt *mpt, const size_t bit) {
    size_t segment, bit_pos;

    if (bit >= mpt->bits) {
        return mpt_get_msb(mpt);
    }
    segment = bit / (sizeof(unsigned int) * 8);
    bit_pos = bit % (sizeof(unsigned int) * 8);
    return (mpt->list->values[segment] >> bit_pos) & 1;
}

mpt *mpt_shift(const mpt *orig, const size_t positions, const int left) {
    size_t i, j;
    mpt *new = NULL;

    if (!orig) {
        return new;
    }
    if (positions <= 0) {
        return orig;
    }
    
    new = create_mpt();
    if (!new) {
        return NULL;
    }

    if (left) {
        i = 0;
        j = positions;
    } else {
        i = positions;
        j = 0;
    }

    for (; i < orig->bits + (sizeof(unsigned int) * 8); ++i, ++j) {
        if (mpt_get_bit(orig, i) == 1) {
            mpt_set_bit(new, j);
        } else {
            mpt_reset_bit(new, j);
        }
    }

    new->bits -= (sizeof(unsigned int) * 8);
    return new;
}

mpt *mpt_negate(const mpt *mpt_a) {
    size_t i;
    mpt *new_tmp, *new, *one = NULL;

    if (!mpt_a) {
        return NULL;
    }

    new_tmp = create_mpt();
    one = create_mpt();
    if (!new_tmp || !one) {
        mpt_free(&new_tmp);
        mpt_free(&one);
        return NULL;
    }

    mpt_set_bit(one, 0);

    for (i = 0; i < mpt_a->bits; ++i) {
        if (mpt_get_bit(mpt_a, i) == 1) {
            mpt_reset_bit(new_tmp, i);
        }
        else {
            mpt_set_bit(new_tmp, i);
        }
    }

    new = mpt_add(new_tmp, one);

    mpt_free(&new_tmp);
    mpt_free(&one);

    if (!new) {
        return NULL;
    }

    return new;
}

mpt *mpt_add(const mpt *mpt_a, const mpt *mpt_b) {
    size_t i;
    size_t max_size;
    int cin, cout = 0;
    int a, b, bit;
    mpt *mpt = NULL;

    if (!mpt_a || !mpt_b) {
        return NULL;
    }

    mpt = create_mpt();
    if (!mpt) {
        return NULL;
    }

    if (mpt_a->bits >= mpt_b->bits) {
        max_size = mpt_a->bits + sizeof(unsigned int) * 8;
    }
    else {
        max_size = mpt_b->bits + sizeof(unsigned int) * 8;
    }

    a = mpt_get_bit(mpt_a, 0);
    b = mpt_get_bit(mpt_b, 0);
    cin = a && b;
    bit = a ^ b;
    if (bit) {
        mpt_set_bit(mpt, 0);
    }

    for (i = 1; i < max_size; ++i) {
        a = mpt_get_bit(mpt_a, i);
        b = mpt_get_bit(mpt_b, i);
        cout = (a && b) | (cin && (a ^ b));
        bit = a ^ b ^ cin;
        cin = cout;
        if (bit) {
            mpt_set_bit(mpt, i);
        } else {
            mpt_reset_bit(mpt, i);
        }
    }

    return mpt;
}

void mpt_print_bin(const mpt *mpt) {
    size_t i;
    size_t bit;
    int msb;

    if (!mpt) {
        return;
    }

    msb = mpt_get_msb(mpt);

    printf("0b%d", msb);

    for (i = 1; i < mpt->bits; ++i) {
        bit = mpt->bits - i - 1;
        if (mpt_get_bit(mpt, bit) != msb) {
            break;
        }
    }

    for (; i < mpt->bits; ++i) {
        bit = mpt->bits - i - 1;
        printf("%i", mpt_get_bit(mpt, bit));
    }
}

void mpt_free(mpt **mpt) {
    if (!mpt || !*mpt) {
        return;
    }

    list_free(&((*mpt)->list));
    free(*mpt);
    *mpt = NULL;
}
