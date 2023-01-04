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
    if (c >= '0' && c <= '9') {
        return c - '0';
    } 
    return -1;
}

static int parse_hex_char_(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
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
    int msb_set = 0, char_value = 0;
    mpt *new, *shifted;
    new = shifted = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            mpt_free(&new); \
            goto clean_and_exit; \
        }

    EXIT_IF(!str || !*str);
    
    EXIT_IF(!(new = create_mpt(0)));

    msb_set = **str == '1';

    EXIT_IF((char_value = parse_bin_char_(**str)) < 0);
    
    while (char_value >= 0) {
        shifted = mpt_shift(new, 1, 1);
        EXIT_IF(!mpt_set_bit_to(shifted, 0, char_value));
        replace_mpt(&new, &shifted);

        char_value = parse_bin_char_(*(++*str));
    }

    if (msb_set) {
        EXIT_IF(!fill_set_bits_(new));
    }

  clean_and_exit:
    mpt_free(&shifted);

    return new;

    #undef EXIT_IF
}

mpt *mpt_parse_str_dec(const char **str) {
    int char_value;
    mpt *new, *mpv_char, *multiplied, *added, *ten;
    new = mpv_char = multiplied = added = ten = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            mpt_free(&new); \
            goto clean_and_exit; \
        }

    EXIT_IF(!str || !*str);

    EXIT_IF(!(ten = create_mpt(10)));
    EXIT_IF(!(new = create_mpt(0)));

    EXIT_IF((char_value = parse_dec_char_(**str)) < 0);

    while (char_value >= 0) {
        mpv_char = create_mpt(char_value);
        multiplied = mpt_mul(new, ten);
        added = mpt_add(multiplied, mpv_char);
        mpt_free(&mpv_char);
        mpt_free(&multiplied);
        replace_mpt(&new, &added);

        EXIT_IF(!new);

        char_value = parse_dec_char_(*(++*str));
    }

  clean_and_exit:
    mpt_free(&ten);
    mpt_free(&added);
    mpt_free(&mpv_char);
    mpt_free(&multiplied);

    return new;

    #undef EXIT_IF
}

mpt *mpt_parse_str_hex(const char **str) {
    int msb_set = 0, char_value;
    mpt *new, *mpv_char, *shifted, *added;
    new = mpv_char = shifted = added = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            mpt_free(&new); \
            goto clean_and_exit; \
        }

    EXIT_IF(!str || !*str);

    EXIT_IF(!(new = create_mpt(0)));

    EXIT_IF((char_value = parse_hex_char_(**str)) < 0)

    msb_set = char_value >= 8;

    while (char_value >= 0) {
        mpv_char = create_mpt(char_value);
        shifted = mpt_shift(new, 4, 1);
        added = mpt_add(shifted, mpv_char);
        mpt_free(&mpv_char);
        mpt_free(&shifted);
        replace_mpt(&new, &added);

        EXIT_IF(!new);

        char_value = parse_hex_char_(*(++*str));
    }

    if (msb_set) {
        EXIT_IF(!fill_set_bits_(new));
    }

  clean_and_exit:
    mpt_free(&mpv_char);
    mpt_free(&shifted);
    mpt_free(&added);

    return new;

    #undef EXIT_IF
}

mpt *mpt_parse_str(const char **str) {
    str_parser parser = NULL;

    if (!str || !*str) {
        return NULL;
    }

    if (**str == '0' && *(*str + 1) == 'b') {
        parser = mpt_parse_str_bin;
        *str += 2;
    }
    else if (**str == '0' && *(*str + 1) == 'x') {
        parser = mpt_parse_str_hex;
        *str += 2;
    }
    else if (**str >= '0' && **str <= '9') {
        parser = mpt_parse_str_dec;
    }

    return parser ? parser(str) : NULL;
}