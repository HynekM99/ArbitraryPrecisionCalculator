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
 *        Výslednou hodnotu zapíše do instance mpt, na kterou ukazuje ukazatel v prvním parametru.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
typedef int (*bi_function)(mpt *, const mpt, const mpt);
/**
 * @brief Definice ukazatele na obecnou funkci, která provádí matematickou operaci jednou instancí struktury typu 'mpt'
 *        Výslednou hodnotu zapíše do instance mpt, na kterou ukazuje ukazatel v prvním parametru.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
typedef int (*un_function)(mpt *, const mpt);

/**
 * @brief Porovná hodnoty dvou instancí mpt.
 * @param a Instance mpt.
 * @param b Porovnávaná instance mpt.
 * @return int 0 pokud jsou hodnoty stejné, -1 pokud je hodnota a < b, 1 pokud je hodnota a > b.
 */
int mpt_compare(const mpt a, const mpt b);

/**
 * @brief Vrátí signum hodnoty instance mpt.
 * @param value Instance mpt.
 * @return int 0 pokud je hodnota rovna 0, -1 pokud je hodnota záporná, 1 pokud je hodnota kladná.
 */
int mpt_signum(const mpt value);

/**
 * @brief Do *dest zapíše absolutní hodnotu zadané instance mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param value Instance mpt.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_abs(mpt *dest, const mpt value);

/**
 * @brief Do *dest zapíše bitově posununou hodnotu zadané instance mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param value Instance mpt.
 * @param positions Počet pozic o kolik se má hodnota posunout.
 * @param shift_left int určující kam se má hodnota posunout. 0 pokud vpravo, jinak vlevo.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_shift(mpt *dest, const mpt value, const size_t positions, const int shift_left);

/**
 * @brief Do *dest zapíše bitově posununou hodnotu zadané instance mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param value Instance mpt.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_negate(mpt *dest, const mpt value);

/**
 * @brief Do *dest zapíše součet zadaných hodnot mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param a Instance mpt s prvním činitelem.
 * @param b Instance mpt s druhým činitelem.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_add(mpt *dest, const mpt a, const mpt b);

/**
 * @brief Do *dest zapíše rozdíl zadaných hodnot mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param a Instance mpt s menšencem.
 * @param b Instance mpt s menšitelem.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_sub(mpt *dest, const mpt a, const mpt b);

/**
 * @brief Do *dest zapíše součin zadaných hodnot mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param a Instance mpt s prvním činitelem.
 * @param b Instance mpt s druhým činitelem.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_mul(mpt *dest, const mpt a, const mpt b);

/**
 * @brief Do *dest zapíše celočíselný podíl zadaných hodnot mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param dividend Instance mpt s dělencem.
 * @param divisor Instance mpt s delitelem.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_div(mpt *dest, const mpt dividend, const mpt divisor);

/**
 * @brief Do *dest zapíše zbytek po celočíselném dělení zadaných hodnot mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param dividend Instance mpt s dělencem.
 * @param divisor Instance mpt s delitelem.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_mod(mpt *dest, const mpt dividend, const mpt divisor);

/**
 * @brief Do *dest zapíše zbytek po celočíselném dělení zadaných hodnot mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param dividend Instance mpt s dělencem.
 * @param divisor Instance mpt s delitelem.
 * @param div_result Ukazatel na instanci mpt s již vypočítánou hodnotou celočíselného dělení dělence a dělitele.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_mod_with_div(mpt *dest, const mpt dividend, const mpt divisor, const mpt div_result);

/**
 * @brief Do *dest zapíše celočíselné umocnění zadaných hodnot mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param base Instance mpt se základem.
 * @param exponent Instance mpt s exponentem.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_pow(mpt *dest, const mpt base, const mpt exponent);

/**
 * @brief Do *dest zapíše faktoriál zadaných hodnot mpt.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param value Instance mpt.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_factorial(mpt *dest, const mpt value);

#endif