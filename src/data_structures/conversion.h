/**
 * @file conversion.h
 * @author Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi funkcí pro konverzi datových struktur.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _CONVERT_H
#define _CONVERT_H

#include "vector.h"
#include "stack.h"

/**
 * @brief Funkce pro přesunutí dat z vektoru do zásobníku. Nultý prvek ve vektoru bude na vrcholu zásobníku.
 * @param v Ukazatel na ukazatel na vektor, jehož data mají být přesunuta. Po úspěšném přesunutí bude dealokován.
 * @return stack_type* Ukazatel na vytvořený zásobník nebo NULL při chybě.
 */
stack_type *vector_to_stack(vector_type **v);

#endif