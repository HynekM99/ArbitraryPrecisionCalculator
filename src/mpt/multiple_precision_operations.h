#ifndef MPT_OPERATIONS
#define MPT_OPERATIONS

#include "multiple_precision_type.h"

typedef mpt* (*bi_function)(const mpt *, const mpt *);
typedef mpt* (*un_function)(const mpt *);

int mpt_compare(const mpt *mpv_a, const mpt *mpv_b);

int mpt_signum(const mpt *mpv);

mpt *mpt_abs(const mpt *mpv);

mpt *mpt_shift(const mpt *orig, const size_t positions, const int shift_left);

mpt *mpt_negate(const mpt *mpv);

mpt *mpt_add(const mpt *mpv_a, const mpt *mpv_b);

mpt *mpt_sub(const mpt *mpv_a, const mpt *mpv_b);

mpt *mpt_mul(const mpt *mpv_a, const mpt *mpv_b);

mpt *mpt_div(const mpt *mpv_dividend, const mpt *mpv_divisor);

mpt *mpt_mod(const mpt *mpv_dividend, const mpt *mpv_divisor);

mpt *mpt_pow(const mpt *mpv_base, const mpt *mpv_exponent);

mpt *mpt_factorial(const mpt *mpv);

#endif