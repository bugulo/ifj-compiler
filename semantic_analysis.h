/* IFJ20 - Semantic analysis
 * Authors:
 * Mario Harvan, xharva03
 */

#ifndef SEMANTIC_ANALYSIS_H_
#define SEMANTIC_ANALYSIS_H_

#include "symtable.h"
#include <stdbool.h>
#include "scanner.h"
#include "vector.h"
#include "error.h"

/**
 * @brief Function checks if variable is defined
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
 * @return True when variable is defined, false when its not
*/
bool isVarDefined(htab_t* symTable, htab_key_t name);

/**
 * @brief Function returns data type of variable
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
 * @return Data type of variable, or NONE when variable is not defined
*/
varDataType getVarType(htab_t* symTable, htab_key_t name);

/**
 * @brief Function sets data type of variable
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
 * @param Data type of variable
*/
void setVarType(htab_t* symTable, htab_key_t name, varDataType varDataType);

/**
 * @brief Checks if variable is set as constant
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
 * @return True when variable is constant, false when it's not
*/
bool isVarConst(htab_t* symTable, htab_key_t name);

/**
 * @brief Set if variable is constant or not
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
 * @param True if you want to set variable as constant, else if you don't
 */
void setVarConst(htab_t* symTable, htab_key_t name, bool isConst);

/**
 * @brief Check if both operands have the same data type
 * @param Pointer to symbol table vector
 * @param Token with name of first variable
 * @param Token with name of second variable
 * @return Data type of both variables, or none if they are not the same
*/
varDataType checkOperationTypes(Vector *tableVector, Token var1, Token var2);

/**
 * @brief Function generates name for temporary variable and stores it into symbol table
 * @param Pointer to table of symbols where variable should be defined
 * @param Datatype of generated variable
 * @param Value of generated variable
 * @param True when variable is constant
 * @return Return name of generated variable
*/
String defineCompilerVar(htab_t *symTable, varDataType varDataType, TokenValue varValue, bool isConst);


/**
 * @brief Function stores variable in symbol table
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
 * @param Data type of variable
 * @param Value of variable
 * @param True when variable is 
*/
void defineUserVar(htab_t *symTable, htab_key_t name, varDataType varDataType, TokenValue varValue, bool isConst);

/**
 * @brief Check if variable is defined by user
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
 * @return True when variable is user defined
*/
bool isVarUserDefined(htab_t *symTable, htab_key_t name);

/**
 * @brief Check if variable value is zero. If yes, throw zero division error
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
*/
void checkZeroDivision(htab_t *symTable, htab_key_t name);

/**
 * @brief Remove variable from symbol table
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
*/
void removeVar(htab_t *symTable, htab_key_t name);

/**
 * @brief Function will find first symbol table where is variable defined
 * @param Pointer to symbol table vector
 * @param Name of variable
 * @return Pointer to symbol table where variable is defined, or NULL when variable is undefined
*/
htab_t *getSymTableForVar(Vector *tableVector, htab_key_t name);

/**
 * @brief Function checks if funtion is defined
 * @param Pointer to table of symbols where function should be defined
 * @param Name of function
 * @return True when function is defined, false when it's not
*/
bool isFuncDefined(htab_t* symTable, htab_key_t name);

/**
 * @brief Function will find first symbol table where function is defined
 * @param Pointer to symbol table vector
 * @param Name of function
 * @return Pointer to symbol table where function is defined, or NULL when function is undefined
*/
htab_t *getSymTableForFunc(Vector *tableVector, htab_key_t name);

#endif