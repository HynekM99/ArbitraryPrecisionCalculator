#include <stdio.h>
#include <stdlib.h>
#include "multiple_precision_parsing.h"

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

int mpt_parse_char(mpt *target, const char c, const enum bases base) {
    parse_char parser;
    char value;
    size_t i;

    if (!target) {
        return 0;
    }

    switch (base) {
        case bin: parser = parse_bin_char_; break;
        case dec: parser = parse_dec_char_; break;
        case hex: parser = parse_hex_char_; break;
        default:  return 0;
    }

    value = (char)parser(c);

    for (i = 0; i < sizeof(value) * 8; ++i) {
        if (!mpt_set_bit_to(target, i, ((value >> i) & 1) == 1)) {
            return 0;
        }
    }

    return 1;
}

int mpt_parse_str(mpt **target, const char str[], const enum bases base) {
    int success = 1, negative = 0;
    char base_value = 0;
    size_t i = 0;
    mpt *mpv_base, *mpv_char, *multiplied, *added, *negated;
    mpv_base = mpv_char = multiplied = added = negated = NULL;

    if (!target || !str || str[0] == '\000') {
        return 0;
    }

    mpv_base = create_mpt();
    mpv_char = create_mpt();
    if (!mpv_base || !mpv_char) {
        success = 0;
        goto clean_and_exit;
    }

    base_value = (char)base;

    for (i = 0; i < sizeof(base_value) * 8; ++i) {
        if (!mpt_set_bit_to(mpv_base, i, ((base_value >> i) & 1) == 1)) {
            success = 0;
            goto clean_and_exit;
        }
    }

    i = 0;

    if (str[0] == '-') {
        negative = 1;
        i = 1;
    }

    for (; str[i] != '\000'; ++i) {
        if (!mpt_parse_char(mpv_char, str[i], base)) {
            success = 0;
            goto clean_and_exit;
        }

        multiplied = mpt_mul(*target, mpv_base);
        added = mpt_add(multiplied, mpv_char);
        if (!added) {
            success = 0;
            goto clean_and_exit;
        }
        mpt_free(&multiplied);
        mpt_free(target);
        *target = added;
    }

    if (negative) {
        negated = mpt_negate(*target);
        if (!negated) {
            success = 0;
            goto clean_and_exit;
        }
        mpt_free(target);
        *target = negated;
    }

  clean_and_exit:
    mpt_free(&mpv_base);
    mpt_free(&mpv_char);
    mpt_free(&multiplied);

    return success;
}