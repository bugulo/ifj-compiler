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

#define SYM_TABLE_SIZE 100
#define EMPTY_SYMTABLE_ID -1

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
 * @brief Function returns value of variable
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
 * @return Variable value
*/
TokenValue getVarValue(htab_t* symTable, htab_key_t name);

/**
 * @brief Function sets value of variable
 * @param Pointer to table of symbols where variable should be defined
 * @param Name of variable
 * @param Data type of variable
*/
void setVarValue(htab_t* symTable, htab_key_t name, TokenValue value);

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
 * @brief Function will find first symbol table where is variable defined and returt its id
 * @param Pointer to symbol table vector
 * @param Name of variable
 * @return Id of symtable where variable is defined, or EMPTY_SYMTABLE_ID when undefined
*/
int getSymtableIdForVar(Vector *tableVector, htab_key_t name);

/**
 * @brief Function will return local symbol table
 * @param Pointer to vector with symbol tables
 * @return NULL when vector is empty or pointer to local symbol table
*/
htab_t *getLocalSymTable(Vector *tableVector);

/**
 * @brief Function will add new symtable to top of vector
 * @param Pointer to vector with symtables
*/
void insertLocalSymTable(Vector *tableVector);

/**
 * @brief Function will remove symtable from top of vector
 * @param Pointer to vector with symtables
*/
void removeLocalSymTable(Vector *tableVector);

/**
 * @brief Function will add data type to vector
 * @param Pointer to vector with dataTypes
 * @param Type of variable
*/
void addFuncType(Vector *types, varDataType varType);

/**
 * @brief Function will remove all data types from vector
 * @param Pointer to vector with dataTypes
*/
void removeFuncTypes(Vector *types);

/**
 * @brief Function checks if funtion is defined
 * @param Pointer to table of symbols where function should be defined
 * @param Name of function
 * @return True when function is defined, false when it's not
*/
bool isFuncDefined(htab_t* symTable, htab_key_t name);

/**
 * @brief Function will compare two sets of parameters
 * @param Pointer to vector with first set of parameters
 * @param Pointer to vector with second set of parameters
 * @return True if types are OK, False if not
*/
bool checkTypes(Vector *types1, Vector *types2);

/**
 * @brief Function will add new function to symbol table
 * @param Pointer to function table
 * @param Name of funtion
 * @param Vector with types of func parameters
 * @param Vector with return types from func
 */
void defineFunc(htab_t* symTable, htab_key_t name, Vector *paramTypes, Vector *returnTypes);

/**
 * @brief Function will return Function paramaters types
 * @param Pointer to symbol table
 * @param Name of function
 * @return Vector with param types or NULL when function is not found
*/
Vector* getFuncParamTypes(htab_t* symTable, htab_key_t name);

/**
 * @brief Function will return Function return types
 * @param Pointer to symbol table
 * @param Name of function
 * @return Vector with return types or NULL when function is not found
*/
Vector* getFuncReturnTypes(htab_t* symTable, htab_key_t name);

#endif