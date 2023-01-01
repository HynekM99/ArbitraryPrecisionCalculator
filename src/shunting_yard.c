#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shunting_yard.h"

static void mpt_free_wrapper_(void *poor) {
    mpt_free(poor);
}

static stack *vector_to_stack_(vector_type **v) {
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

static size_t get_operator_count_(const char *str) {
    size_t i = 0;
    for (; *str != '\000' && *str != '\n'; ++str) {
        if (get_func_operator(*str) || *str == '(') {
            ++i;
        }
    }
    return i;
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

static char *find_closing_bracket_(const char *str) {
    size_t l_brackets = 1, r_brackets = 0;
    for (; *str != '\000' && *str != '\n'; ++str) {
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

static int shunt_next_char_(const char **str, char *last_operator, vector_type *rpn_str, stack *operator_stack, vector_type *vector_values) {
    char minus, *closing_bracket = NULL;
    int res;

    if (**str == ' ') {
        return SYNTAX_OK;
    }

    if (**str == '-') {
        if (*last_operator == 0) {
            minus = RPN_UNARY_MINUS_SYMBOL;
        }
        if (*last_operator != ')' && 
            *last_operator != '!' && 
            *last_operator != RPN_VALUE_SYMBOL) {
            minus = RPN_UNARY_MINUS_SYMBOL;
        }
        else if (*last_operator == '^') {
            minus = RPN_UNARY_MINUS_SYMBOL;

            ++*str;
            if (**str == ' ') { /* 4^- 2 ... Syntax error */
                return SYNTAX_ERROR;
            }

            if (**str == '(') {
                closing_bracket = find_closing_bracket_(*str + 1);
                if (!closing_bracket) {
                    return SYNTAX_ERROR;
                }

                for (; **str != '\000' && **str != '\n' && *str <= closing_bracket; ++*str) {
                    res = shunt_next_char_(str, last_operator, rpn_str, operator_stack, vector_values);
                    if (res != SYNTAX_OK) {
                        return res;
                    }
                }
            }
            else {
                if (!shunt_value_(str, last_operator, rpn_str, vector_values)) {
                    return 0;
                }
                ++*str;
            }

            for (; **str != '\000' && **str != '\n' && **str == '!'; ++*str) {
                res = shunt_next_char_(str, last_operator, rpn_str, operator_stack, vector_values);
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
        else {
            minus = '-';
        }
        
        if (minus == RPN_UNARY_MINUS_SYMBOL && *(*str + 1) == ' ') {
            return SYNTAX_ERROR;
        }

        *last_operator = minus;

        if (!push_operator_(minus, rpn_str, operator_stack)) {
            return ERROR;
        }
    }
    else if (**str >= '0' && **str <= '9') {
        if (*last_operator == RPN_VALUE_SYMBOL ||
            *last_operator == '!') {
            return SYNTAX_ERROR;
        }
        if (!shunt_value_(str, last_operator, rpn_str, vector_values)) {
            return ERROR;
        }
    }
    else if (**str == RPN_UNARY_MINUS_SYMBOL) { /* Speciální znak pro unární mínus není povolen */
        return INVALID_SYMBOL;
    }
    else if (get_func_operator(**str) || **str == '(' || **str == ')') {
        if (**str == '(') {
            if (*last_operator == RPN_VALUE_SYMBOL ||
                *last_operator == '!') {
                return SYNTAX_ERROR;
            }
        }
        if (**str == '!' && (*last_operator == 0 || *(*str - 1) == ' ')) {
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

int shunt(const char *str, vector_type **rpn_str, stack **values) {
    int res;
    char c, last_operator = 0;
    size_t operator_count = 0;
    stack *operator_stack = NULL;
    vector_type *vector_values = NULL;
    if (!str || str[0] == '\000' || !rpn_str || !values) {
        return ERROR;
    }
    
    operator_count = get_operator_count_(str);
    if (operator_count == 0) {
        operator_count = 1;
    }

    operator_stack = stack_create(operator_count, sizeof(char));
    vector_values = vector_allocate(sizeof(mpt *), mpt_free_wrapper_);
    *rpn_str = vector_allocate(sizeof(char), NULL);

    if (!operator_stack || !*rpn_str || !vector_values) {
        res = ERROR;
        goto clean_and_exit;
    }

    for (; *str != '\000' && *str != '\n'; ++str) {
        res = shunt_next_char_(&str, &last_operator, *rpn_str, operator_stack, vector_values);
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

    *values = vector_to_stack_(&vector_values);
    if (!*values) {
        res = ERROR;
        goto clean_and_exit;
    }

    stack_free(&operator_stack);
    return res;
    
  clean_and_exit:
    stack_free(&operator_stack);
    vector_deallocate(rpn_str);
    vector_deallocate(&vector_values);
    return res;
}

int evaluate_rpn(mpt **dest, vector_type *rpn_str, stack *values) {
    int res = RESULT_OK;
    char c;
    size_t i;
    mpt *a, *b, *result;
    stack *stack_values = NULL;
    const func_oper_type *function = NULL;
    a = b = result = NULL;
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
        c = *(char *)vector_at(rpn_str, i);

        if (c == RPN_VALUE_SYMBOL) {
            EXIT_IF_NOT(stack_pop(values, &a) && stack_push(stack_values, &a), ERROR);
            continue;
        }

        if (c == '(') {
            return SYNTAX_ERROR;
        }

        function = get_func_operator(c);
        EXIT_IF_NOT(function, ERROR);

        if (function->bi_handler) {
            EXIT_IF_NOT(stack_pop(stack_values, &b) && stack_pop(stack_values, &a), SYNTAX_ERROR);
            result = function->bi_handler(a, b);

            if (!result) {
                if ((function->bi_handler == mpt_div  || 
                    function->bi_handler == mpt_mod) && 
                    mpt_is_zero(b)) {
                    mpt_free(&a);
                    mpt_free(&b);
                    res = DIV_BY_ZERO;
                    goto clean_and_exit;
                }
            }

            mpt_free(&a);
            mpt_free(&b);
            EXIT_IF_NOT(result, MATH_ERROR);
            EXIT_IF_NOT(stack_push(stack_values, &result), ERROR);
        }
        else if (function->un_handler) {
            EXIT_IF_NOT(stack_pop(stack_values, &a), SYNTAX_ERROR);
            
            result = function->un_handler(a);

            if (!result) {
                if (function->un_handler == mpt_factorial && mpt_is_negative(a)) {
                    mpt_free(&a);
                    res = FACTORIAL_OF_NEGATIVE;
                    goto clean_and_exit;
                }
            }

            mpt_free(&a);
            EXIT_IF_NOT(result, MATH_ERROR);
            EXIT_IF_NOT(stack_push(stack_values, &result), ERROR);
        }
        else {
            res = ERROR;
            goto clean_and_exit;
        }
    }

    if (stack_item_count(stack_values) != 1) {
        res = SYNTAX_ERROR;
        goto clean_and_exit;
    }

    if (!stack_pop(stack_values, dest)) {
        res = ERROR;
    }
    stack_free(&stack_values);
    return res;

  clean_and_exit:
    while (!stack_isempty(stack_values)) {
        stack_pop(stack_values, &a);
        mpt_free(&a);
    }
    stack_free(&stack_values);
    return res;

    #undef EXIT_IF_NOT
}