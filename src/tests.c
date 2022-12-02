#include <stdio.h>
#include <stdlib.h>
#include "multiple_precision_parsing.h"

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

int main() {
    int exit;
    char str_a[1000] = "-100101";
    char str_b[1000] = "-80";
    mpt *mpt_a = create_mpt();
    mpt *mpt_b = create_mpt();

    if (!mpt_parse_str(&mpt_a, str_a, bin) || !mpt_parse_str(&mpt_b, str_b, hex)) {
        exit = EXIT_FAILURE;
        goto clean_and_exit;
    }

    test_mul(mpt_a, mpt_b);

    exit = EXIT_SUCCESS;

  clean_and_exit:
    mpt_free(&mpt_a);
    mpt_free(&mpt_b);
    return exit;
}