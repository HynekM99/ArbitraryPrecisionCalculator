#include <stdio.h>
#include <stdlib.h>
#include "multiple_precision_parsing.h"
#include "multiple_precision_operations.h"

static int fill_set_bits_(mpt *target) {
    size_t i = mpt_bit_count(target) - 1;
    for (; mpt_get_bit(target, i) == 0; --i) {
        if (!mpt_set_bit_to(target, i, 1)) {
            return 0;
        }
    }
    return 1;
}

static int parse_bin_char_(const char c) {
    if (c == '0' || c == '1') {
        return c == '1';
    }
    return -1;
}

static int parse_dec_char_(const char c) {
    if (c <= '9') {
        return c - '0';
    } 
    return -1;
}

static int parse_hex_char_(const char c) {
    if (c <= '9') {
        return c - '0';
    }
    if (c <= 'F') {
        return c - 'A' + 10;
    }
    if (c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

int parse_char(const char c, const enum bases base) {
    char_parser parser;

    switch (base) {
        case bin: parser = parse_bin_char_; break;
        case dec: parser = parse_dec_char_; break;
        case hex: parser = parse_hex_char_; break;
        default:  return -1;
    }

    return parser(c);
}

mpt *mpt_parse_str_bin(const char str[]) {
    size_t i = 0;
    int negative = 0, msb_set = 0;
    mpt *target, *shifted, *mpv_negative;
    target = shifted = mpv_negative = NULL;
    if (!str || str[0] == '\000') {
        return NULL;
    }

    target = create_mpt(0);
    if (!target) {
        return NULL;
    }

    if (str[0] == '-') {
        negative = 1;
        i = 1;
    }
    if (str[i] == '1') {
        msb_set = 1;
    }

    for (; str[i] != '\000'; ++i) {
        shifted = mpt_shift(target, 1, 1);
        mpt_set_bit_to(shifted, 0, parse_bin_char_(str[i]));
        mpt_free(&target);
        target = shifted;
        shifted = NULL;
    }

    if (mpt_is_zero(target) == 1) {
        return NULL;
    }

    if (msb_set) {
        if (!fill_set_bits_(target)) {
            mpt_free(&target);
            return NULL;
        }
    }

    if (negative) {
        mpv_negative = mpt_negate(target);
        mpt_free(&target);
        target = mpv_negative;
        mpv_negative = NULL;
    }

    return target;
}

mpt *mpt_parse_str_dec(const char str[]) {
    size_t i = 0;
    int negative = 0, char_value;
    mpt *target, *mpv_char, *multiplied, *added, *negated, *ten;
    target = mpv_char = multiplied = added = negated = ten = NULL;
    if (!str || str[0] == '\000') {
        return 0;
    }

    ten = create_mpt(10);
    target = create_mpt(0);
    if (!ten || !target) {
        mpt_free(&ten);
        mpt_free(&target);
        return 0;
    }

    if (str[0] == '-') {
        negative = 1;
        i = 1;
    }
    for (; str[i] != '\000'; ++i) {
        char_value = parse_dec_char_(str[i]);
        if (char_value < 0) {
            mpt_free(&target);
            goto clean_and_exit;
        }
        mpv_char = create_mpt(char_value);
        multiplied = mpt_mul(target, ten);
        added = mpt_add(multiplied, mpv_char);
        mpt_free(&mpv_char);
        mpt_free(&multiplied);
        mpt_free(&target);
        target = added;
        added = NULL;

        if (!target) {
            goto clean_and_exit;
        }
    }

    if (negative) {
        negated = mpt_negate(target);
        mpt_free(&target);
        target = negated;
        negated = NULL;
    }

  clean_and_exit:
    mpt_free(&ten);
    mpt_free(&mpv_char);
    mpt_free(&multiplied);

    return target;
}

mpt *mpt_parse_str_hex(const char str[]) {
    size_t i = 0;
    int negative = 0, msb_set = 0, char_value;
    mpt *target, *mpv_char, *shifted, *added, *mpv_negative;
    target = mpv_char = shifted = added = mpv_negative = NULL;
    if (!str || str[0] == '\000') {
        return 0;
    }

    target = create_mpt(0);
    if (!target) {
        return 0;
    }

    if (str[0] == '-') {
        negative = 1;
        i = 1;
    }
    if (parse_hex_char_(str[i]) >= 8) {
        msb_set = 1;
    }

    for (; str[i] != '\000'; ++i) {
        char_value = parse_hex_char_(str[i]);
        if (char_value < 0) {
            mpt_free(&target);
            goto clean_and_exit;
        }
        mpv_char = create_mpt(char_value);
        shifted = mpt_shift(target, 4, 1);
        added = mpt_add(shifted, mpv_char);
        mpt_free(&mpv_char);
        mpt_free(&shifted);
        mpt_free(&target);
        target = added;
        added = NULL;

        if (!target) {
            goto clean_and_exit;
        }
    }

    if (mpt_is_zero(target) == 1) {
        goto clean_and_exit;
    }

    if (msb_set) {
        if (!fill_set_bits_(target)) {
            mpt_free(&target);
            goto clean_and_exit;
        }
    }

    if (negative) {
        mpv_negative = mpt_negate(target);
        mpt_free(&target);
        target = mpv_negative;
        mpv_negative = NULL;
    }

  clean_and_exit:
    mpt_free(&mpv_char);
    mpt_free(&shifted);
    mpt_free(&added);
    return target;
}

mpt *mpt_parse_str(const char str[], const enum bases base) {
    str_parser parser;
    switch (base) {
        case bin: parser = mpt_parse_str_bin; break;
        case dec: parser = mpt_parse_str_dec; break;
        case hex: parser = mpt_parse_str_hex; break;
        default:  return NULL;
    }
    return parser(str);
}