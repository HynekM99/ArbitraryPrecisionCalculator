/**
 * @file stack_type.h
 * @authors František Pártl (fpartl@kiv.zcu.cz), Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi funkcí pracujících se zásobníkovou strukturou.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _STACK_H
#define _STACK_H

#include <stddef.h>

/**
 * @brief Definice ukazatele na obecnou funkci, která umí uvolnit prvek uložený v zásobníku.
 *        Využívá se ve funkci stack_deallocate.
 */
typedef void (*stack_it_dealloc_type)(void *item);

/**
 * @brief Definice zásobníkové struktury s možností vložení libovolně velkého prvku.
 */
typedef struct stack_type_ {
    size_t size;
    size_t item_size;
    size_t sp;
    void *items;
    stack_it_dealloc_type deallocator;
} stack_type;

/**
 * @brief Funkce pro vytvoření nového zásobníku.
 * @param size Maximální počet položek zásobníku.
 * @param item_size Velikost jednoho prvku zásobníku.
 * @return stack_type* Ukazatel na nově dynamicky alokovaný zásobník nebo NULL při chybě.
 */
stack_type *stack_allocate(const size_t size, const size_t item_size, const stack_it_dealloc_type deallocator);

/**
 * @brief Funkce pro vložení nového prvku na vrchol zásobníku.
 * @param s Zásobník, do kterého bude vložen nový záznam.
 * @param item Vkládaný záznam.
 * @return int 1 pokud vše dopadlo dobře, jinak 0.
 */
int stack_push(stack_type *s, const void *item);

/**
 * @brief Funkce pro odebrání prvku z vrcholu zásobníku.
 * @param s Zásobník, ze kterého bude prvek odebrán.
 * @param item Ukazatel na paměť, kam bude odebraný prvek zkopírován.
 * @return int 1, pokud vše dopadlo dobře, jinak 0.
 */
int stack_pop(stack_type *s, void *item);

/**
 * @brief Funkce pro získání prvku z vrcholu zásobníku.
 * @param s Zásobník, jehož poslední přidaný prvek pozorujeme.
 * @param item Ukazatel na paměť, kam se přečtený prvek zkopíruje.
 * @return int 1, pokud všech dopadlo dobře, jinak 0.
 */
int stack_head(const stack_type *s, void *item);

/**
 * @brief Funkce vrátí počet prvků v zásobníku.
 * @param s Ukazatel na zásobník.
 * @return uint Počet prvků v zásobníku.
 */
size_t stack_item_count(const stack_type *s);

/**
 * @brief Říká, zda je zadaný zásobník prázdný nebo obsahuje nějaké prvky.
 * @param v Ukazatel na zásobník.
 * @return int 1, pokud je zásobník prázdný, jinak 0.
 */
int stack_isempty(const stack_type *s);

/**
 * @brief Funkce pro uvolnění prvků v zásobníku z paměti a jejich odstranění ze zásobníku.
 * @param s Ukazatel na zásobník, jehož data budou vymazána.
 */
void stack_clear(stack_type *s);

/**
 * @brief Funkce pro uvolnění zásobníkové struktury z paměti.
 * @param s Ukazatel na ukazatel na zásobník, který bude uvolněn.
 */
void stack_deallocate(stack_type **s);

#endif