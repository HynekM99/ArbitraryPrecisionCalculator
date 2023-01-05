/**
 * @file vector_type.h
 * @authors František Pártl (fpartl@kiv.zcu.cz), Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor knihovny dynamicky realokovaného pole.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _VECTOR_H
#define _VECTOR_H

#include <stddef.h>

/** @brief Násobitel zvětšení pole při realokaci. */
#define VECTOR_SIZE_MULT 2

/** @brief Počáteční velikost vektoru. */
#define VECTOR_INIT_SIZE 1

/**
 * @brief Definice ukazatele na obecnou funkci, která umí uvolnit prvek uložený ve vektoru.
 *        Využívá se ve funkci vector_deallocate.
 */
typedef void (*vec_it_dealloc_type)(void *item);

/**
 * @brief Struktura vector_type_
 * Přestavuje obecné dynamicky alokované pole, do které lze přidávat prvky, manipulovat s nimi a
 * odstraňovat je (včetně jeji uvolnění).
 */
typedef struct vector_type_ {
    size_t count;
    size_t capacity;
    size_t item_size;
    void *data;
    vec_it_dealloc_type deallocator;
} vector_type;

/**
 * @brief Alokuje prázdný vektor s kapacitou VECTOR_INIT_SIZE.
 * @param item_size Velikost jednoho prvku vektoru.
 * @param deallocator Ukazatel na dealokátor alokovaných prvků vektoru (emulace destruktoru).
 * @return vector_type* Ukazatel na alokovaný vektor.
 */
vector_type *vector_allocate(const size_t item_size, const vec_it_dealloc_type deallocator);

/**
 * @brief Funkce provede inicializaci instance struktury `vector_type`, která je dána ukazatelem `v`.
 *        Datový segment bufferu bude prealokován pro VECTOR_INIT_SIZE prvků o velikosti item_size.
 *
 * @param v Ukazatel na inicializovanou instanci struktury `vector_type`.
 * @param item_size Velikost jednoho prvku vektoru.
 * @param deallocator Ukazatel na dealokátor alokovaných prvků vektoru (emulace destruktoru).
 * @return int 1, pokud inicializace proběhla v pořádku, jinak 0.
 */
int vector_init(vector_type *v, const size_t item_size, const vec_it_dealloc_type deallocator);

/**
 * @brief Funkce uvolní prostředy držené vektorem, na který ukazuje pointer `v`. Tato funkce se nám bude hodit, pokud samotný vektor (instanci struktury)
 *        alokujeme automaticky na zásobníku libovolné funkce. Na zásobníku tedy nebudeme alokovat 8B pointer, který bychom při použití museli vždy dereferencovat,
 *        ale místo toho na zásobníku funkce alokujeme rovnou celou instanci struktury vector_type, která není nikterak velká (dokud se budeme při práci se zásobníkem
 *        držet pod hranicí 100 kB, tak nemusíme bát přetečení). Při uvolňování automaticky alokované instance struktury vektor tedy budeme chtít pouze uvolnit jí
 *        držená data (buffer a případně vynulovat paměť), ale samotnou instanci struktury uvolňovat nebudeme, to se stane automaticky při ukončení funkce. V případě
 *        platnosti ukazatele `v->deallocator` je funkce, na kterou ukazatel odkazuje, zavolána nad všemi prvky vektoru (emulace destruktoru).
 * @param v Ukazatel na vektor.
 */
void vector_deinit(vector_type *v);

/**
 * @brief Odstraní a případně uvolní všechny prvky vektoru.
 * @param v Ukazatel na vektor, z něhož se prvky odstraňují.
 * @return int 1, pokud se odstranění prvků povedlo, jinak 0.
 */
int vector_clear(vector_type *v);

/**
 * @brief Uvolní data držená vektorem `*v` z operační paměti počítače pomocí funkce `vector_deinit` a následně uvolní i dynamicky alokovanou instanci sturktury `vector_type`
 *        a zneplatní ukazatel na ní.
 * @param v Ukazatel na ukazatel na vektor.
 */
void vector_deallocate(vector_type **v);

/**
 * @brief Alokuje nový vektor s mělkou kopií dat zadaného vektoru.
 * @param src Ukazatel na vektor.
 * @return vector_type* Ukazatel na zkopírovaný vektor.
 */
vector_type *vector_clone(const vector_type *src);

/**
 * @brief Vrátí kapacitu vektoru (počet prvků, které do vektoru lze uložit bez realokace).
 * @param v Ukazatel na vektor.
 * @return size_t Kapacita vektoru, daného ukazatelem v.
 */
size_t vector_capacity(const vector_type *v);

/**
 * @brief Vrátí počet prvků vektoru.
 * @param v Ukazatel na vektor.
 * @return size_t Počet zapsaných prvků ve vektoru.
 */
size_t vector_count(const vector_type *v);

/**
 * @brief Říká, zda je zadaný vektor prázdný nebo obsahuje nějaké prvky.
 * @param v Ukazatel na vektor.
 * @return int 1, pokud je vektor prázdný, jinak 0.
 */
int vector_isempty(const vector_type *v);

/**
 * @brief Realokuje zadaný vektor tak, aby jeho kapacita byla size. Zmenšení vektoru není povoleno.
 * @param v Realokovaný vektor.
 * @param capacity Nová kapacita vektoru. 
 * @return int 1, pokud se realokace povedla, jinak 0.
 */
int vector_realloc(vector_type *v, const size_t capacity);

/**
 * @brief Vrací ukazatel na at-tý prvek vektoru v. V případě neexistence prvku vratí NULL.
 * @param v Ukazatel na vektor.
 * @param at Index prvku ve vektoru.
 * @return void* Ukazatel na prvek vektoru nebo NULL při neexistenci prvku.
 */
void *vector_at(const vector_type *v, const size_t at);

/**
 * @brief Přidá prvek do vektoru. V případě chyby není vektor daný ukazatel v změněn.
 * @param v Ukazatel na vektor, kam se prvek přidává.
 * @param item Ukazatel na přidávaný prvek.
 * @return int 1, pokud se přidání prvku povedlo, jinak 0.
 */
int vector_push_back(vector_type *v, const void *item);

/**
 * @brief Odstraní zadaný počet prvků z konce vektoru. 
 *        Pokud je zadaný počet větší než počet prvků ve vektoru, odstraní všechny prvky.
 * @param v Ukazatel na vektor, z něhož se prvky odstraňují.
 * @param count Počet prvků k odstranění.
 * @return int 1, pokud se odstranění prvků povedlo, jinak 0.
 */
int vector_remove(vector_type *v, size_t count);

/**
 * @brief Funkce zadanému vektoru odebere vlastnictví udržovaného ukazatele na dynamicky
 *        alokovaný buffer. 
 * @param v Ukazatel na vektor, nad jehož daty přebíráme kontrolu.
 * @return void* Ukazatel na data získaná z vektoru.
 */
void *vector_giveup(vector_type *v);

#endif
