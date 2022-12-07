#ifndef MPT_BASE
#define MPT_BASE

#define BITS_IN_BYTE 8
#define BITS_IN_NIBBLE 4

#include "vector.h"

enum bases { bin = 2, dec = 10, hex = 16 };

typedef struct _mpt {
    vector_type *list;
} mpt;

mpt *create_mpt(const char init_value);

int init_mpt(mpt *mpv, const char init_value);

mpt *copy_mpt(const mpt *orig);

size_t mpt_bit_count(const mpt *mpv);

int mpt_set_bit_to(mpt *mpv, const size_t bit, const int bit_set);

int mpt_get_bit(const mpt *mpv, const size_t bit);

int mpt_get_msb(const mpt *mpv);

size_t mpt_get_mssb_pos(const mpt *mpv);

int mpt_is_zero(const mpt *mpv);

int mpt_is_negative(const mpt *mpv);

mpt *mpt_optimize(const mpt *orig);

void mpt_free(mpt **mpv);

#endif