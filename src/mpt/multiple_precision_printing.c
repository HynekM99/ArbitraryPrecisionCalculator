#include <stdio.h>
#include "multiple_precision_printing.h"
#include "multiple_precision_operations.h"

/**
 * \brief Vrátí nibble na požadované pozici v instanci mpt (např. pro 10001111 01000010 a pozici 2 vrátí char s hodnotou 15 (1111b)).
 *        Vzhledem k tomu že každý segment v mpt má dva nibbly, 
 *        funkce dokáže vrátit pouze nibbly ve spodní polovině bitů nejvyšší možné hodnoty mpt.
 * \param value Ukazatel na mpt.
 * \param nibble_pos Pozice nibblu.
 * \return char s hodnotou nibblu.
 */
static char mpt_get_nibble_(const mpt *value, const size_t nibble_pos) {
    char nibble = 0;
    size_t i, bit_pos;
    bit_pos = nibble_pos * BITS_IN_NIBBLE;

    for (i = 0; i < BITS_IN_NIBBLE; ++i) {
        nibble += mpt_get_bit(value, bit_pos + i) << i;
    }

    return nibble;
}

/**
 * \brief Do konzole vypíše pozpátku řetězec uložený ve vektoru str. 
 *        Používá se při vypisování hodnoty mpt v dekadické formě, 
 *        protože algoritmus pro převod binární na dekadickou soustavu vypočítává jednotlivé číslice pozpátku. 
 * \param str Ukazatel na vektor s řetězcem, jenž má být vypsán pozpátku.
 */
void str_print_reverse_(const vector_type *str) {
    size_t i;
    for (i = 0; i < vector_count(str); ++i) {
        printf("%d", *(char *)vector_at(str, vector_count(str) - i - 1));
    }
}

void mpt_print_bin(const mpt *value) {
    size_t i, bits;
    int msb;

    if (!value) {
        return;
    }

    msb = mpt_is_negative(value);

    printf("0b%d", msb);

    bits = mpt_bit_count(value);
    for (i = 1; i < bits; ++i) {
        if (mpt_get_bit(value, bits - i - 1) != msb) {
            break;
        }
    }

    for (; i < bits; ++i) {
        printf("%i", mpt_get_bit(value, bits - i - 1));
    }
}

void mpt_print_dec(const mpt *value) {
    vector_type *str;
    mpt *mod, *div, *div_next, *ten;
    mod = div = div_next = ten = NULL;

    #define EXIT_IF(v) \
        if (v) { \
            goto clean_and_exit; \
        }

    EXIT_IF(!value);

    if (mpt_is_zero(value) == 1) {
        printf("0");
        return;
    }

    EXIT_IF(!(div = mpt_abs(value)));
    EXIT_IF(!(ten = mpt_allocate(10)));
    EXIT_IF(!(str = vector_allocate(sizeof(char), NULL)));

    while (!mpt_is_zero(div)) {
        div_next = mpt_div(div, ten);
        mod = mpt_mod_with_div(div, ten, div_next);
        mpt_replace(&div, &div_next);

        EXIT_IF(!mod || !div);
        EXIT_IF(!vector_push_back(str, vector_at(mod->list, 0)));

        mpt_deallocate(&mod);
    }
    
    if (mpt_is_negative(value)) {
        printf("-");
    }
    str_print_reverse_(str);

  clean_and_exit:
    vector_deallocate(&str);
    mpt_deallocate(&mod);
    mpt_deallocate(&div_next);
    mpt_deallocate(&div);
    mpt_deallocate(&ten);

    #undef EXIT_IF
}

void mpt_print_hex(const mpt *value) {
    int msb;
    size_t i, nibbles;
    char nibble = 0, to_leave_out;

    if (!value) {
        return;
    }

    msb = mpt_get_msb(value);
    nibbles = mpt_bit_count(value) / BITS_IN_NIBBLE;
    
    to_leave_out = msb * 0xf;
    
    for (i = 0; i < nibbles; ++i) {
        nibble = mpt_get_nibble_(value, nibbles - i - 1);

        if (nibble != to_leave_out) {
            break;
        }
    }

    printf("0x");

    if (msb == 0 && nibble >= 8) {
        printf("0");
    }
    else if (msb == 1 && nibble < 8) {
        printf("f");
    }

    if (i == nibbles) {
        printf("%x", nibble);
        return;
    }

    for (; i < nibbles; ++i) {
        nibble = mpt_get_nibble_(value, nibbles - i - 1);
        printf("%x", nibble);
    }
}

void mpt_print(const mpt *value, const enum bases base) {
    mpt_printer printer;

    switch (base) {
        case bin: printer = mpt_print_bin; break;
        case dec: printer = mpt_print_dec; break;
        case hex: printer = mpt_print_hex; break;
        default:  return;
    }
    
    printer(value);
}