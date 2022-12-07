#include <stdio.h>
#include <stdlib.h>
#include "mpt.h"

typedef mpt* (*bi_function)(const mpt *, const mpt *);
typedef mpt* (*un_function)(const mpt *);

typedef void (*test)(char *);

int assert_equals(const char test_name[], const mpt *mpv_test_res, const mpt *mpv_true_res) {
    if (mpt_compare(mpv_test_res, mpv_true_res) == 0) {
        printf("%s ... OK\n", test_name);
        return 1;
    }
    printf("%s ... Failed!\n", test_name);
    return 0;
} 

void test_bi(const char test_name[], const bi_function func, const char value_a[], const char value_b[], const char true_value[]) {
    mpt *mpv_a, *mpv_b, *mpv_test_res, *mpv_true_res;
    mpv_a = mpv_b = mpv_test_res = mpv_true_res = NULL;

    mpv_a = mpt_parse_str(value_a, dec);
    mpv_b = mpt_parse_str(value_b, dec);
    mpv_true_res = mpt_parse_str(true_value, dec);
    mpv_test_res = func(mpv_a, mpv_b);

    assert_equals(test_name, mpv_test_res, mpv_true_res);

    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_un(const char test_name[], const un_function func, const char value[], const char true_value[]) {
    mpt *mpv, *mpv_test_res, *mpv_true_res;
    mpv = mpv_test_res = mpv_true_res = NULL;

    mpv = mpt_parse_str(value, dec);
    mpv_true_res = mpt_parse_str(true_value, dec);
    mpv_test_res = func(mpv);

    assert_equals(test_name, mpv_test_res, mpv_true_res);

    mpt_free(&mpv);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_add_1(char test_name[]) {
    char value_a[100] =  "100000000000";
    char value_b[100] =   "25000000001";
    char true_value[100] = "125000000001";
    test_bi(test_name, mpt_add, value_a, value_b, true_value);
}

void test_add_2(char test_name[]) {
    char value_a[100] =  "100000000000";
    char value_b[100] =  "-25000000001";
    char true_value[100] =  "74999999999";
    test_bi(test_name, mpt_add, value_a, value_b, true_value);
}

void test_add_3(char test_name[]) {
    char value_a[100] =  "4";
    char value_b[100] =  "-25000000001";
    char true_value[100] =  "-24999999997";
    test_bi(test_name, mpt_add, value_a, value_b, true_value);
}

void test_mul_1(char test_name[]) {
    char value_a[100] = "13000000000000";
    char value_b[100] = "10";
    char true_value[100] = "130000000000000";
    test_bi(test_name, mpt_mul, value_a, value_b, true_value);
}

void test_mul_2(char test_name[]) {
    char value_a[100] = "-2";
    char value_b[100] = "871346078019283655";
    char true_value[100] = "-1742692156038567310";
    test_bi(test_name, mpt_mul, value_a, value_b, true_value);
}

void test_div_1(char test_name[]) {
    char value_a[100] =  "4294967295";
    char value_b[100] =  "8";
    char true_value[100] =  "536870911";
    test_bi(test_name, mpt_div, value_a, value_b, true_value);
}

void test_div_2(char test_name[]) {
    char value_a[100] =  "-10";
    char value_b[100] =  "3787890478781928745";
    char true_value[100] =  "0";
    test_bi(test_name, mpt_div, value_a, value_b, true_value);
}

void test_mod_1(char test_name[]) {
    char value_a[100] =  "4";
    char value_b[100] =  "10";
    char true_value[100] =  "4";
    test_bi(test_name, mpt_mod, value_a, value_b, true_value);
}

void test_mod_2(char test_name[]) {
    char value_a[100] =  "-10";
    char value_b[100] =  "3";
    char true_value[100] =  "-1";
    test_bi(test_name, mpt_mod, value_a, value_b, true_value);
}

void test_pow_1(char test_name[]) {
    char value_base[100] =  "2";
    char value_exp[100] =  "64";
    char true_value[100] =  "18446744073709551616";
    test_bi(test_name, mpt_pow, value_base, value_exp, true_value);
}

void test_pow_2(char test_name[]) {
    char value_base[100] =  "-2";
    char value_exp[100] =  "31";
    char true_value[100] =  "-2147483648";
    test_bi(test_name, mpt_pow, value_base, value_exp, true_value);
}

void test_pow_3(char test_name[]) {
    char value_base[100] =  "-2";
    char value_exp[100] =  "-8";
    char true_value[100] =  "0";
    test_bi(test_name, mpt_pow, value_base, value_exp, true_value);
}

void test_factorial_1(char test_name[]) {
    char value[100] = "69";
    char true_value[329] = "171122452428141311372468338881272839092270544893520369393648040923257279754140647424000000000000000";
    test_un(test_name, mpt_factorial, value, true_value);
}

void test_factorial_2(char test_name[]) {
    char value[100] = "-1";
    char true_value[100] = "N";
    test_un(test_name, mpt_factorial, value, true_value);
}

void test_factorial_3(char test_name[]) {
    char value[100] = "0";
    char true_value[100] = "1";
    test_un(test_name, mpt_factorial, value, true_value);
}

void test_print_bin() {
    char value_dec[1000] = "1010111001011100101000100101011010101111101010100000001101100";
    mpt *mpv = mpt_parse_str(value_dec, bin);
    mpt_print_bin(mpv);
    printf("\n");
    mpt_free(&mpv);
}

void test_print_dec() {
    char value_dec[1000] = "-4098000023456787656848390217408925789234789623897410298375465667576568768767860760867545453423121327";
    mpt *mpv = mpt_parse_str(value_dec, dec);
    mpt_print_dec(mpv);
    printf("\n");
    mpt_free(&mpv);
}

void test_print_hex() {
    char value_dec[1000] = "-f";
    mpt *mpv = mpt_parse_str(value_dec, hex);
    mpt_print_hex(mpv);
    printf("\n");
    mpt_free(&mpv);
}

int main() {
    test_add_1("Add test 1");
    test_add_2("Add test 2");
    test_mul_1("Mul test 1");
    test_mul_2("Mul test 2");
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
    test_print_bin();
    test_print_dec();
    test_print_hex();

    return EXIT_SUCCESS;
}