/* IFJ20 - Semantic analysis
 * Authors:
 * Mario Harvan, xharva03
 */

#include "semantic_analysis.h"

bool isVarDefined(htab_t *symTable, htab_key_t name)
{
    htab_iterator_t tmp = htab_find(symTable, name);
    if (tmp.ptr != NULL)
    {
        if(tmp.ptr->isVar == true)
            return true;
    }
    return false;
}

varDataType getVarType(htab_t *symTable, htab_key_t name)
{
    htab_iterator_t tmp = htab_find(symTable, name);
    if (tmp.ptr != NULL)
    {
        if(tmp.ptr->isVar == true)
            return tmp.ptr->varDataType;
    }
    return NONE;
}

void setVarType(htab_t *symTable, htab_key_t name, varDataType varDataType)
{
    htab_iterator_t tmp = htab_find(symTable, name);
    if (tmp.ptr != NULL)
    {
        if(tmp.ptr->isVar == true)
            tmp.ptr->varDataType = varDataType;
    }
}

bool isVarConst(htab_t *symTable, htab_key_t name)
{
    htab_iterator_t tmp = htab_find(symTable, name);
    if (tmp.ptr != NULL)
    {
        if(tmp.ptr->isVar == true)
            return tmp.ptr->isConst;
    }
    return false;
}

void setVarConst(htab_t *symTable, htab_key_t name, bool isConst)
{
    htab_iterator_t tmp = htab_find(symTable, name);
    if (tmp.ptr != NULL)
    {
        if(tmp.ptr->isVar == true)
            tmp.ptr->isConst = isConst;
    }
}

varDataType checkOperationTypes(Vector *tableVector, Token var1, Token var2)
{
    #ifdef DEBUG
        print("var1 string: %s var2 string: %s", var1.value.s.ptr, var2.value.s.ptr);
    #endif
    htab_iterator_t operator1;
    htab_iterator_t operator2;

    operator1.ptr = NULL;
    operator2.ptr = NULL;

    htab_t *tableForOp1 = getSymTableForVar(tableVector, var1.value.s.ptr);
    if (tableForOp1 == NULL)
        throw_error_fatal(DEFINITION_ERROR, "%s", "Variable is undefined");
    operator1 = htab_find(tableForOp1, var1.value.s.ptr);

    htab_t *tableForOp2 = getSymTableForVar(tableVector, var2.value.s.ptr);
    if (tableForOp1 == NULL)
        throw_error_fatal(DEFINITION_ERROR, "%s", "Variable is undefined");
    operator2 = htab_find(tableForOp2, var2.value.s.ptr);

    if (operator1.ptr != NULL && operator2.ptr != NULL)
    {
        #ifdef DEBUG
            print("Operator 1 token type: %d Operator 2 token type: %d", operator1.ptr->tokenType, operator2.ptr->tokenType);
        #endif
        if (operator1.ptr->tokenType == TOKEN_IDENTIFIER && operator2.ptr->tokenType == TOKEN_IDENTIFIER)
        {
            #ifdef DEBUG
                print("Operator 1 data type: %d Operator 2 data type: %d", operator1.ptr->varDataType, operator2.ptr->varDataType);
            #endif
            if (operator1.ptr->varDataType == operator2.ptr->varDataType)
                return operator1.ptr->varDataType;
        }
    }
    return NONE;
}

String defineCompilerVar(htab_t *symTable, varDataType varDataType, TokenValue varValue, bool isConst)
{
    static int cnt = 0;
    String varName;
    string_init(&varName);
    string_append_string(&varName, "Expression");

    //count length of number
    int length = snprintf(NULL, 0, "%d", cnt);
    char *tmpCnt = malloc(sizeof(char) * length + 1);
    if (tmpCnt == NULL)
        throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
    sprintf(tmpCnt, "%d", cnt);
    string_append_string(&varName, tmpCnt);
    free(tmpCnt);

    htab_iterator_t tmp = htab_insert(symTable, varName.ptr);
    if (tmp.ptr != NULL)
    {
        tmp.ptr->tokenType = TOKEN_IDENTIFIER;
        tmp.ptr->varDataType = varDataType;
        tmp.ptr->isVarUsedDefined = false;
        if (isConst == true)
            tmp.ptr->varValue = varValue;
        tmp.ptr->isConst = isConst;
        tmp.ptr->isVar = true;
        cnt++;
        return varName;
    }
    else
        throw_error_fatal(INTERNAL_ERROR, "%s", "Inconsistency in compiler variables");
}

void defineUserVar(htab_t *symTable, htab_key_t name, varDataType varDataType, TokenValue varValue, bool isConst)
{
    htab_iterator_t tmp = htab_insert(symTable, name);
    if (tmp.ptr != NULL)
    {
        tmp.ptr->tokenType = TOKEN_IDENTIFIER;
        tmp.ptr->varDataType = varDataType;
        tmp.ptr->isVarUsedDefined = true;
        if (isConst == true)
            tmp.ptr->varValue = varValue;
        tmp.ptr->isConst = isConst;
        tmp.ptr->isVar = true;
    }
    else
        throw_error_fatal(DEFINITION_ERROR, "%s", "Multiple definitions of variable");
}

bool isVarUserDefined(htab_t *symTable, htab_key_t name)
{
    htab_iterator_t tmp = htab_find(symTable, name);
    if (tmp.ptr != NULL)
    {
        if(tmp.ptr->isVar == true)
            return tmp.ptr->isVarUsedDefined;
    }
    return false;
}

void checkZeroDivision(htab_t *symTable, htab_key_t name)
{
    if (isVarConst(symTable, name) == true)
    {
        htab_iterator_t tmp = htab_find(symTable, name);
        if (tmp.ptr != NULL)
        {
            switch (tmp.ptr->varDataType)
            {
            case INTEGER:
                if (tmp.ptr->varValue.i == 0)
                    throw_error_fatal(ZERO_DIVISION_ERROR, "%s", "Can't divide by zero");
                break;
            case FLOAT:
                if (tmp.ptr->varValue.d == 0)
                    throw_error_fatal(ZERO_DIVISION_ERROR, "%s", "Can't divide by zero");
            default:
                break;
            }
        }
    }
}

void removeVar(htab_t *symTable, htab_key_t name)
{
    htab_iterator_t tmp = htab_find(symTable, name);
    if (tmp.ptr != NULL)
    {
        if(tmp.ptr->isVar == true){
            if (tmp.ptr->varDataType == STRING)
                string_free(&tmp.ptr->varValue.s);
            htab_erase(symTable, tmp);
        }
    }
}

htab_t *getSymTableForVar(Vector *tableVector, htab_key_t name)
{
    htab_iterator_t tmp;
    unsigned int vecLength = vectorLength(tableVector) - 1;
    for (unsigned int i = 0; i <= vecLength; i++)
    {
        htab_t *tmpTable = (htab_t *)vectorGet(tableVector, vecLength - i);
        tmp = htab_find(tmpTable, name);
        if (tmp.ptr != NULL){
            if(tmp.ptr->isVar == true)
                return (htab_t *)tmp.t;
        }
    }
    return NULL;
}

bool isFuncDefined(htab_t *symTable, htab_key_t name)
{
    if(symTable != NULL){
        htab_iterator_t tmp = htab_find(symTable, name);
        if (tmp.ptr != NULL)
        {
            if (tmp.ptr->isVar == false)
                return true;
        }
    }
    return false;
}

htab_t *getSymTableForFunc(Vector *tableVector, htab_key_t name)
{
    htab_iterator_t tmp;
    if(tableVector != NULL){
        unsigned int vecLength = vectorLength(tableVector) - 1;
        for (unsigned int i = 0; i <= vecLength; i++)
        {
            htab_t *tmpTable = (htab_t *)vectorGet(tableVector, vecLength - i);
            tmp = htab_find(tmpTable, name);
            if (tmp.ptr != NULL)
                if(tmp.ptr->isVar == false)
                    return (htab_t *)tmp.t;
        }
    }
    return NULL;
}