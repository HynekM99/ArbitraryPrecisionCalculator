#ifndef MPT_BASE
#define MPT_BASE

#define BITS_IN_BYTE 8
#define BITS_IN_NIBBLE 4

#include "../data_structures/vector.h"

enum bases { bin = 2, dec = 10, hex = 16 };

typedef struct _mpt {
    vector_type *list;
} mpt;

mpt *create_mpt(const char init_value);

int init_mpt(mpt *mpv, const char init_value);

mpt *clone_mpt(const mpt *orig);

void replace_mpt(mpt **to_replace, mpt **replace_with);

size_t mpt_bit_count(const mpt *mpv);

int mpt_set_bit_to(mpt *mpv, const size_t bit, const int bit_set);

size_t mpt_bits_in_segment(const mpt *mpv);

char *mpt_get_segment_ptr(const mpt *mpv, const size_t index);

char mpt_get_segment(const mpt *mpv, const size_t index);

int mpt_get_bit(const mpt *mpv, const size_t bit);

int mpt_get_msb(const mpt *mpv);

size_t mpt_get_mssb_pos(const mpt *mpv);

int mpt_is_zero(const mpt *mpv);

int mpt_is_negative(const mpt *mpv);

mpt *mpt_optimize(const mpt *orig);

void mpt_free(mpt **mpv);

#endif