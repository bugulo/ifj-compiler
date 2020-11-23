/* IFJ20 - Code generator
 * Authors:
 * Juraj Marticek, xmarti97
 * Mario Harvan, xharva03
 */

#include <stdarg.h>
#include "code_gen.h"
#include "string.h"
#include "scanner.h"
#include "error.h"
#include "semantic_analysis.h"

#define FRAME_TAG_LEN 3

char *variableToString(Var variable, Vector *varScopeVec)
{
    int scopeId = getSymtableIdForVar(varScopeVec, variable.name.ptr);
    int varLength = snprintf(NULL, 0, "%d$%s", scopeId, variable.name.ptr);
    if (isVarUserDefined(getSymTableForVar(varScopeVec, variable.name.ptr), variable.name.ptr) == false)
        variable.frame = TEMP_FRAME;

    char *nameString = malloc(sizeof(char) * varLength + sizeof(char) * FRAME_TAG_LEN + 1);
    if (nameString == NULL)
        throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
    switch (variable.frame)
    {
    case TEMP_FRAME:
        sprintf(nameString, "TF@%d$%s", scopeId, variable.name.ptr);
        break;
    case LOCAL_FRAME:
        sprintf(nameString, "LF@%d$%s", scopeId, variable.name.ptr);
        break;
    case GLOBAL_FRAME:
        sprintf(nameString, "GF@%d$%s", scopeId, variable.name.ptr);
        break;
    }
    return nameString;
}

char *symbolToString(Symb symbol, Vector *varScopeVec)
{
    if (symbol.isVar)
        return variableToString(symbol.var, varScopeVec);

    char *numberString;

    if (symbol.token.type == TOKEN_STRING)
    {
        char *formatString = "string@%s";
        int length = snprintf(NULL, 0, formatString, symbol.token.value.i);
        numberString = malloc(sizeof(char) * length + 1);
        if (numberString == NULL)
            throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
        sprintf(numberString, formatString, symbol.token.value.i);
    }

    if (symbol.token.type == TOKEN_NUMBER_INT)
    {
        char *formatString = "int@%d";
        int length = snprintf(NULL, 0, formatString, symbol.token.value.i);
        numberString = malloc(sizeof(char) * length + 1);
        if (numberString == NULL)
            throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
        sprintf(numberString, formatString, symbol.token.value.i);
    }

    if (symbol.token.type == TOKEN_NUMBER_FLOAT)
    {
        char *formatString = "float@%a";
        int length = snprintf(NULL, 0, formatString, symbol.token.value.d);
        numberString = malloc(sizeof(char) * length + 1);
        if (numberString == NULL)
            throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
        sprintf(numberString, formatString, symbol.token.value.d);
    }

    return numberString;
}

void MOVE(Var dest, Symb src, Vector *varScopeVec)
{
    char *destString = variableToString(dest, varScopeVec);
    char *srcToString = symbolToString(src, varScopeVec);
    print_i("%s %s %s", "MOVE", destString, srcToString);
    free(destString);
    free(srcToString);
}

void CREATEFRAME()
{
    print_i("%s", "CREATEFRAME");
}

void PUSHFRAME()
{
    print_i("%s", "PUSHFRAME");
}

void POPFRAME()
{
    print_i("%s", "POPFRAME");
}

void DEFVAR(Var id, Vector *varScopeVec)
{
    char *stringId = variableToString(id, varScopeVec);
    print_i("%s %s", "DEFVAR", stringId);
    free(stringId);
}

void CALL(char *id)
{
    print_i("%s %s", "CALL", id);
}

void RETURN()
{
    print_i("%s", "RETURN");
}

void PUSHS(Symb op, Vector *varScopeVec)
{
    char *opString = symbolToString(op, varScopeVec);
    print_i("%s %s", "PUSHS", opString);
    free(opString);
}

void POPS(Var dest, Vector *varScopeVec)
{
    char *destString = variableToString(dest, varScopeVec);
    print_i("%s %s", "POPS", destString);
    free(destString);
}

void CLEARS()
{
    print_i("%s", "CLEARS");
}

void ADD(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "ADD", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }

    else
        print_i("%s", "ADDS");
}

void SUB(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "SUB", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "SUBS");
}

void MUL(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "MUL", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "MULS");
}

void DIV(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "DIV", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "DIVS");
}

void IDIV(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "IDIV", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "IDIVS");
}

void LT(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "LT", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "LTS");
}

void GT(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "GT", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "GTS");
}

void EQ(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "EQ", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "EQS");
}

void AND(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "AND", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "ANDS");
}

void OR(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "OR", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "ORS");
}

void NOT(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "NOT", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }
    else
        print_i("%s", "NOTS");
}

void INT2FLOAT(Var dest, Symb op, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *opString = symbolToString(op, varScopeVec);
        print_i("%s %s %s", "INT2FLOAT", destString, opString);
        free(destString);
        free(opString);
    }
    else
        print_i("%s", "INT2FLOATS");
}

void FLOAT2INT(Var dest, Symb op, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *opString = symbolToString(op, varScopeVec);
        print_i("%s %s %s", "FLOAT2INT", destString, opString);
        free(destString);
        free(opString);
    }
    else
        print_i("%s", "FLOAT2INTS");
}

void INT2CHAR(Var dest, Symb op, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *opString = symbolToString(op, varScopeVec);
        print_i("%s %s %s", "INT2CHAR", destString, opString);
        free(destString);
        free(opString);
    }
    else
        print_i("%s", "INT2CHARS");
}

void PRINT(Symb symb, Vector *varScopeVec)
{
    print_i("WRITE %s", symbolToString(symb, varScopeVec));
}

void STRI2INT(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        char *op2String = symbolToString(op2, varScopeVec);
        print_i("%s %s %s %s", "STRI2INT", destString, op1String, op2String);
        free(destString);
        free(op1String);
        free(op2String);
    }

    else
        print_i("%s", "STRI2INTS");
}

void declare_function(char *name, Vector *params, Vector *varScopeVec)
{
#ifdef DEBUG
    print("Declaring function %s, with %d params", name, vectorLength(params));
#endif
    print_i("LABEL %s", name);
    print_i("CREATEFRAME");
    print_i("PUSHFRAME");
    char *paramString;
    Var tmpVar;
    tmpVar.frame = LOCAL_FRAME;
    for (unsigned i = 0; i < vectorLength(params); i++)
    {
        tmpVar.name.ptr = vectorGet(params, i);
        paramString = variableToString(tmpVar, varScopeVec);
        print_i("DEFVAR %s", paramString);
        free(paramString);
    }

    for (unsigned i = 1; i <= vectorLength(params); i++)
    {
        tmpVar.name.ptr = vectorGet(params, vectorLength(params) - i);
        paramString = variableToString(tmpVar, varScopeVec);
        print_i("POPS %s", paramString);
        free(paramString);
    }
}

char *createNilVar()
{
    char *formatString = "LF@$nilVar%d";
    static unsigned counter = 0;
    int varLength = snprintf(NULL, 0, formatString, counter);
    char *varName = malloc(varLength * sizeof(char));
    sprintf(varName, formatString, counter);
    if (varName == NULL)
        throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
    return varName;
}

void call_function(char *name, Vector *call_params, Vector *return_params, Vector *varScopeVec)
{
#ifdef DEBUG
    print("Calling function %s, with %d input params, %d return params", name, vectorLength(call_params), vectorLength(return_params));
#endif
    char *paramString;
    Var tmpVar;
    tmpVar.frame = LOCAL_FRAME;
    for (unsigned i = 0; i < vectorLength(call_params); i++)
    {
        tmpVar.name.ptr = vectorGet(call_params, i);
        paramString = variableToString(tmpVar, varScopeVec);
        print_i("PUSHS %s", paramString);
        free(paramString);
    }
    print_i("CALL %s", name);
    for (unsigned i = 1; i <= vectorLength(return_params); i++)
    {
        tmpVar.name.ptr = vectorGet(return_params, vectorLength(return_params) - i);
        if (strcmp(tmpVar.name.ptr, "_") == 0)
        {
            char *nilVar = createNilVar();
            print_i("DEFVAR %s", nilVar);
            print_i("POPS %s", nilVar);
            free(nilVar);
            continue;
        }
        paramString = variableToString(tmpVar, varScopeVec);
        print_i("POPS %s", paramString);
        free(paramString);
    }
}

void return_function(Vector *return_params, Vector *varScopeVec)
{
    char *paramString;
    Var tmpVar;
    tmpVar.frame = LOCAL_FRAME;
    for (unsigned i = 0; i < vectorLength(return_params); i++)
    {
        tmpVar.name.ptr = vectorGet(return_params, i);
        paramString = variableToString(tmpVar, varScopeVec);
        print_i("PUSHS %s", paramString);
        free(paramString);
    }
    print_i("POPFRAME");
    print_i("RETURN");
}

void gen_init()
{
    print_i(".IFJcode20");
    print_i("JUMP main");
}

void if_start(char *result, Vector *varScopeVec)
{
    static unsigned label_counter = 0;
#ifdef DEBUG
    print("Starting `if`, watch out for core and end !! (id: %d)", label_counter);
#endif
    Var tmpVar;
    tmpVar.frame = LOCAL_FRAME;
    tmpVar.name.ptr = result;
    char *resultString = variableToString(tmpVar, varScopeVec);
    print_i("JUMPIFNEQ $else%d %s bool@true", label_counter, resultString);
    free(resultString);
    label_counter++;
}

void if_core()
{
    static unsigned label_counter = 0;
#ifdef DEBUG
    print("This is the core (else branch) of `if` (id: %d)", label_counter);
#endif
    print_i("JUMP $endif%d", label_counter);
    print_i("LABEL $else%d", label_counter);
    label_counter++;
}

void if_end()
{
    static unsigned label_counter = 0;
#ifdef DEBUG
    print("This is the end of `if` (id: %d)", label_counter);
#endif
    print_i("LABEL $endif%d", label_counter);
    label_counter++;
}

void for_start()
{
    static unsigned label_counter = 0;
#ifdef DEBUG
    print("Starting `for`, watch out for body and end !! (id: %d)", label_counter);
#endif
    print_i("LABEL $for_start%d", label_counter);
    label_counter++;
}

void for_body(Var res, Vector *varScopeVec)
{
    static unsigned label_counter = 0;
#ifdef DEBUG
    print("`For` body, watch out for the end !! (id: %d)", label_counter);
#endif
    char *resultString = variableToString(res, varScopeVec);
    print_i("JMPIFNEQ $for_end%d %s bool@true", label_counter, resultString);
    free(resultString);
    label_counter++;
}

void for_end()
{
    static unsigned label_counter = 0;
#ifdef DEBUG
    print("Ending `for`. (id: %d)", label_counter);
#endif
    print_i("JMP $for_start%d", label_counter);
    print_i("LABEL $for_end%d", label_counter);
    label_counter++;
}

void define_var(Var var, Symb value, Vector *varScopeVec)
{
#ifdef DEBUG
    print("Creating variable `%s`, and assigning value `%s`", variableToString(var), symbolToString(value));
#endif
    print_i("DEFVAR %s", variableToString(var, varScopeVec));
    print_i("MOVE %s %s", variableToString(var, varScopeVec), symbolToString(value, varScopeVec));
}

void main_end() {
    print_i("JMP $program_end");
}

void program_end() {
    print_i("LABEL $program_end");
}

void print_i(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int stringLength = snprintf(NULL, 0, "%s\n", fmt);
    char *formatString = malloc(stringLength * sizeof(char) + 1);
    sprintf(formatString, "%s\n", fmt);
    va_end(args);
    vprintf(formatString, args);
    free(formatString);
}
