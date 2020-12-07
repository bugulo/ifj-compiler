/* IFJ20 - Parser
 * Authors:
 * Erik Belko, xbelko02
 * Michal Slesar, xslesa01
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#include "file.h"
#include "stack.h"
#include "vector.h"
#include "string.h"
#include "scanner.h"
#include "symtable.h"
#include "expression.h"

typedef struct {
    dynamicArr *file;
    bool isFirstScan; // Is the parser in first scan
    htab_t *table; // Table with functions in it
    Vector *scopes; // Vector of scopes
    Vector *strings; // Vector of allocated strings
    Token token; // Current token
    Stack *stack; // Stack of tokens
    bool inFunction; // Is parser in function
    bool returned; // Did parser find return in function
    String function; // In what function parser is
} ParserData;

/**
 * Parse file
 */
void parse();

/**
 * Program rule
 * 
 * Beginning of the source file
 * 
 * 1: <program> -> package id EOL <body> EOF
 * 
 * @param data ParserData instance
 */
void ruleProgram(ParserData *data);

/**
 * Body rule
 * 
 * After prolog
 * 
 * 2: <body> -> <func_n>
 * 
 * @param data ParserData instance
 */
void ruleBody(ParserData *data);

/**
 * FuncN rule
 * 
 * Optional function
 * 
 * 3: <func_n> -> <func> <func_n>
 * 4: <func_n> -> eps
 * 
 * @param data ParserData instance
 */
void ruleFuncN(ParserData *data);

/**
 * Func rule
 * 
 * Function declaration
 * 
 * 5: <func> -> func id ( <params> ) <ret_types> { EOL <st_list> }
 * 
 * @param data ParserData instance
 */
void ruleFunc(ParserData *data);

/**
 * Params rule
 * 
 * Function parameters
 * 
 * 6: <params> -> id <type> <params_n>
 * 7: <params> -> eps
 * 
 * @param data ParserData instance
 * @param params Vector of param types
 * @param names Vector of param names
 */
void ruleParams(ParserData *data, Vector *params, Vector *names);

/**
 * ParamsN rule
 * 
 * Option function parameter
 * 
 * 8: <params_n> -> , id <type> <params_n>
 * 9: <params_n> -> eps
 * 
 * @param data ParserData instance
 * @param params Vector of param types
 * @param names Vector of param names
 */
void ruleParamsN(ParserData *data, Vector *params, Vector *names);

/**
 * Type rule
 * 
 * Function parameter/return type
 * 
 * 10: <type> -> int
 * 11: <type> -> float64
 * 12: <type> -> string
 * 
 * @param data ParserData instance
 * @param types Vector of param types
 * 
 * @return Type
 */
varDataType ruleType(ParserData *data, Vector *types);

/**
 * RetTypes rule
 * 
 * Function return types
 * 
 * 13: <ret_types> -> ( <type> <ret_types_n> )
 * 14: <ret_types> -> eps
 * 
 * @param data ParserData instance
 * @param returns Vector of return types
 */
void ruleRetTypes(ParserData *data, Vector *returns);

/**
 * RetTypesN rule
 * 
 * Optional function return type
 * 
 * 15: <ret_types_n> -> , <type> <ret_types_n>
 * 16: <ret_types_n> -> eps
 * 
 * @param data ParserData instance
 * @param returns Vector of return types
 */
void ruleRetTypesN(ParserData *data, Vector *returns);

/**
 * StList rule
 * 
 * Function body
 * 
 * 17: <st_list> -> <stat> EOL <st_list>
 * 18: <st_list> -> eps
 * 
 * @param data ParserData instance
 */
void ruleStList(ParserData *data);

/**
 * Stat rule
 * 
 * Command in function body
 * 
 * 19: <stat> -> id <stat_body>
 * 35: <stat> -> if <expression> { <st_list> } else { <st_list> }
 * 36: <stat> -> for <for_def> ; <expression> ; <for_assign> { <st_list> }
 * 41: <stat> -> return <return_exp>
 * 
 * @param data ParserData instance
 */
bool ruleStat(ParserData *data);

/**
 * StatBody rule
 * 
 * 20: <stat_body> -> <id_n> = <expression> <expression_n>
 * 21: <stat_body> -> := <expression>
 * 22: <stat_body> -> ( <call_params> )
 * 
 * @param data ParserData instance
 * @param id First token of stat
 */
void ruleStatBody(ParserData *data, Token id);

/**
 * IdN rule
 * 
 * Optional identifier
 * 
 * 31: <id_n> -> , id <id_n>
 * 32: <id_n> -> eps
 * 
 * @param data ParserData instance
 * @param data Vector of identifier names
 */
void ruleIdN(ParserData *data, Vector *names);

/**
 * ExpN rule
 * 
 * Optional expression
 * 
 * 33: <expression_n> -> , <expression> <expression_n>
 * 34: <expression_n> -> eps
 * 
 * @param data ParserData instance
 * @param names Vector of expression names
 * @param types Vector of expression types
 */
void ruleExpN(ParserData *data, Vector *names, Vector *types);

/**
 * ReturnExp rule
 * 
 * Function return expressions
 * 
 * 42: <return_exp> -> <expression> <expression_n>
 * 43: <return_exp> -> eps
 * 
 * @param data ParserData instance
 */
void ruleReturnExp(ParserData *data);

/**
 * CallParams rule
 * 
 * Parameters in function call
 * 
 * 23: <call_params> -> <values> <call_params_n>
 * 24: <call_params> -> eps
 * 
 * @param data ParserData instance
 * @param names Vector of param names
 * @param types Vector of param types
 */
void ruleCallParams(ParserData *data, Vector *names, Vector *types);

/**
 * CallParamsN rule
 * 
 * Optional parameter in function call
 * 
 * 25: <call_params_n> -> , <values> <call_params_n>
 * 26: <call_params_n> -> eps
 * 
 * @param data ParserData instance
 * @param names Vector of param names
 * @param types Vector of param types
 */
void ruleCallParamsN(ParserData *data, Vector *names, Vector *types);

/**
 * Values rule
 * 
 * Value of function call parameter
 * 
 * 27: <values> -> value_int
 * 28: <values> -> value_float64
 * 29: <values> -> value_string
 * 30: <values> -> id
 * 
 * @param data ParserData instance
 * @param names Vector of param names
 * @param types Vector of param types
 */
bool ruleValues(ParserData *data, Vector *names, Vector *types);

/**
 * ForDef rule
 * 
 * Definition part of for
 * 
 * 37: <for_def> -> id := <expression>
 * 38: <for_def> -> eps
 * 
 * @param data ParserData instance
 */
void ruleForDef(ParserData *data);

/**
 * ForAssign rule
 * 
 * Assign part of for
 * 
 * 39: <for_assign> -> id <id_n> = <expression> <expression_n>
 * 40: <for_assign> -> eps
 * 
 * @param data ParserData instance
 */
void ruleForAssign(ParserData *data);

/**
 * Exp rule
 * 
 * Expression
 * 
 * 44: <expression> -> expression
 * 
 * @param data ParserData instance
 * @param allowEmpty Are empty expressions allowed
 * @param allowFunc Are functions allowed
 */
expResult ruleExp(ParserData *data, bool allowEmpty, bool allowFunc);

#endif