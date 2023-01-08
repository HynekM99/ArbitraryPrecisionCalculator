/**
 * @file multiple_precision_printing.h
 * @author Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi funkcí pro vypisování hodnot struktur typu 'mpt'.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _MPT_PRINT_H
#define _MPT_PRINT_H

#include "multiple_precision_type.h"

/**
 * @brief Definice ukazatele na obecnou funkci, která do konzole vypíše hodnotu instance mpt.
 */
typedef void (*mpt_printer)(const mpt);

/**
 * @brief Vypíše do konzole hodnotu instance mpt v binárním tvaru. Nevypíše nic, pokud value na nic neukazuje.
 * @param value Ukazatel na mpt.
 */
void mpt_print_bin(const mpt value);

/**
 * @brief Vypíše do konzole hodnotu instance mpt v dekadickém tvaru. Nevypíše nic, pokud value na nic neukazuje.
 * @param value Ukazatel na mpt.
 */
void mpt_print_dec(const mpt value);

/**
 * @brief Vypíše do konzole hodnotu instance mpt v hexadecimálním tvaru. Nevypíše nic, pokud value na nic neukazuje.
 * @param value Ukazatel na mpt.
 */
void mpt_print_hex(const mpt value);

/**
 * @brief Vypíše do konzole hodnotu instance mpt ve tvaru zadané soustavy. Nevypíše nic, pokud value na nic neukazuje.
 * @param value Ukazatel na mpt.
 * @param base Požadovaná číselná soustava.
 */
void mpt_print(const mpt value, const enum bases base);

#endif