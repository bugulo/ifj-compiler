/* IFJ20 - Parser
 * Authors:
 * Erik Belko, xbelko02
 * Michal Slesar, xslesa01
 */

#include "parser.h"

/*
- MAJO STACK
- EOL V PROLOGU
*/

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
    while(true) {\
        RETRIEVE_TOKEN();\
        if(token.type != TOKEN_EOL) { PUSH(); break; }\
    }

/*  1: <program> -> package id EOL <body> EOF   */
void ruleProgram() {
    htab_t *table = htab_init(10);

    OPTIONAL_EOL();
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);

    if(token.value.k != KEYWORD_PACKAGE)
        THROW_ERROR_SYNTAX();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);
    if(!string_compare(&token.value.s, "main"))
        throw_error_fatal(DEFINITION_ERROR, "%s", "Wrong package name");

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_EOL);

    ruleBody(table);

    OPTIONAL_EOL();
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_EOF);

    if(!isFuncDefined(table, "main"))
        throw_error_fatal(DEFINITION_ERROR, "%s", "Missing function main");
    else {
        Vector *empty = vectorInit();
        Vector *params = getFuncParamTypes(table, "main");
        Vector *returns = getFuncReturnTypes(table, "main");

        checkFuncTypes(params, empty);
        checkFuncTypes(returns, empty);
    }
}

/*  2: <body> -> <func> <func_n>    */
void ruleBody(htab_t *table) {
    ruleFunc(table);
    ruleFuncN(table);
}

/*  3: <func_n> -> EOL <func> <func_n>
    4: <func_n> -> eps  */
void ruleFuncN(htab_t *table) {
    GET_TOKEN_AND_COMPARE_RETURN(TOKEN_EOL)

    GET_TOKEN_AND_COMPARE_RETURN(TOKEN_KEYWORD)

    if(token.value.k != KEYWORD_FUNC)
        THROW_ERROR_SYNTAX();

    PUSH();
    ruleFunc(table);
    ruleFuncN(table);
}

/*  5: <func> -> func id ( <params> ) <ret_types> { EOL <st_list> } */
void ruleFunc(htab_t *table) {
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);

    if(token.value.k != KEYWORD_FUNC)
        THROW_ERROR_SYNTAX();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);

    Vector *params = vectorInit();
    Vector *returns = vectorInit();
    defineFunc(table, token.value.s.ptr, params, returns);

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_LEFT);

    ruleParams(params);

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_RIGHT);

    ruleRetTypes(returns);

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_LEFT);
    
    OPTIONAL_EOL();

    ruleStList();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_RIGHT);
}

/*  6: <params> -> id <type> <params_n>
    7: <params> -> eps  */
void ruleParams(Vector *params) {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_IDENTIFIER);

    ruleType(params);
    ruleParamsN(params);
}

/*  8: <params_n> -> , id <type> <params_n>
    9: <params_n> -> eps */
void ruleParamsN(Vector *params) {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_COMA);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);

    ruleType(params);
    ruleParamsN(params);
}

/*  10: <type> -> int
    11: <type> -> float64
    12: <type> -> string */
void ruleType(Vector *types) {
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);

    if ((token.value.k == KEYWORD_INT) || (token.value.k == KEYWORD_FLOAT64) || (token.value.k == KEYWORD_STRING)) {
        varDataType type;

        if(token.value.k == KEYWORD_INT) { type = INTEGER; }
        else if(token.value.k == KEYWORD_FLOAT64) { type = FLOAT; }
        else if(token.value.k == KEYWORD_STRING) { type = STRING; }

        addFuncType(types, type);
        return;
    } else {
        THROW_ERROR_SYNTAX();
    }
}

/*  13: <ret_types> -> ( <type> <ret_types_n> )
    14: <ret_types> -> eps */
void ruleRetTypes(Vector *returns) {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_BRACKET_LEFT);

    ruleType(returns);
    ruleRetTypesN(returns);

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_RIGHT);
}

/*  15: <ret_types_n> -> , <type> <ret_types_n>
    16: <ret_types_n> -> eps */
void ruleRetTypesN(Vector *returns) {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_COMA);

    ruleType(returns);
    ruleRetTypesN(returns);
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
        ruleExp(false, false);
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
        ruleForDef();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_SEMICOLON);
        ruleExp(false, false);
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_SEMICOLON);
        ruleForAssign();
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

/*  20: <stat_body> -> <id_n> = <expression> <expression_n>
    21: <stat_body> -> := <expression>
    22: <stat_body> -> ( <call_params> ) */
void ruleStatBody() {
    RETRIEVE_TOKEN();

    if(token.type == TOKEN_DECLARATION) {
        ruleExp(false, false);
    } else if(token.type == TOKEN_BRACKET_LEFT) {
        ruleCallParams();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_RIGHT);
    } else {
        PUSH();
        ruleIdN();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_ASSIGNMENT);

        ruleExp(false, true);
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
    ruleExp(false, false);
    ruleExpN();
}

/*  37: <return_exp> -> <expression> <expression_n>
    38: <return_exp> -> eps */
void ruleReturnExp() {
    ruleExp(true, false);
    ruleExpN();
}

/*  23: <call_params> -> <values> <call_params_n>
        <call_params> -> eps    */

void ruleCallParams() {
    if(ruleValues()) {
        ruleCallParamsN();
    } else {
        PUSH();
    }
}

/*  <call_params_n> -> , <values> <call_params_n>
    <call_params_n> -> eps  */
void ruleCallParamsN() {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_COMA);

    if(ruleValues()) {
        ruleCallParamsN();
    } else {
        PUSH();
    }
}

/*  <values> -> value_int
    <values> -> value_float
    <values> -> value_string
    <values> -> id  */
bool ruleValues() {
    RETRIEVE_TOKEN();

    if(token.type != TOKEN_IDENTIFIER && token.type != TOKEN_NUMBER_FLOAT && token.type != TOKEN_NUMBER_INT && token.type != TOKEN_STRING)
        return false;
    else
        return true;
}

/*  32: <for_def> -> id := <expression>
    33: <for_def> -> eps    */
void ruleForDef() {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_IDENTIFIER);

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_DECLARATION);
    ruleExp(false, false);
}

/*  34: <for_assign> -> id = <expression>
    35: <for_assign> -> eps */
void ruleForAssign() {
    GET_TOKEN_AND_COMPARE_RETURN_PUSH(TOKEN_IDENTIFIER);

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_ASSIGNMENT);
    ruleExp(false, false);
}

/*  39: <expression> -> expression */
void ruleExp(bool allowEmpty, bool allowFunc) {
    Vector *vector = vectorInit();
    htab_t *table = htab_init(1);
    //defineFunc(table, "test", NULL, NULL);
    vectorPush(vector, (void*) table);

    expResult result = expression(vector, table);

    if(!allowEmpty && result.isEmpty)
        THROW_ERROR_SYNTAX();

    if(!allowFunc && result.isFunc)
        THROW_ERROR_SYNTAX();

    stackPush(stack, result.newToken);

    if(result.isFunc) {
        /*GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_LEFT);
        ruleCallParams();
        GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_RIGHT);*/

        ruleStat();
    }
}

void parser_main() {
    stack = stackInit();

    ruleProgram();
    printf("Syntax analysis good\n");
}