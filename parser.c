/* IFJ20 - Parser
 * Authors:
 * Erik Belko, xbelko02
 * Michal Slesar, xslesa01
 */

#include "parser.h"
#include "vector.h"
#include "stack.h"
#include "symtable.h"
#include "expression.h"

Token token;
Stack *stack;

#define THROW_ERROR_SYNTAX()\
    throw_error_fatal(SYNTAX_ERROR, "%s", "Syntax analysis error!");

#define PUSH()\
    stackPush(stack, token);

#define RETRIEVE_TOKEN()\
    if(stackIsEmpty(stack)) scanner_get_token(&token); else token = stackPop(stack);

#define GET_TOKEN_AND_COMPARE_ERROR(token_type)\
    RETRIEVE_TOKEN();\
    if(token.type != token_type) THROW_ERROR_SYNTAX();

#define GET_TOKEN_AND_COMPARE_RETURN(token_type)\
    RETRIEVE_TOKEN();\
    if(token.type != token_type) return;

#define GET_TOKEN_AND_COMPARE_RETURN_PUSH(token_type)\
    RETRIEVE_TOKEN();\
    if(token.type != token_type) { PUSH(); return; }

#define OPTIONAL_EOL()\
    RETRIEVE_TOKEN();\
    if(token.type != TOKEN_EOL) PUSH();

/*  1: <program> -> package id EOL <body> EOF   */
void ruleProgram() {
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);

    if(token.value.k != KEYWORD_PACKAGE)
        THROW_ERROR_SYNTAX();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_EOL);

    ruleBody();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_EOF);
}

/*  2: <body> -> <func> <func_n>    */
void ruleBody() {
    ruleFunc();
    ruleFuncN();
}

/*  3: <func_n> -> EOL <func> <func_n>
    4: <func_n> -> eps  */
void ruleFuncN() {
    GET_TOKEN_AND_COMPARE_RETURN(TOKEN_EOL)

    GET_TOKEN_AND_COMPARE_RETURN(TOKEN_KEYWORD)

    if(token.value.k != KEYWORD_FUNC)
        THROW_ERROR_SYNTAX();

    PUSH();
    ruleFunc();
    ruleFuncN();
}

/*  5: <func> -> func id ( <params> ) <ret_types> { EOL <st_list> } */
void ruleFunc() {
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);

    if(token.value.k != KEYWORD_FUNC)
        THROW_ERROR_SYNTAX();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_LEFT);

    ruleParams();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_RIGHT);

    ruleRetTypes();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_LEFT);
    //GET_TOKEN_AND_COMPARE_ERROR(TOKEN_EOL);
    OPTIONAL_EOL();

    ruleStList();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_RIGHT);
}

/*  6: <params> -> id <type> <params_n>
    7: <params> -> eps  */
void ruleParams() {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_IDENTIFIER);

    ruleType(true);
    ruleParamsN();
}

/*  8: <params_n> -> , id <type> <params_n>
    9: <params_n> -> eps */
void ruleParamsN() {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_COMA);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);

    ruleType(true);
    ruleParamsN();
}

/*  10: <type> -> int
    11: <type> -> float64
    12: <type> -> string
    13: <type> -> eps */
void ruleType(bool param) {
    if(param) {
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);
    } else {
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);
    }

    if ((token.value.k == KEYWORD_INT) || (token.value.k == KEYWORD_FLOAT64) || (token.value.k == KEYWORD_STRING)) {
        return;
    } else {
        THROW_ERROR_SYNTAX();
    }
}

/*  13: <ret_types> -> ( <type> <ret_types_n> )
    14: <ret_types> -> eps */
void ruleRetTypes() {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_BRACKET_LEFT);

    ruleType(false);
    ruleRetTypesN();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_RIGHT);
}

/*  15: <ret_types_n> -> , <type> <ret_types_n>
    16: <ret_types_n> -> eps */
void ruleRetTypesN() {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_COMA);

    ruleType(false);
    ruleRetTypesN();
}

/*  17: <st_list> -> <stat> EOL <st_list>
    18: <st_list> -> eps */
void ruleStList() {
    if(ruleStat()) {
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_EOL);
        ruleStList();
    }
}

/*  19: <stat> -> id <stat_body>
    30: <stat> -> if <expression> { <st_list> } else { <st_list> }
    31: <stat> -> for <for_def> ; <expression> ; <for_assign> { <st_list> }
    36: <stat> -> return <return_exp> */
bool ruleStat() {
    RETRIEVE_TOKEN();

    if(token.type == TOKEN_IDENTIFIER) {
        ruleStatBody();
        return true;
    } else if(token.type == TOKEN_KEYWORD && token.value.k == KEYWORD_IF) {
        //ruleExp(); -- MAJO
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_LEFT);
        OPTIONAL_EOL();
        ruleStList();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_RIGHT);

        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);

        if(token.value.k != KEYWORD_ELSE)
            THROW_ERROR_SYNTAX();

        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_LEFT);
        OPTIONAL_EOL();
        ruleStList();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_RIGHT);
        return true;
    } else if(token.type == TOKEN_KEYWORD && token.value.k == KEYWORD_FOR) {
        //ruleStat();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_SEMICOLON);
        //ruleExp(); -- MAJO
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_SEMICOLON);
        //ruleStat();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_LEFT);
        OPTIONAL_EOL();
        ruleStList();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_RIGHT);
        return true;
    } else if(token.type == TOKEN_KEYWORD && token.value.k == KEYWORD_RETURN) {
        ruleReturnExp();
        return true;
    } else {
        PUSH();
        return false;
    }
}

/*  20: <stat_body> -> <id_n> = <expression> <expression_n> ----------------toto je tu nove <assign> pravidla sa zrusili
    21: <stat_body> -> := <expression>
    22: <stat_body> -> ( <call_params> ) --------------------- nove pravidla, vyhodit v type eps */
void ruleStatBody() {
    RETRIEVE_TOKEN();

    if(token.type == TOKEN_DECLARATION) {
        ruleExp();
    } else if(token.type == TOKEN_BRACKET_LEFT) {
        //NOVE PRAVIDLA
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_RIGHT);
    } else {
        PUSH();
        ruleIdN();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_ASSIGNMENT);
        
        //ruleAssign();

        ruleExp();
        ruleExpN();
    }
}

/*  26: <id_n> -> , id <id_n>
    27: <id_n> -> eps */
void ruleIdN() {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_COMA);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);
    ruleIdN();
}

/*  28: <expression_n> -> , <expression> <expression_n>
    29: <expression_n> -> eps */
void ruleExpN() {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_COMA);
    ruleExp();
    ruleExpN();
}

/*  37: <return_exp> -> <expression> <expression_n>
    38: <return_exp> -> eps */
void ruleReturnExp() {
    ruleExp();
    ruleExpN(); // MAJO
}

/*  23: <call_params> -> id <id_n>
    24: <call_params> -> <type> <ret_types_n>
    25: <call_params> -> eps    */
//void ruleCallParams()

/*  32: <for_def> -> id := <expression>
    33: <for_def> -> eps    */
//void ruleForDef()

/*  34: <for_assign> -> id := <expression>
    35: <for_assign> -> eps */
//void ruleForAssign()

/*  39: <expression> -> expression ----------nezabudnut posielat isFunc a to ci sa nejedna o prazdny return */
void ruleExp() {
    Vector *vector = vectorInit();
    htab_t *table = htab_init(1);
    vectorPush(vector, (void*) table);

    expResult result = expression(vector, table);
    stackPush(stack, result.newToken);
}

void parser_main() {
    stack = stackInit();

    ruleProgram();
    printf("Syntax analysis good\n");
}