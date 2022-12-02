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

static size_t mpt_get_mssb_pos_(const mpt *mpv) {
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

static int mpt_get_msb_(const mpt *mpv) {
    return mpt_get_bit(mpv, mpt_bit_count(mpv) - 1);
}

static int init_mpt_(mpt *mpv) {
    char def = 0;
    if (!mpv) {
        return 0;
    }
    
    mpv->list = vector_allocate(sizeof(char), NULL);
    if (!mpv->list) {
        return 0;
    }

    vector_push_back(mpv->list, &def);

    return 1;
}

static void half_adder_(const int a, const int b, int *bit, int *carry) {
    *bit = a ^ b;
    *carry = a && b;
}

static void full_adder_(const int a, const int b, int cin, int *bit, int *cout) {
    *bit = a ^ b ^ cin;
    *cout = (a && b) | (cin && (a ^ b));
}

static size_t mpt_get_segment_(const mpt *mpv, const size_t bit) {
    return bit / (mpv->list->item_size * 8);
}

static size_t mpt_bit_pos_in_segment_(const mpt *mpv, const size_t bit) {
    return bit % (mpv->list->item_size * 8);
}

mpt *create_mpt() {
    mpt *new = (mpt *)malloc(sizeof(mpt));

    if (!new || !init_mpt_(new)) {
        mpt_free(&new);
        return NULL;
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
        return mpt_get_msb_(mpv);
    }
    segment = (char *)vector_at(mpv->list, mpt_get_segment_(mpv, bit));
    bit_pos = mpt_bit_pos_in_segment_(mpv, bit);

    return (*segment >> bit_pos) & 1;
}

mpt *mpt_shift(const mpt *orig, const size_t positions, const int shift_left) {
    size_t i, j;
    mpt *new = NULL, *new_tmp = NULL;
    if (!orig) {
        return NULL;
    }
    
    new_tmp = create_mpt();
    if (!new_tmp) {
        return NULL;
    }

    if (shift_left) {
        i = 0, j = positions;
    } else {
        i = positions, j = 0;
    }

    for (; i < mpt_bit_count(orig) + orig->list->item_size * 8; ++i, ++j) {
        if (!mpt_set_bit_to(new_tmp, j, mpt_get_bit(orig, i))) {
            goto clean_and_exit;
        }
    }

    if (!vector_remove(new_tmp->list, 1)) {
        goto clean_and_exit;
    }

    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_free(&new_tmp);
    return new;
}

mpt *mpt_negate(const mpt *mpv) {
    size_t i;
    mpt *new_tmp, *new, *one;
    new_tmp = new = one = NULL;
    if (!mpv) {
        return NULL;
    }

    one = create_mpt();
    new_tmp = create_mpt();
    if (!one || !new_tmp) {
        goto clean_and_exit;
    }

    if (!mpt_set_bit_to(one, 0, 1)) {
        goto clean_and_exit;
    }

    for (i = 0; i < mpt_bit_count(mpv); ++i) {
        if (!mpt_set_bit_to(new_tmp, i, !mpt_get_bit(mpv, i))) {
            goto clean_and_exit;
        }
    }

    new = mpt_add(new_tmp, one);

  clean_and_exit:
    mpt_free(&new_tmp);
    mpt_free(&one);
    return new;
}

mpt *mpt_add(const mpt *mpv_a, const mpt *mpv_b) {
    size_t i, max_bits;
    int a, b, bit, carry;
    mpt *new, *new_tmp;
    new = new_tmp = NULL;
    if (!mpv_a || !mpv_b) {
        return NULL;
    }

    new_tmp = create_mpt();
    if (!new_tmp) {
        return NULL;
    }

    if (mpt_bit_count(mpv_a) >= mpt_bit_count(mpv_b)) {
        max_bits = mpt_bit_count(mpv_a);
    } else {
        max_bits = mpt_bit_count(mpv_b);
    }
    max_bits += mpv_a->list->item_size * 8 * 2;

    a = mpt_get_bit(mpv_a, 0);
    b = mpt_get_bit(mpv_b, 0);
    half_adder_(a, b, &bit, &carry);
    if (!mpt_set_bit_to(new_tmp, 0, bit)) {
        goto clean_and_exit;
    }

    for (i = 1; i < max_bits; ++i) {
        a = mpt_get_bit(mpv_a, i);
        b = mpt_get_bit(mpv_b, i);
        full_adder_(a, b, carry, &bit, &carry);
        if (!mpt_set_bit_to(new_tmp, i, bit)) {
            goto clean_and_exit;
        }   
    }

    if (!vector_remove(new_tmp->list, 1)) {
        goto clean_and_exit;
    }

    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_free(&new_tmp);
    return new;
}

mpt *mpt_mul(const mpt *mpv_a, const mpt *mpv_b) {
    size_t i, max_bits;
    mpt *new, *new_tmp, *added, *shifted;
    new = new_tmp = added = shifted = NULL;
    if (!mpv_a || !mpv_b) {
        return NULL;
    }

    new_tmp = create_mpt();
    if (!new_tmp) {
        return NULL;
    }

    if (mpt_bit_count(mpv_a) >= mpt_bit_count(mpv_b)) {
        max_bits = mpt_bit_count(mpv_a);
    } else {
        max_bits = mpt_bit_count(mpv_b);
    }
    max_bits *= 2;

    for (i = 0; i < max_bits; ++i) {
        if (mpt_get_bit(mpv_b, i) == 0) {
            continue;
        }

        shifted = mpt_shift(mpv_a, i, 1);
        added = mpt_add(new_tmp, shifted);

        if (!added) {
            goto clean_and_exit;
        }
        mpt_free(&shifted);
        mpt_free(&new_tmp);
        new_tmp = added;
    }

    if (mpt_bit_count(new_tmp) > max_bits) {
        if (!vector_remove(new_tmp->list, 1)) {
            goto clean_and_exit;
        }
    }
    
    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_free(&shifted);
    mpt_free(&new_tmp);
    return new;
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

    new = create_mpt();
    EXIT_IF_NOT(new);

    msb = mpt_get_msb_(orig);
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

    if (mpt_get_msb_(new) != msb) {
        bits = mpt_bit_count(new);
        for (i = bits; i < bits + sizeof(unsigned int) * 8; ++i) {
            EXIT_IF_NOT(mpt_set_bit_to(new, i, msb));
        }
    }

    return new;
}

void mpt_print_bin(const mpt *mpv) {
    size_t i, bits;
    int msb;
    if (!mpv) {
        return;
    }

    msb = mpt_get_msb_(mpv);

    printf("0b%d", msb);

    bits = mpt_bit_count(mpv);
    for (i = 1; i < bits; ++i) {
        if (mpt_get_bit(mpv, bits - i - 1) != msb) {
            break;
        }
    }

    for (; i < bits; ++i) {
        printf("%i", mpt_get_bit(mpv, bits - i - 1));
    }
}

void mpt_free(mpt **mpv) {
    if (!mpv || !*mpv) {
        return;
    }

    vector_deallocate(&((*mpv)->list));
    free(*mpv);
    *mpv = NULL;
}
