#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpt/mpt.h"
#include "stack/stack.h"
#include "operators.h"
#include "shunting_yard.h"

#define MAX_INPUT_LENGTH 255

#define QUIT_CODE -1

int evaluate_command(const char *input, enum bases *out) {
    vector_type *rpn_str = NULL;
    stack *values = NULL;
    mpt *result = NULL;
    int res;

    if (strcmp(input, "quit") == 0) {
        return QUIT_CODE;
    }
    if (strcmp(input, "bin") == 0) {
        printf("bin\n");
        *out = bin;
        return 1;
    }
    if (strcmp(input, "dec") == 0) {
        printf("dec\n");
        *out = dec;
        return 1;
    }
    if (strcmp(input, "hex") == 0) {
        printf("hex\n");
        *out = hex;
        return 1;
    }
    if (strcmp(input, "out") == 0) {
        switch (*out) {
            case bin: printf("bin\n"); break;
            case dec: printf("dec\n"); break;
            case hex: printf("hex\n"); break;
            default:  break;
        }
        return 1;
    }

    res = shunt(input, &rpn_str, &values);
    switch (res) {
        case INVALID_SYMBOL: 
            printf("Invalid command \"%s\"!\n", input);
            return 1;
        case SYNTAX_ERROR:
            printf("Syntax error!\n");
            return 1;
        case ERROR:
            printf("Error while shunting!\n");
            return 1;
        default: break;
    }
    
    res = evaluate_rpn(&result, rpn_str, values);
    switch (res) {
        case SYNTAX_ERROR:
            printf("Syntax error!\n");
            return 1;
        case MATH_ERROR:
            printf("Math error!\n");
            return 1;
        case DIV_BY_ZERO:
            printf("Division by zero!\n");
            return 1;
        case FACTORIAL_OF_NEGATIVE:
            printf("Input of factorial must not be negative!\n");
            return 1;
        case ERROR:
            printf("Error while evaluating!\n");
            return 1;
        default: break;
    }
    if (!result) {
        printf("Error!\n");
        return 1;
    }

    mpt_print(result, *out);
    vector_deallocate(&rpn_str);
    stack_free(&values);
    mpt_free(&result);
    printf("\n");
    return 1;
}

int main(int argc, char *argv[]) {
    enum bases out = dec;
    char input[MAX_INPUT_LENGTH];
    FILE *stream;

    if (argc < 2) {
        stream = stdin;
    }
    else if (argc == 2) {
        stream = fopen(argv[1], "r");
        if (!stream) {
            printf("Invalid input file!");
            return EXIT_FAILURE;
        }
    }
    else {
        printf("Usage: %s <file.txt>", __FILE__);
        return EXIT_FAILURE;
    }

    for (;;) {
        printf("> ");
        if (!fgets(input, MAX_INPUT_LENGTH, stream)) {
            break;
        }

        input[strcspn(input, "\r\n")] = '\000';
        if (argc == 2) {
            printf("%s\n", input);
        }
        if (evaluate_command(input, &out) == QUIT_CODE) {
            break;
        }
    }

    fclose(stream);
    return EXIT_SUCCESS;
}