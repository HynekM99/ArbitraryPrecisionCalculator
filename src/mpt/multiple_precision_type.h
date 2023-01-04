/**
 * @file multiple_precision_tzpe.h
 * @author Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi funkcí pro práci s instancemi struktur typu 'mpt'.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _MPT_BASE_H
#define _MPT_BASE_H

#define BITS_IN_BYTE 8
#define BITS_IN_NIBBLE 4

#include "../data_structures/vector.h"

enum bases { bin = 2, dec = 10, hex = 16 };

typedef struct _mpt {
    vector_type *list;
} mpt;

mpt *mpt_allocate(const char init_value);

mpt *mpt_clone(const mpt *orig);

void mpt_replace(mpt **to_replace, mpt **replace_with);

size_t mpt_bits_in_segment(const mpt *mpv);

size_t mpt_segment_count(const mpt *mpv);

size_t mpt_bit_count(const mpt *mpv);

char *mpt_get_segment_ptr(const mpt *mpv, const size_t index);

char mpt_get_segment(const mpt *mpv, const size_t index);

int mpt_set_bit_to(mpt *mpv, const size_t bit, const int bit_set);

int mpt_get_bit(const mpt *mpv, const size_t bit);

int mpt_get_msb(const mpt *mpv);

size_t mpt_get_mssb_pos(const mpt *mpv);

int mpt_is_zero(const mpt *mpv);

int mpt_is_negative(const mpt *mpv);

int mpt_is_odd(const mpt *mpv);

mpt *mpt_optimize(const mpt *orig);

void mpt_deallocate(mpt **mpv);

#endif