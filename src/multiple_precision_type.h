#ifndef MPT_BASE
#define MPT_BASE

#include "vector.h"

enum bases { bin = 2, dec = 10, hex = 16 };

typedef struct _mpt {
    vector_type *list;
} mpt;

mpt *create_mpt(const char init_value);

mpt *copy_mpt(const mpt *orig);

size_t mpt_bit_count(const mpt *mpv);

int mpt_set_bit_to(mpt *mpv, const size_t bit, const int bit_set);

int mpt_get_bit(const mpt *mpv, const size_t bit);

size_t mpt_get_mssb_pos(const mpt *mpv);

int mpt_is_zero(const mpt *mpv);

int mpt_is_negative(const mpt *mpv);

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

mpt *mpt_optimize(const mpt *orig);

void mpt_free(mpt **mpv);

#endif