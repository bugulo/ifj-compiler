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
    print_i("CREATEFRAME");
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
    char *formatString = "nilVar%d";
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
    print_i("PUSHFRAME");
    print_i("CALL %s", name);
    for (unsigned i = 1; i <= vectorLength(return_params); i++)
    {
        tmpVar.name.ptr = vectorGet(return_params, vectorLength(return_params) - i);
        if (strcmp(tmpVar.name.ptr, "_") == 0)
        {
            TokenValue emptyVal;
            char *nilVar = createNilVar();
            defineUserVar(getLocalSymTable(varScopeVec), nilVar, KEYWORD_NONE, emptyVal, false);
            Var nil = {.frame = LOCAL_FRAME, .name.ptr = nilVar};
            DEFVAR(nil, varScopeVec);
            POPS(nil, varScopeVec);
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
    print_i("POPFRAME");
    print_i("RETURN");
}

// This fn PUSHes error label to the stack if TF@res == true
void declare_error_setter()
{
    print_i("LABEL $error_push");
    print_i("JUMPIFEQ $error_setter_else TF@res bool@true");
    print_i("PUSHS int@0");
    print_i("JUMP $error_setter_end");
    print_i("LABEL $error_setter_else");
    print_i("PUSHS int@1");
    print_i("LABEL $error_setter_end");
    print_i("RETURN");
}

void declare_inputs()
{
    print_i("LABEL inputs");
    print_i("CREATEFRAME");
    char *inputStringVarName = "TF@inputString";
    print_i("DEFVAR %s", inputStringVarName);
    print_i("READ %s string", inputStringVarName);
    char *typeName = "TF@type";
    print_i("DEFVAR %s", typeName);
    print_i("TYPE %s %s", typeName, inputStringVarName);
    print_i("PUSHS %s", inputStringVarName);
    print_i("DEFVAR TF@res");
    print_i("EQ TF@res %s string@nil", typeName);
    print_i("CALL $error_push");
    print_i("POPFRAME");
    print_i("RETURN");
}

void declare_inputi()
{
    print_i("LABEL inputi");
    print_i("CREATEFRAME");
    char *inputIntVarName = "TF@inputInteger";
    print_i("DEFVAR %s", inputIntVarName);
    print_i("READ %s int", inputIntVarName);
    char *typeName = "TF@type";
    print_i("DEFVAR %s", typeName);
    print_i("TYPE %s %s", typeName, inputIntVarName);
    print_i("PUSHS %s", inputIntVarName);
    print_i("DEFVAR TF@res");
    print_i("EQ TF@res %s string@nil", typeName);
    print_i("CALL $error_push");
    print_i("POPFRAME");
    print_i("RETURN");
}

void declare_inputf()
{
    print_i("LABEL inputf");
    print_i("CREATEFRAME");
    char *inputFloatVarName = "TF@inputFloat";
    print_i("DEFVAR %s", inputFloatVarName);
    print_i("READ %s float", inputFloatVarName);
    char *typeName = "TF@type";
    print_i("DEFVAR %s", typeName);
    print_i("TYPE %s %s", typeName, inputFloatVarName);
    print_i("PUSHS %s", inputFloatVarName);
    print_i("DEFVAR TF@res");
    print_i("EQ TF@res %s string@nil", typeName);
    print_i("CALL $error_push");
    print_i("POPFRAME");
    print_i("RETURN");
}

void declare_int2float()
{
    print_i("LABEL int2float");
    print_i("CREATEFRAME");
    char *inputInt = "TF@inputInt";
    print_i("DEFVAR %s", inputInt);
    print_i("POPS %s", inputInt);
    char *newFloat = "TF@newFloat";
    print_i("DEFVAR %s", newFloat);
    print_i("INT2FLOAT %s %s", newFloat, inputInt);
    print_i("PUSHS %s", newFloat);
    print_i("POPFRAME");
    print_i("RETURN");
}

void declare_float2int()
{
    print_i("LABEL float2int");
    print_i("CREATEFRAME");
    char *inputFloat = "TF@inputFloat";
    print_i("DEFVAR %s", inputFloat);
    print_i("POPS %s", inputFloat);
    char *newInt = "TF@newInt";
    print_i("DEFVAR %s", newInt);
    print_i("FLOAT2INT %s %s", newInt, inputFloat);
    print_i("PUSHS %s", newInt);
    print_i("POPFRAME");
    print_i("RETURN");
}

void declare_len()
{
    print_i("LABEL len");
    print_i("CREATEFRAME");
    char *inputString = "TF@inputString";
    print_i("DEFVAR %s", inputString);
    print_i("POPS %s", inputString);
    char *length = "TF@length";
    print_i("DEFVAR %s", length);
    print_i("STRLEN %s %s", length, inputString);
    print_i("PUSHS %s", length);
    print_i("POPFRAME");
    print_i("RETURN");
}

void declare_ord()
{
    print_i("LABEL ord");
    print_i("CREATEFRAME");
    char *s = "TF@s";
    char *i = "TF@i";
    print_i("DEFVAR %s", s);
    print_i("DEFVAR %s", i);
    print_i("POPS %s", i);
    print_i("POPS %s", s);

    // Check 0 < i < strlen(s)
    char *length = "TF@length";
    print_i("DEFVAR %s", length);
    print_i("STRLEN %s %s", length, s);
    print_i("DEFVAR TF@res");
    print_i("LT TF@res %s int@0", i);
    print_i("PUSHS TF@res");
    print_i("GT TF@res %s %s", i, length);
    print_i("PUSHS TF@res");
    print_i("ORS");
    print_i("POPS TF@res");

    char *c = "TF@c";
    print_i("DEFVAR %s", c);
    print_i("MOVE %s int@", c);

    // If errored, jump to end
    print_i("JUMPIFEQ $ord_end TF@res bool@true");
    print_i("STRI2INT %s %s %s", c, s, i);

    print_i("LABEL $ord_end");
    print_i("PUSHS %s", c);
    print_i("CALL $error_push");
    print_i("POPFRAME");
    print_i("RETURN");
}

void declare_chr()
{
    print_i("LABEL chr");
    print_i("CREATEFRAME");
    char *i = "TF@i";
    print_i("DEFVAR %s", i);
    print_i("POPS %s", i);

    // Check 0 < i < 255
    print_i("DEFVAR TF@res");
    print_i("LT TF@res %s int@0", i);
    print_i("PUSHS TF@res");
    print_i("GT TF@res %s int@255", i);
    print_i("PUSHS TF@res");
    print_i("ORS");
    print_i("POPS TF@res");

    char *c = "TF@c";
    print_i("DEFVAR %s", c);
    print_i("MOVE %s string@", c);

    // If errored, jump to end
    print_i("JUMPIFEQ $chr_end TF@res bool@true");
    print_i("INT2CHAR %s %s", c, i);

    print_i("LABEL $chr_end");
    print_i("PUSHS %s", c);
    print_i("CALL $error_push");
    print_i("POPFRAME");
    print_i("RETURN");
}

void declare_substr()
{
    print_i("LABEL substr");
    print_i("CREATEFRAME");
    char *s = "TF@s";
    char *i = "TF@i";
    char *n = "TF@n";
    print_i("DEFVAR %s", s);
    print_i("DEFVAR %s", i);
    print_i("DEFVAR %s", n);
    print_i("POPS %s", n);
    print_i("POPS %s", i);
    print_i("POPS %s", s);

    // Check n < 0 || 0 < i < strlen(s)
    char *length = "TF@length";
    print_i("DEFVAR %s", length);
    print_i("STRLEN %s %s", length, s);
    print_i("DEFVAR TF@res");
    print_i("LT TF@res %s int@0", n);
    print_i("PUSHS TF@res");
    print_i("LT TF@res %s int@0", i);
    print_i("PUSHS TF@res");
    print_i("GT TF@res %s %s", i, length);
    print_i("PUSHS TF@res");
    print_i("ORS");
    print_i("ORS");

    // if n > length - i => n = length - i
    char *lenMinusI = "TF@lenMinusI";
    print_i("DEFVAR %s", lenMinusI);
    print_i("SUB %s %s %s", lenMinusI, length, i);
    print_i("GT TF@res %s %s", n, lenMinusI);
    print_i("JUMPIFNEQ $substr_not_till_end TF@res bool@true");
    print_i("MOVE %s %s", n, lenMinusI);
    print_i("LABEL $substr_not_till_end");

    print_i("POPS TF@res");
    print_i("CALL $error_push");
    print_i("DEFVAR TF@errInt");
    print_i("POPS TF@errInt");

    // Substr
    char *finalString = "TF@finalString";
    char *c = "TF@c";
    print_i("DEFVAR %s", finalString);
    print_i("MOVE %s string@", finalString);

    // If errored, jump to end
    print_i("JUMPIFEQ $substr_end TF@errInt int@1");

    print_i("DEFVAR %s", c);
    // while(i < n)
    print_i("LABEL $substr_start");
    print_i("MOVE %s string@", c);
    print_i("GETCHAR %s %s %s", c, s, i);
    print_i("CONCAT %s %s %s", finalString, finalString, c);
    print_i("SUB %s %s int@1", n, n);
    print_i("ADD %s %s int@1", i, i);
    print_i("JUMPIFNEQ $substr_start %s int@0", n);

    print_i("LABEL $substr_end");
    print_i("PUSHS %s", finalString);
    print_i("PUSHS TF@errInt");
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
    declare_error_setter();
    declare_inputs();
    declare_inputi();
    declare_inputf();
    declare_int2float();
    declare_float2int();
    declare_len();
    declare_substr();
    declare_ord();
    declare_chr();
}

void DEFVAR(Var id, Vector *varScopeVec)
{
    char *stringId = variableToString(id, varScopeVec);
    if (vectorLength(for_count_stack) == 0)
    {
        print_i("DEFVAR %s", stringId);
        free(stringId);
    }
    else
        vectorPush(for_string_stack, stringId);
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
        throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");

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
    DEFVAR(var, varScopeVec);
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

void TYPE(Var dest, Symb op, Vector *varScopeVec)
{
    char *destString = variableToString(dest, varScopeVec);
    char *opString = symbolToString(op, varScopeVec);
    print_i("%s %s %s", "TYPE", destString, opString);
    free(destString);
    free(opString);
}

void CONCAT(Var dest, Symb op1, Symb op2, Vector *varScopeVec)
{
    char *destString = variableToString(dest, varScopeVec);
    char *op1String = symbolToString(op1, varScopeVec);
    char *op2String = symbolToString(op2, varScopeVec);
    print_i("%s %s %s %s", "CONCAT", destString, op1String, op2String);
    free(destString);
    free(op1String);
    free(op2String);
}

void STRLEN(Var dest, Symb op, Vector *varScopeVec)
{
    char *destString = variableToString(dest, varScopeVec);
    char *opString = symbolToString(op, varScopeVec);
    print_i("%s %s %s", "STRLEN", destString, opString);
    free(destString);
    free(opString);
}

void GETCHAR(Var dest, Symb op1, Symb op2, Vector *varScopeVec)
{
    char *destString = variableToString(dest, varScopeVec);
    char *op1String = symbolToString(op1, varScopeVec);
    char *op2String = symbolToString(op2, varScopeVec);
    print_i("%s %s %s %s", "GETCHAR", destString, op1String, op2String);
    free(destString);
    free(op1String);
    free(op2String);
}

void SETCHAR(Var dest, Symb op1, Symb op2, Vector *varScopeVec)
{
    char *destString = variableToString(dest, varScopeVec);
    char *op1String = symbolToString(op1, varScopeVec);
    char *op2String = symbolToString(op2, varScopeVec);
    print_i("%s %s %s %s", "SETCHAR", destString, op1String, op2String);
    free(destString);
    free(op1String);
    free(op2String);
}