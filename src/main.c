#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpt/mpt.h"
#include "stack/stack.h"
#include "operators.h"
#include "shunting_yard.h"

#define QUIT_CODE -1
#define EVALUATION_FAILURE 0
#define EVALUATION_SUCCESS 1

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

int evaluate_expression(const char *input, enum bases *out) {
    vector_type *rpn_str = NULL;
    stack *values = NULL;
    mpt *result = NULL;
    int res = shunt(input, &rpn_str, &values);

    switch (res) {
        case INVALID_SYMBOL: 
            printf("Invalid command \"%s\"!\n", input);
            return EVALUATION_FAILURE;
        case SYNTAX_ERROR:
            printf("Syntax error!\n");
            return EVALUATION_FAILURE;
        case ERROR:
            printf("Error while shunting!\n");
            return EVALUATION_FAILURE;
        default: break;
    }
    
    res = evaluate_rpn(&result, rpn_str, values);
    switch (res) {
        case SYNTAX_ERROR:
            printf("Syntax error!\n");
            return EVALUATION_FAILURE;
        case MATH_ERROR:
            printf("Math error!\n");
            return EVALUATION_FAILURE;
        case DIV_BY_ZERO:
            printf("Division by zero!\n");
            return EVALUATION_FAILURE;
        case FACTORIAL_OF_NEGATIVE:
            printf("Input of factorial must not be negative!\n");
            return EVALUATION_FAILURE;
        case ERROR:
            printf("Error while evaluating!\n");
            return EVALUATION_FAILURE;
        default: break;
    }

    if (!result) {
        printf("Error!\n");
        return EVALUATION_FAILURE;
    }

    mpt_print(result, *out);
    printf("\n");

    vector_deallocate(&rpn_str);
    stack_free(&values);
    mpt_free(&result);

    return EVALUATION_SUCCESS;
}

int evaluate_command(const char *input, enum bases *out) {
    if (strcmp(input, "quit") == 0) {
        return QUIT_CODE;
    }
    if (strcmp(input, "bin") == 0) {
        *out = bin;
        print_out(*out);
        return EVALUATION_SUCCESS;
    }
    if (strcmp(input, "dec") == 0) {
        *out = dec;
        print_out(*out);
        return EVALUATION_SUCCESS;
    }
    if (strcmp(input, "hex") == 0) {
        *out = hex;
        print_out(*out);
        return EVALUATION_SUCCESS;
    }
    if (strcmp(input, "out") == 0) {
        print_out(*out);
        return EVALUATION_SUCCESS;
    }

    return evaluate_expression(input, out);
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

    stream = fopen(argv[1], "r");
    if (!stream) {
        printf("Invalid input file!");
        return NULL;
    }

    return stream;
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

    input_vector = vector_allocate(sizeof(char), NULL);
    FAIL_IF_NOT(input_vector);
    
    stream = init_stream(argc, argv);
    FAIL_IF_NOT(stream);

    for (;;) {
        printf("> ");

        FAIL_IF_NOT(load_line(stream, input_vector));

        input = (char *)vector_at(input_vector, 0);
        FAIL_IF_NOT(input);

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