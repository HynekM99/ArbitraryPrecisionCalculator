#ifndef MPT
#define MPT

#include "vector.h"

enum bases { bin = 2, dec = 10, hex = 16 };

typedef struct _mpt {
    vector_type *list;
    size_t segment_size; /* TODO */
} mpt;

mpt *create_mpt();

size_t mpt_bit_count(const mpt *mpv);

int mpt_set_bit_to(mpt *mpv, const size_t bit, const int bit_set);

int mpt_get_bit(const mpt *mpv, const size_t bit);

mpt *mpt_shift(const mpt *orig, const size_t positions, const int shift_left);

mpt *mpt_negate(const mpt *mpv);

mpt *mpt_add(const mpt *mpv_a, const mpt *mpv_b);

mpt *mpt_mul(const mpt *mpv_a, const mpt *mpv_b);

mpt *mpt_optimize(const mpt *orig);

void mpt_print_bin(const mpt *mpv);

void mpt_free(mpt **mpv);

#endif