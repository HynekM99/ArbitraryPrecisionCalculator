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
typedef struct bi_func_oper_type_ {
    char operator;          /*!< Znak, kterým je operace popsána. */
    bi_function handler;     /*!< Přidružená aritmetická operace. */
    size_t precedence;
    enum associativity assoc;
} bi_func_oper_type;

typedef struct un_func_oper_type_ {
    char operator;          /*!< Znak, kterým je operace popsána. */
    un_function handler;     /*!< Přidružená aritmetická operace. */
    size_t precedence;
    enum associativity assoc;
} un_func_oper_type;

bi_func_oper_type *get_bi_func_operator(const char operator);

un_func_oper_type *get_un_func_operator(const char operator);

#endif