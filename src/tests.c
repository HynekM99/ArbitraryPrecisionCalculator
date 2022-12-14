#include <stdio.h>
#include <stdlib.h>
#include "mpt/mpt.h"
#include "shunting_yard.h"

typedef void (*test)();

int assert_equals(const mpt *mpv_test_res, const mpt *mpv_true_res) {
    if (mpt_compare(mpv_test_res, mpv_true_res) == 0) {
        printf("OK\n");
        return 1;
    }
    printf("Failed!\n");
    return 0;
} 

void test_bi(const bi_function func, const char *value_a, const char *value_b, const char *true_value) {
    mpt *mpv_a, *mpv_b, *mpv_test_res, *mpv_true_res;
    mpv_a = mpv_b = mpv_test_res = mpv_true_res = NULL;

    mpv_a = mpt_parse_str(&value_a);
    mpv_b = mpt_parse_str(&value_b);
    mpv_true_res = mpt_parse_str(&true_value);
    mpv_test_res = func(mpv_a, mpv_b);

    assert_equals(mpv_test_res, mpv_true_res);

    mpt_free(&mpv_a);
    mpt_free(&mpv_b);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_un(const un_function func, const char *value, const char *true_value) {
    mpt *mpv, *mpv_test_res, *mpv_true_res;
    mpv = mpv_test_res = mpv_true_res = NULL;

    mpv = mpt_parse_str(&value);
    mpv_true_res = mpt_parse_str(&true_value);
    mpv_test_res = func(mpv);

    assert_equals(mpv_test_res, mpv_true_res);

    mpt_free(&mpv);
    mpt_free(&mpv_test_res);
    mpt_free(&mpv_true_res);
}

void test_add_1() {
    char value_a[100] =  "10";
    char value_b[100] =   "25";
    char true_value[100] = "35";
    test_bi(mpt_add, value_a, value_b, true_value);
}

void test_add_2() {
    char value_a[100] =  "100000000000";
    char value_b[100] =  "-25000000001";
    char true_value[100] =  "74999999999";
    test_bi(mpt_add, value_a, value_b, true_value);
}

void test_add_3() {
    char value_a[100] =  "4";
    char value_b[100] =  "-25000000001";
    char true_value[100] =  "-24999999997";
    test_bi(mpt_add, value_a, value_b, true_value);
}

void test_mul_1() {
    char value_a[100] = "13000000000000";
    char value_b[100] = "10";
    char true_value[100] = "130000000000000";
    test_bi(mpt_mul, value_a, value_b, true_value);
}

void test_mul_2() {
    char value_a[100] = "-2";
    char value_b[100] = "871346078019283655";
    char true_value[100] = "-1742692156038567310";
    test_bi(mpt_mul, value_a, value_b, true_value);
}

void test_div_1() {
    char value_a[100] =  "4294967295";
    char value_b[100] =  "8";
    char true_value[100] =  "536870911";
    test_bi(mpt_div, value_a, value_b, true_value);
}

void test_div_2() {
    char value_a[100] =  "-10";
    char value_b[100] =  "3787890478781928745";
    char true_value[100] =  "0";
    test_bi(mpt_div, value_a, value_b, true_value);
}

void test_mod_1() {
    char value_a[100] =  "4";
    char value_b[100] =  "10";
    char true_value[100] =  "4";
    test_bi(mpt_mod, value_a, value_b, true_value);
}

void test_mod_2() {
    char value_a[100] =  "-10";
    char value_b[100] =  "3";
    char true_value[100] =  "-1";
    test_bi(mpt_mod, value_a, value_b, true_value);
}

void test_pow_1() {
    char value_base[100] =  "2";
    char value_exp[100] =  "64";
    char true_value[100] =  "18446744073709551616";
    test_bi(mpt_pow, value_base, value_exp, true_value);
}

void test_pow_2() {
    char value_base[100] =  "-2";
    char value_exp[100] =  "31";
    char true_value[100] =  "-2147483648";
    test_bi(mpt_pow, value_base, value_exp, true_value);
}

void test_pow_3() {
    char value_base[100] =  "-66679278278920452";
    char value_exp[100] =  "-877508928346509";
    char true_value[100] =  "0";
    test_bi(mpt_pow, value_base, value_exp, true_value);
}

void test_fac_1() {
    char value[100] = "69";
    char true_value[329] = "171122452428141311372468338881272839092270544893520369393648040923257279754140647424000000000000000";
    test_un(mpt_factorial, value, true_value);
}

void test_fac_2() {
    char value[100] = "-1";
    char true_value[100] = "NULL";
    test_un(mpt_factorial, value, true_value);
}

void test_fac_3() {
    char value[100] = "0";
    char true_value[100] = "1";
    test_un(mpt_factorial, value, true_value);
}

void test_parse() {
    const char *value = "0x0afadf7868373875afedbcddcbad";
    mpt *mpv = mpt_parse_str(&value);
    mpt_print(mpv, dec);
    printf("\n");
    mpt_free(&mpv);
}

void run_tests(const char name[], const test tests[], const size_t count) {
    size_t i;

    printf("%s\n", name);
    for (i = 0; i < count; ++i) {
        printf("%li ... ", i + 1);
        tests[i]();
    }
}

stack *vector_to_stack(vector_type **v) {
    size_t i = 0;
    stack *s = NULL;
    if (!v || !*v) {
        return NULL;
    }

    s = stack_create(vector_count(*v), (*v)->item_size);
    if (!s) {
        return NULL;
    }

    for (i = 0; i < vector_count(*v); ++i) {
        if (!stack_push(s, vector_at(*v, vector_count(*v) - i - 1))) {
            stack_free(&s);
            return NULL;
        }
    }

    (*v)->deallocator = NULL;
    vector_deallocate(v);
    return s;
}

void test_shunting() {
    const char *str = "5939875389* 0b10010101/ (0x1 -0b01)\000";
    vector_type *rpn_str = NULL;
    vector_type *vector_values = NULL;
    stack *values = NULL;
    mpt *result = NULL;

    int i = shunt(str, &rpn_str, &vector_values);
    if (!i) {
        goto clean_and_exit;
    }

    values = vector_to_stack(&vector_values);
    result = evaluate_rpn(rpn_str, values);

    mpt_print(result, hex);
    printf("\n");

  clean_and_exit:
    vector_deallocate(&rpn_str);
    vector_deallocate(&vector_values);
    stack_free(&values);
    mpt_free(&result);
    free(vector_values);
}

int main() {
    test add_tests[3] = {test_add_1, test_add_2, test_add_3};
    test mul_tests[2] = {test_mul_1, test_mul_2};
    test div_tests[2] = {test_div_1, test_div_2};
    test mod_tests[2] = {test_mod_1, test_mod_2};
    test pow_tests[3] = {test_pow_1, test_pow_2, test_pow_3};
    test fac_tests[3] = {test_fac_1, test_fac_2, test_fac_3};

    run_tests("Addition tests", add_tests, sizeof(add_tests) / sizeof(test));
    run_tests("Multiplication tests", mul_tests, sizeof(mul_tests) / sizeof(test));
    run_tests("Division tests", div_tests, sizeof(div_tests) / sizeof(test));
    run_tests("Modulo tests", mod_tests, sizeof(mod_tests) / sizeof(test));
    run_tests("Power tests", pow_tests, sizeof(pow_tests) / sizeof(test));
    run_tests("Factorial tests", fac_tests, sizeof(fac_tests) / sizeof(test));
    test_parse();
    test_shunting();

    return EXIT_SUCCESS;
}