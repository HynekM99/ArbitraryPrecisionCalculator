#include <stdio.h>
#include <stdlib.h>
#include "multiple_precision_type.h"

void test_add(const mpt *mpt_a, const mpt *mpt_b) {
    mpt *mpt_res;

    if (!mpt_a || !mpt_b) {
        return;
    }

    mpt_res = mpt_add(mpt_a, mpt_b);
    if (!mpt_res) {
        return;
    }

    mpt_print_bin(mpt_a);
    printf(" + ");
    mpt_print_bin(mpt_b);
    printf(" = ");
    mpt_print_bin(mpt_res);
    mpt_free(&mpt_res);
    printf("\n");
}

void test_negate(const mpt *mpt_a) {
    mpt *mpt_res;

    if (!mpt_a) {
        return;
    }

    mpt_res = mpt_negate(mpt_a);
    if (!mpt_res) {
        return;
    }

    printf("-");
    mpt_print_bin(mpt_a);
    printf(" = ");
    mpt_print_bin(mpt_res);
    printf("\n");
    mpt_free(&mpt_res);
}

void test_compare(const mpt *mpt_a, const mpt *mpt_b) {
    int cmp = mpt_compare(mpt_a, mpt_b);

    if (cmp == -2) {
        return;
    }

    mpt_print_bin(mpt_a);
    switch (cmp)
    {
        case -1: printf(" < "); break;
        case  1: printf(" > "); break;
        case  0: printf(" = "); break;
        default: break;
    }
    mpt_print_bin(mpt_b);
    printf("\n");
}

void set_bits(mpt *mpt, int value) {
    size_t i;

    if (!mpt) {
        return;
    }

    for (i = 0; i < sizeof(value) * 8; ++i) {
        if (((value >> i) & 1) == 1) {
            mpt_set_bit(mpt, i);
        }
        else {
            mpt_reset_bit(mpt, i);
        }
    }
}

int main() {
    int value_a = -5;
    int value_b = 1;
    mpt *mpt_a = create_mpt();
    mpt *mpt_b = create_mpt();

    if (!mpt_a || !mpt_b) {
        return EXIT_FAILURE;
    }

    set_bits(mpt_a, value_a);
    set_bits(mpt_b, value_b);

    test_add(mpt_a, mpt_b);
    test_negate(mpt_a);
    test_compare(mpt_a, mpt_b);

    mpt_free(&mpt_a);
    mpt_free(&mpt_b);
    return EXIT_SUCCESS;
}