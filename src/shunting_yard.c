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
    if (**str == ' ') {
        return 1;
    }

    if (**str == '-') {
        if (*last_operator == 0 ||
            *last_operator == '-' || *last_operator == '(' || 
            *last_operator == '*' || *last_operator == '+') {
            minus = RPN_UNARY_MINUS_SYMBOL;
        }
        else if (*last_operator == '^') {
            minus = RPN_UNARY_MINUS_SYMBOL;

            ++*str;
            if (**str == ' ') { /* 4^- 2 ... Syntax error */
                return 0;
            }

            if (**str == '(') {
                closing_bracket = find_closing_bracket_(*str + 1);
                if (!closing_bracket) {
                    return 0;
                }

                for (; **str != '\000' && **str != '\n' && *str <= closing_bracket; ++*str) {
                    if (!shunt_next_char_(str, last_operator, rpn_str, operator_stack, vector_values)) {
                        return 0;
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
                if (!shunt_next_char_(str, last_operator, rpn_str, operator_stack, vector_values)) {
                    return 0;
                }
            }
            --*str;

            return vector_push_back(rpn_str, &minus);
        }
        else {
            minus = '-';
        }
        
        if (minus == RPN_UNARY_MINUS_SYMBOL && *(*str + 1) == ' ') {
            return 0;
        }

        *last_operator = minus;

        if (!push_operator_(minus, rpn_str, operator_stack)) {
            return 0;
        }
    }
    else if (**str >= '0' && **str <= '9') {
        if (!shunt_value_(str, last_operator, rpn_str, vector_values)) {
            return 0;
        }
    }
    else if (get_func_operator(**str) || **str == '(' || **str == ')') {
        if (**str == '!') {
            if (*last_operator == 0 || *(*str - 1) == ' ') {
                return 0;
            }
        }
        if (!push_operator_(**str, rpn_str, operator_stack)) {
            return 0;
        }
        *last_operator = **str;
    }
    else {
        return 0;
    }

    return 1;
}

int shunt(const char *str, vector_type **rpn_str, stack **values) {
    char c, last_operator = 0;
    size_t operator_count = 0;
    stack *operator_stack = NULL;
    vector_type *vector_values = NULL;
    if (!str || str[0] == '\000' || !rpn_str || !values) {
        return 0;
    }
    
    operator_count = get_operator_count_(str);
    if (operator_count == 0) {
        operator_count = 1;
    }

    operator_stack = stack_create(operator_count, sizeof(char));
    vector_values = vector_allocate(sizeof(mpt *), mpt_free_wrapper_);
    *rpn_str = vector_allocate(sizeof(char), NULL);

    if (!operator_stack || !*rpn_str || !vector_values) {
        goto clean_and_exit;
    }

    for (; *str != '\000' && *str != '\n'; ++str) {
        if (!shunt_next_char_(&str, &last_operator, *rpn_str, operator_stack, vector_values)) {
            goto clean_and_exit;
        }
    }

    while (stack_item_count(operator_stack) > 0) {
        if (!stack_pop(operator_stack, &c) ||
            !vector_push_back(*rpn_str, &c)) {
            goto clean_and_exit;
        }
    }

    *values = vector_to_stack_(&vector_values);
    if (!*values) {
        goto clean_and_exit;
    }

    stack_free(&operator_stack);
    return 1;
    
  clean_and_exit:
    stack_free(&operator_stack);
    vector_deallocate(rpn_str);
    vector_deallocate(&vector_values);
    return 0;
}

mpt *evaluate_rpn(vector_type *rpn_str, stack *values) {
    char c;
    size_t i;
    mpt *a, *b, *result;
    stack *stack_values = NULL;
    const func_oper_type *function = NULL;
    a = b = result = NULL;
    if (!rpn_str || !values) {
        return NULL;
    }

    #define EXIT_IF_NOT(v) \
        if (!(v)) { \
            return NULL; \
        }

    stack_values = stack_create(stack_item_count(values), values->item_size);
    EXIT_IF_NOT(stack_values);

    for (i = 0; i < vector_count(rpn_str); ++i) {
        c = *(char *)vector_at(rpn_str, i);

        if (c == RPN_VALUE_SYMBOL) {
            EXIT_IF_NOT(stack_pop(values, &a) && stack_push(stack_values, &a));
            continue;
        }

        function = get_func_operator(c);
        EXIT_IF_NOT(function);

        if (function->bi_handler) {
            EXIT_IF_NOT(stack_pop(stack_values, &b) && stack_pop(stack_values, &a));
            result = function->bi_handler(a, b);
            mpt_free(&a);
            mpt_free(&b);

            EXIT_IF_NOT(result && stack_push(stack_values, &result));
        }
        else if (function->un_handler) {
            EXIT_IF_NOT(stack_pop(stack_values, &a));
            
            result = function->un_handler(a);
            mpt_free(&a);

            EXIT_IF_NOT(result && stack_push(stack_values, &result));
        }
        else {
            return NULL;
        }
    }

    if (stack_item_count(stack_values) != 1) {
        return NULL;
    }

    stack_pop(stack_values, &result);
    stack_free(&stack_values);
    return result;

    #undef EXIT_IF_NOT
}