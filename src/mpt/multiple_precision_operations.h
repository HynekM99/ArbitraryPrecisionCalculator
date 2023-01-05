/**
 * @file multiple_precision_operations.h
 * @author Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi funkcí pro matematické operace nad instancemi struktur typu 'mpt'.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _MPT_OPERATIONS_H
#define _MPT_OPERATIONS_H

#include "multiple_precision_type.h"

/**
 * @brief Definice ukazatele na obecnou funkci, která provádí matematickou operaci nad dvěma instancemi struktur typu 'mpt'
 *        a vrací instanci struktury typu 'mpt' s výslednou hodnotou.
 */
typedef mpt* (*bi_function)(const mpt *, const mpt *);
/**
 * @brief Definice ukazatele na obecnou funkci, která provádí matematickou operaci nad jednou instancí struktury typu 'mpt'
 *        a vrací instanci struktury typu 'mpt' s výslednou hodnotou.
 */
typedef mpt* (*un_function)(const mpt *);

/**
 * @brief Porovná hodnoty dvou instancí mpt.
 * @param a Ukazatel na mpt.
 * @param b Porovnávaná instance mpt.
 * @return int 0 pokud jsou hodnoty stejné, -1 pokud je hodnota a < b, 1 pokud je hodnota a > b
 */
int mpt_compare(const mpt *a, const mpt *b);

/**
 * @brief Vrátí signum hodnoty instance mpt.
 * @param value Ukazatel na mpt.
 * @return int 0 pokud je hodnota rovna 0, -1 pokud je hodnota záporná, 1 pokud je hodnota kladná
 */
int mpt_signum(const mpt *value);

/**
 * @brief Vrátí novou instanci mpt s absolutní hodnotou zadané instance mpt.
 * @param value Ukazatel na mpt.
 * @return mpt* Ukazatel na novou instanci mpt s absolutní hodnotou zadané instance mpt
 */
mpt *mpt_abs(const mpt *value);

/**
 * @brief Vrátí novou instanci mpt s bitově posunutou hodnotou zadané instance mpt.
 * @param value Ukazatel na mpt.
 * @param positions Počet pozic o kolik se má hodnota posunout.
 * @param shift_left int určující kam se má hodnota posunout. 0 pokud vpravo, jinak vlevo.
 * @return mpt* Ukazatel na novou instanci mpt s bitově posunutou hodnotou zadané instance mpt
 */
mpt *mpt_shift(const mpt *value, const size_t positions, const int shift_left);

/**
 * @brief Vrátí novou instanci mpt se zápornou hodnotou zadané instance mpt.
 * @param value Ukazatel na mpt.
 * @return mpt* Ukazatel na novou instanci mpt se zápornou hodnotou zadané instance mpt
 */
mpt *mpt_negate(const mpt *value);

/**
 * @brief Vrátí novou instanci mpt se součtem zadaných hodnot instancí mpt.
 * @param a Ukazatel na instanci mpt s prvním činitelem.
 * @param b Ukazatel na instanci mpt s druhým činitelem.
 * @return mpt* Ukazatel na novou instanci mpt se součtem zadaných hodnot instancí mpt
 */
mpt *mpt_add(const mpt *a, const mpt *b);

/**
 * @brief Vrátí novou instanci mpt s rozdílem zadaných hodnot instancí mpt.
 * @param a Ukazatel na instanci mpt s menšencem.
 * @param b Ukazatel na instanci mpt s menšitelem.
 * @return mpt* Ukazatel na novou instanci mpt s rozdílem zadaných hodnot instancí mpt
 */
mpt *mpt_sub(const mpt *a, const mpt *b);

/**
 * @brief Vrátí novou instanci mpt se součinem zadaných hodnot instancí mpt.
 * @param a Ukazatel na instanci mpt s prvním činitelem.
 * @param b Ukazatel na instanci mpt s druhým činitelem.
 * @return mpt* Ukazatel na novou instanci mpt se součinem zadaných hodnot instancí mpt
 */
mpt *mpt_mul(const mpt *a, const mpt *b);

/**
 * @brief Vrátí novou instanci mpt s celočíselným podílem zadaných hodnot instancí mpt.
 * @param dividend Ukazatel na instanci mpt s dělencem.
 * @param divisor Ukazatel na instanci mpt s delitelem.
 * @return mpt* Ukazatel na novou instanci mpt s celočíselným podílem zadaných hodnot instancí mpt
 */
mpt *mpt_div(const mpt *dividend, const mpt *divisor);

/**
 * @brief Vrátí novou instanci mpt se zbytkem po dělení zadaných hodnot instancí mpt.
 * @param dividend Ukazatel na instanci mpt s dělencem.
 * @param divisor Ukazatel na instanci mpt s delitelem.
 * @return mpt* Ukazatel na novou instanci mpt se zbytkem po dělení zadaných hodnot instancí mpt
 */
mpt *mpt_mod(const mpt *dividend, const mpt *divisor);

/**
 * @brief Vrátí novou instanci mpt s výsledkem celočíselného umocnění základu exponentem.
 * @param base Ukazatel na instanci mpt se základem.
 * @param exponent Ukazatel na instanci mpt s exponentem.
 * @return mpt* Ukazatel na novou instanci mpt s výsledkem celočíselného umocnění základu exponentem
 */
mpt *mpt_pow(const mpt *base, const mpt *exponent);

/**
 * @brief Vrátí novou instanci mpt s faktoriálem zadané instance mpt.
 * @param value Ukazatel na mpt.
 * @return mpt* Ukazatel na novou instanci mpt s faktoriálem zadané instance mpt
 */
mpt *mpt_factorial(const mpt *value);

#endif