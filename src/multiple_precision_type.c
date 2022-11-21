#include <stdio.h>
#include <limits.h>
#include "array_list.h"
#include "multiple_precision_type.h"

static void mpt_add_bits(mpt *mpt, const int bits_set) {
    if (!mpt) {
        return;
    }

    if (bits_set == 0) {
        list_add(mpt->list, 0);
    } else {
        list_add(mpt->list, ~0);
    }
    mpt->bits += sizeof(unsigned int) * 8;
}

static size_t mpt_get_mssb_pos(const mpt *mpt) {
    size_t i;
    size_t bit_pos;

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

    mpt->list = create_list(10);
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
    size_t index;
    size_t to_add;
    size_t i;

    if (!mpt) {
        return;
    }
    
    index = bit / (sizeof(unsigned int) * 8);

    if (index >= mpt->list->size) {
        to_add = index - mpt->list->size + 1;

        for (i = 0; i < to_add; ++i) {
            mpt_add_bits(mpt, 0);
        }
    }

    mpt->list->values[index] |= 1 << (bit % (sizeof(unsigned int) * 8));
}

void mpt_reset_bit(mpt *mpt, const size_t bit) {
    size_t index;
    size_t to_add;
    size_t i;

    if (!mpt) {
        return;
    }
    
    index = bit / (sizeof(unsigned int) * 8);

    if (index >= mpt->list->size) {
        to_add = index - mpt->list->size + 1;

        for (i = 0; i < to_add; ++i) {
            mpt_add_bits(mpt, 1);
        }
    }

    mpt->list->values[index] &= ~(1 << (bit % (sizeof(unsigned int) * 8)));
}

int mpt_get_bit(const mpt *mpt, const size_t bit) {
    size_t index;
    size_t bit_pos;

    if (bit >= mpt->bits) {
        return mpt_get_msb(mpt);
    }
    index = bit / (sizeof(unsigned int) * 8);
    bit_pos = bit % (sizeof(unsigned int) * 8);
    return (mpt->list->values[index] >> bit_pos) & 1;
}

int mpt_compare(const mpt *mpt_a, const mpt *mpt_b) {
    size_t i;
    int bit_a, bit_b, msb_a, msb_b;
    size_t mssb_a;
    size_t mssb_b;

    if (!mpt_a || !mpt_b) {
        return -2;
    }

    msb_a = mpt_get_msb(mpt_a);
    msb_b = mpt_get_msb(mpt_b);

    if (msb_a == 0 && msb_b == 1) {
        return 1;
    }
    if (msb_a == 1 && msb_b == 0) {
        return -1;
    }

    mssb_a = mpt_get_mssb_pos(mpt_a);
    mssb_b = mpt_get_mssb_pos(mpt_b);

    if (mssb_a > mssb_b) {
        return 1;
    }
    if (mssb_a < mssb_b) {
        return -1;
    }

    for (i = 0; i < mssb_a; ++i) {
        bit_a = mpt_get_bit(mpt_a, mssb_a - i - 1);
        bit_b = mpt_get_bit(mpt_b, mssb_a - i - 1);
        
        if (bit_a > bit_b) {
            return 1;
        }
        if (bit_a < bit_b) {
            return -1;
        }
    }

    return 0;
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
