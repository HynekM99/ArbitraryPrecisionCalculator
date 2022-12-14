#include <stdlib.h>
#include <string.h>
#include "shunting_yard.h"

static void mpt_free_wrapper_(void *poor) {
    mpt_free(poor);
}

static size_t get_operator_count_(const char *str) {
    size_t i = 0;
    for (; *str != '\000' && *str != '\n'; ++str) {
        if (get_bi_func_operator(*str) || 
            get_un_func_operator(*str) ||
            *str == '(') {
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
    enum associativity assoc = left;
    size_t precedence, prev_precedence;
    bi_func_oper_type *bi_operator, *prev_bi_operator;
    un_func_oper_type *un_operator, *prev_un_operator;
    bi_operator = prev_bi_operator = NULL;
    un_operator = prev_un_operator = NULL;
    if (operator == '!') {
        return vector_push_back(rpn_str, &operator);
    }
    if (operator == '(') {
        return stack_push(operator_stack, &operator);
    }

    bi_operator = get_bi_func_operator(operator);
    un_operator = get_un_func_operator(operator);

    if (bi_operator || un_operator) {
        if (bi_operator && !un_operator) {
            precedence = bi_operator->precedence;
            assoc = bi_operator->assoc;
        }
        else if (un_operator && !bi_operator) {
            precedence = un_operator->precedence;
            assoc = un_operator->assoc;
        }
        else {
            return 0;
        }

        while (stack_item_count(operator_stack) > 0) {
            if (!stack_head(operator_stack, &c)) {
                return 0;
            }
            if (c == '(') {
                break;
            }

            prev_bi_operator = get_bi_func_operator(c);
            prev_un_operator = get_un_func_operator(c);

            prev_precedence = prev_bi_operator ? prev_bi_operator->precedence : prev_un_operator->precedence;

            if (prev_precedence < precedence ||
                (prev_precedence == precedence && assoc == left)) {
                break;
            }

            if (!stack_pop(operator_stack, &c) ||
                !vector_push_back(rpn_str, &c)) {
                return 0;
            }
        }

        return stack_push(operator_stack, &operator);
    }
    else if (operator == ')') {
        while (stack_item_count(operator_stack) > 0) {
            if (!stack_pop(operator_stack, &c)) {
                return 0;
            }
            if (c == '(') {
                return 1;
            }
            if (!vector_push_back(rpn_str, &c)) {
                return 0;
            }
        }
        return 0;
    }
    return 0;
}

int shunt(const char *str, vector_type **rpn_str, vector_type **values) {
    const char *orig_ptr = str;
    char c, last_operator = 0, minus;
    stack *operator_stack = NULL;
    mpt *parsed_value = NULL;
    if (!str || str[0] == '\000' || !rpn_str || !values) {
        return 0;
    }
    
    operator_stack = stack_create(get_operator_count_(str), sizeof(char));
    *rpn_str = vector_allocate(sizeof(char), NULL);
    *values = vector_allocate(sizeof(mpt *), mpt_free_wrapper_);

    if (!operator_stack || !*rpn_str || !*values) {
        goto clean_and_exit;
    }

    for (; *str != '\000' && *str != '\n'; ++str) {
        if (*str == ' ') {
            continue;
        }
        else if (*str == '-') {
            if (str == orig_ptr) {
                minus = RPN_UNARY_MINUS_SYMBOL;
            }
            else if (last_operator == '^' || last_operator == '-' || 
                    last_operator == '(' || last_operator == '*') {
                minus = RPN_UNARY_MINUS_SYMBOL;
            }
            else {
                minus = '-';
            }

            if (minus == RPN_UNARY_MINUS_SYMBOL && 
                *(str + 1) == ' ') {
                goto clean_and_exit;
            }

            if (!push_operator_(minus, *rpn_str, operator_stack)) {
                goto clean_and_exit;
            }
        }
        else if (*str >= '0' && *str <= '9') {
            parsed_value = mpt_parse_str(&str);
            if (!push_parsed_value_(parsed_value, *rpn_str, *values)) {
                mpt_free(&parsed_value);
                goto clean_and_exit;
            }
            last_operator = 0;
            --str;
        }
        else if (get_bi_func_operator(*str) ||
                 get_un_func_operator(*str) ||
                 *str == '(' || *str == ')') {
            if (!push_operator_(*str, *rpn_str, operator_stack)) {
                goto clean_and_exit;
            }
            last_operator = *str;
        }
        else {
            goto clean_and_exit;
        }
    }

    while (stack_item_count(operator_stack) > 0) {
        if (!stack_pop(operator_stack, &c) ||
            !vector_push_back(*rpn_str, &c)) {
            goto clean_and_exit;
        }
    }

    stack_free(&operator_stack);
    return 1;
    
  clean_and_exit:
    stack_free(&operator_stack);
    vector_deallocate(rpn_str);
    vector_deallocate(values);
    return 0;
}

mpt *evaluate_rpn(vector_type *rpn_str, stack *values) {
    char c;
    size_t i;
    mpt *a, *b, *result;
    stack *stack_values = NULL;
    bi_func_oper_type *bi_function = NULL;
    un_func_oper_type *un_function = NULL;
    a = b = result = NULL;
    if (!rpn_str || !values) {
        return NULL;
    }

    stack_values = stack_create(stack_item_count(values), values->item_size);
    if (!stack_values) {
        return NULL;
    }

    for (i = 0; i < vector_count(rpn_str); ++i) {
        c = *(char *)vector_at(rpn_str, i);

        bi_function = get_bi_func_operator(c);
        un_function = get_un_func_operator(c);

        if (c == RPN_VALUE_SYMBOL) {
            if (!stack_pop(values, &a) || 
                !stack_push(stack_values, &a)) {
                return NULL;
            }
        }
        else if (bi_function) {
            if (!stack_pop(stack_values, &b) ||
                !stack_pop(stack_values, &a)) {
                return NULL;
            }
            
            result = bi_function->handler(a, b);
            mpt_free(&a);
            mpt_free(&b);
            if (!result || !stack_push(stack_values, &result)) {
                return NULL;
            }
        }
        else if (un_function) {
            if (!stack_pop(stack_values, &a)) {
                return NULL;
            }
            
            result = un_function->handler(a);
            mpt_free(&a);
            if (!result || !stack_push(stack_values, &result)) {
                return NULL;
            }
        }
        else {
            return NULL;
        }
    }

    if (stack_item_count(stack_values) > 1) {
        return NULL;
    }

    stack_pop(stack_values, &result);
    stack_free(&stack_values);
    return result;
}