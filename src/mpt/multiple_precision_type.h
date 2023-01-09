/**
 * @file multiple_precision_type.h
 * @author Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Hlavičkový soubor s deklaracemi funkcí pro práci s instancemi struktur typu 'mpt'.
 * @version 1.0
 * @date 2023-01-04
 */

#ifndef _MPT_BASE_H
#define _MPT_BASE_H

#include "../data_structures/vector.h"

#define BITS_IN_BYTE 8
#define BITS_IN_NIBBLE 4

/** 
 * @brief Výčtový typ pro podporované číselné soustavy 
 */
enum bases { bin = 2, dec = 10, hex = 16 };

/** 
 * @brief Uživatelský typ pro jednotlivé segmenty v instancích struktury mpt.
 */
typedef unsigned int segment_type;

/** 
 * @brief Struktura mpt_
 * Představuje "multiple precision type", tedy celočíselný datový typ s vysokou přesností, který je dynamicky alokovaný.
 * Na 64-bitovém stroji může představovat čísla v rozsahu  <-2^(2^63), 2^(2^63)-1>.
 */
typedef struct mpt_ {
    vector_type *list;
} mpt;

/**
 * @brief Funkce provede inicializaci instance struktury mpt.
 *        Instance bude mít jeden segment s hodnotou init_value, nebo bude neinicializovaná při neúspěšném provedení.
 * @param value Ukazatel na instanci struktury mpt.
 * @param init_value Výchozí hodnota inicializované instance mpt.
 * @return int 1, pokud inicializace proběhla v pořádku, jinak 0.
 */
int mpt_init(mpt *value, const segment_type init_value);

/**
 * @brief Alokuje novou instanci mpt se zadanou hodnotou.
 * @param init_value Počáteční hodnota.
 * @return mpt* Ukazatel na vytvořenou instanci mpt.
 */
mpt *mpt_allocate(const segment_type init_value);

/**
 * @brief Inicializuje instanci mpt 'dest' tak, aby obsahovala stejnou hodnotou jako má 'orig'.
 * @param dest Ukazatel na instanci mpt, do níž se 
 * @param orig Instance mpt, která má být naklonovaná.
 * @return int 1 při úspěšném naklonování, jinak 0.
 */
int mpt_clone(mpt *dest, const mpt orig);

/**
 * @brief Nahradí 'to_replace' za 'replace_with' tak, že deinicializuje data v 'to_replace', data z '*replace_with' zkopíruje do '*to_replace' a data v '*replace_with' vymaže.
 *        Používá se často v matematických operacích pro zpřehlednění kódu.
 * @param to_replace Ukazatel na instanci mpt, která bude nahrazena.
 * @param replace_with Ukazatel na instanci mpt, která nahradí 'to_replace'.
 */
void mpt_replace(mpt *to_replace, mpt *replace_with);

/**
 * @brief Vrátí počet bitů v jednom segmentu instance mpt.
 * @param value Instance mpt.
 * @return size_t Počet bitů v jednom segmentu instance mpt.
 */
size_t mpt_bits_in_segment(const mpt value);

/**
 * @brief Vrátí počet segmentů v instanci mpt.
 * @param value Instance mpt.
 * @return size_t Počet segmentů v instanci mpt.
 */
size_t mpt_segment_count(const mpt value);

/**
 * @brief Vrátí počet bitů v instanci mpt.
 * @param value Instance mpt.
 * @return size_t Počet bitů v instanci mpt.
 */
size_t mpt_bit_count(const mpt value);

/**
 * @brief Vrátí ukazatel na at-tý segment v instanci mpt.
 * @param value Instance mpt.
 * @param at Index segmentu.
 * @return segment_type* Ukazatel na at-tý segment v instanci mpt, NULL pokud je at mimo rozsah.
 */
segment_type *mpt_get_segment_ptr(const mpt value, const size_t at);

/**
 * @brief Vrátí at-tý segment v instanci mpt.
 * @param value Instance mpt.
 * @param at Index segmentu.
 * @return segment_type At-tý segment v instanci mpt. Pokud je at mimo rozsah, vrátí buď 0x00 nebo 0xff, podle toho jestli je hodnota v mpt kladná nebo záporná.
 */
segment_type mpt_get_segment(const mpt value, const size_t at);

/**
 * @brief Nastaví at-tý bit v instanci mpt na 0 pokud je bit_set roven nule, jinak na 1.
 *        Pokud je at mimo rozsah, přidá chybějící segmenty.
 * @param value Instance mpt.
 * @param at Index bitu.
 * @param bit_set Na jakou hodnotu má být bit nastaven.
 * @return int 1 pokud se nastavní bitu podařilo, 0 pokud ne.
 */
int mpt_set_bit_to(mpt *value, const size_t at, const int bit_set);

/**
 * @brief Zjistí hodnotu at-tého bitu v instanci mpt.
 *        Pokud je at mimo rozsah, vrátí MSB.
 * @param value Instance mpt.
 * @param at Index bitu.
 * @return int 1 pokud je at-tý bit nastaven, jinak 0.
 */
int mpt_get_bit(const mpt value, const size_t at);

/**
 * @brief Zjistí hodnotu MSB v instanci mpt.
 * @param value Instance mpt.
 * @return int 1 pokud je MSB nastaven, jinak 0.
 */
int mpt_get_msb(const mpt value);

/**
 * @brief Zjistí hodnota v instanci mpt rovna nule.
 * @param value Instance mpt.
 * @return int 1 pokud je mpt rovna nule, jinak 0.
 */
int mpt_is_zero(const mpt value);

/**
 * @brief Zjistí hodnota v instanci mpt rovna nule.
 * @param value Instance mpt.
 * @return int 1 pokud je hodnota mpt rovna nule, jinak 0.
 */
int mpt_is_negative(const mpt value);

/**
 * @brief Zjistí hodnota v instanci mpt je lichá.
 * @param value Instance mpt.
 * @return int 1 pokud je hodnota mpt lichá, jinak 0.
 */
int mpt_is_odd(const mpt value);

/**
 * @brief Optimalizuje zadanou instanci mpt, aby měla co nejmenší možný počet segmentů potřebných k reprezentaci její hodnoty.
 * @param orig Ukazatel na instanci mpt, která má být optimalizována.
 * @return int 1 pokud se optimalizace povede, 0 pokud ne
 */
int mpt_optimize(mpt *orig);

/**
 * @brief Funkce uvolní prostředy držené mpt, na kterou ukazuje pointer `value`. Využívá se hlavně pro případy, kdy je instance alokována na zásobníku.
 * @param value Ukazatel na instanci mpt.
 */
void mpt_deinit(mpt *value);

/**
 * @brief Funkce pro uvolnění struktury mpt z paměti.
 * @param value Ukazatel na ukazatel na instanci mpt, která bude uvolněna.
 */
void mpt_deallocate(mpt **value);

#endif