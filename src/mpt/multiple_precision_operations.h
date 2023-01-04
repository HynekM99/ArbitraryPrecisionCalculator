#ifndef MPT_OPERATIONS
#define MPT_OPERATIONS

#include "multiple_precision_type.h"

typedef mpt* (*bi_function)(const mpt *, const mpt *);
typedef mpt* (*un_function)(const mpt *);

int mpt_compare(const mpt *a, const mpt *b);

int mpt_signum(const mpt *value);

mpt *mpt_abs(const mpt *value);

mpt *mpt_shift(const mpt *orig, const size_t positions, const int shift_left);

mpt *mpt_negate(const mpt *value);

mpt *mpt_add(const mpt *a, const mpt *b);

mpt *mpt_sub(const mpt *a, const mpt *b);

mpt *mpt_mul(const mpt *a, const mpt *b);

mpt *mpt_div(const mpt *dividend, const mpt *divisor);

mpt *mpt_mod(const mpt *dividend, const mpt *divisor);

mpt *mpt_pow(const mpt *base, const mpt *exponent);

mpt *mpt_factorial(const mpt *value);

#endif