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

static int mpt_is_zero_(const mpt *mpv) {
    size_t i;
    if (mpt_get_msb_(mpv) == 1) {
        return 0;
    }
    for (i = 0; i < mpt_bit_count(mpv); ++i) {
        if (mpt_get_bit(mpv, i) == 1) {
            return 0;
        }
    }
    return 1;
}

static int mpt_is_negative_(const mpt *mpv) {
    return mpt_get_msb_(mpv) == 1;
}

static int init_mpt_(mpt *mpv, const char init_value) {
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

mpt *create_mpt(const char init_value) {
    mpt *new = (mpt *)malloc(sizeof(mpt));
    if (!new) {
        return NULL;
    }

    new->list = NULL;

    if (!init_mpt_(new, init_value)) {
        mpt_free(&new);
    }

    return new;
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
        return mpt_get_msb_(mpv);
    }
    segment = (char *)vector_at(mpv->list, mpt_get_segment_(mpv, bit));
    bit_pos = mpt_bit_pos_in_segment_(mpv, bit);

    return (*segment >> bit_pos) & 1;
}

int mpt_compare(const mpt *mpv_a, const mpt *mpv_b) {
    size_t mssb_pos_a, mssb_pos_b, i;
    int bit_a, bit_b;
    if (!mpv_a && !mpv_b) {
        return 0;
    }
    if (!mpv_a) {
        return -1;
    }
    if (!mpv_b) {
        return 1;
    }

    if (mpt_signum(mpv_a) == 0 && mpt_signum(mpv_b) == 0) {
        return 0;
    }
    if (mpt_signum(mpv_a) > mpt_signum(mpv_b)) {
        return 1;
    }
    if (mpt_signum(mpv_a) < mpt_signum(mpv_b)) {
        return -1;
    }
    
    mssb_pos_a = mpt_get_mssb_pos_(mpv_a);
    mssb_pos_b = mpt_get_mssb_pos_(mpv_b);

    if (mssb_pos_a > mssb_pos_b) {
        return 1;
    }
    if (mssb_pos_a < mssb_pos_b) {
        return -1;
    }

    for (i = 0; i < mssb_pos_a; ++i) {
        bit_a = mpt_get_bit(mpv_a, mssb_pos_a - i - 1);
        bit_b = mpt_get_bit(mpv_b, mssb_pos_a - i - 1);
        
        if (bit_a > bit_b) {
            return 1;
        }
        if (bit_a < bit_b) {
            return -1;
        }
    }

    return 0;
}

int mpt_signum(const mpt *mpv) {
    if (!mpv) {
        return 0;
    }
    if (mpt_get_msb_(mpv) == 1) {
        return -1;
    }
    return !mpt_is_zero_(mpv);
}

mpt *mpt_shift(const mpt *orig, const size_t positions, const int shift_left) {
    size_t i, j;
    mpt *new = NULL, *new_tmp = NULL;
    if (!orig) {
        return NULL;
    }
    
    new_tmp = create_mpt(0);
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

    one = create_mpt(1);
    new_tmp = create_mpt(0);
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

    new_tmp = create_mpt(0);
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

mpt *mpt_sub(const mpt *mpv_a, const mpt *mpv_b) {
    mpt *negated_b = mpt_negate(mpv_b);
    mpt *new = mpt_add(mpv_a, negated_b);
    mpt_free(&negated_b);
    return new;
}

mpt *mpt_mul(const mpt *mpv_a, const mpt *mpv_b) {
    size_t i, max_bits;
    mpt *new, *new_tmp, *added, *shifted;
    new = new_tmp = added = shifted = NULL;
    if (!mpv_a || !mpv_b) {
        return NULL;
    }

    new_tmp = create_mpt(0);
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

mpt *mpt_factorial(const mpt *mpv) {
    mpt *new, *new_tmp, *sub, *sub_tmp, *one;
    new = new_tmp = sub = sub_tmp = one = NULL;
    if (!mpv) {
        return NULL;
    }
    if (mpt_is_negative_(mpv)) {
        return NULL;
    }
    if (mpt_is_zero_(mpv)) {
        return create_mpt(1);
    }

    one = create_mpt(1);
    new = copy_mpt(mpv);
    sub = mpt_sub(new, one);
    if (!new || !one || !sub) {
        goto clean_and_exit;
    }

    while (mpt_compare(sub, one) >= 1) {
        new_tmp = mpt_mul(new, sub);
        sub_tmp = mpt_sub(sub, one);

        if (!new_tmp || !sub_tmp) {
            mpt_free(&new_tmp);
            mpt_free(&sub_tmp);
            goto clean_and_exit;
        }

        mpt_free(&new);
        mpt_free(&sub);
        new = new_tmp;
        sub = sub_tmp;
    }

  clean_and_exit:
    mpt_free(&one);
    mpt_free(&sub);
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

    new = create_mpt(0);
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
