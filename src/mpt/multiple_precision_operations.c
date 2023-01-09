#include <limits.h>
#include "multiple_precision_operations.h"

/**
 * \brief Zjistí, jestli má být příznak carry nastaven, když sečteme a, b a předchozí carry.
 * \param a První hodnota pro sečtení.
 * \param b Druhá hodnota pro sečtení.
 * \param carry Předchozí carry.
 * \return uint 1 jestli má být příznak carry nastaven, 0 pokud ne.
 */
static segment_type addition_carry_(const segment_type a, const segment_type b, const segment_type carry) {
    segment_type c = a + b;
    return (c < a) || (c == UINT_MAX && carry == 1);
}

/**
 * \brief Najde 'most significant set bit (mssb)', tedy pozici nastaveného bitu s nejvyšší vahou. Používá se při výpočtu umocnění.
 * \param value Ukazatel na instanci mpt.
 * \return size_t Index nastaveného bitu s nejvyšší vahou. Vrátí 0 pokud value ukazuje na NULL, nebo je hodnota value rovna nule.
 */
static size_t mpt_get_mssb_pos_(const mpt value) {
    size_t i, bit_pos, bit_count;

    if (mpt_is_zero(value)) {
        return 0;
    }

    bit_count = mpt_bit_count(value);

    for (i = 0; i < bit_count; ++i) {
        bit_pos = bit_count - i - 1;
        if (mpt_get_bit(value, bit_pos)) {
            return bit_pos;
        }
    }

    return ~0;
}

int mpt_compare(const mpt a, const mpt b) {
    size_t bits_a, bits_b, i;
    int bit_a, bit_b;

    #define RETURN_IF(v, e) \
        if (v) { \
            return e; \
        }

    RETURN_IF(a.list == b.list, 0);

    RETURN_IF(mpt_is_zero(a) && mpt_is_zero(b), 0);
    RETURN_IF(mpt_signum(a) > mpt_signum(b), 1);
    RETURN_IF(mpt_signum(a) < mpt_signum(b), -1);
    
    bits_a = mpt_bit_count(a);
    bits_b = mpt_bit_count(b);

    if (mpt_is_negative(a)) { /* a i b jsou záporná */
        if (bits_a > bits_b) {
            return -1;
        }
        if (bits_a < bits_b) {
            return 1;
        }
    }
    else { /* a i b jsou kladná */
        if (bits_a > bits_b) {
            return 1;
        }
        if (bits_a < bits_b) {
            return -1;
        }
    }

    for (i = 0; i < bits_a; ++i) {
        bit_a = mpt_get_bit(a, bits_a - i - 1);
        bit_b = mpt_get_bit(b, bits_a - i - 1);
        
        RETURN_IF(bit_a > bit_b, 1);
        RETURN_IF(bit_a < bit_b, -1);
    }

    return 0;

    #undef RETURN_IF
}

int mpt_signum(const mpt value) {
    if (mpt_is_negative(value)) {
        return -1;
    }
    return !mpt_is_zero(value);
}

int mpt_abs(mpt *dest, const mpt value) {
    if (!dest) {
        return 0;
    }
    if (mpt_is_negative(value)) {
        return mpt_negate(dest, value);
    }
    return mpt_clone(dest, value);
}

int mpt_shift(mpt *dest, const mpt value, const size_t positions, const int shift_left) {
    size_t i, j;

    #define EXIT_IF(v) \
        if (v) { \
            mpt_deinit(dest); \
            return 0; \
        }

    EXIT_IF(!dest);

    EXIT_IF(!mpt_init(dest, 0));

    if (shift_left) {
        i = 0;
        j = positions;
    } else {
        i = positions;
        j = 0;
    }

    for (; i < mpt_bit_count(value) + mpt_bits_in_segment(value); ++i, ++j) {
        EXIT_IF(!mpt_set_bit_to(dest, j, mpt_get_bit(value, i)));
    }

    EXIT_IF(!vector_remove(dest->list, 1));
    EXIT_IF(!mpt_optimize(dest));

    return 1;

    #undef EXIT_IF
}

int mpt_negate(mpt *dest, const mpt value) {
    int res = 1;
    size_t i;
    segment_type segment;
    mpt negated, one;
    negated.list = one.list = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest, 0);

    EXIT_IF(!mpt_init(&one, 1), 0);
    EXIT_IF(!mpt_init(dest, 0), 0);
    EXIT_IF(!mpt_init(&negated, 0), 0);

    EXIT_IF(!vector_clear(negated.list), 0);

    for (i = 0; i < mpt_segment_count(value); ++i) {
        segment = ~(mpt_get_segment(value, i));
        EXIT_IF(!vector_push_back(negated.list, &segment), 0);
    }

    EXIT_IF(!mpt_add(dest, negated, one), 0);

  clean_and_exit:
    mpt_deinit(&one);
    mpt_deinit(&negated);

    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}

int mpt_add(mpt *dest, const mpt a, const mpt b) {
    segment_type segment_a, segment_b, res = 0, carry = 0;
    size_t i, segments;

    #define EXIT_IF(v) \
        if (v) { \
            mpt_deinit(dest); \
            return 0; \
        }
        
    EXIT_IF(!dest);

    EXIT_IF(!mpt_init(dest, 0));
    EXIT_IF(!vector_clear(dest->list));

    if (mpt_bit_count(a) >= mpt_bit_count(b)) {
        segments = mpt_segment_count(a);
    } else {
        segments = mpt_segment_count(b);
    }
    ++segments;

    for (i = 0; i < segments; ++i) {
        segment_a = mpt_get_segment(a, i);
        segment_b = mpt_get_segment(b, i);

        res = segment_a + segment_b + carry;
        carry = addition_carry_(segment_a, segment_b, carry);

        EXIT_IF(!vector_push_back(dest->list, &res));
    }

    EXIT_IF(!mpt_optimize(dest));

    return 1;

    #undef EXIT_IF
}

int mpt_sub(mpt *dest, const mpt a, const mpt b) {
    int res = 1;
    mpt negated_b;
    negated_b.list = NULL;

    if (!dest) {
        return 0;
    }

    if (!mpt_negate(&negated_b, b)) {
        return 0;
    }

    res = mpt_add(dest, a, negated_b);
    mpt_deinit(&negated_b);

    return res;
}

int mpt_mul(mpt *dest, const mpt a, const mpt b) {
    int res = 1;
    size_t i, max_bits, to_remove;
    mpt added, shifted;
    added.list = shifted.list = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest, 0);
    
    EXIT_IF(!mpt_init(dest, 0), 0);

    if (mpt_is_zero(a) || mpt_is_zero(b)) {
        return 1;
    }

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
        
        EXIT_IF(!mpt_shift(&shifted, a, i, 1), 0);
        EXIT_IF(!mpt_add(&added, *dest, shifted), 0);
        mpt_deinit(&shifted);
        mpt_replace(dest, &added);
    }

    if (mpt_bit_count(*dest) > max_bits) {
        to_remove = (mpt_bit_count(*dest) - max_bits) / mpt_bits_in_segment(*dest);
        EXIT_IF(!vector_remove(dest->list, to_remove), 0);
    }
    
    EXIT_IF(!mpt_optimize(dest), 0);

  clean_and_exit:
    mpt_deinit(&added);
    mpt_deinit(&shifted);
    
    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}

int mpt_div(mpt *dest, const mpt dividend, const mpt divisor) {
    size_t current_bit;
    int res = 1, is_res_negative = 0;
    mpt negative, part, part_shifted, one, abs_dividend, sub, abs_divisor;
    negative.list = part.list = part_shifted.list = one.list = NULL;
    abs_dividend.list = sub.list = abs_divisor.list = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest, 0);

    EXIT_IF(mpt_is_zero(divisor), 0);

    EXIT_IF(!mpt_init(&one, 1), 0);

    if (mpt_compare(divisor, one) == 0) {
        mpt_deinit(&one);
        return mpt_clone(dest, dividend);
    }

    EXIT_IF(!mpt_abs(&abs_dividend, dividend), 0);
    EXIT_IF(!mpt_abs(&abs_divisor, divisor), 0);
    EXIT_IF(!mpt_init(&part, 0), 0);
    EXIT_IF(!mpt_init(dest, 0), 0);

    is_res_negative = mpt_is_negative(dividend) != mpt_is_negative(divisor);
    current_bit = mpt_bit_count(abs_dividend) - 1;

    for (;;) {
        while (mpt_compare(part, abs_divisor) < 0) {
            EXIT_IF(!mpt_set_bit_to(dest, current_bit, 0), 0);

            if (current_bit == 0) {
                goto clean_and_exit;
            }

            EXIT_IF(!mpt_shift(&part_shifted, part, 1, 1), 0);
            EXIT_IF(!mpt_set_bit_to(&part_shifted, 0, mpt_get_bit(abs_dividend, --current_bit)), 0);
            mpt_replace(&part, &part_shifted);
        }

        EXIT_IF(!mpt_set_bit_to(dest, current_bit, 1), 0);

        if (current_bit == 0) {
            goto clean_and_exit;
        }

        EXIT_IF(!mpt_sub(&sub, part, abs_divisor), 0);
        mpt_deinit(&part);
        EXIT_IF(!mpt_shift(&part, sub, 1, 1), 0);
        mpt_deinit(&sub);

        EXIT_IF(!mpt_set_bit_to(&part, 0, mpt_get_bit(abs_dividend, --current_bit)), 0);
    }


  clean_and_exit:
    mpt_deinit(&one);
    mpt_deinit(&part);
    mpt_deinit(&part_shifted);
    mpt_deinit(&abs_dividend);
    mpt_deinit(&abs_divisor);

    if (res && is_res_negative) {
        res = mpt_negate(&negative, *dest);
        mpt_replace(dest, &negative);
    }

    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}

int mpt_mod(mpt *dest, const mpt dividend, const mpt divisor) {
    int res = 1;
    mpt div, mul;
    div.list = mul.list = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest, 0);

    EXIT_IF(!mpt_div(&div, dividend, divisor), 0);
    EXIT_IF(!mpt_mul(&mul, div, divisor), 0);
    EXIT_IF(!mpt_sub(dest, dividend, mul), 0);

  clean_and_exit:
    mpt_deinit(&div);
    mpt_deinit(&mul);
    
    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}

int mpt_mod_with_div(mpt *dest, const mpt dividend, const mpt divisor, const mpt div_result) {
    int res = 1;
    mpt mul;
    mul.list = NULL;
    
    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest, 0);

    EXIT_IF(!mpt_mul(&mul, div_result, divisor), 0);
    EXIT_IF(!mpt_sub(dest, dividend, mul), 0);

  clean_and_exit:
    mpt_deinit(&mul);
    
    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}

int mpt_pow(mpt *dest, const mpt base, const mpt exponent) {
    int res = 1;
    size_t i, mssb_pos;
    mpt x, mul;
    x.list = mul.list = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest, 0);

    if (mpt_is_negative(exponent)) {
        return mpt_init(dest, 0);
    }
    if (mpt_is_zero(exponent)) {
        return mpt_init(dest, 1);
    }

    EXIT_IF(!mpt_clone(&x, base), 0);

    mssb_pos = mpt_get_mssb_pos_(exponent);
    for (i = 1; i <= mssb_pos; ++i) {
        EXIT_IF(!mpt_mul(&mul, x, x), 0);
        mpt_replace(&x, &mul);

        if (mpt_get_bit(exponent, mssb_pos - i) == 1) {
            EXIT_IF(!mpt_mul(&mul, x, base), 0);
            mpt_replace(&x, &mul);
        }
    }

    mpt_replace(dest, &x);

  clean_and_exit:
    mpt_deinit(&x);
    mpt_deinit(&mul);

    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}

int mpt_factorial(mpt *dest, const mpt value) {
    int res = 1;
    mpt x, index, index_next, one;
    x.list = index.list = index_next.list = one.list = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest, 0);

    EXIT_IF(!mpt_init(dest, 0), 0);

    EXIT_IF(mpt_is_negative(value), 0);

    if (mpt_is_zero(value)) {
        return mpt_init(dest, 1);
    }

    EXIT_IF(!mpt_init(dest, 1), 0);
    EXIT_IF(!mpt_init(&one, 1), 0);
    EXIT_IF(!mpt_init(&index, 1), 0);

    while (mpt_compare(index, value) <= 0) {
        EXIT_IF(!mpt_mul(&x, *dest, index), 0);
        mpt_replace(dest, &x);

        EXIT_IF(!mpt_add(&index_next, index, one), 0);
        mpt_replace(&index, &index_next);
    }

  clean_and_exit:
    mpt_deinit(&x);
    mpt_deinit(&index);
    mpt_deinit(&index_next);
    mpt_deinit(&one);

    if (!res) {
        mpt_deinit(dest);
    }

    return res;

    #undef EXIT_IF
}
