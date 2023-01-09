#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpt.h"

/**
 * \brief Nastavuje v instanci mpt 'most significant' bity na jedničku dokud nenarazí na již nastavený bit.
 *        Používá se když parsujeme řetězec s binární nebo hexadecimální zápornou hodnotu, abychom doplnily 
 *        jedničky doplňkového kódu v segmentech (např. "0b1101" -> 0b11111101).
 * \param target Ukazatel na instanci mpt.
 * \return int 1 jestli se nastavení bitů podařilo, 0 pokud ne.
 */
static int fill_set_bits_(mpt *target) {
    size_t i, bit_count;
    if ((bit_count = mpt_bit_count(*target)) == 0) {
        return 0;
    }
    
    for (i = 0; i < bit_count; ++i) {
        if (mpt_get_bit(*target, bit_count - i - 1) == 1) {
            break;
        }
        if (!mpt_set_bit_to(target, bit_count - i - 1, 1)) {
            return 0;
        }
    }
    return 1;
}

/**
 * \brief Převede znak na odpovídající binární hodnotu 1 nebo 0.
 * \param c Znak na převedení.
 * \return int 1 pokud je znak '1', 0 pokud je znak '0', jinak -1.
 */
static int parse_bin_char_(const char c) {
    if (c == '0' || c == '1') {
        return c == '1';
    }
    return -1;
}

/**
 * \brief Převede znak na odpovídající dekadickou hodnotu v intervalu <0,9>.
 * \param c Znak na převedení.
 * \return int s dekadickou hodnotou pokud znak odpovídá znaku s dekadickou hodnotou, jinak -1.
 */
static int parse_dec_char_(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } 
    return -1;
}

/**
 * \brief Převede znak na odpovídající hexadecimální hodnotu v intervalu <0,f>.
 * \param c Znak na převedení (jsou platné malé i velké znaky).
 * \return int s hexadecimální hodnotou pokud znak odpovídá znaku s hexadecimální hodnotou, jinak -1.
 */
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

int mpt_parse_str_bin(mpt *dest, const char **str) {
    int res = 1, msb_set, char_value;
    mpt shifted;
    shifted.list = NULL;
    
    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest || !str || !*str, 0);
    
    EXIT_IF(!mpt_init(dest, 0), 0);

    EXIT_IF((char_value = parse_bin_char_(**str)) < 0, 0);

    msb_set = char_value == 1;

    while (char_value >= 0) {
        EXIT_IF(!mpt_shift(&shifted, *dest, 1, 1), 0);
        EXIT_IF(!mpt_set_bit_to(&shifted, 0, char_value), 0);
        mpt_replace(dest, &shifted);
        
        char_value = parse_bin_char_(*(++*str));
    }

    if (msb_set) {
        EXIT_IF(!fill_set_bits_(dest), 0);
    }

  clean_and_exit:
    mpt_deinit(&shifted);

    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}

int mpt_parse_str_dec(mpt *dest, const char **str) {
    int res = 1, char_value;
    mpt mpv_char, mul, added, ten;
    mpv_char.list = mul.list = added.list = ten.list = NULL;

     #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest || !str || !*str, 0);

    EXIT_IF(!mpt_init(&ten, 10), 0);
    EXIT_IF(!mpt_init(dest, 0), 0);

    EXIT_IF((char_value = parse_dec_char_(**str)) < 0, 0);

    while (char_value >= 0) {
        EXIT_IF(!mpt_init(&mpv_char, char_value), 0);
        EXIT_IF(!mpt_mul(&mul, *dest, ten), 0);
        EXIT_IF(!mpt_add(&added, mul, mpv_char), 0);
        mpt_deinit(&mpv_char);
        mpt_deinit(&mul);
        mpt_replace(dest, &added);

        char_value = parse_dec_char_(*(++*str));
    }

  clean_and_exit:
    mpt_deinit(&ten);
    mpt_deinit(&added);
    mpt_deinit(&mpv_char);
    mpt_deinit(&mul);

    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}

int mpt_parse_str_hex(mpt *dest, const char **str) {
    int res = 1, msb_set, char_value;
    mpt added, mpv_char, shifted;
    added.list = mpv_char.list = shifted.list = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest || !str || !*str, 0);
    
    EXIT_IF(!mpt_init(dest, 0), 0);

    EXIT_IF((char_value = parse_hex_char_(**str)) < 0, 0);

    msb_set = char_value >= 8;

    while (char_value >= 0) {
        EXIT_IF(!mpt_init(&mpv_char, char_value), 0);
        EXIT_IF(!mpt_shift(&shifted, *dest, BITS_IN_NIBBLE, 1), 0);
        EXIT_IF(!mpt_add(&added, shifted, mpv_char), 0);
        mpt_deinit(&mpv_char);
        mpt_deinit(&shifted);
        mpt_replace(dest, &added);

        char_value = parse_hex_char_(*(++*str));
    }

    if (msb_set) {
        EXIT_IF(!fill_set_bits_(dest), 0);
    }

  clean_and_exit:
    mpt_deinit(&mpv_char);
    mpt_deinit(&shifted);
    mpt_deinit(&added);

    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}

int mpt_parse_str(mpt *dest, const char **str) {
    str_parser parser = NULL;

    if (!str || !*str) {
        return 0;
    }

    if (**str == '0' && tolower(*(*str + 1)) == 'b') {
        parser = mpt_parse_str_bin;
        *str += 2;
    }
    else if (**str == '0' && tolower(*(*str + 1)) == 'x') {
        parser = mpt_parse_str_hex;
        *str += 2;
    }
    else if (**str >= '0' && **str <= '9') {
        parser = mpt_parse_str_dec;
    }

    return parser(dest, str);
}