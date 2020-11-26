/* IFJ20 - Precedence analysis
 * Authors:
 * Mario Harvan, xharva03
 * Juraj Marticek, xmarti97
 */

#include "expression.h"
#include "scanner.h"
#include "error.h"
#include "stdbool.h"
#include "semantic_analysis.h"
#include "string.h"
#include "code_gen.h"

//operand1Symb = {.isVar = true, .var.name = operand1.value.s, .var.frame = LOCAL_FRAME};

#define ASSIGN_DEST(destFrame, destName)    \
    dest.frame = destFrame;                 \
    dest.name = destName;

#define ASSIGN_OPERAND1SYMB(opIsVar, opVarName, opVarFrame) \
    operand1Symb.isVar = opIsVar;                           \
    operand1Symb.var.name = opVarName;                      \
    operand1Symb.var.frame = opVarFrame;

#define ASSIGN_OPERAND2SYMB(opIsVar, opVarName, opVarFrame) \
    operand2Symb.isVar = opIsVar;                           \
    operand2Symb.var.name = opVarName;                      \
    operand2Symb.var.frame = opVarFrame;

bool isEndToken(Token token)
{
    if (token.type == TOKEN_EOL)
        return true;
    if (token.type == TOKEN_COMA)
        return true;
    if (token.type == TOKEN_SEMICOLON)
        return true;
    if (token.type == TOKEN_BRACE_LEFT)
        return true;
    return false;
}

int reduceTokenCount(Stack *stack)
{
    Stack *tmpStack = stackInit();
    int tokenCount = 0;
    Token tmpToken;
    //count tokens before limiter
    do
    {
        tmpToken = stackPop(stack);
        if (tmpToken.type != TOKEN_DELIMITER)
            tokenCount++;
        if (tmpToken.type != TOKEN_NONE)
            stackPush(tmpStack, tmpToken);
    } while (tmpToken.type != TOKEN_DELIMITER && tmpToken.type != TOKEN_NONE);

    if (tmpToken.type == TOKEN_NONE)
    {
        if (stackPeek(tmpStack).type == TOKEN_EXPRESSION)
        {
            stackFree(tmpStack);
            stackPush(stack, tmpToken);
            return ANALYSIS_DONE;
        }
        else
        {
            stackFree(tmpStack);
            return REDUCE_ERROR;
        }
    }
    //return tokens to stack
    while (stackIsEmpty(tmpStack) == false)
    {
        stackPush(stack, stackPop(tmpStack));
    }
    stackFree(tmpStack);
    return tokenCount;
}

Token semanticCheckFor3Op(Token operand1, Token operation, Token operand2, Vector *symtableVector)
{
    TokenValue emptyVal;
    Token tmp;
    tmp.type = TOKEN_EXPRESSION;
    if (operand1.type == TOKEN_EXPRESSION && operand2.type == TOKEN_EXPRESSION)
    {
        varDataType varType = checkOperationTypes(symtableVector, operand1, operand2);
        if (varType == NONE)
            throw_error_fatal(INCOMPATIBLE_EXPRESSION_ERROR, "%s", "Incompatible data types in expression");

        if(operation.type == TOKEN_DIV)
            checkZeroDivision(getSymTableForVar(symtableVector, operand2.value.s.ptr), operand2.value.s.ptr);

        if (varType == STRING){
            switch(operation.type){
                case TOKEN_MINUS:
                case TOKEN_MUL:
                case TOKEN_DIV:
                    throw_error_fatal(INCOMPATIBLE_EXPRESSION_ERROR, "%s", "Operation is not supported with string");
                    break;
                default:
                    break;
            }
        }
        if (varType == BOOL)
        {
            switch(operation.type){
                case TOKEN_PLUS:
                case TOKEN_MINUS:
                case TOKEN_MUL:
                case TOKEN_DIV:
                    throw_error_fatal(INCOMPATIBLE_EXPRESSION_ERROR, "%s", "Operation is not supported with bool");
                    break;
                default:
                    break;
            }
        }
        //prepare var for result
        String tmpVarName = defineCompilerVar((htab_t *)vectorGet(symtableVector, vectorLength(symtableVector) - 1), varType, emptyVal, false);
        tmp.value.s = tmpVarName;
        //code gen
        Var id = {.frame = TEMP_FRAME, .name = tmpVarName};
        DEFVAR(id, symtableVector);

        return tmp;
    }
    else
        throw_error_fatal(INTERNAL_ERROR, "%s", "Wrong token types in semantic checks");
    return tmp;
}

void removeTokenAfterOp(Token token, Vector *symtableVector)
{
    if (token.type == TOKEN_EXPRESSION)
    {
        if (isVarUserDefined(getSymTableForVar(symtableVector, token.value.s.ptr), token.value.s.ptr) == false)
        {
            removeVar(getSymTableForVar(symtableVector, token.value.s.ptr), token.value.s.ptr);
            free(token.value.s.ptr);
        }
        else
        {
            free(token.value.s.ptr);
        }
    }
}

void reduceRules1Op(Stack *stack, Token operand1, Vector *symtableVector)
{
    Token expressionToken;
    expressionToken.type = TOKEN_EXPRESSION;
    String tmpVarName;
    Var var;
    Symb value;

    switch (operand1.type)
    {
    case TOKEN_IDENTIFIER:;
    #ifdef DEBUG
        print("ID: %s", "E->i");
    #endif
        //special case of _ var
        if(strcmp("_", operand1.value.s.ptr) == 0)
            throw_error_fatal(SYNTAX_ERROR, "%s", "Can't use _ in expression");
        //check if variable is defined
        htab_t *symTable = getSymTableForVar(symtableVector, operand1.value.s.ptr);
        if (symTable != NULL)
        {
            if (isVarDefined(symTable, operand1.value.s.ptr) == true)
            {
                expressionToken.value.s = operand1.value.s;
                stackPush(stack, expressionToken);
            }
            else
                throw_error_fatal(DEFINITION_ERROR, "%s", "Variable is not defined");
        }
        else
            throw_error_fatal(DEFINITION_ERROR, "%s", "Variable is not defined");
        break;
    case TOKEN_NUMBER_FLOAT:
        #ifdef DEBUG
            print("FLOAT: %s", "E->i");
        #endif
        tmpVarName = defineCompilerVar((htab_t *)vectorGet(symtableVector, vectorLength(symtableVector) - 1), FLOAT, operand1.value, true);
        expressionToken.value.s = tmpVarName;
        //call code generator
        var.frame = TEMP_FRAME;
        var.name = tmpVarName;
        value.token = operand1;
        value.isVar = false;
        define_var(var, value, symtableVector);

        stackPush(stack, expressionToken);
        break;
    case TOKEN_NUMBER_INT:
        #ifdef DEBUG
            print("INT: %s", "E->i");
        #endif
        tmpVarName = defineCompilerVar((htab_t *)vectorGet(symtableVector, vectorLength(symtableVector) - 1), INTEGER, operand1.value, true);
        expressionToken.value.s = tmpVarName;
        //call code generator
        var.frame = TEMP_FRAME;
        var.name = tmpVarName;
        value.token = operand1;
        value.isVar = false;
        define_var(var, value, symtableVector);

        stackPush(stack, expressionToken);
        break;
    case TOKEN_STRING:
        #ifdef DEBUG
            print("STRING: %s", "E->i");
        #endif
        tmpVarName = defineCompilerVar((htab_t *)vectorGet(symtableVector, vectorLength(symtableVector) - 1), STRING, operand1.value, true);
        expressionToken.value.s = tmpVarName;
        //call code generator
        var.frame = TEMP_FRAME;
        var.name = tmpVarName;
        value.token = operand1;
        value.isVar = false;
        define_var(var, value, symtableVector);
        
        stackPush(stack, expressionToken);
        break;
    default:
        throw_error_fatal(SYNTAX_ERROR, "%s", "Syntax error in expression");
        break;
    }
    return;
}

void reduceRules3Op(Stack *stack, Token operand1, Token operation, Token operand2, Vector *symtableVector)
{
    Token expressionToken;
    expressionToken.type = TOKEN_EXPRESSION;
    Var dest;
    Symb operand1Symb;
    Symb operand2Symb;

    switch (operation.type)
    {
    case TOKEN_PLUS:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E+E");
        #endif

        //code generator
        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        if(getVarType(getSymTableForVar(symtableVector, expressionToken.value.s.ptr), expressionToken.value.s.ptr) == STRING)
            CONCAT(dest, operand1Symb, operand2Symb, symtableVector);
        else
            ADD(dest, operand1Symb, operand2Symb, false, symtableVector);

        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        stackPush(stack, expressionToken);
        break;
    case TOKEN_MINUS:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E-E");
        #endif

        //code generator
        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        SUB(dest, operand1Symb, operand2Symb, false, symtableVector);

        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        stackPush(stack, expressionToken);
        break;
    case TOKEN_MUL:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E*E");
        #endif

        //code generator
        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        MUL(dest, operand1Symb, operand2Symb, false, symtableVector);

        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        stackPush(stack, expressionToken);
        break;
    case TOKEN_DIV:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E/E");
        #endif

        //code generator
        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        varDataType tmp = getVarType(getSymTableForVar(symtableVector, expressionToken.value.s.ptr), expressionToken.value.s.ptr);
        if(tmp == FLOAT)
            DIV(dest, operand1Symb, operand2Symb, false, symtableVector);
        if(tmp == INTEGER)
            IDIV(dest, operand1Symb, operand2Symb, false, symtableVector);
        
        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        stackPush(stack, expressionToken);
        break;
    case TOKEN_EXPRESSION:
        if (operand1.type == TOKEN_BRACKET_LEFT && operand2.type == TOKEN_BRACKET_RIGHT)
        {
        #ifdef DEBUG
            print("%s", "E->(E)");
        #endif
            //check if variable is defined
            //todo
            //code generator
            stackPush(stack, operation);
        }
        else
        {
            throw_error_fatal(SYNTAX_ERROR, "%s", "Syntax error in expression");
        }
        break;

    case TOKEN_GREATER:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E>E");
        #endif

        //code generator
        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        GT(dest, operand1Symb, operand2Symb, false, symtableVector);
        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        setVarType(getSymTableForVar(symtableVector, expressionToken.value.s.ptr), expressionToken.value.s.ptr, BOOL);
        stackPush(stack, expressionToken);
        break;
    case TOKEN_LESS:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E<E");
        #endif

        //code generator
        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        LT(dest, operand1Symb, operand2Symb, false, symtableVector);

        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        setVarType(getSymTableForVar(symtableVector, expressionToken.value.s.ptr), expressionToken.value.s.ptr, BOOL);
        stackPush(stack, expressionToken);
        break;
    case TOKEN_GREATER_EQ:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E>=E");
        #endif

        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        LT(dest, operand1Symb, operand2Symb, false, symtableVector);
        ASSIGN_OPERAND1SYMB(true, expressionToken.value.s, TEMP_FRAME);
        NOT(dest, operand1Symb, false, symtableVector);

        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        setVarType(getSymTableForVar(symtableVector, expressionToken.value.s.ptr), expressionToken.value.s.ptr, BOOL);
        stackPush(stack, expressionToken);
        break;

    case TOKEN_LESS_EQ:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E<=E");
        #endif

        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        GT(dest, operand1Symb, operand2Symb, false, symtableVector);
        ASSIGN_OPERAND1SYMB(true, expressionToken.value.s, TEMP_FRAME);
        NOT(dest, operand1Symb, false, symtableVector);

        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        setVarType(getSymTableForVar(symtableVector, expressionToken.value.s.ptr), expressionToken.value.s.ptr, BOOL);
        stackPush(stack, expressionToken);
        break;
    case TOKEN_EQUALS:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E==E");
        #endif

        //code generator
        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        EQ(dest, operand1Symb, operand2Symb, false, symtableVector);

        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        setVarType(getSymTableForVar(symtableVector, expressionToken.value.s.ptr), expressionToken.value.s.ptr, BOOL);
        stackPush(stack, expressionToken);
        break;
    case TOKEN_NOT_EQUALS:
        expressionToken = semanticCheckFor3Op(operand1, operation, operand2, symtableVector);
        #ifdef DEBUG
            print("%s", "E->E!=E");
        #endif

        ASSIGN_DEST(TEMP_FRAME, expressionToken.value.s);
        ASSIGN_OPERAND1SYMB(true, operand1.value.s, LOCAL_FRAME);
        ASSIGN_OPERAND2SYMB(true, operand2.value.s, LOCAL_FRAME);
        EQ(dest, operand1Symb, operand2Symb, false, symtableVector);
        ASSIGN_OPERAND1SYMB(true, expressionToken.value.s, TEMP_FRAME);
        NOT(dest, operand1Symb, false, symtableVector);

        removeTokenAfterOp(operand1, symtableVector);
        removeTokenAfterOp(operand2, symtableVector);
        setVarType(getSymTableForVar(symtableVector, expressionToken.value.s.ptr), expressionToken.value.s.ptr, BOOL);
        stackPush(stack, expressionToken);
        break;

    default:
        throw_error_fatal(SYNTAX_ERROR, "%s", "Syntax error in expression");
    }
}

void reduceByRule(Stack *stack, Vector *symtableVector)
{
    int reduceCount = reduceTokenCount(stack);

    Token operand1;
    Token operand2;
    Token operation;

    switch (reduceCount)
    {
    case 1:
        operand1 = stackPop(stack);
        if (stackPop(stack).type != TOKEN_DELIMITER)
            throw_error_fatal(SYNTAX_ERROR, "%s", "Syntax error in expression");
        reduceRules1Op(stack, operand1, symtableVector);
        break;
    case 3:
        operand2 = stackPop(stack);
        operation = stackPop(stack);
        operand1 = stackPop(stack);
        if (stackPop(stack).type != TOKEN_DELIMITER)
            throw_error_fatal(SYNTAX_ERROR, "%s", "Syntax error in expression");
        reduceRules3Op(stack, operand1, operation, operand2, symtableVector);
        break;
    case ANALYSIS_DONE:
        return;
        break;
    default:
        throw_error_fatal(SYNTAX_ERROR, "%s", "Syntax error in expression");
    }
    return;
}

expResult expression(Vector *symtableVector, htab_t *funcTable)
{
    //CREATEFRAME();
    Stack *stack = stackInit();

    Token inputToken;
    Token stackToken;

    Token delimiterToken;
    Token expressionToken;
    Token tmpToken;

    inputToken.type = TOKEN_NONE;
    delimiterToken.type = TOKEN_DELIMITER;
    expressionToken.type = TOKEN_EXPRESSION;

    expResult result;
    result.isFunc = false;
    result.isEmpty = false;

    stackPush(stack, inputToken);
    scanner_get_token(&inputToken);

    //check for func
    if (inputToken.type == TOKEN_IDENTIFIER)
    {
        if (isVarDefined(getSymTableForVar(symtableVector, inputToken.value.s.ptr), inputToken.value.s.ptr) == false)
        {
            if (isFuncDefined(funcTable, inputToken.value.s.ptr) == true)
            {
                result.isFunc = true;
                result.newToken = inputToken;
                return result;
            } 
            else 
                throw_error_fatal(DEFINITION_ERROR, "%s", "Variable is not defined");
        }
    }
    //check if expression is empty
    if (isEndToken(inputToken) == true)
    {
        result.isEmpty = true;
        result.newToken = inputToken;
        return result;
    }
    while (stackPeek(stack).type != TOKEN_NONE || inputToken.type != TOKEN_NONE)
    {
        //check if var is defined
        /*if(inputToken.type == TOKEN_IDENTIFIER)
        {
            if (isVarDefined(getSymTableForVar(symtableVector, inputToken.value.s.ptr), inputToken.value.s.ptr) == false)
                throw_error_fatal(DEFINITION_ERROR, "%s", "Variable is not defined");
        }*/
        //detect end of expression
        if (isEndToken(inputToken) == true)
        {
            result.newToken = inputToken;
            inputToken.type = TOKEN_NONE;
        }
        stackToken = stackPeek(stack);
        //special case of expression on top of stack
        if (stackToken.type == TOKEN_EXPRESSION)
        {
            expressionToken = stackPop(stack);
            stackToken = stackPeek(stack);
            stackPush(stack, expressionToken);
            //check for end of analysis
            if (stackToken.type == TOKEN_NONE && inputToken.type == TOKEN_NONE)
            {
                result.result = expressionToken.value.s;
                break;
            }
        }
        switch (precedenceTable(stackToken, inputToken))
        {
        case PRECEDENCE_SHIFT:
            if (stackPeek(stack).type == TOKEN_EXPRESSION)
            {
                tmpToken = stackPop(stack);
                stackPush(stack, delimiterToken);
                stackPush(stack, tmpToken);
                stackPush(stack, inputToken);
            }
            else
            {
                stackPush(stack, delimiterToken);
                stackPush(stack, inputToken);
            }
            scanner_get_token(&inputToken);
            break;
        case PRECEDENCE_REDUCE:
            reduceByRule(stack, symtableVector);
            break;
        case PRECEDENCE_EQUAL:
            stackPush(stack, inputToken);
            scanner_get_token(&inputToken);
            break;
        case PRECEDENCE_NONE:
            stackFree(stack);
            throw_error_fatal(SYNTAX_ERROR, "%s", "Syntax error in expression");
            break;
        }
    }
    stackFree(stack);
    return result;
}

precedenceSigns precedenceTable(Token stackToken, Token inputToken)
{
    precedenceSigns table[14][14] = {
        {'>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
        {'>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
        {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
        {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
        {'<', '<', '<', '<',  63,  63,  63,  63,  63,  63, '<', '>', '<', '>'},
        {'<', '<', '<', '<',  63,  63,  63,  63,  63,  63, '<', '>', '<', '>'},
        {'<', '<', '<', '<',  63,  63,  63,  63,  63,  63, '<', '>', '<', '>'},
        {'<', '<', '<', '<',  63,  63,  63,  63,  63,  63, '<', '>', '<', '>'},
        {'<', '<', '<', '<',  63,  63,  63,  63,  63,  63, '<', '>', '<', '>'},
        {'<', '<', '<', '<',  63,  63,  63,  63,  63,  63, '<', '>', '<', '>'},
        {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<',  63},
        {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  63, '>',  63, '>'},
        {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  63, '>', '>', '>'},
        {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<',  63, '<',  63},
    };
    precedenceTableIndex a_index = getPrecedenceTableIndex(stackToken);
    precedenceTableIndex b_index = getPrecedenceTableIndex(inputToken);
    if (a_index == 14 || b_index == 14)
    {
        return PRECEDENCE_NONE;
    }
    return table[a_index][b_index];
}

precedenceTableIndex getPrecedenceTableIndex(Token token)
{
    TokenType val = token.type;
    switch (val)
    {
    case TOKEN_PLUS:
        return 0;
        break;
    case TOKEN_MINUS:
        return 1;
        break;
    case TOKEN_MUL:
        return 2;
        break;
    case TOKEN_DIV:
        return 3;
        break;
    case TOKEN_GREATER:
        return 4;
        break;
    case TOKEN_LESS:
        return 5;
        break;
    case TOKEN_LESS_EQ:
        return 6;
        break;
    case TOKEN_GREATER_EQ:
        return 7;
        break;
    case TOKEN_EQUALS:
        return 8;
        break;
    case TOKEN_NOT_EQUALS:
        return 9;
        break;
    case TOKEN_BRACKET_LEFT:
        return 10;
        break;
    case TOKEN_BRACKET_RIGHT:
        return 11;
        break;
    case TOKEN_IDENTIFIER:
    case TOKEN_NUMBER_INT:
    case TOKEN_NUMBER_FLOAT:
    case TOKEN_STRING:
        return 12;
        break;
    case TOKEN_NONE:
        return 13;
        break;
    default:
        throw_error(SYNTAX_ERROR, "Unexpected token in expression, token type: %d", val);
        return 14;
        break;
    }
}
