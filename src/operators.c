#include "operators.h"

#include <stddef.h>

/**
 * @brief Pole dostupných dvou-operandových operací a konstanta, která udržuje jejich počet.
 */
const bi_func_oper_type BI_OPERATORS[] = {
    { '+', mpt_add, 1, left },       /* Sčítání.  */
    { '-', mpt_sub, 1, left },      /* Odčítání. */
    { '*', mpt_mul, 3, left },      /* Násobení. */
    { '/', mpt_div, 3, left },      /* Dělení.   */
    { '%', mpt_mod, 2, left },
    { '^', mpt_pow, 4, right }
};
const size_t BI_OPERATORS_COUNT = sizeof(BI_OPERATORS) / sizeof(*BI_OPERATORS);

/**
 * @brief Pole dostupných jedno-operandových operací a konstanta, která udržuje jejich počet.
 */
const un_func_oper_type UN_OPERATORS[] = {
    { RPN_UNARY_MINUS_SYMBOL, mpt_negate, 5, right }, 
    { '!', mpt_factorial, 4, left }
};
const size_t UN_OPERATORS_COUNT = sizeof(UN_OPERATORS) / sizeof(*UN_OPERATORS);

bi_func_oper_type *get_bi_func_operator(const char operator) {
    size_t i;

    for (i = 0; i < BI_OPERATORS_COUNT; ++i) {
        if (BI_OPERATORS[i].operator == operator) {
            return &(BI_OPERATORS[i]);
        }
    }

    return NULL;
}

un_func_oper_type *get_un_func_operator(const char operator) {
    size_t i;

    for (i = 0; i < UN_OPERATORS_COUNT; ++i) {
        if (UN_OPERATORS[i].operator == operator) {
            return &(UN_OPERATORS[i]);
        }
    }

    return NULL;
}