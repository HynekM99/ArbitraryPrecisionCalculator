#include "multiple_precision_operations.h"

static char addition_carry_(const unsigned char a, const unsigned char b, const unsigned char carry) {
    unsigned char c = a + b;
    return (c < a) || (c == 255 && carry == 1);
}

int mpt_compare(const mpt *a, const mpt *b) {
    size_t mssb_pos_a, mssb_pos_b, i;
    int bit_a, bit_b;

    #define RETURN_IF(v, e) \
        if (v) { \
            return e; \
        }

    RETURN_IF(!a && !b, 0);
    RETURN_IF(!a, -1);
    RETURN_IF(!b, 1);

    RETURN_IF(mpt_is_zero(a) && mpt_is_zero(b), 0);
    RETURN_IF(mpt_signum(a) > mpt_signum(b), 1);
    RETURN_IF(mpt_signum(a) < mpt_signum(b), -1);
    
    mssb_pos_a = mpt_get_mssb_pos(a);
    mssb_pos_b = mpt_get_mssb_pos(b);

    RETURN_IF(mssb_pos_a > mssb_pos_b, 1);
    RETURN_IF(mssb_pos_a < mssb_pos_b, -1);

    for (i = 0; i < mssb_pos_a; ++i) {
        bit_a = mpt_get_bit(a, mssb_pos_a - i - 1);
        bit_b = mpt_get_bit(b, mssb_pos_a - i - 1);
        
        RETURN_IF(bit_a > bit_b, 1);
        RETURN_IF(bit_a < bit_b, -1);
    }

    return 0;

    #undef RETURN_IF
}

int mpt_signum(const mpt *value) {
    if (!value) {
        return 0;
    }
    if (mpt_is_negative(value)) {
        return -1;
    }
    
    return !mpt_is_zero(value);
}

mpt *mpt_abs(const mpt *value) {
    if (!value) {
        return NULL;
    }
    if (mpt_is_negative(value)) {
        return mpt_negate(value);
    }

    return mpt_clone(value);
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
    EXIT_IF(!(new_tmp = mpt_allocate(0)));

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
    mpt_deallocate(&new_tmp);

    return new;

    #undef EXIT_IF
}

mpt *mpt_negate(const mpt *value) {
    size_t i;
    char segment;
    mpt *new_tmp, *new, *one;
    new_tmp = new = one = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }

    EXIT_IF(!value);

    EXIT_IF(!(one = mpt_allocate(1)));
    EXIT_IF(!(new_tmp = mpt_allocate(0)));

    EXIT_IF(!vector_remove(new_tmp->list, vector_count(new_tmp->list)));

    for (i = 0; i < vector_count(value->list); ++i) {
        segment = ~(mpt_get_segment(value, i));
        EXIT_IF(!vector_push_back(new_tmp->list, &segment));
    }

    new = mpt_add(new_tmp, one);

  clean_and_exit:
    mpt_deallocate(&new_tmp);
    mpt_deallocate(&one);

    return new;

    #undef EXIT_IF
}

mpt *mpt_add(const mpt *a, const mpt *b) {
    unsigned char segment_a, segment_b, res = 0, carry = 0;
    size_t i, segments;
    mpt *new, *new_tmp;
    new = new_tmp = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }
        
    EXIT_IF(!a || !b);

    EXIT_IF(!(new_tmp = mpt_allocate(0)));

    if (mpt_bit_count(a) >= mpt_bit_count(b)) {
        segments = vector_count(a->list);
    } else {
        segments = vector_count(b->list);
    }

    EXIT_IF(!vector_clear(new_tmp->list));

    for (i = 0; i < segments + 1; ++i) {
        segment_a = mpt_get_segment(a, i);
        segment_b = mpt_get_segment(b, i);

        res = segment_a + segment_b + carry;
        carry = addition_carry_(segment_a, segment_b, carry);

        EXIT_IF(!vector_push_back(new_tmp->list, &res));
    }

    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_deallocate(&new_tmp);
    return new;
}

mpt *mpt_sub(const mpt *a, const mpt *b) {
    mpt *new, *negated_b;
    new = negated_b = NULL;

    negated_b = mpt_negate(b);
    new = mpt_add(a, negated_b);

    mpt_deallocate(&negated_b);

    return new;
}

mpt *mpt_mul(const mpt *a, const mpt *b) {
    size_t i, max_bits, to_remove;
    mpt *new, *new_tmp, *added, *shifted;
    new = new_tmp = added = shifted = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }

    EXIT_IF(!a || !b);
    
    if (mpt_is_zero(a) || mpt_is_zero(b)) {
        return mpt_allocate(0);
    }

    EXIT_IF(!(new_tmp = mpt_allocate(0)));

    if (mpt_bit_count(a) >= mpt_bit_count(b)) {
        max_bits = mpt_bit_count(a);
    } else {
        max_bits = mpt_bit_count(b);
    }
    max_bits *= 2;

    for (i = 0; i < max_bits; ++i) {
        if (mpt_get_bit(b, i) == 0) {
            continue;
        }
        shifted = mpt_shift(a, i, 1);
        added = mpt_add(new_tmp, shifted);
        mpt_deallocate(&shifted);
        mpt_replace(&new_tmp, &added);
        EXIT_IF(!new_tmp);
    }

    if (mpt_bit_count(new_tmp) > max_bits) {
        to_remove = (mpt_bit_count(new_tmp) - max_bits) / mpt_bits_in_segment(new_tmp);
        EXIT_IF(!vector_remove(new_tmp->list, to_remove));
    }
    
    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_deallocate(&shifted);
    mpt_deallocate(&new_tmp);
    return new;

    #undef EXIT_IF
}

mpt *mpt_div(const mpt *dividend, const mpt *divisor) {
    size_t rb;
    int is_res_negative = 0;
    mpt *new, *new_tmp, *new_negative, *part, *part_shifted, *one, *abs_dividend, *abs_dividend_next, *abs_divisor;
    new = new_tmp = new_negative = part = part_shifted = one = abs_dividend = abs_dividend_next = abs_divisor = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }

    EXIT_IF(!dividend || !divisor);

    EXIT_IF(mpt_is_zero(divisor));
    EXIT_IF(!(one = mpt_allocate(1)));

    if (mpt_compare(divisor, one) == 0) {
        mpt_deallocate(&one);
        return mpt_clone(dividend);
    }

    EXIT_IF(!(abs_dividend = mpt_abs(dividend)));
    EXIT_IF(!(abs_divisor = mpt_abs(divisor)));
    EXIT_IF(!(new_tmp = mpt_allocate(0)));
    EXIT_IF(!(part = mpt_allocate(0)));

    is_res_negative = mpt_is_negative(dividend) != mpt_is_negative(divisor);
    rb = mpt_bit_count(abs_dividend) - 1;

    for (;;) {
        while (mpt_compare(part, abs_divisor) < 0) {
            EXIT_IF(!mpt_set_bit_to(new_tmp, rb, 0));

            if (rb == 0) {
                mpt_replace(&new, &new_tmp);
                goto clean_and_exit;
            }

            part_shifted = mpt_shift(part, 1, 1);
            EXIT_IF(!mpt_set_bit_to(part_shifted, 0, mpt_get_bit(abs_dividend, --rb)));
            mpt_replace(&part, &part_shifted);
        }

        mpt_set_bit_to(new_tmp, rb, 1);
        if (rb == 0) {
            mpt_replace(&new, &new_tmp);
            goto clean_and_exit;
        }

        abs_dividend_next = mpt_sub(part, abs_divisor);
        mpt_deallocate(&part);

        part = mpt_shift(abs_dividend_next, 1, 1);
        mpt_deallocate(&abs_dividend_next);

        EXIT_IF(!mpt_set_bit_to(part, 0, mpt_get_bit(abs_dividend, --rb)));
    }


  clean_and_exit:
    if (is_res_negative) {
        new_negative = mpt_negate(new);
        mpt_replace(&new, &new_negative);
    }
    
    mpt_deallocate(&one);
    mpt_deallocate(&part);
    mpt_deallocate(&new_tmp);
    mpt_deallocate(&part_shifted);
    mpt_deallocate(&abs_dividend);
    mpt_deallocate(&abs_divisor);

    return new;

    #undef EXIT_IF
}

mpt *mpt_mod(const mpt *dividend, const mpt *divisor) {
    mpt *res, *div, *mul;
    res = div = mul = NULL;

    div = mpt_div(dividend, divisor);
    mul = mpt_mul(div, divisor);
    res = mpt_sub(dividend, mul);

    mpt_deallocate(&div);
    mpt_deallocate(&mul);

    return res;
}

static mpt *mpt_pow_(const mpt *base, const mpt* exponent, const mpt *one, const mpt *two) {
    mpt *new, *new_tmp, *div;
    new = new_tmp = div = NULL;

    if (!base || !exponent || !one || !two) {
        return NULL;
    }
    if (mpt_is_negative(exponent)) {
        return mpt_allocate(0);
    }
    if (mpt_is_zero(exponent)) {
        return mpt_allocate(1);
    }
    if (mpt_compare(exponent, one) == 0) {
        return mpt_clone(base);
    }
    if (mpt_compare(exponent, two) == 0) {
        return mpt_mul(base, base);
    }

    new_tmp = mpt_pow_(base, two, one, two);
    div = mpt_div(exponent, two);
    new = mpt_pow_(new_tmp, div, one, two);
    mpt_deallocate(&new_tmp);
    
    if (mpt_is_odd(exponent)) {
        new_tmp = mpt_mul(new, base);
        mpt_replace(&new, &new_tmp);
    }

    mpt_deallocate(&new_tmp);
    mpt_deallocate(&div);

    return new;
}

mpt *mpt_pow(const mpt *base, const mpt *exponent) {
    mpt *res, *one, *two;
    res = one = two = NULL;

    one = mpt_allocate(1);
    two = mpt_allocate(2);
    res = mpt_pow_(base, exponent, one, two);

    mpt_deallocate(&one);
    mpt_deallocate(&two);

    return res;
}

mpt *mpt_factorial(const mpt *value) {
    mpt *new, *new_tmp, *sub, *sub_tmp, *one;
    new = new_tmp = sub = sub_tmp = one = NULL;
    
    #define EXIT_IF(v) \
        if (v) { \
            mpt_deallocate(&new); \
            goto clean_and_exit; \
        }

    EXIT_IF(!value);
    EXIT_IF(mpt_is_negative(value));

    if (mpt_is_zero(value)) {
        return mpt_allocate(1);
    }

    EXIT_IF(!(one = mpt_allocate(1)));
    EXIT_IF(!(new = mpt_clone(value)));
    EXIT_IF(!(sub = mpt_sub(new, one)));

    while (mpt_compare(sub, one) >= 1) {
        new_tmp = mpt_mul(new, sub);
        sub_tmp = mpt_sub(sub, one);

        mpt_replace(&new, &new_tmp);
        mpt_replace(&sub, &sub_tmp);

        EXIT_IF(!new || !sub);
    }

  clean_and_exit:
    mpt_deallocate(&new_tmp);
    mpt_deallocate(&sub_tmp);
    mpt_deallocate(&one);
    mpt_deallocate(&sub);

    return new;

    #undef EXIT_IF
}
