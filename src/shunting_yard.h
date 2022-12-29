#ifndef SHUNTING_YARD
#define SHUNTING_YARD

#define RPN_VALUE_SYMBOL 'n'

#include "stack/stack.h"
#include "vector.h"
#include "operators.h"
#include "mpt/mpt.h"

int shunt(const char *str, vector_type **rpn_str, stack **values);

mpt *evaluate_rpn(vector_type *rpn_str, stack *values);

#endif