#include <stdio.h>
#include "multiple_precision_printing.h"

static char mpt_get_nibble_(const mpt *mpv, const size_t nibble_pos) {
    size_t i, bit_pos = nibble_pos * 4;
    char nibble = 0;

    for (i = 0; i < 4; ++i) {
        nibble += mpt_get_bit(mpv, bit_pos + i) << i;
    }
    return nibble;
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
    size_t i;
    vector_type *str;
    mpt *mod, *div, *div_next, *ten;
    mod = div = div_next = ten = NULL;
    if (!mpv) {
        return;
    }

    ten = create_mpt(10);
    str = vector_allocate(sizeof(char), NULL);
    if (!ten) {
        goto clean_and_exit;
    }

    if (mpt_is_negative(mpv)) {
        printf("-");
    }

    div = mpt_abs(mpv);
    if (!div) {
        goto clean_and_exit;
    }

    while (!mpt_is_zero(div)) {
        mod = mpt_mod(div, ten);
        div_next = mpt_div(div, ten);
        if (!mod || !div_next) {
            mpt_free(&mod);
            mpt_free(&div_next);
            goto clean_and_exit;
        }
        vector_push_back(str, vector_at(mod->list, 0));
        mpt_free(&mod);
        mpt_free(&div);
        div = div_next;
    }

    for (i = 0; i < vector_count(str); ++i) {
        printf("%d", (*(char *)vector_at(str, vector_count(str) - i - 1)));
    }

  clean_and_exit:
    vector_deallocate(&str);
    mpt_free(&div);
    mpt_free(&ten);
}

void mpt_print_hex(const mpt *mpv) {
    size_t i, nibbles;
    int msb;
    char nibble, to_leave_out;
    if (!mpv) {
        return;
    }

    msb = mpt_is_negative(mpv);
    nibbles = vector_count(mpv->list) * (mpv->list->item_size * 8) / 4;
    
    to_leave_out = msb * 0xf;
    printf("0x");
    
    for (i = 0; i < nibbles; ++i) {
        nibble = mpt_get_nibble_(mpv, nibbles - i - 1);
        if (nibble != to_leave_out) {
            break;
        }
    }

    if (msb == 0 && nibble >= 8) {
        printf("0");
    }
    else if (msb == 1 && nibble < 8) {
        printf("f");
    }

    if (i == nibbles) {
        printf("%x", nibble);
    }

    for (; i < nibbles; ++i) {
        nibble = mpt_get_nibble_(mpv, nibbles - i - 1);
        printf("%x", nibble);
    }
}