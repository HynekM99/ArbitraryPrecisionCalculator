#include <stdio.h>
#include <stdlib.h>
#include "multiple_precision_type.h"

void test_add(const mpt *mpt_a, const mpt *mpt_b) {
    mpt *mpt_res = mpt_add(mpt_a, mpt_b);
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
    mpt *mpt_res = mpt_negate(mpt_a);
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

void test_shift(const mpt *mpt_a, const size_t positions, const int left) {
    mpt *mpt_res_1 = mpt_shift(mpt_a, positions, left);
    mpt *mpt_res_2;
    if (!mpt_res_1) {
        return;
    }

    mpt_res_2 = mpt_shift(mpt_res_1, positions, !left);
    if (!mpt_res_2) {
        mpt_free(&mpt_res_1);
        return;
    }

    mpt_print_bin(mpt_a);
    if (left) {
        printf(" << %li", positions);
    }
    else {
        printf(" >> %li", positions);
    }
    printf(" = ");
    mpt_print_bin(mpt_res_1);
    printf("\n");
    
    mpt_print_bin(mpt_res_1);
    if (!left) {
        printf(" << %li", positions);
    }
    else {
        printf(" >> %li", positions);
    }
    printf(" = ");
    mpt_print_bin(mpt_res_2);
    printf("\n");
    mpt_free(&mpt_res_1);
    mpt_free(&mpt_res_2);
}

void test_mul(const mpt *mpt_a, const mpt *mpt_b) {
    mpt *mpt_res = mpt_mul(mpt_a, mpt_b);
    if (!mpt_res) {
        return;
    }

    mpt_print_bin(mpt_a);
    printf(" * ");
    mpt_print_bin(mpt_b);
    printf(" = ");
    mpt_print_bin(mpt_res);
    mpt_free(&mpt_res);
    printf("\n");
}

void set_bits(mpt *mpt, long value) {
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
    int value_a = 40;
    int value_b = 20;
    mpt *mpt_a = create_mpt();
    mpt *mpt_b = create_mpt();

    if (!mpt_a || !mpt_b) {
        return EXIT_FAILURE;
    }

    set_bits(mpt_a, value_a);
    set_bits(mpt_b, value_b);

    test_add(mpt_a, mpt_b);
    test_mul(mpt_a, mpt_b);
    test_negate(mpt_a);
    test_shift(mpt_a, 3, 1);

    mpt_free(&mpt_a);
    mpt_free(&mpt_b);
    return EXIT_SUCCESS;
}