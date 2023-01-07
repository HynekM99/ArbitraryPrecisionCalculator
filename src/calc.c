/**
 * @file calc.c
 * @authors Hynek Moudrý (hmoudry@students.zcu.cz)
 * @brief Celočíselná kalkulačka s neomezenou přesností.
 * @version 1.0
 * @date 2023-01-04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpt/mpt.h"
#include "data_structures/vector.h"
#include "operators.h"
#include "shunting_yard.h"

/** Makro s hodnotou vyhodnocení příkazu pro ukončení programu */
#define QUIT_CODE -1

/** Makra pro úspěšnost vyhodnocení příkazu */

#define EVALUATION_FAILURE 0
#define EVALUATION_SUCCESS 1

/** 
 * \brief Zjistí, jestli je znak ukončující, tedy nulový nebo '\n'
 * \param c Znak.
 * \return int 1 jestli je znak ukončující, jinak 0.
*/
static int is_end_char_(const char c) {
    return c == 0 || c == '\n';
}

/** 
 * \brief Zjistí, jestli je řetězec prázdný, tedy složený pouze z mezer.
 * \param str Řetězec.
 * \return int 1 jestli je řetězec prázdný, jinak 0.
*/
int str_empty_(const char *str) {
    if (!str) {
        return 1;
    }

    for (; !is_end_char_(*str); ++str) {
        if (*str != ' ') {
            return 0;
        }
    }

    return 1;
}

/** 
 * @brief Vrátí stream, se kterým bude kalkulačka pracovat.
 * @param argc Počet parametrů z příkazové řádky.
 * @param argv Pole řetězců příkazů z příkazové řádky.
 * @return FILE* stdin pokud byl program spuštěn bez příkazu, jinak stream se zadaným souborem pokud se ho podařilo otevřít, jinak NULL.
*/
FILE *init_stream(const int argc, char *argv[]) {
    FILE *stream = NULL;

    if (argc < 2) {
        return stdin;
    }

    if (argc > 2) {
        printf("Usage: %s <file.txt>\n", __FILE__);
        return NULL;
    }

    if (!(stream = fopen(argv[1], "r"))) {
        printf("Invalid input file!\n");
        return NULL;
    }

    return stream;
}

/** 
 * @brief Načte řádek ze streamu a uloží ho jako řetězec do vektoru.
 * @param stream Stream, ze kterého čteme.
 * @param dest Ukazatel na vektor.
 * @return int 1 jestli se podařilo řádek načíst, jinak 0.
*/
int load_line(FILE *stream, vector_type *dest) {
    int c_int;
    char c = 0;

    if (!stream || !dest || !vector_isempty(dest)) {
        return 0;
    }

    while ((c_int = getc(stream)) != EOF) {
        c = (char)c_int;
        if (is_end_char_(c)) {
            c = 0;
        }
        if (!vector_push_back(dest, &c)) {
            return 0;
        }
        if (c == 0) {
            break;
        }
    }

    if (c != 0) {
        c = 0;
        if (!vector_push_back(dest, &c)) {
            return 0;
        }
    }

    return 1;
}

/** 
 * @brief Vypíše číselnou soustavu, která je právě zvolená pro výpis výsledků.
 * @param out Aktuální číselná soustava.
*/
void print_out(const enum bases out) {
    switch (out) {
        case bin: printf("bin\n"); break;
        case dec: printf("dec\n"); break;
        case hex: printf("hex\n"); break;
        default:  break;
    }
}

/** 
 * @brief Vyhodnotí zadaný matematický výraz.
 * @param input Řetězec s výrazem.
 * @param out Ukazatel na aktuální číselnou soustavu.
 * @return int s hodnotou některého z maker pro úspěšnost výsledku (viz shunting_yard.h).
*/
int evaluate_expression(const char *input, const enum bases *out) {
    int evaluation_res = EVALUATION_FAILURE;
    vector_type *rpn_str = NULL;
    stack_type *values = NULL;
    mpt *result = NULL;
    int res;

    switch (res = shunt(input, &rpn_str, &values)) {
        case INVALID_SYMBOL: printf("Invalid command \"%s\"!\n", input); break;
        case SYNTAX_ERROR:   printf("Syntax error!\n"); break;
        case ERROR:          printf("Error while parsing!\n"); break;
        default: break;
    }

    if (res != SYNTAX_OK) {
        goto clean_and_exit;
    }

    switch (res = evaluate_rpn(&result, rpn_str, values)) {
        case SYNTAX_ERROR:          printf("Syntax error!\n"); break;
        case MATH_ERROR:            printf("Math error!\n"); break;
        case DIV_BY_ZERO:           printf("Division by zero!\n"); break;
        case FACTORIAL_OF_NEGATIVE: printf("Input of factorial must not be negative!\n"); break;
        case ERROR:                 printf("Error while evaluating!\n"); break;
        default: 
            evaluation_res = EVALUATION_SUCCESS;
            mpt_print(result, *out);
            printf("\n");
            break;
    }

  clean_and_exit:
    vector_deallocate(&rpn_str);
    stack_deallocate(&values);
    mpt_deallocate(&result);

    return evaluation_res;
}

/** 
 * @brief Vyhodnotí zadaný příkaz.
 * @param input Řetězec s výrazem.
 * @param out Ukazatel na aktuální číselnou soustavu.
 * @return int s hodnotou některého z maker pro vyhodnocení příkazu (viz začátek calc.c).
*/
int evaluate_command(const char *input, enum bases *out) {
    if (!out) {
        return EVALUATION_FAILURE;
    }

    #define SET_OUT_IF(v, e) \
        if (v) { \
            *out = e; \
            print_out(*out); \
            return EVALUATION_SUCCESS; \
        }

    if (str_empty_(input)) {
        return EVALUATION_SUCCESS;
    }
    if (strcmp(input, "quit") == 0) {
        return QUIT_CODE;
    }
    if (strcmp(input, "out") == 0) {
        print_out(*out);
        return EVALUATION_SUCCESS;
    }

    SET_OUT_IF(strcmp(input, "bin") == 0, bin);
    SET_OUT_IF(strcmp(input, "dec") == 0, dec);
    SET_OUT_IF(strcmp(input, "hex") == 0, hex);

    return evaluate_expression(input, out);

    #undef SET_OUT_IF
}

/** 
 * @brief Spouštěcí funkce programu.
 * @param argc Počet parametrů z příkazové řádky.
 * @param argv Pole řetězců příkazů z příkazové řádky.
 * @return EXIT_SUCCESS pokud byl program ukončen úspěšně, EXIT_FAILURE pokud ne.
*/
int main(int argc, char *argv[]) {
    int exit = EXIT_SUCCESS;
    char *input = NULL;
    enum bases out = dec;
    vector_type *input_vector = NULL;
    FILE *stream = NULL;

    #define FAIL_IF_NOT(v) \
        if (!(v)) { \
            exit = EXIT_FAILURE; \
            goto clean_and_exit; \
        }

    FAIL_IF_NOT(input_vector = vector_allocate(sizeof(char), NULL));
    FAIL_IF_NOT(stream = init_stream(argc, argv));

    for (;;) {
        printf("> ");

        FAIL_IF_NOT(load_line(stream, input_vector));

        if (vector_isempty(input_vector)) {
            break;
        }

        FAIL_IF_NOT(input = (char *)vector_at(input_vector, 0));

        if (stream != stdin) {
            printf("%s\n", input);
        }

        if (evaluate_command(input, &out) == QUIT_CODE) {
            break;
        }

        FAIL_IF_NOT(vector_clear(input_vector));

        if (feof(stream)) {
            break;
        }
    }

  clean_and_exit:
    vector_deallocate(&input_vector);
    if (stream) {
        fclose(stream);
    }

    return exit;

    #undef FAIL_IF_NOT
}