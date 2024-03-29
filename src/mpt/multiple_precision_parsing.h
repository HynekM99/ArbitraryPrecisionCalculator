/**
 * @file multiple_precision_parsing.h
 * @author Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi funkcí pro převod stringů do instancí struktur typu 'mpt'.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _MPT_PARSING_H
#define _MPT_PARSING_H

#include "multiple_precision_type.h"

/**
 * @brief Definice ukazatele na obecnou funkci, která převádí znak na int.
 */
typedef int (*char_parser)(const char);
/**
 * @brief Definice ukazatele na obecnou funkci, která převádí řetězec na instanci mpt.
 */
typedef int (*str_parser)(mpt *dest, const char **);

/**
 * @brief Převádí znak na hodnotu odpovídající zadané číselné soustavě.
 * @param c Znak na převedení (v případě hexadecimální soustavy jsou platné malé i velké znaky).
 * @param base Požadovaná číselná soustava.
 * @return int s hodnotou, pokud se znak podařilo převést, jinak -1.
 */
int parse_char(const char c, const enum bases base);

/**
 * @brief Převádí řezězec reprezentující binární hodnotu na instanci mpt s odpovídající hodnotou.
 *        Převádění provádí dokud znaky odpovídají znakům binární soustavy.
 *        Při převádění postupně inkrementuje ukazatel na řetězec (tedy *str),
 *        *str tedy na konci převádění bude ukazovat na znak za převedenou binární hodnotou.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param str Ukazatel na ukazatel na řetězec.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_parse_str_bin(mpt *dest, const char **str);

/**
 * @brief Převádí řezězec reprezentující decimální hodnotu na instanci mpt s odpovídající hodnotou.
 *        Převádění provádí dokud znaky odpovídají znakům decimální soustavy.
 *        Při převádění postupně inkrementuje ukazatel na řetězec (tedy *str),
 *        *str tedy na konci převádění bude ukazovat na znak za převedenou decimální hodnotou.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param str Ukazatel na ukazatel na řetězec.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_parse_str_dec(mpt *dest, const char **str);

/**
 * @brief Převádí řezězec reprezentující hexadecimální hodnotu na instanci mpt s odpovídající hodnotou.
 *        Převádění provádí dokud znaky odpovídají znakům hexadecimální soustavy.
 *        Při převádění postupně inkrementuje ukazatel na řetězec (tedy *str),
 *        *str tedy na konci převádění bude ukazovat na znak za převedenou hexadecimální hodnotou.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param str Ukazatel na ukazatel na řetězec.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_parse_str_hex(mpt *dest, const char **str);

/**
 * @brief Převádí řezězec reprezentující binární, decimální, nebo hexadecimální hodnotu na instanci mpt s odpovídající hodnotou.
 *        Příslušnou soustavu rozpozná podle standardních zápisů (0b... binární, 0x... hexadecimální, jinak decimální).
 *        Převádění provádí dokud znaky odpovídají znakům příslušné soustavy.
 *        Při převádění postupně inkrementuje ukazatel na řetězec (tedy *str),
 *        *str tedy na konci převádění bude ukazovat na znak za převedenou hodnotou.
 * @param dest Ukazatel na výslednou instanci mpt.
 * @param str Ukazatel na ukazatel na řetězec.
 * @return int 1 pokud se operace podařila, 0 pokud ne.
 */
int mpt_parse_str(mpt *dest, const char **str);

#endif