/**
 * @file operators.h
 * @authors František Pártl (fpartl@kiv.zcu.cz), Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi výčtových typů, struktur a funkcí pro matematické operátory.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _OPERATORS_H
#define _OPERATORS_H

#include "mpt/multiple_precision_operations.h"

/** Speciální znak pro rozpoznání unárního mínusu */
#define RPN_UNARY_MINUS_SYMBOL '_'

/** Výčtový typ pro asociativitu operátorů */
enum associativity { left, right };

/**
 * @brief Struktura, která obaluje operátor, k němu přidruženou obslužnou funkci, precedenci a asociativitu.
 */
typedef struct func_oper_type_ {
    char operator;              /** Znak, kterým je operace popsána. */
    bi_function bi_handler;     /** Přidružená binární aritmetická operace. */
    un_function un_handler;     /** Přidružená unární aritmetická operace. */
    size_t precedence;          /** Precedence operátoru. */
    enum associativity assoc;   /** Asociativita operátoru. */
} func_oper_type;

/**
 * @brief Vrátí ukazatel na func_oper_type odpovídající požadovanému operátoru.
 * @param operator Znak operátoru.
 * @return Ukazatel na func_oper_type odpovídající požadovanému operátoru
 */
const func_oper_type *get_func_operator(const char operator);

#endif