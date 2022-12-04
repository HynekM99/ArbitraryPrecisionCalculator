#include <stdio.h>
#include <stdlib.h>
#include "multiple_precision_parsing.h"

int assert_equals(const char test_name[], const mpt *mpv_test_res, const mpt *mpv_true_res) {
    if (mpt_compare(mpv_test_res, mpv_true_res) == 0) {
        printf("%s ... OK\n", test_name);
        return 1;
    }
    printf("%s ... Failed!\n", test_name);
    return 0;
} 

void test_add_1(char test_name[]) {
    char value_a[100] =  "100000000000";
    char value_b[100] =   "25000000001";
    char true_res[100] = "125000000001";
    mpt *mpv_a, *mpv_b, *mpv_test_res, *mpv_true_res;
    mpv_a = mpv_b = mpv_test_res = mpv_true_res = NULL;

    mpv_a = create_mpt(0);
    mpt_parse_str(&mpv_a, value_a, dec);
    mpv_b = create_mpt(0);
    mpt_parse_str(&mpv_b, value_b, dec);
    mpv_true_res = create_mpt(0);
    mpt_parse_str(&mpv_true_res, true_res, dec);

    mpv_test_res = mpt_add(mpv_a, mpv_b);
    if (!mpv_test_res) {
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_add_2(char test_name[]) {
    char value_a[100] =  "100000000000";
    char value_b[100] =  "-25000000001";
    char true_res[100] =  "74999999999";
    mpt *mpv_a, *mpv_b, *mpv_test_res, *mpv_true_res;
    mpv_a = mpv_b = mpv_test_res = mpv_true_res = NULL;

    mpv_a = create_mpt(0);
    mpt_parse_str(&mpv_a, value_a, dec);
    mpv_b = create_mpt(0);
    mpt_parse_str(&mpv_b, value_b, dec);
    mpv_true_res = create_mpt(0);
    mpt_parse_str(&mpv_true_res, true_res, dec);

    mpv_test_res = mpt_add(mpv_a, mpv_b);
    if (!mpv_test_res) {
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_factorial_1(char test_name[]) {
    char true_res[329] = "0101000000011101001011101011010011000100111001100000110111011000001011100010010100000011100000110001111010010111101100001000110010001101101011010100010111001001011001100111111100110000100110000011011011100000000000000110101101101110011110001100011011010011010101000000000000000000000000000000000000000000000000000000000000000000";
    mpt *mpv, *mpv_test_res, *mpv_true_res;
    mpv = mpv_test_res = mpv_true_res = NULL;

    mpv_true_res = create_mpt(0);
    if (!mpt_parse_str(&mpv_true_res, true_res, bin)) {
        return;
    }

    mpv = create_mpt(69);
    mpv_test_res = mpt_factorial(mpv);

    assert_equals(test_name, mpv_test_res, mpv_true_res);

    mpt_free(&mpv);
    mpt_free(&mpv_true_res);
    mpt_free(&mpv_test_res);
}

void test_factorial_2(char test_name[]) {
    mpt *mpv, *mpv_test_res, *mpv_true_res;
    mpv = mpv_test_res = mpv_true_res = NULL;

    mpv = create_mpt(-1);
    if (!mpv) {
        return;
    }
    mpv_test_res = mpt_factorial(mpv);

    assert_equals(test_name, mpv_test_res, mpv_true_res);
    
    mpt_free(&mpv);
    mpt_free(&mpv_true_res);
    mpt_free(&mpv_test_res);
}

void test_factorial_3(char test_name[]) {
    mpt *mpv, *mpv_test_res, *mpv_true_res;
    mpv = mpv_test_res = mpv_true_res = NULL;

    mpv = create_mpt(0);
    mpv_true_res = create_mpt(1);
    if (!mpv || !mpv_true_res) {
        goto clean_and_exit;
    }
    mpv_test_res = mpt_factorial(mpv);

    assert_equals(test_name, mpv_test_res, mpv_true_res);
    
  clean_and_exit:
    mpt_free(&mpv);
    mpt_free(&mpv_true_res);
    mpt_free(&mpv_test_res);
}

int main() {
    int exit;
    char str_a[1000] = "1000";
    char str_b[1000] = "40";
    mpt *mpt_a = create_mpt(0);
    mpt *mpt_b = create_mpt(0);

    if (!mpt_parse_str(&mpt_a, str_a, dec) || !mpt_parse_str(&mpt_b, str_b, hex)) {
        exit = EXIT_FAILURE;
        goto clean_and_exit;
    }

    test_add_1("Add test 1");
    test_add_2("Add test 2");
    test_factorial_1("Factorial test 1");
    test_factorial_2("Factorial test 2");
    test_factorial_3("Factorial test 3");

    exit = EXIT_SUCCESS;

  clean_and_exit:
    mpt_free(&mpt_a);
    mpt_free(&mpt_b);
    return exit;
}