#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shunting_yard.h"
#include "data_structures/convert.h"

static int shunt_char_(const char **str, char *last_operator, vector_type *rpn_str, stack *operator_stack, vector_type *vector_values);

static void mpt_free_wrapper_(void *poor) {
    mpt_free(poor);
}

static int is_end_char(const char c) {
    return c == 0 || c == '\n';
}

static size_t get_operator_count_(const char *str) {
    size_t i = 0;
    for (; !is_end_char(*str); ++str) {
        if (get_func_operator(*str) || *str == '(') {
            ++i;
        }
    }
    return i > 0 ? i : 1;
}

static int push_parsed_value_(const mpt *mpv, vector_type *rpn_str, vector_type *values) {
    char value_symbol = RPN_VALUE_SYMBOL;
    return mpv && 
        vector_push_back(rpn_str, &value_symbol) && 
        vector_push_back(values, &mpv);
}

static int push_operator_(const char operator, vector_type *rpn_str, stack *operator_stack) {
    char c = 0;
    const func_oper_type *o1, *o2;
    o1 = o2 = NULL;

    #define EXIT_IF_NOT(v) \
        if (!(v)) { \
            return 0; \
        }

    switch (operator) {
        case '!': return vector_push_back(rpn_str, &operator);
        case '(': return stack_push(operator_stack, &operator);
        case ')': 
            while (!stack_isempty(operator_stack)) {
                EXIT_IF_NOT(stack_pop(operator_stack, &c));
                if (c == '(') {
                    return 1;
                }
                EXIT_IF_NOT(vector_push_back(rpn_str, &c));
            }
            return 0;
        default: break;
    }

    o1 = get_func_operator(operator);
    EXIT_IF_NOT(o1)

    while (!stack_isempty(operator_stack)) {
        EXIT_IF_NOT(stack_head(operator_stack, &c));
        if (c == '(') {
            return stack_push(operator_stack, &operator);
        }

        o2 = get_func_operator(c);

        if ((o2->precedence == o1->precedence && o1->assoc == right) ||
            (o1->precedence > o2->precedence)) {
            return stack_push(operator_stack, &operator);
        }

        EXIT_IF_NOT(stack_pop(operator_stack, &c) && vector_push_back(rpn_str, &c));
    }

    return stack_push(operator_stack, &operator);

    #undef EXIT_IF_NOT
}

static int shunt_value_(const char **str, char *last_operator, vector_type *rpn_str, vector_type *vector_values) {
    mpt *parsed_value = mpt_parse_str(str);

    if (!push_parsed_value_(parsed_value, rpn_str, vector_values)) {
        mpt_free(&parsed_value);
        return 0;
    }

    *last_operator = RPN_VALUE_SYMBOL;
    --*str;

    return 1;
}

static int infix_syntax_ok_(const char c, const char last_operator, const int space) {
    const func_oper_type *c_func, *last_func; 
    c_func = get_func_operator(c);
    last_func = get_func_operator(last_operator);

    if ((c == '(') || 
        (c == RPN_VALUE_SYMBOL) || 
        (c_func && c_func->un_handler && c_func->assoc == right)) {
        if (!last_func) {
            return last_operator == 0 || last_operator == '(';
        }
        return (last_func->un_handler && last_func->assoc == right) || last_func->bi_handler;
    }

    if ((c == ')') || 
        (c_func && c_func->bi_handler) ||
        (c_func && c_func->un_handler && c_func->assoc == left)) {
        if (!last_func) {
            return last_operator == RPN_VALUE_SYMBOL || last_operator == ')';
        }
        return last_func->un_handler && last_func->assoc == left && !space;
    }
    
    if (c_func && c_func->bi_handler) {
        return last_operator == RPN_VALUE_SYMBOL || last_operator == ')';
    }

    return 0;
}

static char *find_closing_bracket_(const char *str) {
    size_t l_brackets = 1, r_brackets = 0;
    for (; !is_end_char(*str); ++str) {
        switch (*str) {
            case '(': ++l_brackets; break;
            case ')': ++r_brackets; break;
            default:  break;
        }

        if (l_brackets == r_brackets) {
            return (char *)str;
        }
    }

    return NULL;
}

static int shunt_minus_(const char **str, char *last_operator, vector_type *rpn_str, stack *operator_stack, vector_type *vector_values) {
    int res;
    char minus = 0, *closing_bracket = NULL;

    if (*last_operator == ')' || *last_operator == RPN_VALUE_SYMBOL) {
        minus = '-';
    }
    else {
        minus = RPN_UNARY_MINUS_SYMBOL;
    }

    if (!infix_syntax_ok_(minus, *last_operator, *last_operator == 0 || *(*str - 1) == ' ')) {
        return SYNTAX_ERROR;
    }
    if (minus == RPN_UNARY_MINUS_SYMBOL && *(*str + 1) == ' ') {
        return SYNTAX_ERROR;
    }

    if (minus == '-' || *last_operator != '^') {
        *last_operator = minus;
        if (!push_operator_(minus, rpn_str, operator_stack)) {
            return ERROR;
        }
        return SYNTAX_OK;
    }

    *last_operator = minus;
    ++*str;

    if (**str != '(') {
        if (!shunt_value_(str, last_operator, rpn_str, vector_values)) {
            return ERROR;
        }
        ++*str;
    }
    else {
        if (!(closing_bracket = find_closing_bracket_(*str + 1))) {
            return SYNTAX_ERROR;
        }

        for (; !is_end_char(**str) && *str <= closing_bracket; ++*str) {
            res = shunt_char_(str, last_operator, rpn_str, operator_stack, vector_values);
            if (res != SYNTAX_OK) {
                return res;
            }
        }
    }

    for (; !is_end_char(**str) && **str == '!'; ++*str) {
        res = shunt_char_(str, last_operator, rpn_str, operator_stack, vector_values);
        if (res != SYNTAX_OK) {
            return res;
        }
    }
    --*str;

    if (!vector_push_back(rpn_str, &minus)) {
        return ERROR;
    }
    return SYNTAX_OK;
}

static int shunt_char_(const char **str, char *last_operator, vector_type *rpn_str, stack *operator_stack, vector_type *vector_values) {
    if (**str == ' ') {
        return SYNTAX_OK;
    }
    if (**str == RPN_UNARY_MINUS_SYMBOL) {
        return INVALID_SYMBOL;
    }
    
    if (**str == '-') {
        return shunt_minus_(str, last_operator, rpn_str, operator_stack, vector_values);
    }
    else if (**str >= '0' && **str <= '9') {
        if (!infix_syntax_ok_(RPN_VALUE_SYMBOL, *last_operator, *last_operator == 0 || *(*str - 1) == ' ')) {
            return SYNTAX_ERROR;
        }
        if (!shunt_value_(str, last_operator, rpn_str, vector_values)) {
            return ERROR;
        }
    }
    else if (get_func_operator(**str) || **str == '(' || **str == ')') {
        if (!infix_syntax_ok_(**str, *last_operator, *last_operator == 0 || *(*str - 1) == ' ')) {
            return SYNTAX_ERROR;
        }
        if (!push_operator_(**str, rpn_str, operator_stack)) {
            return ERROR;
        }

        *last_operator = **str;
    }
    else {
        return INVALID_SYMBOL;
    }

    return SYNTAX_OK;
}

static int get_math_error_un_func_(const char operator, const mpt *a) {
    if (operator == '!' && mpt_is_negative(a)) {
        return FACTORIAL_OF_NEGATIVE;
    }
    return MATH_ERROR;
}

static int get_math_error_bi_func_(const char operator, const mpt *a, const mpt *b) {
    if ((operator == '/' || operator == '%') && mpt_is_zero(b)) {
        return DIV_BY_ZERO;
    }
    return MATH_ERROR;
}

int shunt(const char *str, vector_type **rpn_str, stack **values) {
    int res;
    char c, last_operator = 0;
    stack *operator_stack = NULL;
    vector_type *vector_values = NULL;
    if (!str || is_end_char(*str) || !rpn_str || !values) {
        return ERROR;
    }
    
    operator_stack = stack_create(get_operator_count_(str), sizeof(char));
    vector_values = vector_allocate(sizeof(mpt *), mpt_free_wrapper_);
    *rpn_str = vector_allocate(sizeof(char), NULL);

    if (!operator_stack || !*rpn_str || !vector_values) {
        res = ERROR;
        goto clean_and_exit;
    }

    for (; !is_end_char(*str); ++str) {
        res = shunt_char_(&str, &last_operator, *rpn_str, operator_stack, vector_values);
        if (res != SYNTAX_OK) {
            goto clean_and_exit;
        }
    }

    while (stack_item_count(operator_stack) > 0) {
        if (!stack_pop(operator_stack, &c) ||
            !vector_push_back(*rpn_str, &c)) {
            res = ERROR;
            goto clean_and_exit;
        }
    }

    if (vector_isempty(vector_values)) {
        res = SYNTAX_ERROR;
        goto clean_and_exit;
    }

    *values = vector_to_stack(&vector_values);
    if (!*values) {
        res = ERROR;
        goto clean_and_exit;
    }

    vector_deallocate(&vector_values);
    stack_free(&operator_stack);
    return res;
    
  clean_and_exit:
    stack_free(&operator_stack);
    vector_deallocate(rpn_str);
    vector_deallocate(&vector_values);
    return res;
}

static int evaluate_rpn_char_(const char c, stack *orig_values, stack *stack_values) {
    int res = RESULT_OK;
    const func_oper_type *function = NULL;
    mpt *a, *b, *result;
    a = b = result = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(c == '(', SYNTAX_ERROR);

    if (c == RPN_VALUE_SYMBOL) {
        EXIT_IF(!stack_pop(orig_values, &a) || !stack_push(stack_values, &a), ERROR);
        return RESULT_OK;
    }

    function = get_func_operator(c);
    EXIT_IF(!function, ERROR);

    if (function->bi_handler) {
        EXIT_IF(!stack_pop(stack_values, &b) || !stack_pop(stack_values, &a), SYNTAX_ERROR);
        result = function->bi_handler(a, b);
        EXIT_IF(!result, get_math_error_bi_func_(c, a, b));
    }
    else if (function->un_handler) {
        EXIT_IF(!stack_pop(stack_values, &a), SYNTAX_ERROR);
        result = function->un_handler(a);
        EXIT_IF(!result, get_math_error_un_func_(c, a));
    }
    else {
        return ERROR;
    }

  clean_and_exit:
    mpt_free(&a);
    mpt_free(&b);

    if (result && !stack_push(stack_values, &result)) {
        return ERROR;
    }
    return res;

    #undef EXIT_IF
}

int evaluate_rpn(mpt **dest, vector_type *rpn_str, stack *values) {
    int res = RESULT_OK;
    char *c;
    size_t i;
    stack *stack_values = NULL;
    mpt *a = NULL;
    if (!dest || !rpn_str || !values) {
        return ERROR;
    }

    #define EXIT_IF_NOT(v, e) \
        if (!(v)) { \
            res = e; \
            goto clean_and_exit; \
        }

    stack_values = stack_create(stack_item_count(values), values->item_size);
    EXIT_IF_NOT(stack_values, ERROR);

    for (i = 0; i < vector_count(rpn_str); ++i) {
        c = (char *)vector_at(rpn_str, i);
        EXIT_IF_NOT(c, ERROR);

        res = evaluate_rpn_char_(*c, values, stack_values);
        EXIT_IF_NOT(res == RESULT_OK, res);
    }

    EXIT_IF_NOT(stack_item_count(stack_values) == 1, SYNTAX_ERROR);
    EXIT_IF_NOT(stack_pop(stack_values, dest), ERROR);
    
  clean_and_exit:
    while (!stack_isempty(stack_values)) {
        stack_pop(stack_values, &a);
        mpt_free(&a);
    }
    while (!stack_isempty(values)) {
        stack_pop(values, &a);
        mpt_free(&a);
    }
    stack_free(&stack_values);
    
    return res;

    #undef EXIT_IF_NOT
}