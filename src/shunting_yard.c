#include "shunting_yard.h"
#include "data_structures/conversion.h"

/** Deklarace funkce shunt_char_. Je nutná, protože funkce shunt_char_ může zavolat funkci shunt_minus_, 
 * která může opět zavolat shunt_char_ (při ošetřování speciálního případu n^-...). */

static int shunt_char_(const char **str, char *last_operator, vector_type *rpn_str, stack_type *operator_stack, vector_type *values_vector);

/**
 * \brief Obalovací funkce pro funkci pro uvolnění instance mpt z paměti.
 * \param poor Ukazatel na ukazatel na instanci mpt.
 */
static void mpt_deallocate_wrapper_(void *poor) {
    mpt_deallocate(poor);
}

/** 
 * \brief Zjistí, jestli je znak ukončující, tedy nulový nebo '\n'
 * \param c Znak.
 * \return int 1 jestli je znak ukončující, jinak 0.
 */
static int is_end_char_(const char c) {
    return c == 0 || c == '\n';
}

/** 
 * \brief Zjistí, kolik je ve výrazu matematických operátorů.
 * \param str Řetězec s matematickým výrazem v infixové formě.
 * \return size_t počet operátorů ve výrazu, 1 pokud výraz nemá žádný operátor.
 */
static size_t get_operator_count_(const char *str) {
    size_t i = 0;
    for (; !is_end_char_(*str); ++str) {
        if (get_func_operator(*str) || *str == '(') {
            ++i;
        }
    }
    return i > 0 ? i : 1;
}

/** 
 * \brief Přidá symbol RPN_VALUE_SYMBOL do vektoru rpn_str a také přidá ukazatel na instanci mpt do vektoru values_vector.
 * \param mpv Ukazatel na instanci mpt.
 * \param rpn_str Ukazatel na vektor, který obsahuje řetězec s RPN výrazem.
 * \param values_vector Ukazatel na vektor s ukazateli na instance mpt.
 * \return int 1 pokud se prvky přidaly, 0 pokud ne.
 */
static int push_parsed_value_(const mpt *mpv, vector_type *rpn_str, vector_type *values_vector) {
    char value_symbol = RPN_VALUE_SYMBOL;
    return mpv && 
        vector_push_back(rpn_str, &value_symbol) && 
        vector_push_back(values_vector, &mpv);
}

/** 
 * \brief Přidá matematický operátoru podle shunting yard algoritmu buď na zásobník operátorů, nebo rovnou do vektoru s RPN výrazem.
 * \param operator Znak operátoru.
 * \param rpn_str Ukazatel na vektor s RPN výrazem.
 * \param operator_stack Ukazatel na zásobník operátorů.
 * \return int 1 pokud se podařilo prvek přidat, 0 pokud ne.
 */
static int push_operator_(const char operator, vector_type *rpn_str, stack_type *operator_stack) {
    char c = 0;
    const func_oper_type *o1, *o2;
    o1 = o2 = NULL;

    #define EXIT_IF_NOT(v) \
        if (!(v)) { \
            return 0; \
        }

    switch (operator) {
        case '!': return vector_push_back(rpn_str, &operator);
        case '(': return stack_push(operator_stack, &operator);
        case ')': 
            while (!stack_isempty(operator_stack)) {
                EXIT_IF_NOT(stack_pop(operator_stack, &c));
                if (c == '(') {
                    return 1;
                }
                EXIT_IF_NOT(vector_push_back(rpn_str, &c));
            }
            return 0;
        default: break;
    }

    o1 = get_func_operator(operator);
    EXIT_IF_NOT(o1)

    while (!stack_isempty(operator_stack)) {
        EXIT_IF_NOT(stack_head(operator_stack, &c));
        if (c == '(') {
            return stack_push(operator_stack, &operator);
        }

        o2 = get_func_operator(c);

        if ((o2->precedence == o1->precedence && o1->assoc == right) ||
            (o1->precedence > o2->precedence)) {
            return stack_push(operator_stack, &operator);
        }

        EXIT_IF_NOT(stack_pop(operator_stack, &c) && vector_push_back(rpn_str, &c));
    }

    return stack_push(operator_stack, &operator);

    #undef EXIT_IF_NOT
}

/** 
 * \brief Převede řetězec na instanci mpt a přidá ji do RPN výrazu funkcí push_parsed_value_.
 * \param str Ukazatel na řetězec. Při provádění funkce se mění ukazatel na znak v řetězci a po úspěšném provedení funkce bude
 *            ukazovat na poslední znak naparsované hodnoty. 
 * \param last_operator Ukazatel na znak posledního operátoru.
 * \param rpn_str Ukazatel na vektor, který obsahuje řetězec s RPN výrazem.
 * \param values_vector Ukazatel na vektor s ukazateli na instance mpt.
 * \return int 1 pokud se hodnota úspěšně převedla a přidala, 0 pokud ne.
 */
static int shunt_value_(const char **str, char *last_operator, vector_type *rpn_str, vector_type *values_vector) {
    mpt *parsed_value;

    if (!last_operator) {
        return 0;
    }

    parsed_value = mpt_parse_str(str);

    if (!push_parsed_value_(parsed_value, rpn_str, values_vector)) {
        mpt_deallocate(&parsed_value);
        return 0;
    }

    *last_operator = RPN_VALUE_SYMBOL;
    --*str;

    return 1;
}

/** 
 * \brief Zjistí, jestli může operátor c následovat za operátorem last_operator, podle infixové formy.
 * \param c Znak matematického operátoru, který má následovat za operátorem last_operator.
 * \param last_operator Znak matematického operátoru, který předchází operátoru last_operator.
 * \return int 1 pokud je syntax v pořádku, 0 pokud ne.
 */
static int infix_syntax_ok_(const char c, const char last_operator) {
    const func_oper_type *c_func, *last_func; 
    c_func = get_func_operator(c);
    last_func = get_func_operator(last_operator);

    if ((c == '(') || 
        (c == RPN_VALUE_SYMBOL) || 
        (c_func && c_func->un_handler && c_func->assoc == right)) {
        if (!last_func) {
            return last_operator == 0 || last_operator == '(';
        }
        return (last_func->un_handler && last_func->assoc == right) || last_func->bi_handler;
    }

    if ((c == ')') || 
        (c_func && c_func->bi_handler) ||
        (c_func && c_func->un_handler && c_func->assoc == left)) {
        if (!last_func) {
            return last_operator == RPN_VALUE_SYMBOL || last_operator == ')';
        }
        return last_func->un_handler && last_func->assoc == left;
    }

    return 0;
}

/** 
 * \brief Najde uzavírací závorku, která uzavírá matematický výraz.
 * \param str Řetězec s matematickým výrazem. První znak by měl být hned za otevírací závorkou.
 * \return char* Ukazatel na znak nalezené uzavírací závorky v řetězci, NULL pokud závorka neexistuje.
 */
static char *find_closing_bracket_(const char *str) {
    size_t l_brackets = 1, r_brackets = 0;
    for (; !is_end_char_(*str); ++str) {
        switch (*str) {
            case '(': ++l_brackets; break;
            case ')': ++r_brackets; break;
            default:  break;
        }

        if (l_brackets == r_brackets) {
            return (char *)str;
        }
    }

    return NULL;
}

/** 
 * \brief Zjistí, jestli je znak **str unární nebo binární mínus a provede nad ním shunting yard algoritmus.
 *        Ošetřuje speciální případy, kdy se unární mínus špatně vyhodnotí (např. n^-n, n^-(n+n), ...).
 * \param str Ukazatel na řetězec s matematickým výrazem. První znak by měl být znak mínusu.
 *            Při provádění funkce se mění ukazatel na znak v řetězci.
 * \param last_operator Ukazatel na znak posledního operátoru.
 * \param rpn_str Ukazatel na vektor, který obsahuje řetězec s RPN výrazem.
 * \param operator_stack Ukazatel na zásobník operátorů.
 * \param values_vector Ukazatel na vektor s ukazateli na instance mpt.
 * \return int s hodnotou některého z maker pro úspěšnost výsledku.
 */
static int shunt_minus_(const char **str, char *last_operator, vector_type *rpn_str, stack_type *operator_stack, vector_type *values_vector) {
    int res;
    const func_oper_type *last_func; 
    char minus = 0, *closing_bracket = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            return e; \
        }

    EXIT_IF(**str != '-' || !last_operator || !rpn_str || !operator_stack || !values_vector, ERROR);

    last_func = get_func_operator(*last_operator);

    if ((*last_operator == ')') || 
        (*last_operator == RPN_VALUE_SYMBOL) ||
        (last_func && last_func->un_handler && last_func->assoc == left)) {
        minus = '-';
    }
    else {
        minus = RPN_UNARY_MINUS_SYMBOL;
    }

    EXIT_IF(!infix_syntax_ok_(minus, *last_operator), SYNTAX_ERROR);
    EXIT_IF(minus == RPN_UNARY_MINUS_SYMBOL && *(*str + 1) == ' ', SYNTAX_ERROR);

    if (minus == '-' || *last_operator != '^') {
        *last_operator = minus;
        EXIT_IF(!push_operator_(minus, rpn_str, operator_stack), ERROR);
        return SYNTAX_OK;
    }

    *last_operator = minus;
    ++*str;

    if (**str != '(') {
        EXIT_IF(!shunt_value_(str, last_operator, rpn_str, values_vector), ERROR);
        ++*str;
    }
    else {
        EXIT_IF(!(closing_bracket = find_closing_bracket_(*str + 1)), SYNTAX_ERROR);

        for (; !is_end_char_(**str) && *str <= closing_bracket; ++*str) {
            EXIT_IF((res = shunt_char_(str, last_operator, rpn_str, operator_stack, values_vector)) != SYNTAX_OK, res);
        }
    }

    for (; !is_end_char_(**str) && **str == '!'; ++*str) {
        EXIT_IF((res = shunt_char_(str, last_operator, rpn_str, operator_stack, values_vector)) != SYNTAX_OK, res);
    }
    --*str;

    EXIT_IF(!vector_push_back(rpn_str, &minus), ERROR);

    return SYNTAX_OK;

    #undef EXIT_IF
}

/** 
 * \brief Provede shunting yard algoritmus nad znakem **str.
 * \param str Ukazatel na řetězec s matematickým výrazem. První znak by měl být znak, který chceme vyhodnotit shunting yardem.
 * \param last_operator Ukazatel na znak posledního operátoru.
 * \param rpn_str Ukazatel na vektor, který obsahuje řetězec s RPN výrazem.
 * \param operator_stack Ukazatel na zásobník operátorů.
 * \param values_vector Ukazatel na vektor s ukazateli na instance mpt.
 * \return int s hodnotou některého z maker pro úspěšnost výsledku.
 */
static int shunt_char_(const char **str, char *last_operator, vector_type *rpn_str, stack_type *operator_stack, vector_type *values_vector) {
    #define EXIT_IF(v, e) \
        if (v) { \
            return e; \
        }

    EXIT_IF(!str || !last_operator || !rpn_str || !operator_stack || !values_vector, ERROR);

    EXIT_IF(**str == ' ', SYNTAX_OK);
    EXIT_IF(**str == RPN_UNARY_MINUS_SYMBOL, INVALID_SYMBOL);
    
    if (**str == '-') {
        return shunt_minus_(str, last_operator, rpn_str, operator_stack, values_vector);
    }
    else if (**str >= '0' && **str <= '9') {
        EXIT_IF(!infix_syntax_ok_(RPN_VALUE_SYMBOL, *last_operator), SYNTAX_ERROR);
        EXIT_IF(!shunt_value_(str, last_operator, rpn_str, values_vector), SYNTAX_ERROR);
    }
    else if (get_func_operator(**str) || **str == '(' || **str == ')') {
        EXIT_IF(!infix_syntax_ok_(**str, *last_operator), SYNTAX_ERROR);
        EXIT_IF(!push_operator_(**str, rpn_str, operator_stack), ERROR);
        *last_operator = **str;
    }
    else {
        return INVALID_SYMBOL;
    }

    return SYNTAX_OK;

    #undef EXIT_IF
}

/** 
 * \brief Zjistí, o jaký error se jedná při neúspěšné matematické operaci s jedním operandem.
 * \param operator Znak operátoru matematické operace s jedním operandem.
 * \param a Ukazatel na instanci operandu matematické operace.
 * \return int s hodnotou některého z maker pro matematický error.
 */
static int get_math_error_un_func_(const char operator, const mpt *a) {
    if (!a) {
        return ERROR;
    }
    if (operator == '!' && mpt_is_negative(a)) {
        return FACTORIAL_OF_NEGATIVE;
    }
    return MATH_ERROR;
}

/** 
 * \brief Zjistí, o jaký error se jedná při neúspěšné matematické operaci se dvěma operandy.
 *        Funkce by měla obsahovat ještě parametr 'const mpt *a', nicméně by nebyl používán,
 *        takže je vynechán, aby překladač nehlásil varování.
 * \param operator Znak operátoru matematické operace se dvěma operandy.
 * \param b Ukazatel na instanci operandu matematické operace.
 * \return int s hodnotou některého z maker pro matematický error.
 */
static int get_math_error_bi_func_(const char operator, const mpt *b) {
    if ((operator == '/' || operator == '%') && mpt_is_zero(b)) {
        return DIV_BY_ZERO;
    }
    return MATH_ERROR;
}

/**
 * @brief Provede příslušnou operaci nad znakem RPN výrazu.
 * @param c Znak RPN výrazu.
 * @param rpn_values Ukazatel na zásobník, který obsahuje ukazatele na instance mpt s hodnotami v RPN výrazu.
 * @param values_stack Ukazatel na zásobník, který obsahuje ukazatele na instance mpt s hodnotami pro vyhodnocování RPN výrazu.
 *                     V průběhu vyhodnocování bude funkce v zásobníku hodnoty odstraňovat a přidávat.
 * @return int s hodnotou některého z maker pro úspěšnost výsledku.
 */
static int evaluate_rpn_char_(const char c, stack_type *rpn_values, stack_type *values_stack) {
    int res = RESULT_OK;
    const func_oper_type *function = NULL;
    mpt *a, *b, *result;
    a = b = result = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!rpn_values || !values_stack, ERROR);

    EXIT_IF(c == '(', SYNTAX_ERROR);

    if (c == RPN_VALUE_SYMBOL) {
        EXIT_IF(!stack_pop(rpn_values, &a) || !stack_push(values_stack, &a), ERROR);
        return RESULT_OK;
    }

    function = get_func_operator(c);
    EXIT_IF(!function, ERROR);

    if (function->bi_handler) {
        EXIT_IF(!stack_pop(values_stack, &b) || !stack_pop(values_stack, &a), SYNTAX_ERROR);
        EXIT_IF(!(result = function->bi_handler(a, b)), get_math_error_bi_func_(c, b));
    }
    else if (function->un_handler) {
        EXIT_IF(!stack_pop(values_stack, &a), SYNTAX_ERROR);
        EXIT_IF(!(result = function->un_handler(a)), get_math_error_un_func_(c, a));
    }
    else {
        return ERROR;
    }

  clean_and_exit:
    mpt_deallocate(&a);
    mpt_deallocate(&b);

    if (result && !stack_push(values_stack, &result)) {
        return ERROR;
    }
    return res;

    #undef EXIT_IF
}

int shunt(const char *str, vector_type **rpn_str, stack_type **values) {
    int res = SYNTAX_OK;
    char c, last_operator = 0;
    stack_type *operator_stack = NULL;
    vector_type *values_vector = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!str || is_end_char_(*str) || !rpn_str || !values, ERROR);

    operator_stack = stack_allocate(get_operator_count_(str), sizeof(char), NULL);
    values_vector = vector_allocate(sizeof(mpt *), mpt_deallocate_wrapper_);
    *rpn_str = vector_allocate(sizeof(char), NULL);

    EXIT_IF(!operator_stack || !*rpn_str || !values_vector, ERROR);

    for (; !is_end_char_(*str); ++str) {
        EXIT_IF((res = shunt_char_(&str, &last_operator, *rpn_str, operator_stack, values_vector)) != SYNTAX_OK, res);
    }

    while (stack_pop(operator_stack, &c)) {
        EXIT_IF(!vector_push_back(*rpn_str, &c), ERROR);
    }

    EXIT_IF(vector_isempty(values_vector), SYNTAX_ERROR);
    EXIT_IF(!(*values = vector_to_stack(&values_vector)), ERROR);

  clean_and_exit:
    vector_deallocate(&values_vector);
    stack_deallocate(&operator_stack);

    if (res != SYNTAX_OK) {
        vector_deallocate(rpn_str);
        stack_deallocate(values);
    }

    return res;

    #undef EXIT_IF
}

int evaluate_rpn(mpt **dest, const vector_type *rpn_str, stack_type *values) {
    int res = RESULT_OK;
    char *c;
    size_t i;
    stack_type *stack_values = NULL;

    #define EXIT_IF(v, e) \
        if (v) { \
            res = e; \
            goto clean_and_exit; \
        }

    EXIT_IF(!dest || !rpn_str || !values, ERROR);

    EXIT_IF(!(stack_values = stack_allocate(stack_item_count(values), values->item_size, mpt_deallocate_wrapper_)), ERROR);

    for (i = 0; i < vector_count(rpn_str); ++i) {
        EXIT_IF(!(c = (char *)vector_at(rpn_str, i)), ERROR);
        EXIT_IF((res = evaluate_rpn_char_(*c, values, stack_values)) != RESULT_OK, res);
    }

    EXIT_IF(stack_item_count(stack_values) != 1, SYNTAX_ERROR);
    EXIT_IF(!stack_pop(stack_values, dest), ERROR);
    
  clean_and_exit:
    stack_deallocate(&stack_values);
    return res;

    #undef EXIT_IF
}