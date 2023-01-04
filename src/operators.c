#include "operators.h"

/**
 * @brief Pole dostupných operací a konstanta, která udržuje jejich počet.
 */
const func_oper_type OPERATORS[] = {
    { '!', NULL, mpt_factorial, 4, left },
    { '^', mpt_pow, NULL, 4, right },
    { RPN_UNARY_MINUS_SYMBOL, NULL, mpt_negate, 3, right },
    { '*', mpt_mul, NULL, 3, left },
    { '/', mpt_div, NULL, 3, left },
    { '%', mpt_mod, NULL, 2, left },
    { '+', mpt_add, NULL, 1, left },
    { '-', mpt_sub, NULL, 1, left },
};
const size_t OPERATORS_COUNT = sizeof(OPERATORS) / sizeof(*OPERATORS);

const func_oper_type *get_func_operator(const char operator) {
    size_t i;

    for (i = 0; i < OPERATORS_COUNT; ++i) {
        if (OPERATORS[i].operator == operator) {
            return &(OPERATORS[i]);
        }
    }

    return NULL;
}