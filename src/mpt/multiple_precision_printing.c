#include <stdio.h>
#include "multiple_precision_printing.h"
#include "multiple_precision_operations.h"

static char mpt_get_nibble_(const mpt *mpv, const size_t nibble_pos) {
    char nibble = 0;
    size_t i, bit_pos;
    bit_pos = nibble_pos * BITS_IN_NIBBLE;

    for (i = 0; i < BITS_IN_NIBBLE; ++i) {
        nibble += mpt_get_bit(mpv, bit_pos + i) << i;
    }

    return nibble;
}

void str_print_reverse_(const vector_type *str) {
    size_t i;
    for (i = 0; i < vector_count(str); ++i) {
        printf("%d", *(char *)vector_at(str, vector_count(str) - i - 1));
    }
}

void mpt_print_bin(const mpt *mpv) {
    size_t i, bits;
    int msb;

    if (!mpv) {
        return;
    }

    msb = mpt_is_negative(mpv);

    printf("0b%d", msb);

    bits = mpt_bit_count(mpv);
    for (i = 1; i < bits; ++i) {
        if (mpt_get_bit(mpv, bits - i - 1) != msb) {
            break;
        }
    }

    for (; i < bits; ++i) {
        printf("%i", mpt_get_bit(mpv, bits - i - 1));
    }
}

void mpt_print_dec(const mpt *mpv) {
    vector_type *str;
    mpt *mod, *div, *div_next, *ten;
    mod = div = div_next = ten = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }

    EXIT_IF(!mpv);

    if (mpt_is_zero(mpv) == 1) {
        printf("0");
        return;
    }

    EXIT_IF(!(div = mpt_abs(mpv)));
    EXIT_IF(!(ten = mpt_allocate(10)));
    EXIT_IF(!(str = vector_allocate(sizeof(char), NULL)));

    while (!mpt_is_zero(div)) {
        mod = mpt_mod(div, ten);
        div_next = mpt_div(div, ten);
        mpt_replace(&div, &div_next);

        EXIT_IF(!mod || !div);
        EXIT_IF(!vector_push_back(str, vector_at(mod->list, 0)));

        mpt_deallocate(&mod);
    }
    
    if (mpt_is_negative(mpv)) {
        printf("-");
    }
    str_print_reverse_(str);

  clean_and_exit:
    vector_deallocate(&str);
    mpt_deallocate(&mod);
    mpt_deallocate(&div_next);
    mpt_deallocate(&div);
    mpt_deallocate(&ten);

    #undef EXIT_IF
}

void mpt_print_hex(const mpt *mpv) {
    int msb;
    size_t i, nibbles;
    char nibble = 0, to_leave_out;

    if (!mpv) {
        return;
    }

    msb = mpt_get_msb(mpv);
    nibbles = mpt_bit_count(mpv) / BITS_IN_NIBBLE;
    
    to_leave_out = msb * 0xf;
    
    for (i = 0; i < nibbles; ++i) {
        nibble = mpt_get_nibble_(mpv, nibbles - i - 1);

        if (nibble != to_leave_out) {
            break;
        }
    }

    printf("0x");

    if (msb == 0 && nibble >= 8) {
        printf("0");
    }
    else if (msb == 1 && nibble < 8) {
        printf("f");
    }

    if (i == nibbles) {
        printf("%x", nibble);
        return;
    }

    for (; i < nibbles; ++i) {
        nibble = mpt_get_nibble_(mpv, nibbles - i - 1);
        printf("%x", nibble);
    }
}

void mpt_print(const mpt *mpv, const enum bases base) {
    mpt_printer printer;

    switch (base) {
        case bin: printer = mpt_print_bin; break;
        case dec: printer = mpt_print_dec; break;
        case hex: printer = mpt_print_hex; break;
        default:  return;
    }
    
    printer(mpv);
}