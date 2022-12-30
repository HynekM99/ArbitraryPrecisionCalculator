/**
 * @file operators.h
 * @author František Pártl (fpartl@kiv.zcu.cz)
 * @version 1.0
 * @date 2020-09-18
 */

#ifndef OPERATORS_H
#define OPERATORS_H

#define RPN_UNARY_MINUS_SYMBOL '_'

#include "mpt/multiple_precision_operations.h"

enum associativity { left, right };

/**
 * @brief Struktura, která obaluje operátor a k němu přidruženou obslužnou funkci.
 */
typedef struct func_oper_type_ {
    char operator;          /*!< Znak, kterým je operace popsána. */
    bi_function bi_handler;     /*!< Přidružená binární aritmetická operace. */
    un_function un_handler;     /*!< Přidružená binární aritmetická operace. */
    size_t precedence;
    enum associativity assoc;
} func_oper_type;

const func_oper_type *get_func_operator(const char operator);

#endif