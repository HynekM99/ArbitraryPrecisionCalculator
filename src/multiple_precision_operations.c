#include "multiple_precision_operations.h"

static void half_adder_(const int a, const int b, int *bit, int *carry) {
    *bit = a ^ b;
    *carry = a && b;
}

static void full_adder_(const int a, const int b, int cin, int *bit, int *cout) {
    *bit = a ^ b ^ cin;
    *cout = (a && b) | (cin && (a ^ b));
}

int mpt_compare(const mpt *mpv_a, const mpt *mpv_b) {
    size_t mssb_pos_a, mssb_pos_b, i;
    int bit_a, bit_b;
    if (!mpv_a && !mpv_b) {
        return 0;
    }
    if (!mpv_a) {
        return -1;
    }
    if (!mpv_b) {
        return 1;
    }

    if (mpt_signum(mpv_a) == 0 && mpt_signum(mpv_b) == 0) {
        return 0;
    }
    if (mpt_signum(mpv_a) > mpt_signum(mpv_b)) {
        return 1;
    }
    if (mpt_signum(mpv_a) < mpt_signum(mpv_b)) {
        return -1;
    }
    
    mssb_pos_a = mpt_get_mssb_pos(mpv_a);
    mssb_pos_b = mpt_get_mssb_pos(mpv_b);

    if (mssb_pos_a > mssb_pos_b) {
        return 1;
    }
    if (mssb_pos_a < mssb_pos_b) {
        return -1;
    }

    for (i = 0; i < mssb_pos_a; ++i) {
        bit_a = mpt_get_bit(mpv_a, mssb_pos_a - i - 1);
        bit_b = mpt_get_bit(mpv_b, mssb_pos_a - i - 1);
        
        if (bit_a > bit_b) {
            return 1;
        }
        if (bit_a < bit_b) {
            return -1;
        }
    }

    return 0;
}

int mpt_signum(const mpt *mpv) {
    if (!mpv) {
        return 0;
    }
    if (mpt_get_msb(mpv) == 1) {
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
    return copy_mpt(mpv);
}

mpt *mpt_shift(const mpt *orig, const size_t positions, const int shift_left) {
    size_t i, j;
    mpt *new = NULL, *new_tmp = NULL;
    if (!orig) {
        return NULL;
    }
    
    new_tmp = create_mpt(0);
    if (!new_tmp) {
        return NULL;
    }

    if (shift_left) {
        i = 0, j = positions;
    } else {
        i = positions, j = 0;
    }

    for (; i < mpt_bit_count(orig) + orig->list->item_size * 8; ++i, ++j) {
        if (!mpt_set_bit_to(new_tmp, j, mpt_get_bit(orig, i))) {
            goto clean_and_exit;
        }
    }

    if (!vector_remove(new_tmp->list, 1)) {
        goto clean_and_exit;
    }

    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_free(&new_tmp);
    return new;
}

mpt *mpt_negate(const mpt *mpv) {
    size_t i;
    mpt *new_tmp, *new, *one;
    new_tmp = new = one = NULL;
    if (!mpv) {
        return NULL;
    }

    one = create_mpt(1);
    new_tmp = create_mpt(0);
    if (!one || !new_tmp) {
        goto clean_and_exit;
    }

    if (!mpt_set_bit_to(one, 0, 1)) {
        goto clean_and_exit;
    }

    for (i = 0; i < mpt_bit_count(mpv); ++i) {
        if (!mpt_set_bit_to(new_tmp, i, !mpt_get_bit(mpv, i))) {
            goto clean_and_exit;
        }
    }

    new = mpt_add(new_tmp, one);

  clean_and_exit:
    mpt_free(&new_tmp);
    mpt_free(&one);
    return new;
}

mpt *mpt_add(const mpt *mpv_a, const mpt *mpv_b) {
    size_t i, max_bits;
    int a, b, bit, carry;
    mpt *new, *new_tmp;
    new = new_tmp = NULL;
    if (!mpv_a || !mpv_b) {
        return NULL;
    }

    new_tmp = create_mpt(0);
    if (!new_tmp) {
        return NULL;
    }

    if (mpt_bit_count(mpv_a) >= mpt_bit_count(mpv_b)) {
        max_bits = mpt_bit_count(mpv_a);
    } else {
        max_bits = mpt_bit_count(mpv_b);
    }
    max_bits += mpv_a->list->item_size * 8 * 2;

    a = mpt_get_bit(mpv_a, 0);
    b = mpt_get_bit(mpv_b, 0);
    half_adder_(a, b, &bit, &carry);
    if (!mpt_set_bit_to(new_tmp, 0, bit)) {
        goto clean_and_exit;
    }

    for (i = 1; i < max_bits; ++i) {
        a = mpt_get_bit(mpv_a, i);
        b = mpt_get_bit(mpv_b, i);
        full_adder_(a, b, carry, &bit, &carry);
        if (!mpt_set_bit_to(new_tmp, i, bit)) {
            goto clean_and_exit;
        }   
    }

    if (!vector_remove(new_tmp->list, 1)) {
        goto clean_and_exit;
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
    if (!mpv_a || !mpv_b) {
        return NULL;
    }

    new_tmp = create_mpt(0);
    if (!new_tmp) {
        return NULL;
    }

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

        if (!added) {
            goto clean_and_exit;
        }
        mpt_free(&shifted);
        mpt_free(&new_tmp);
        new_tmp = added;
    }

    if (mpt_bit_count(new_tmp) > max_bits) {
        to_remove = (mpt_bit_count(new_tmp) - max_bits) / (new_tmp->list->item_size * 8);
        if (!vector_remove(new_tmp->list, to_remove)) {
            goto clean_and_exit;
        }
    }
    
    new = mpt_optimize(new_tmp);

  clean_and_exit:
    mpt_free(&shifted);
    mpt_free(&new_tmp);
    return new;
}

mpt *mpt_div(const mpt *mpv_dividend, const mpt *mpv_divisor) {
    size_t rb;
    int is_res_negative, end = 0;
    mpt *res, *res_negative, *part, *part_shifted, *one, *abs_dividend, *abs_dividend_next, *abs_divisor;
    res = part = part_shifted = one = abs_dividend = abs_dividend_next = abs_divisor = NULL;
    if (!mpv_dividend || !mpv_divisor) {
        return NULL;
    }

    if (mpt_is_zero(mpv_divisor)) {
        return NULL;
    }

    one = create_mpt(1);
    if (!one) {
        return NULL;
    }
    if (mpt_compare(mpv_divisor, one) == 0) {
        mpt_free(&one);
        return copy_mpt(mpv_dividend);
    }

    abs_dividend = mpt_abs(mpv_dividend);
    if (!abs_dividend) {
        return NULL;
    }
    abs_divisor = mpt_abs(mpv_divisor);
    if (!abs_divisor) {
        mpt_free(&abs_dividend);
        return NULL;
    }

    rb = mpt_bit_count(abs_dividend) - 1;
    is_res_negative = mpt_is_negative(mpv_dividend) != mpt_is_negative(mpv_divisor);

    res = create_mpt(0);
    if (!res) {
        goto clean_and_exit;
    }
    part = create_mpt(0);
    if (!part) {
        goto clean_and_exit;
    }

    for (;;) {
        while (mpt_compare(part, abs_divisor) < 0) {
            mpt_set_bit_to(res, rb, 0);

            if (rb == 0) {
                end = 1;
                break;
            }
            part_shifted = mpt_shift(part, 1, 1);
            mpt_set_bit_to(part_shifted, 0, mpt_get_bit(abs_dividend, --rb));
            mpt_free(&part);
            part = part_shifted;
        }
        if (end) {
            break;
        }

        mpt_set_bit_to(res, rb, 1);

        if (rb == 0) {
            break;
        }
        abs_dividend_next = mpt_sub(part, abs_divisor);
        mpt_free(&part);
        part = mpt_shift(abs_dividend_next, 1, 1);
        mpt_free(&abs_dividend_next);
        mpt_set_bit_to(part, 0, mpt_get_bit(abs_dividend, --rb));
    }

  clean_and_exit:
    mpt_free(&one);
    mpt_free(&part);
    mpt_free(&abs_dividend);
    mpt_free(&abs_divisor);

    if (is_res_negative) {
        res_negative = mpt_negate(res);
        mpt_free(&res);
        res = res_negative;
    }

    return res;
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

mpt *mpt_pow(const mpt *mpv_base, const mpt *mpv_exponent) {
    mpt *new, *new_next, *exp_sub, *exp_sub_next, *one;
    new = new_next = exp_sub = exp_sub_next = one = NULL;
    if (!mpv_base || !mpv_exponent) {
        return NULL;
    }
    if (mpt_is_negative(mpv_exponent)) {
        return create_mpt(0);
    }

    exp_sub = copy_mpt(mpv_exponent);
    new = copy_mpt(mpv_base);
    one = create_mpt(1);
    if (!exp_sub || !new || !one) {
        mpt_free(&new);
        goto clean_and_exit;
    }

    while (mpt_compare(exp_sub, one) > 0) {
        new_next = mpt_mul(new, mpv_base);
        exp_sub_next = mpt_sub(exp_sub, one);

        if (!new_next || !exp_sub_next) {
            mpt_free(&new_next);
            mpt_free(&exp_sub_next);
            new = exp_sub_next = NULL;
            goto clean_and_exit;
        }

        mpt_free(&exp_sub);
        mpt_free(&new);
        exp_sub = exp_sub_next;
        new = new_next;
    }

  clean_and_exit:
    mpt_free(&one);
    mpt_free(&exp_sub);
    return new;
}

mpt *mpt_factorial(const mpt *mpv) {
    mpt *new, *new_tmp, *sub, *sub_tmp, *one;
    new = new_tmp = sub = sub_tmp = one = NULL;
    if (!mpv) {
        return NULL;
    }
    if (mpt_is_negative(mpv)) {
        return NULL;
    }
    if (mpt_is_zero(mpv)) {
        return create_mpt(1);
    }

    one = create_mpt(1);
    new = copy_mpt(mpv);
    sub = mpt_sub(new, one);
    if (!new || !one || !sub) {
        goto clean_and_exit;
    }

    while (mpt_compare(sub, one) >= 1) {
        new_tmp = mpt_mul(new, sub);
        sub_tmp = mpt_sub(sub, one);

        if (!new_tmp || !sub_tmp) {
            mpt_free(&new_tmp);
            mpt_free(&sub_tmp);
            goto clean_and_exit;
        }

        mpt_free(&new);
        mpt_free(&sub);
        new = new_tmp;
        sub = sub_tmp;
    }

  clean_and_exit:
    mpt_free(&one);
    mpt_free(&sub);
    return new;
}
