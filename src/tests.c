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
        printf("%s ... Failed!\n", test_name);
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
        printf("%s ... Failed!\n", test_name);
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_mul_1(char test_name[]) {
    char value_a[100] =  "13";
    char value_b[100] =  "-10";
    char true_res[100] =  "-130";
    mpt *mpv_a, *mpv_b, *mpv_test_res, *mpv_true_res;
    mpv_a = mpv_b = mpv_test_res = mpv_true_res = NULL;

    mpv_a = create_mpt(0);
    mpt_parse_str(&mpv_a, value_a, dec);
    mpv_b = create_mpt(0);
    mpt_parse_str(&mpv_b, value_b, dec);
    mpv_true_res = create_mpt(0);
    mpt_parse_str(&mpv_true_res, true_res, dec);

    mpv_test_res = mpt_mul(mpv_a, mpv_b);
    if (!mpv_test_res) {
        printf("%s ... Failed!\n", test_name);
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_div_1(char test_name[]) {
    char value_a[100] =  "4294967295";
    char value_b[100] =  "8";
    char true_res[100] =  "536870911";
    mpt *mpv_a, *mpv_b, *mpv_test_res, *mpv_true_res;
    mpv_a = mpv_b = mpv_test_res = mpv_true_res = NULL;

    mpv_a = create_mpt(0);
    mpt_parse_str(&mpv_a, value_a, dec);
    mpv_b = create_mpt(0);
    mpt_parse_str(&mpv_b, value_b, dec);
    mpv_true_res = create_mpt(0);
    mpt_parse_str(&mpv_true_res, true_res, dec);

    mpv_test_res = mpt_div(mpv_a, mpv_b);
    if (!mpv_test_res) {
        printf("%s ... Failed!\n", test_name);
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_div_2(char test_name[]) {
    char value_a[100] =  "-10";
    char value_b[100] =  "3787890478781928745";
    char true_res[100] =  "0";
    mpt *mpv_a, *mpv_b, *mpv_test_res, *mpv_true_res;
    mpv_a = mpv_b = mpv_test_res = mpv_true_res = NULL;

    mpv_a = create_mpt(0);
    mpt_parse_str(&mpv_a, value_a, dec);
    mpv_b = create_mpt(0);
    mpt_parse_str(&mpv_b, value_b, dec);
    mpv_true_res = create_mpt(0);
    mpt_parse_str(&mpv_true_res, true_res, dec);

    mpv_test_res = mpt_div(mpv_a, mpv_b);
    if (!mpv_test_res) {
        printf("%s ... Failed!\n", test_name);
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_mod_1(char test_name[]) {
    char value_a[100] =  "4";
    char value_b[100] =  "10";
    char true_res[100] =  "4";
    mpt *mpv_a, *mpv_b, *mpv_test_res, *mpv_true_res;
    mpv_a = mpv_b = mpv_test_res = mpv_true_res = NULL;

    mpv_a = create_mpt(0);
    mpt_parse_str(&mpv_a, value_a, dec);
    mpv_b = create_mpt(0);
    mpt_parse_str(&mpv_b, value_b, dec);
    mpv_true_res = create_mpt(0);
    mpt_parse_str(&mpv_true_res, true_res, dec);

    mpv_test_res = mpt_mod(mpv_a, mpv_b);
    if (!mpv_test_res) {
        printf("%s ... Failed!\n", test_name);
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_mod_2(char test_name[]) {
    char value_a[100] =  "-10";
    char value_b[100] =  "3";
    char true_res[100] =  "-1";
    mpt *mpv_a, *mpv_b, *mpv_test_res, *mpv_true_res;
    mpv_a = mpv_b = mpv_test_res = mpv_true_res = NULL;

    mpv_a = create_mpt(0);
    mpt_parse_str(&mpv_a, value_a, dec);
    mpv_b = create_mpt(0);
    mpt_parse_str(&mpv_b, value_b, dec);
    mpv_true_res = create_mpt(0);
    mpt_parse_str(&mpv_true_res, true_res, dec);

    mpv_test_res = mpt_mod(mpv_a, mpv_b);
    if (!mpv_test_res) {
        printf("%s ... Failed!\n", test_name);
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_pow_1(char test_name[]) {
    char value_base[100] =  "2";
    char value_exp[100] =  "64";
    char true_res[100] =  "10000000000000000000000000000000000000000000000000000000000000000";
    mpt *mpv_base, *mpv_exp, *mpv_test_res, *mpv_true_res;
    mpv_base = mpv_exp = mpv_test_res = mpv_true_res = NULL;

    mpv_base = create_mpt(0);
    mpt_parse_str(&mpv_base, value_base, dec);
    mpv_exp = create_mpt(0);
    mpt_parse_str(&mpv_exp, value_exp, dec);
    mpv_true_res = create_mpt(0);
    mpt_parse_str(&mpv_true_res, true_res, bin);

    mpv_test_res = mpt_pow(mpv_base, mpv_exp);
    if (!mpv_test_res) {
        printf("%s ... Failed!\n", test_name);
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_base);
    mpt_free(&mpv_exp);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_pow_2(char test_name[]) {
    char value_base[100] =  "-2";
    char value_exp[100] =  "31";
    char true_res[100] =  "-2147483648";
    mpt *mpv_base, *mpv_exp, *mpv_test_res, *mpv_true_res;
    mpv_base = mpv_exp = mpv_test_res = mpv_true_res = NULL;

    mpv_base = create_mpt(0);
    mpt_parse_str(&mpv_base, value_base, dec);
    mpv_exp = create_mpt(0);
    mpt_parse_str(&mpv_exp, value_exp, dec);
    mpv_true_res = create_mpt(0);
    mpt_parse_str(&mpv_true_res, true_res, dec);

    mpv_test_res = mpt_pow(mpv_base, mpv_exp);
    if (!mpv_test_res) {
        printf("%s ... Failed!\n", test_name);
        goto clean_and_exit;
    }

    assert_equals(test_name, mpv_test_res, mpv_true_res);

  clean_and_exit:
    mpt_free(&mpv_base);
    mpt_free(&mpv_exp);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_pow_3(char test_name[]) {
    char value_base[100] =  "2";
    char value_exp[100] =  "-8";
    mpt *mpv_base, *mpv_exp, *mpv_test_res, *mpv_true_res;
    mpv_base = mpv_exp = mpv_test_res = mpv_true_res = NULL;

    mpv_base = create_mpt(0);
    mpt_parse_str(&mpv_base, value_base, dec);
    mpv_exp = create_mpt(0);
    mpt_parse_str(&mpv_exp, value_exp, dec);
    mpv_true_res = create_mpt(0);

    mpv_test_res = mpt_pow(mpv_base, mpv_exp);
    assert_equals(test_name, mpv_test_res, mpv_true_res);

    mpt_free(&mpv_base);
    mpt_free(&mpv_exp);
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

void test_print_dec() {
    char value_dec[1000] = "-4098000023456787656848390217408925789234789623897410298375465667576568768767860760867545453423121327";
    mpt *mpv = create_mpt(0);
    mpt_parse_str(&mpv, value_dec, dec);
    mpt_print_dec(mpv);
    printf("\n");
    mpt_free(&mpv);
}

void test_print_hex() {
    char value_dec[1000] = "7ffff";
    mpt *mpv = create_mpt(0);
    mpt_parse_str(&mpv, value_dec, hex);
    mpt_print_hex(mpv);
    printf("\n");
    mpt_free(&mpv);
}

int main() {
    test_add_1("Add test 1");
    test_add_2("Add test 2");
    test_mul_1("Mul test 1");
    test_div_1("Division test 1");
    test_div_2("Division test 2");
    test_mod_1("Mod test 1");
    test_mod_2("Mod test 2");
    test_pow_1("Power test 1");
    test_pow_2("Power test 2");
    test_pow_3("Power test 3");
    test_factorial_1("Factorial test 1");
    test_factorial_2("Factorial test 2");
    test_factorial_3("Factorial test 3");
    test_print_dec();
    test_print_hex();

    return EXIT_SUCCESS;
}