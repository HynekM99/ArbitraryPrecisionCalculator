#include "multiple_precision_operations.h"

static char addition_carry_(const unsigned char a, const unsigned char b, unsigned char carry) {
    unsigned char c = a + b;
    return (c < a) || (c == 255 && carry == 1);
}

int mpt_compare(const mpt *mpv_a, const mpt *mpv_b) {
    size_t mssb_pos_a, mssb_pos_b, i;
    int bit_a, bit_b;

    #define RETURN_IF(v, e) \
        if (v) { \
            return e; \
        }

    RETURN_IF(!mpv_a && !mpv_b, 0);
    RETURN_IF(!mpv_a, -1);
    RETURN_IF(!mpv_b, 1);

    RETURN_IF(mpt_is_zero(mpv_a) && mpt_is_zero(mpv_b), 0);
    RETURN_IF(mpt_signum(mpv_a) > mpt_signum(mpv_b), 1);
    RETURN_IF(mpt_signum(mpv_a) < mpt_signum(mpv_b), -1);
    
    mssb_pos_a = mpt_get_mssb_pos(mpv_a);
    mssb_pos_b = mpt_get_mssb_pos(mpv_b);

    RETURN_IF(mssb_pos_a > mssb_pos_b, 1);
    RETURN_IF(mssb_pos_a < mssb_pos_b, -1);

    for (i = 0; i < mssb_pos_a; ++i) {
        bit_a = mpt_get_bit(mpv_a, mssb_pos_a - i - 1);
        bit_b = mpt_get_bit(mpv_b, mssb_pos_a - i - 1);
        
        RETURN_IF(bit_a > bit_b, 1);
        RETURN_IF(bit_a < bit_b, -1);
    }

    return 0;

    #undef RETURN_IF
}

int mpt_signum(const mpt *mpv) {
    if (!mpv) {
        return 0;
    }
    if (mpt_is_negative(mpv)) {
        return -1;
    }
    return !mpt_is_zero(mpv);
}

mpt *mpt_abs(const mpt *mpv) {
    if (!mpv) {
        return NULL;
    }
    if (mpt_is_negative(mpv)) {
        return mpt_negate(mpv);
    }
    return clone_mpt(mpv);
}

mpt *mpt_shift(const mpt *orig, const size_t positions, const int shift_left) {
    size_t i, j;
    mpt *new, *new_tmp;
    new = new_tmp = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }

    EXIT_IF(!orig);
    EXIT_IF(!(new_tmp = create_mpt(0)));

    if (shift_left) {
        i = 0;
        j = positions;
    } else {
        i = positions;
        j = 0;
    }

    for (; i < mpt_bit_count(orig) + mpt_bits_in_segment(orig); ++i, ++j) {
        EXIT_IF(!mpt_set_bit_to(new_tmp, j, mpt_get_bit(orig, i)));
    }

    EXIT_IF(!vector_remove(new_tmp->list, 1));

    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_free(&new_tmp);
    return new;

    #undef EXIT_IF
}

mpt *mpt_negate(const mpt *mpv) {
    size_t i;
    char segment;
    mpt *new_tmp, *new, *one;
    new_tmp = new = one = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }

    EXIT_IF(!mpv);

    EXIT_IF(!(one = create_mpt(1)));
    EXIT_IF(!(new_tmp = create_mpt(0)));

    EXIT_IF(!vector_remove(new_tmp->list, vector_count(new_tmp->list)));

    for (i = 0; i < vector_count(mpv->list); ++i) {
        segment = ~(mpt_get_segment(mpv, i));
        EXIT_IF(!vector_push_back(new_tmp->list, &segment));
    }

    new = mpt_add(new_tmp, one);

  clean_and_exit:
    mpt_free(&new_tmp);
    mpt_free(&one);
    return new;

    #undef EXIT_IF
}

mpt *mpt_add(const mpt *mpv_a, const mpt *mpv_b) {
    unsigned char segment_a, segment_b, res = 0, carry = 0;
    size_t i, segments;
    mpt *new, *new_tmp;
    new = new_tmp = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }
        
    EXIT_IF(!mpv_a || !mpv_b);

    EXIT_IF(!(new_tmp = create_mpt(0)));

    if (mpt_bit_count(mpv_a) >= mpt_bit_count(mpv_b)) {
        segments = vector_count(mpv_a->list);
    } else {
        segments = vector_count(mpv_b->list);
    }

    EXIT_IF(!(new_tmp = create_mpt(0)));
    EXIT_IF(!vector_clear(new_tmp->list));

    for (i = 0; i < segments + 1; ++i) {
        segment_a = mpt_get_segment(mpv_a, i);
        segment_b = mpt_get_segment(mpv_b, i);

        res = segment_a + segment_b + carry;
        carry = addition_carry_(segment_a, segment_b, carry);

        EXIT_IF(!vector_push_back(new_tmp->list, &res));
    }

    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_free(&new_tmp);
    return new;
}

mpt *mpt_sub(const mpt *mpv_a, const mpt *mpv_b) {
    mpt *negated_b = mpt_negate(mpv_b);
    mpt *new = mpt_add(mpv_a, negated_b);
    mpt_free(&negated_b);
    return new;
}

mpt *mpt_mul(const mpt *mpv_a, const mpt *mpv_b) {
    size_t i, max_bits, to_remove;
    mpt *new, *new_tmp, *added, *shifted;
    new = new_tmp = added = shifted = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }

    EXIT_IF(!mpv_a || !mpv_b);
    
    if (mpt_is_zero(mpv_a) || mpt_is_zero(mpv_b)) {
        return create_mpt(0);
    }

    EXIT_IF(!(new_tmp = create_mpt(0)));

    if (mpt_bit_count(mpv_a) >= mpt_bit_count(mpv_b)) {
        max_bits = mpt_bit_count(mpv_a);
    } else {
        max_bits = mpt_bit_count(mpv_b);
    }
    max_bits *= 2;

    for (i = 0; i < max_bits; ++i) {
        if (mpt_get_bit(mpv_b, i) == 0) {
            continue;
        }
        shifted = mpt_shift(mpv_a, i, 1);
        added = mpt_add(new_tmp, shifted);
        mpt_free(&shifted);
        replace_mpt(&new_tmp, &added);
        EXIT_IF(!new_tmp);
    }

    if (mpt_bit_count(new_tmp) > max_bits) {
        to_remove = (mpt_bit_count(new_tmp) - max_bits) / mpt_bits_in_segment(new_tmp);
        EXIT_IF(!vector_remove(new_tmp->list, to_remove));
    }
    
    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_free(&shifted);
    mpt_free(&new_tmp);
    return new;

    #undef EXIT_IF
}

mpt *mpt_div(const mpt *mpv_dividend, const mpt *mpv_divisor) {
    size_t rb;
    int is_res_negative = 0;
    mpt *new, *new_tmp, *new_negative, *part, *part_shifted, *one, *abs_dividend, *abs_dividend_next, *abs_divisor;
    new = new_tmp = new_negative = part = part_shifted = one = abs_dividend = abs_dividend_next = abs_divisor = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }

    EXIT_IF(!mpv_dividend || !mpv_divisor);

    EXIT_IF(mpt_is_zero(mpv_divisor));
    EXIT_IF(!(one = create_mpt(1)));

    if (mpt_compare(mpv_divisor, one) == 0) {
        mpt_free(&one);
        return clone_mpt(mpv_dividend);
    }

    EXIT_IF(!(abs_dividend = mpt_abs(mpv_dividend)));
    EXIT_IF(!(abs_divisor = mpt_abs(mpv_divisor)));
    EXIT_IF(!(new_tmp = create_mpt(0)));
    EXIT_IF(!(part = create_mpt(0)));

    is_res_negative = mpt_is_negative(mpv_dividend) != mpt_is_negative(mpv_divisor);
    rb = mpt_bit_count(abs_dividend) - 1;

    for (;;) {
        while (mpt_compare(part, abs_divisor) < 0) {
            EXIT_IF(!mpt_set_bit_to(new_tmp, rb, 0));

            if (rb == 0) {
                replace_mpt(&new, &new_tmp);
                goto clean_and_exit;
            }

            part_shifted = mpt_shift(part, 1, 1);
            EXIT_IF(!mpt_set_bit_to(part_shifted, 0, mpt_get_bit(abs_dividend, --rb)));
            replace_mpt(&part, &part_shifted);
        }

        mpt_set_bit_to(new_tmp, rb, 1);
        if (rb == 0) {
            replace_mpt(&new, &new_tmp);
            goto clean_and_exit;
        }

        abs_dividend_next = mpt_sub(part, abs_divisor);
        mpt_free(&part);

        part = mpt_shift(abs_dividend_next, 1, 1);
        mpt_free(&abs_dividend_next);

        EXIT_IF(!mpt_set_bit_to(part, 0, mpt_get_bit(abs_dividend, --rb)));
    }


  clean_and_exit:
    if (is_res_negative) {
        new_negative = mpt_negate(new);
        replace_mpt(&new, &new_negative);
    }
    
    mpt_free(&one);
    mpt_free(&part);
    mpt_free(&new_tmp);
    mpt_free(&part_shifted);
    mpt_free(&abs_dividend);
    mpt_free(&abs_divisor);

    return new;

    #undef EXIT_IF
}

mpt *mpt_mod(const mpt *mpv_dividend, const mpt *mpv_divisor) {
    mpt *res, *div, *mul;
    res = div = mul = NULL;
    div = mpt_div(mpv_dividend, mpv_divisor);
    mul = mpt_mul(div, mpv_divisor);
    res = mpt_sub(mpv_dividend, mul);
    mpt_free(&div);
    mpt_free(&mul);
    return res;
}

static mpt *mpt_pow_(const mpt *mpv_base, const mpt* mpv_exponent, const mpt *one, const mpt *two) {
    mpt *new, *new_tmp, *div;
    new = new_tmp = div = NULL;

    if (!mpv_base || !mpv_exponent || !one || !two) {
        return NULL;
    }
    if (mpt_is_negative(mpv_exponent)) {
        return create_mpt(0);
    }
    if (mpt_is_zero(mpv_exponent)) {
        return create_mpt(1);
    }
    if (mpt_compare(mpv_exponent, one) == 0) {
        return clone_mpt(mpv_base);
    }
    if (mpt_compare(mpv_exponent, two) == 0) {
        return mpt_mul(mpv_base, mpv_base);
    }

    new_tmp = mpt_pow_(mpv_base, two, one, two);
    div = mpt_div(mpv_exponent, two);
    new = mpt_pow_(new_tmp, div, one, two);

    if (mpt_is_odd(mpv_exponent)) {
        new_tmp = mpt_mul(new, mpv_base);
        replace_mpt(&new, &new_tmp);
    }

    mpt_free(&new_tmp);
    mpt_free(&div);

    return new;
}

mpt *mpt_pow(const mpt *mpv_base, const mpt *mpv_exponent) {
    mpt *res, *one, *two;
    res = one = two = NULL;

    one = create_mpt(1);
    two = create_mpt(2);
    res = mpt_pow_(mpv_base, mpv_exponent, one, two);

    mpt_free(&one);
    mpt_free(&two);

    return res;
}

mpt *mpt_factorial(const mpt *mpv) {
    mpt *new, *new_tmp, *sub, *sub_tmp, *one;
    new = new_tmp = sub = sub_tmp = one = NULL;
    
    #define EXIT_IF(v) \
        if (v) { \
            mpt_free(&new); \
            goto clean_and_exit; \
        }

    EXIT_IF(!mpv);
    EXIT_IF(mpt_is_negative(mpv));

    if (mpt_is_zero(mpv)) {
        return create_mpt(1);
    }

    EXIT_IF(!(one = create_mpt(1)));
    EXIT_IF(!(new = clone_mpt(mpv)));
    EXIT_IF(!(sub = mpt_sub(new, one)));

    while (mpt_compare(sub, one) >= 1) {
        new_tmp = mpt_mul(new, sub);
        sub_tmp = mpt_sub(sub, one);

        replace_mpt(&new, &new_tmp);
        replace_mpt(&sub, &sub_tmp);

        EXIT_IF(!new || !sub);
    }

  clean_and_exit:
    mpt_free(&new_tmp);
    mpt_free(&sub_tmp);
    mpt_free(&one);
    mpt_free(&sub);

    return new;

    #undef EXIT_IF
}
