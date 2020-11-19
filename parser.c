/* IFJ20 - Parser
 * Authors:
 * Erik Belko, xbelko02
 */

#include "parser.h"
#include "vector.h"
#include "stack.h"

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

/*  1: <program> -> package id EOL <body> EOF   */
void ruleProgram() {
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);

    if(token.value.k != KEYWORD_PACKAGE)
        THROW_ERROR_SYNTAX();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_EOL);

    ruleBody();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_EOF);

    /*Token actual_token = getToken();
    if ((actual_token.type == TOKEN_KEYWORD) && (actual_token.value.k == KEYWORD_PACKAGE)) {
        actual_token = getToken();
        if ((actual_token.type == TOKEN_IDENTIFIER)) {
            actual_token = getToken();
            if ((actual_token.type == TOKEN_EOL)) {
                if (ruleBody()) {
                    actual_token = getToken();
                    if (actual_token.type == TOKEN_EOF) {
                        return true;
                    }
                }
            }
        }
    }
    return false;*/
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


    /*Token actual_token = getToken();
    if(actual_token.type == TOKEN_EOL) {
        actual_token = getToken();
        if ((actual_token.type == TOKEN_KEYWORD) && (actual_token.value.k == KEYWORD_FUNC)) {
            stackPush(stack, actual_token);

            if(ruleFunc()) {
                if(ruleFuncN()) 
                    return true;
                else
                    return false;
            } else
                return false;
        } else
            return true;
    } else
        return true;*/
}

/*  5: <func> -> func id ( <params> ) <ret_types> { <st_list> } */
void ruleFunc() {
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_KEYWORD);

    if(token.value.k != KEYWORD_FUNC)
        THROW_ERROR_SYNTAX();

    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_IDENTIFIER);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_LEFT);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACKET_RIGHT);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_LEFT);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_EOL);
    GET_TOKEN_AND_COMPARE_ERROR(TOKEN_BRACE_RIGHT);

    /*Token actual_token;

    if(!stackIsEmpty(stack))
        actual_token = stackPop(stack);
    else
        actual_token = getToken();

    if ((actual_token.type == TOKEN_KEYWORD) && (actual_token.value.k == KEYWORD_FUNC)) {
        actual_token = getToken();
        if (actual_token.type == TOKEN_IDENTIFIER) {
            actual_token = getToken();
            if (actual_token.type == TOKEN_BRACKET_LEFT) {
                actual_token = getToken();
                if (actual_token.type == TOKEN_BRACKET_RIGHT) {
                    actual_token = getToken();
                    if (actual_token.type == TOKEN_BRACE_LEFT) {
                        actual_token = getToken();
                        if (actual_token.type == TOKEN_EOL) {
                            actual_token = getToken();
                            if (actual_token.type == TOKEN_BRACE_RIGHT) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;*/
}

/*  6: <params> -> id <type> <params_n>
    7: <params> -> eps  */
//void ruleParams() 

/*  8: <params_n> -> , id <type> <params_n>
    9: <params_n> -> eps */
//void ruleParamsN()

/*  10: <type> -> int
    11: <type> -> float64
    12: <type> -> string
    13: <type> -> eps */
//void ruleType()

/*  14: <ret_types> -> ( <type> <ret_types_n> )
    15: <ret_types> -> eps */
//void ruleRetTypes()

/*  16: <ret_types_n> -> , <type> <ret_types_n>
    17: <ret_types_n> -> eps */
//void ruleRetTypesN()

/*  18: <st_list> -> <stat> EOL <st_list>
    19: <st_list> -> eps */
//void ruleStList()

/*  20: <stat> -> id <stat_body>
    30: <stat> -> if <expression> { <st_list> } else { <st_list> }
    31: <stat> -> for <stat> ; <expression> ; <stat> { <st_list> }
    32: <stat> -> <return> */
//void ruleStat()

/*  21: <stat_body> -> <id_n> = <assign>
    24: <stat_body> -> := <expression>
    25: <stat_body> -> ( <params> ) */
//void ruleStatBody()

/*  22: <assign> -> <expression> <expression_n>
    23: <assign> -> id ( <params> ) */
//void ruleAssign()

/*  26: <id_n> -> , id <id_n>
    27: <id_n> -> eps */
//void ruleIdN()

/*  28: <expression_n> -> , <expression> <expression_n>
    29: <expression_n> -> eps */
//void ruleExpressionN()

/*  33: <return> -> return <return_exp>
    34: <return> -> eps */
//void ruleReturn()

/*  35: <return_exp> -> <expression> <expression_n>
    36: <return_exp> -> eps */
//void ruleReturnExp()

/*  37: <expression> -> expression */
//void ruleExpression()

void parser_main() {
    stack = stackInit();

    ruleProgram();
    printf("Syntax analysis good\n");
}