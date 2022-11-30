#include <stdio.h>
#include <stdlib.h>
#include "multiple_precision_type.h"
#include "multiple_precision_parsing.h"

int mpt_parse_dec_char(mpt *target, const char c) {
    char value;
    size_t i;

    if (!target) {
        return 0;
    }
    if (c < '0' || c > '9') {
        return 0;
    }

    value = (char)(c - '0');

    for (i = 0; i < sizeof(value) * 8; ++i) {
        if (((value >> i) & 1) == 1) {
            mpt_set_bit(target, i);
        } else {
            mpt_reset_bit(target, i);
        }
    }

    return 1;
}

int mpt_parse_str(mpt **target, const char *str) {
    int negative = 0;
    size_t i = 0;
    mpt *ten, *mpt_c, *target_tmp_1, *target_tmp_2 = NULL;

    if (!target || !str || *str == '\000') {
        return 0;
    }

    ten = create_mpt();
    mpt_c = create_mpt();
    if (!ten || !mpt_c) {
        mpt_free(&ten);
        mpt_free(&mpt_c);
        return 0;
    }
    mpt_set_bit(ten, 1);
    mpt_set_bit(ten, 3);

    if (*str == '-') {
        i = 1;
        negative = 1;
    }

    while (*(str + i) != '\000') {
        if (!mpt_parse_dec_char(mpt_c, *(str + i))) {
            mpt_free(&ten);
            mpt_free(&mpt_c);
            return 0;
        }
        target_tmp_1 = mpt_mul(*target, ten);
        target_tmp_2 = mpt_add(target_tmp_1, mpt_c);
        mpt_free(target);
        mpt_free(&target_tmp_1);
        *target = target_tmp_2;
        ++i;
    }

    if (negative) {
        target_tmp_1 = mpt_negate(*target);
        mpt_free(target);
        *target = target_tmp_1;
    }

  
    mpt_free(&ten);
    mpt_free(&mpt_c);

    return 1;
}