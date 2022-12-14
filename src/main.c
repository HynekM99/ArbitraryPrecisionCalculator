#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpt/mpt.h"
#include "stack/stack.h"
#include "operators.h"

#define MAX_INPUT_LENGTH 255

/**
 * @brief Funkce slouží k vyhodnocení výrazu zapsaného v reverzní polské notaci (RPN).
 * @param input Zpracovávaný výraz.
 * @param result Ukazatel na paměť, kam bude zkopírován výsledek výrazu.
 * @return int 1, pokud zpracování proběhlo v pořádku, jinak 0.
 */
int evalutate_rpn_expression_3(const char *input, mpt **result) {
    size_t i, input_length;
    stack *s = NULL;
    mpt *a, *b, *temp;
    bi_function bi_func_handler = NULL;
    un_function un_func_handler = NULL;

    #define EXIT_IF_NOT(expression)   \
        if (!(expression)) {            \
            stack_free(&s);             \
            return 0;                   \
        }

    EXIT_IF_NOT(input && result);

    input_length = strlen(input);
    EXIT_IF_NOT(input_length != 0);

    s = stack_create(input_length, sizeof(mpt *));
    EXIT_IF_NOT(s);

    for (i = 0; i < input_length; ++i) {
        if (input[i] >= '0' && input[i] <= '9') {
            temp = create_mpt(input[i] - '0');
            stack_push(s, &temp);
        }
        else {
            if (is_bi_func_operator(input[i])) {
                bi_func_handler = get_bi_func_operator_handler(input[i]);
            }
            if (is_un_func_operator(input[i])) {
                un_func_handler = get_un_func_operator_handler(input[i]);
            }
            EXIT_IF_NOT(bi_func_handler || un_func_handler);

            if (un_func_handler) {
                EXIT_IF_NOT(stack_pop(s, &a));
                temp = un_func_handler(a);
            }
            else if (bi_func_handler) {
                EXIT_IF_NOT(stack_pop(s, &b) && stack_pop(s, &a));
                temp = bi_func_handler(a, b);
            }
            
            stack_push(s, &temp);   /* Přidání kontroly návratové hodnoty funkce stack_push by při použití makra bylo celkem bezbolestné. */
            bi_func_handler = NULL;
            un_func_handler = NULL;
        }
    }

    EXIT_IF_NOT(stack_item_count(s) == 1);

    stack_pop(s, result);
    stack_free(&s);
    return 1;

    #undef EXIT_IF_NOT  /* Zneplatním makro, aby jej nebylo možné použít v dalších funkcích. */
}

int main() {
    enum bases out = dec;
    char input[MAX_INPUT_LENGTH];
    mpt  *result;

    printf("Enter \"quit\" to exit this amazing calculator.\n\n");

    for (;;) {
        printf("> ");
        fgets(input, MAX_INPUT_LENGTH, stdin);
        input[strcspn(input, "\r\n")] = '\000';

        if (strcmp(input, "quit") == 0) {
            break;
        }
        if (strcmp(input, "bin") == 0) {
            out = bin;
            continue;
        }
        if (strcmp(input, "dec") == 0) {
            out = dec;
            continue;
        }
        if (strcmp(input, "hex") == 0) {
            out = hex;
            continue;
        }
        if (strcmp(input, "out") == 0) {
            switch (out) {
                case bin: printf("bin\n"); break;
                case dec: printf("dec\n"); break;
                case hex: printf("hex\n"); break;
                default:  break;
            }
            continue;
        }

        if (evalutate_rpn_expression_3(input, &result)) {
            if (!result) {
                printf("Math error");
            }
            mpt_print(result, out);
            printf("\n");
        }
        else printf("syntax error\n");
    }

    printf("You are leaving an awesome calculator. Be back soon!\n");
    return EXIT_SUCCESS;
}