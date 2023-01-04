#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpt/mpt.h"
#include "data_structures/vector.h"
#include "operators.h"
#include "shunting_yard.h"

#define QUIT_CODE -1
#define EVALUATION_FAILURE 0
#define EVALUATION_SUCCESS 1

static int is_end_char_(const char c) {
    return c == 0 || c == '\n';
}

FILE *init_stream(const int argc, char *argv[]) {
    FILE *stream = NULL;

    if (argc < 2) {
        return stdin;
    }

    if (argc > 2) {
        printf("Usage: %s <file.txt>", __FILE__);
        return NULL;
    }

    if (!(stream = fopen(argv[1], "r"))) {
        printf("Invalid input file!");
        return NULL;
    }

    return stream;
}

int load_line(FILE *stream, vector_type *dest) {
    int c_int;
    char c = 0;
    if (!stream || !dest || !vector_isempty(dest)) {
        return 0;
    }

    while ((c_int = getc(stream)) != EOF) {
        c = (char)c_int;
        if (c == '\n' || c == '\r') {
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

void print_out(const enum bases out) {
    switch (out) {
        case bin: printf("bin\n"); break;
        case dec: printf("dec\n"); break;
        case hex: printf("hex\n"); break;
        default:  break;
    }
}

int str_empty(const char *str) {
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

int evaluate_expression(const char *input, enum bases *out) {
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

    if (str_empty(input)) {
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
        FAIL_IF_NOT(input = (char *)vector_at(input_vector, 0));

        if (vector_isempty(input_vector)) {
            break;
        }

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
    fclose(stream);

    return exit;

    #undef FAIL_IF_NOT
}