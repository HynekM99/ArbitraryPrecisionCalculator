#include "array_list.h"

#ifndef MPT
#define MPT

enum bases { bin = 2, dec = 10, hex = 16 };

typedef struct _mpt {
    array_list *list;
    size_t bits;
} mpt;

mpt *create_mpt();

void mpt_set_bit(mpt *mpt, const size_t bit);

void mpt_reset_bit(mpt *mpt, const size_t bit);

int mpt_get_bit(const mpt *mpt, const size_t bit);

mpt *mpt_negate(const mpt *mpt_a);

mpt *mpt_add(const mpt *mpt_a, const mpt *mpt_b);

int mpt_compare(const mpt *mpt_a, const mpt *mpt_b);

void mpt_print_bin(const mpt *mpt);

void mpt_free(mpt **mpt);

#endif