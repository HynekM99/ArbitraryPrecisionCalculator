#ifndef SHUNTING_YARD
#define SHUNTING_YARD

#define RPN_VALUE_SYMBOL 'n'

#define ERROR -1
#define SYNTAX_ERROR 0
#define SYNTAX_OK 1
#define INVALID_SYMBOL 2

#define MATH_ERROR 3
#define RESULT_OK 1
#define DIV_BY_ZERO 4
#define FACTORIAL_OF_NEGATIVE 5

#include "data_structures/stack.h"
#include "data_structures/vector.h"
#include "operators.h"
#include "mpt/mpt.h"

int shunt(const char *str, vector_type **rpn_str, stack_type **values);

int evaluate_rpn(mpt **dest, const vector_type *rpn_str, stack_type *values);

#endif