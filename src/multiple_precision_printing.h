#ifndef MPT_PRINT
#define MPT_PRINT

#include "multiple_precision_type.h"

typedef void (*mpt_printer)(const mpt *);

void mpt_print_bin(const mpt *mpv);

void mpt_print_dec(const mpt *mpv);

void mpt_print_hex(const mpt *mpv);

void mpt_print(const mpt *mpv, const enum bases base);

#endif