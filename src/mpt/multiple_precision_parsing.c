#include <stdio.h>
#include <stdlib.h>
#include "mpt.h"

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

mpt *mpt_parse_str_bin(const char **str) {
    int negative = 0, msb_set = 0, char_value = 0;
    mpt *target, *shifted, *mpv_negative;
    target = shifted = mpv_negative = NULL;
    if (!str || !*str) {
        return NULL;
    }
    if (**str == '\000' || **str == ' ') {
        return NULL;
    }

    target = create_mpt(0);
    if (!target) {
        return NULL;
    }

    if (*str[0] == '-') {
        negative = 1;
        ++(*str);
    }
    if (**str != '0' && **str != '1') {
        mpt_free(&target);
        return NULL;
    }

    if (**str == '1') {
        msb_set = 1;
    }

    char_value = parse_bin_char_(**str);
    for (; char_value >= 0; char_value = parse_bin_char_(**str)) {
        shifted = mpt_shift(target, 1, 1);
        mpt_set_bit_to(shifted, 0, char_value);
        replace_mpt(&target, &shifted);
        ++(*str);
    }

    if (msb_set) {
        if (!fill_set_bits_(target)) {
            mpt_free(&target);
            return NULL;
        }
    }

    if (negative) {
        mpv_negative = mpt_negate(target);
        replace_mpt(&target, &mpv_negative);
    }

    return target;
}

mpt *mpt_parse_str_dec(const char **str) {
    int negative = 0, char_value;
    mpt *target, *mpv_char, *multiplied, *added, *negated, *ten;
    target = mpv_char = multiplied = added = negated = ten = NULL;
    if (!str || !*str) {
        return NULL;
    }
    if (**str == '\000' || **str == ' ') {
        return NULL;
    }

    ten = create_mpt(10);
    target = create_mpt(0);
    if (!ten || !target) {
        mpt_free(&ten);
        mpt_free(&target);
        return NULL;
    }

    if (**str == '-') {
        negative = 1;
        ++(*str);
    }

    char_value = parse_dec_char_(**str);
    for (; char_value >= 0; char_value = parse_dec_char_(**str)) {
        mpv_char = create_mpt(char_value);
        multiplied = mpt_mul(target, ten);
        added = mpt_add(multiplied, mpv_char);
        mpt_free(&mpv_char);
        mpt_free(&multiplied);
        replace_mpt(&target, &added);

        if (!target) {
            goto clean_and_exit;
        }

        ++(*str);
    }

    if (negative) {
        negated = mpt_negate(target);
        replace_mpt(&target, &negated);
    }

  clean_and_exit:
    mpt_free(&ten);
    mpt_free(&mpv_char);
    mpt_free(&multiplied);

    return target;
}

mpt *mpt_parse_str_hex(const char **str) {
    int negative = 0, msb_set = 0, char_value;
    mpt *target, *mpv_char, *shifted, *added, *mpv_negative;
    target = mpv_char = shifted = added = mpv_negative = NULL;
    if (!str || !*str) {
        return NULL;
    }
    if (**str == '\000' || **str == ' ') {
        return NULL;
    }

    target = create_mpt(0);
    if (!target) {
        return 0;
    }

    if (**str == '-') {
        negative = 1;
        ++(*str);
    }
    if (parse_hex_char_(**str) >= 8) {
        msb_set = 1;
    }

    char_value = parse_hex_char_(**str);
    for (; char_value >= 0; char_value = parse_hex_char_(**str)) {
        mpv_char = create_mpt(char_value);
        shifted = mpt_shift(target, 4, 1);
        added = mpt_add(shifted, mpv_char);
        mpt_free(&mpv_char);
        mpt_free(&shifted);
        replace_mpt(&target, &added);

        if (!target) {
            goto clean_and_exit;
        }

        ++(*str);
    }

    if (msb_set) {
        if (!fill_set_bits_(target)) {
            mpt_free(&target);
            goto clean_and_exit;
        }
    }

    if (negative) {
        mpv_negative = mpt_negate(target);
        replace_mpt(&target, &mpv_negative);
    }

  clean_and_exit:
    mpt_free(&mpv_char);
    mpt_free(&shifted);
    mpt_free(&added);
    return target;
}

mpt *mpt_parse_str(const char **str) {
    int is_negative = 0;
    char c;
    str_parser parser;
    mpt *new, *mpv_negative;
    new = mpv_negative = NULL;
    if (!str || !*str) {
        return NULL;
    }
    c = **str;

    if (c == '-') {
        ++(*str);
        c = **str;
        is_negative = 1;
        if (c == ' ') {
            return NULL;
        }
    }

    if (c == '0' && *(*str + 1) == 'b') {
        parser = mpt_parse_str_bin;
        *str += 2;
    }
    else if (c == '0' && *(*str + 1) == 'x') {
        parser = mpt_parse_str_hex;
        *str += 2;
    }
    else if (c >= '0' && c <= '9') {
        parser = mpt_parse_str_dec;
    }
    else {
        return NULL;
    }

    new = parser(str);

    if (is_negative) {
        mpv_negative = mpt_negate(new);
        mpt_free(&new);
        return mpv_negative;
    }

    return new;
}