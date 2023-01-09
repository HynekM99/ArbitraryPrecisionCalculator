/**
 * @file shunting_yard.h
 * @author Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi výčtových typů, struktur a funkcí pro matematické operátory.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _SHUNTING_YARD_H
#define _SHUNTING_YARD_H

#include "data_structures/stack.h"
#include "data_structures/vector.h"
#include "operators.h"
#include "mpt/mpt.h"

/** Symbol pro rpn, který označuje hodnotu */
#define RPN_VALUE_SYMBOL 'n'

/** Makra pro úspěšnost parsování a vyhodnocení matematického výrazu */

#define ERROR -1
#define SYNTAX_ERROR 0
#define SYNTAX_OK 1
#define INVALID_SYMBOL 2

/** Makra pro výsledek matematického výrazu */

#define MATH_ERROR 3
#define RESULT_OK 1
#define DIV_BY_ZERO 4
#define FACTORIAL_OF_NEGATIVE 5

/**
 * @brief Provede nad řetězcem s matematickým výrazem v infixové formě algoritmus shunting yard
 *        a výsledný RPN výraz uloží do dynamicky alokovaného vektoru. 
 *        Číselné hodnoty jsou v RPN výrazu reprezentovány symbolem RPN_VALUE_SYMBOL
 *        a skutečné instance mpt jsou uloženy do dynamicky alokovaného zásobníku.
 * @param str Řetězec s matematickým výrazem v infixové formě.
 * @param rpn_str Ukazatel na ukazatel na vektor, který bude bude vytvořen a bude obsahovat řetězec s RPN výrazem.
 *                Při neúspěšném shunting yardu bude rpn_str ukazovat na NULL.
 * @param rpn_values Ukazatel na ukazatel na zásobník, který bude bude vytvořen a bude obsahovat ukazatele na instance mpt s hodnotami pro RPN výraz.
 *                   Při neúspěšném shunting yardu bude rpn_values ukazovat na NULL.
 * @return int s hodnotou některého z maker pro úspěšnost výsledku.
 */
int shunt(const char *str, vector_type **rpn_str, stack_type **rpn_values);

/**
 * @brief Ze zadaného RPN výrazu vypočítá výslednou hodnotu a zapíše ji do instance mpt, na kterou ukazuje ukazatel 'dest'.
 * @param dest Ukazatel na vytvořenou instanci mpt s výslednou hodnotou. 
 *             Při neúspěšném vyhodnocení bude dest ukazovat na NULL.
 * @param rpn_str Ukazatel na vektor, který obsahuje řetězec s RPN výrazem.
 * @param rpn_values Ukazatel na zásobník, který obsahuje ukazatele na instance mpt s hodnotami pro RPN.
 *                   V průběhu vyhodnocování bude ze zásobníku hodnoty odstraňovat.
 * @return int s hodnotou některého z maker pro úspěšnost výsledku.
 */
int evaluate_rpn(mpt *dest, const vector_type *rpn_str, stack_type *rpn_values);

#endif