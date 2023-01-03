/**
 * @file stack.h
 * @author František Pártl (fpartl@kiv.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi funkcí pracujících se zásobníkovou strukturou.
 * @version 1.0
 * @date 2020-09-08
 */

#ifndef STACK_H
#define STACK_H

#include <stddef.h>

/**
 * @brief Definice ukazatele na obecnou funkci, která umí uvolnit prvek uložený v zásobníku.
 *        Využívá se ve funkci stack_free.
 */
typedef void (*stack_it_dealloc_type)(void *item);

/**
 * @brief Definice zásobníkové struktury s možností vložení libovolně velkého prvku.
 */
typedef struct stack_ {
    size_t size;
    size_t item_size;
    size_t sp;
    void *items;
    stack_it_dealloc_type deallocator;
} stack;

/**
 * @brief Funkce pro vytvoření nového zásobníku.
 * @param size Maximální počet položek zásobníku.
 * @param item_size Velikost jednoho prvku zásobníku.
 * @return stack* Ukazatel na nově dynamicky alokovaný zásobník nebo NULL při chybě.
 */
stack *stack_create(const size_t size, const size_t item_size, const stack_it_dealloc_type deallocator);

/**
 * @brief Funkce pro vložení nového prvku na vrchol zásobníku.
 * @param s Zásobník, do kterého bude vložen nový záznam.
 * @param item Vkládaný záznam.
 * @return int 1 pokud vše dopadlo dobře, jinak 0.
 */
int stack_push(stack *s, const void *item);

/**
 * @brief Funkce pro odebrání prvku z vrcholu zásobníku.
 * @param s Zásobník, ze kterého bude prvek odebrán.
 * @param item Ukazatel na paměť, kam bude odebraný prvek zkopírován.
 * @return int 1, pokud vše dopadlo dobře, jinak 0.
 */
int stack_pop(stack *s, void *item);

/**
 * @brief Funkce pro získání prvku z vrcholu zásobníku.
 * @param s Zásobník, jehož poslední přidaný prvek pozorujeme.
 * @param item Ukazatel na paměť, kam se přečtený prvek zkopíruje.
 * @return int 1, pokud všech dopadlo dobře, jinak 0.
 */
int stack_head(const stack *s, void *item);

/**
 * @brief Funkce vrátí počet prvků v zásobníku.
 * @param s Ukazatel na zásobník.
 * @return uint Počet prvků v zásobníku.
 */
size_t stack_item_count(const stack *s);

/**
 * @brief Říká, zda je zadaný zásobník prázdný nebo obsahuje nějaké prvky.
 * @param v Ukazatel na zásobník.
 * @return int 1, pokud je zásobník prázdný, jinak 0.
 */
int stack_isempty(const stack *s);

void stack_clear(stack *s);

/**
 * @brief Funkce pro uvolnění zásobníkové struktury z paměti.
 * @param ss Ukazatel na ukazatel na zásobník, který bude uvolněn.
 */
void stack_free(stack **s);

#endif