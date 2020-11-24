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
    unsigned varCnt = getVarCnt(getSymTableForVar(varScopeVec, variable.name.ptr), variable.name.ptr);
    int varLength = snprintf(NULL, 0, "$%d$%d$%s", varCnt, scopeId, variable.name.ptr);
    if (isVarUserDefined(getSymTableForVar(varScopeVec, variable.name.ptr), variable.name.ptr) == false)
        variable.frame = TEMP_FRAME;

    char *nameString = malloc(sizeof(char) * varLength + sizeof(char) * FRAME_TAG_LEN + 1);
    if (nameString == NULL)
        throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
    switch (variable.frame)
    {
    case TEMP_FRAME:
        sprintf(nameString, "TF@$%d$%d$%s", varCnt, scopeId, variable.name.ptr);
        break;
    case LOCAL_FRAME:
        sprintf(nameString, "LF@$%d$%d$%s", varCnt, scopeId, variable.name.ptr);
        break;
    case GLOBAL_FRAME:
        sprintf(nameString, "GF@$%d$%d$%s", varCnt, scopeId, variable.name.ptr);
        break;
    }
    return nameString;
}

char *symbolToString(Symb symbol, Vector *varScopeVec)
{
    if (symbol.isVar)
        return variableToString(symbol.var, varScopeVec);

    char *finalString;
    if (symbol.token.type == TOKEN_STRING)
    {
        // String escaping
        int stringLength = strlen(symbol.token.value.s.ptr);
        char *newEscapedString = calloc(stringLength * 4 + 1, sizeof(char));
        if (newEscapedString == NULL)
            throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");

        for (int i = 0, j = 0; i < stringLength; i++)
        {
            int c = symbol.token.value.s.ptr[i];
            if ((c >= 0 && c <= 32) || c == 35 || c == 92)
            {
                int printLen = snprintf(newEscapedString + j, 5, "\\0%d", c);
                j += printLen;
            }
            else
                newEscapedString[j++] = c;
        }
        char *formatString = "string@%s";
        int length = snprintf(NULL, 0, formatString, newEscapedString);
        finalString = malloc(sizeof(char) * length + 1);
        if (finalString == NULL)
            throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");

        sprintf(finalString, formatString, newEscapedString);
        free(newEscapedString);
    }

    if (symbol.token.type == TOKEN_NUMBER_INT)
    {
        char *formatString = "int@%d";
        int length = snprintf(NULL, 0, formatString, symbol.token.value.i);
        finalString = malloc(sizeof(char) * length + 1);
        if (finalString == NULL)
            throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
        sprintf(finalString, formatString, symbol.token.value.i);
    }

    if (symbol.token.type == TOKEN_NUMBER_FLOAT)
    {
        char *formatString = "float@%a";
        int length = snprintf(NULL, 0, formatString, symbol.token.value.d);
        finalString = malloc(sizeof(char) * length + 1);
        if (finalString == NULL)
            throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
        sprintf(finalString, formatString, symbol.token.value.d);
    }

    return finalString;
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

void NOT(Var dest, Symb op1, bool useStack, Vector *varScopeVec)
{
    if (!useStack)
    {
        char *destString = variableToString(dest, varScopeVec);
        char *op1String = symbolToString(op1, varScopeVec);
        print_i("%s %s %s", "NOT", destString, op1String);
        free(destString);
        free(op1String);
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

Vector *if_count_stack;
Vector *for_count_stack;
Vector *for_string_stack;

void gen_init()
{
    //sglobalne spremenne
    if_count_stack = vectorInit();
    for_count_stack = vectorInit();
    for_string_stack = vectorInit();
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
    unsigned *new_label_count = malloc(sizeof(unsigned));
    *new_label_count = label_counter++;
    vectorPush(if_count_stack, new_label_count);
}

void if_core()
{
    unsigned *label_counter = vectorGet(if_count_stack, vectorLength(if_count_stack) - 1);
#ifdef DEBUG
    print("This is the core (else branch) of `if` (id: %d)", label_counter);
#endif
    print_i("JUMP $endif%d", *label_counter);
    print_i("LABEL $else%d", *label_counter);
}

void if_end()
{
    unsigned *label_counter = vectorPop(if_count_stack);
#ifdef DEBUG
    print("This is the end of `if` (id: %d)", label_counter);
#endif
    print_i("LABEL $endif%d", *label_counter);
    free(label_counter);
}

void for_start()
{
    static unsigned label_counter = 0;
#ifdef DEBUG
    print("Starting `for`, watch out for body and end !! (id: %d)", label_counter);
#endif
    unsigned *tmpCnt = malloc(sizeof(unsigned));
    if (tmpCnt == NULL)
        throw_error_fatal(INTERNAL_ERROR, "Memory allocation error");

    *tmpCnt = label_counter++;
    print_i("JUMP $for_def%d", *tmpCnt);
    print_i("LABEL $for_start%d", *tmpCnt);
    vectorPush(for_count_stack, tmpCnt);
}

void for_expression(Var res, Vector *varScopeVec)
{
    unsigned *tmpCnt = vectorGet(for_count_stack, vectorLength(for_count_stack) - 1);
    char *resultString = variableToString(res, varScopeVec);
    print_i("JUMPIFNEQ $for_end%d %s bool@true", *tmpCnt, resultString);
    free(resultString);
    print_i("JUMP $for_body_start%d", *tmpCnt);
}

void for_assign_start()
{
    unsigned *tmpCnt = vectorGet(for_count_stack, vectorLength(for_count_stack) - 1);
    print_i("LABEL $for_assign%d", *tmpCnt);
}

void for_body()
{
#ifdef DEBUG
    print("`For` body, watch out for the end !! (id: %d)", label_counter);
#endif
    unsigned *tmpCnt = vectorGet(for_count_stack, vectorLength(for_count_stack) - 1);
    print_i("JUMP $for_start%d", *tmpCnt);
    print_i("LABEL $for_body_start%d", *tmpCnt);
}

void for_end()
{
#ifdef DEBUG
    print("Ending `for`. (id: %d)", label_counter);
#endif
    unsigned *tmpCnt = vectorPop(for_count_stack);
    print_i("JUMP $for_assign%d", *tmpCnt);
    print_i("LABEL $for_def%d", *tmpCnt);

    char *defVarString;
    if (vectorLength(for_count_stack) == 0)
    {
        while (vectorLength(for_string_stack) > 0)
        {
            defVarString = vectorPop(for_string_stack);
            print_i("DEFVAR %s", defVarString);
            free(defVarString);
        }
    }

    print_i("JUMP $for_start%d", *tmpCnt);
    print_i("LABEL $for_end%d", *tmpCnt);

    free(tmpCnt);
}

void define_var(Var var, Symb value, Vector *varScopeVec)
{
#ifdef DEBUG
    //print("Creating variable `%s`, and assigning value `%s`", variableToString(var), symbolToString(value));
#endif
    if (vectorLength(for_count_stack) == 0 || isVarUserDefined(getSymTableForVar(varScopeVec, var.name.ptr), var.name.ptr) == 0)
        print_i("DEFVAR %s", variableToString(var, varScopeVec));
    else
        vectorPush(for_string_stack, variableToString(var, varScopeVec));

    print_i("MOVE %s %s", variableToString(var, varScopeVec), symbolToString(value, varScopeVec));
}

void main_end()
{
    print_i("JUMP $program_end");
}

void program_end()
{
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
