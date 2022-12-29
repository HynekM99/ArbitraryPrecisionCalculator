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
                (prev_precedence == precedence && assoc == right)) {
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

static int shunt_value_(const char **str, char *last_operator, vector_type *rpn_str, vector_type *vector_values) {
    mpt *parsed_value = NULL;
    parsed_value = mpt_parse_str(str);
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
            return str;
        }
    }

    return NULL;
}

static int shunt_next_char_(const char **str, const char *until, char *last_operator, vector_type *rpn_str, stack *operator_stack, vector_type *vector_values) {
    char minus, *closing_bracket = NULL;

    if (**str == ' ') {
        return 1;
    }
    else if (**str == '-') {
        if (stack_item_count(operator_stack) == 0 && vector_isempty(vector_values)) {
            minus = RPN_UNARY_MINUS_SYMBOL;
        }
        else if (*last_operator == '^') {
            minus = RPN_UNARY_MINUS_SYMBOL;

            if (*(*str + 1) == '(') {
                closing_bracket = find_closing_bracket_(*str + 2);
                *str += 1;

                if (!closing_bracket) {
                    return 0;
                }

                for (; **str != '\000' && **str != '\n' && *str <= closing_bracket; ++*str) {
                    if (until && *str <= until) {
                        break;
                    }
                    if (!shunt_next_char_(str, closing_bracket, last_operator, rpn_str, operator_stack, vector_values)) {
                        return 0;
                    }
                }

                if (!vector_push_back(rpn_str, &minus)) {
                    return 0;
                }
            }
            else if (*(*str + 1) == ' ') {
                return 0;
            }
            else if (!shunt_value_(str, last_operator, rpn_str, vector_values)) {
                return 0;
            }
            *last_operator = minus;
            return 1;
        }
        else if (*last_operator == '-' || 
                *last_operator == '(' || 
                *last_operator == '*' || 
                *last_operator == '+') {
            minus = RPN_UNARY_MINUS_SYMBOL;
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
    else if (get_bi_func_operator(**str) || get_un_func_operator(**str) || **str == '(' || **str == ')') {
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
        if (!shunt_next_char_(&str, NULL, &last_operator, *rpn_str, operator_stack, vector_values)) {
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