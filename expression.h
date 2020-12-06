/* IFJ20 - Precedence analysis
 * Authors:
 * Mario Harvan, xharva03
 * Juraj Marticek, xmarti97
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "stack.h"
#include "symtable.h"
#include "scanner.h"
#include "vector.h"

#define REDUCE_ERROR -1
#define ANALYSIS_DONE -2
#define ANALYSIS_OK 0

typedef enum{
    PRECEDENCE_SHIFT = '<',
    PRECEDENCE_EQUAL = '=',
    PRECEDENCE_REDUCE = '>',
    PRECEDENCE_NONE,
} precedenceSigns;

typedef struct{
    Token newToken; //newly read token from scanner
    String result;   //String with name of tmp var where intermediate result is stored
    bool isFunc;    //if true, expression was function, check always!
    bool isEmpty;   //true when expression is empty
} expResult;

typedef unsigned char precedenceTableIndex;

precedenceTableIndex getPrecedenceTableIndex(Token token);

precedenceSigns precedenceTable(Token stackToken, Token inputToken);


/**
 * @brief Function will check if token is one of allowed token ending tokens for expression
 * @param Token
 * @return True if token is valid ending token, else false
*/
bool isEndToken(Token token);

/**
 * @brief Function counts how many tokens there are on stack before delimiter
 * @param Pointer to stack
 * @returns Number of tokens before delimiter, or REDUCE_ERROR when there is syntax err
*/
int reduceTokenCount(Stack *stack);

/**
 * @brief Function checks all semantic rules for 3 operand operations, and generate temporary variable to store intermediate result
 * @param Operand 1 token
 * @param Operation token
 * @param Operand 2 token
 * @param Pointer to symbol tables vector
 * @return Returns token with name of newly generated temporary variable
*/
Token semanticCheckFor3Op(Token operand1, Token operation, Token operand2, Vector *symtableVector);

/**
 * @brief Function frees token data, and removes temporary variable created to store intermediate result
 * @param Token storing name of variable
 * @param Pointer to symbol tables vector
*/
void removeTokenAfterOp(Token token, Vector *symtableVector);

/**
 * @brief Reduction rules for 1 operand operations
 * @param Pointer to stack with tokens
 * @param First operand of operation
 * @param Pointer to symbol tables vector
*/
void reduceRules1Op(Stack *stack, Token operand1, Vector *symtableVector);

/**
 * @brief Reduction rules for 3 operand operations
 * @param Pointer to stack with tokens
 * @param First operand of operation
 * @param Operation token
 * @param Second operand of operation
 * @param Pointer to symbol tables vector
*/
void reduceRules3Op(Stack *stack, Token operand1, Token operation, Token operand2, Vector *symtableVector);

/**
 * @brief Function will reduce current content of stack, if there is valid reduction rule
 * @param Pointer to stack with tokens
 * @param Pointer to symbol tables vector
*/
void reduceByRule(Stack *stack, Vector *symtableVector);

/**
 * @brief Validate syntax and semantics of expression and generate code for it
 * @param Pointer to symbol tables vector
 * @return Returns result structure, which consists of newly read Token, token with result of expression, and special bool if expression was function call.
*/
expResult expression(Vector *symtableVector, htab_t *funcTable);


#endif