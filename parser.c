/* IFJ20 - Parser
 * Authors:
 * Erik Belko, xbelko02
 * Michal Slesar, xslesa01
 */

#include "parser.h"

#include <stdio.h>

#include "file.h"
#include "error.h"
#include "code_gen.h"
#include "semantic_analysis.h"

/**
 * Load next token from scanner or stack if it is not empty
 * 
 * @param data ParserData instance
 */
void load_token(ParserData *data) {
    if(stackIsEmpty(data->stack)) {
        scanner_get_token(&data->token);

        /** Add string to vector of allocated string so we can free them later */
        if(data->token.type == TOKEN_STRING || data->token.type == TOKEN_IDENTIFIER)
            vectorPush(data->strings, (void *) data->token.value.s.ptr);
    } else {
        data->token = stackPop(data->stack);
    }
}

/**
 * Push token to stack
 * 
 * @param data ParserData instance
 * @param token Token to push
 */
void push_token(ParserData *data, Token token) {
    stackPush(data->stack, token);
}

/**
 * Load token and compare it with token type
 * 
 * @param data ParserData instance
 * @param type Token type
 * @param push Push token to stack if the type does not match
 */
bool load_and_compare(ParserData *data, TokenType type, bool push) {
    load_token(data);

    if(data->token.type != type) {
        if(push) {
            push_token(data, data->token);
        }
        return false;
    }

    return true;
}

/**
 * Skip all consecutive eols
 * 
 * @param data ParserData instance
 */
void optional_eol(ParserData *data) {
    while(true) {
        load_token(data);
        if(data->token.type != TOKEN_EOL) {
            push_token(data, data->token);
            break;
        }
    }
}

/**
 * Free allocated strings
 * 
 * @param data ParserData instance
 */
void free_strings(ParserData *data) {
    while(vectorLength(data->strings) != 0)
        free(vectorPop(data->strings));
}

/**
 * Free ParserData resources
 * 
 * @param data ParserData instance
 */
void free_parser(ParserData *data) {
    htab_free(data->table);
    free_strings(data);

    vectorFree(data->scopes);
    vectorFree(data->strings);
    stackFree(data->stack);
    arrFree(data->file);
}

/** Wrap throw_error_fatal so it cleans up ParserData before exit */
#define throw_error_parser(data, err_no, fmt, ...) \
    do { \
        free_parser(data); \
        throw_error_fatal(err_no, fmt, ##__VA_ARGS__); \
    } while(0)

/**
 * Parse function body in first scan
 * 
 * @param data ParserData instance
 * @param first First call
 */
bool wait_for_scope_end(ParserData *data, bool first) {
    while(true) {
        load_token(data);
        if(data->token.type == TOKEN_EOF)
            return false;
        if(data->token.type == TOKEN_BRACE_LEFT) {
            if(!wait_for_scope_end(data, false))
                return false;
        } else if(data->token.type == TOKEN_BRACE_RIGHT) {
            if(first) push_token(data, data->token);
            return true;
        }
    }
}

void ruleProgram(ParserData *data) {
    optional_eol(data);
    if(!load_and_compare(data, TOKEN_KEYWORD, false) || data->token.value.k != KEYWORD_PACKAGE)
        throw_error_parser(data, SYNTAX_ERROR, "%s", "Missing prolog");

    if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);
    if(!string_compare(&data->token.value.s, "main"))
        throw_error_parser(data, SYNTAX_ERROR, "%s", "Wrong package name");

    if(!load_and_compare(data, TOKEN_EOL, false)) /** Required EOL after prolog */
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);

    optional_eol(data);
    ruleBody(data);
    optional_eol(data);
    
    if(!load_and_compare(data, TOKEN_EOF, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_EOF, got token type %d", data->token.type);

    if(data->isFirstScan) {
        /** We should check if main exists in first scan */
        if(!isFuncDefined(data->table, "main"))
            throw_error_parser(data, DEFINITION_ERROR, "%s", "Missing function main");
        else {
            Vector *empty = vectorInit();
            Vector *params = getFuncParamTypes(data->table, "main");
            Vector *returns = getFuncReturnTypes(data->table, "main");

            if(!checkTypes(params, empty))
                throw_error_parser(data, FUNCTION_DEFINITION_ERROR, "%s", "Invalid number of arguments for main function");
            if(!checkTypes(returns, empty))
                throw_error_parser(data, FUNCTION_DEFINITION_ERROR, "%s", "Invalid number of return parameters for main function");
        }
    }
}

void ruleBody(ParserData *data) {
    ruleFuncN(data);
}

void ruleFuncN(ParserData *data) {
    optional_eol(data);
    if(!load_and_compare(data, TOKEN_KEYWORD, true)) 
        return;

    if(data->token.value.k != KEYWORD_FUNC)
        throw_error_parser(data, SYNTAX_ERROR, "Expected KEYWORD_FUNC, got token type %d", data->token.type);

    push_token(data, data->token);
    ruleFunc(data);
    ruleFuncN(data);
}

void ruleFunc(ParserData *data) {
    if(!load_and_compare(data, TOKEN_KEYWORD, false) || data->token.value.k != KEYWORD_FUNC)
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_KEYWORD - FUNC, got token type %d", data->token.type);

    if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);

    /** Create new scope */
    insertLocalSymTable(data->scopes);

    String func_name = data->token.value.s;

    Vector *params = vectorInit();
    Vector *names = vectorInit();
    Vector *returns = vectorInit();

    if(!load_and_compare(data, TOKEN_BRACKET_LEFT, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACKET_LEFT, got token type %d", data->token.type);
    ruleParams(data, params, names);
    if(!load_and_compare(data, TOKEN_BRACKET_RIGHT, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACKET_RIGHT, got token type %d", data->token.type);

    ruleRetTypes(data, returns);

    /** In first scan we only want to define function, in second scan we want to generate it */
    if(data->isFirstScan)
        defineFunc(data->table, func_name.ptr, params, returns);
    else
        declare_function(func_name.ptr, names, data->scopes);

    if(!load_and_compare(data, TOKEN_BRACE_LEFT, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACE_LEFT, got token type %d", data->token.type);

    if(!load_and_compare(data, TOKEN_EOL, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);

    data->inFunction = true;
    data->returned = false;
    data->function = func_name;

    /** In first scan we do not care about function body, so we only parse function declarations */
    if(data->isFirstScan) {
        if(!wait_for_scope_end(data, true))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);
    } else {
        ruleStList(data);

        /** Void functions can have implicit return */
        if(getFuncReturnTypes(data->table, data->function.ptr)->length == 0) {
            /** Return in main is treated differently then in functions */
            if(string_compare(&data->function, "main"))
                main_end();
            else
                return_function(vectorInit(), data->scopes);
        } else if(!data->returned)
            throw_error_parser(data, FUNCTION_DEFINITION_ERROR, "Missing return in function %s", data->function.ptr);
    }

    /** Remove current scope */
    removeLocalSymTable(data->scopes);

    data->inFunction = false;

    if(!load_and_compare(data, TOKEN_BRACE_RIGHT, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);

    /** If this is the last function, there does not need to be EOL */
    if(load_and_compare(data, TOKEN_EOF, true)) {
        push_token(data, data->token);
        return;
    }

    /** There is something after this function, so expect EOL */
    if(!load_and_compare(data, TOKEN_EOL, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);
}

void ruleParams(ParserData *data, Vector *params, Vector *names) {
    if(!load_and_compare(data, TOKEN_IDENTIFIER, true)) 
        return;
    if(string_compare(&data->token.value.s, "_")) 
        throw_error_parser(data, OTHER_SEMANTIC_ERROR, "%s", "Can not define variable with name _");

    String name = data->token.value.s;
    varDataType type = ruleType(data, params);

    /** We want to save params only in first scan, in which we are parsing function declarations */
    if(!data->isFirstScan) {
        TokenValue value;
        vectorPush(names, (void *) name.ptr);
        defineUserVar(getLocalSymTable(data->scopes), name.ptr, type, value, false);
    }

    ruleParamsN(data, params, names);
}

void ruleParamsN(ParserData *data, Vector *params, Vector *names) {
    if(!load_and_compare(data, TOKEN_COMA, true)) 
        return;
    if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);
    if(string_compare(&data->token.value.s, "_")) 
        throw_error_parser(data, OTHER_SEMANTIC_ERROR, "%s", "Can not define variable with name _");

    String name = data->token.value.s;
    varDataType type = ruleType(data, params);

    /** We want to save params only in first scan, in which we are parsing function declarations */
    if(!data->isFirstScan) {
        TokenValue value;
        vectorPush(names, (void *) name.ptr);
        defineUserVar(getLocalSymTable(data->scopes), name.ptr, type, value, false);
    }

    ruleParamsN(data, params, names);
}

varDataType ruleType(ParserData *data, Vector *types) {
    if(!load_and_compare(data, TOKEN_KEYWORD, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_KEYWORD, got token type %d", data->token.type);

    if ((data->token.value.k == KEYWORD_INT) || (data->token.value.k == KEYWORD_FLOAT64) || (data->token.value.k == KEYWORD_STRING)) {
        varDataType type;

        if(data->token.value.k == KEYWORD_INT) { type = INTEGER; }
        else if(data->token.value.k == KEYWORD_FLOAT64) { type = FLOAT; }
        else if(data->token.value.k == KEYWORD_STRING) { type = STRING; }

        addFuncType(types, type);
        return type;
    } else {
        throw_error_parser(data, SYNTAX_ERROR, "Expected KEYWORD_INT|KEYWORD_FLOAT64|KEYWORD_STRING, got token type %d", data->token.type);
    }
}

void ruleRetTypes(ParserData *data, Vector *returns) {
    if(!load_and_compare(data, TOKEN_BRACKET_LEFT, true))
        return;
    if(load_and_compare(data, TOKEN_BRACKET_RIGHT, true))
        return;

    ruleType(data, returns);
    ruleRetTypesN(data, returns);

    if(!load_and_compare(data, TOKEN_BRACKET_RIGHT, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACKET_RIGHT, got token type %d", data->token.type);
}

void ruleRetTypesN(ParserData *data, Vector *returns) {
    if(!load_and_compare(data, TOKEN_COMA, true))
        return;

    ruleType(data, returns);
    ruleRetTypesN(data, returns);
}

void ruleStList(ParserData *data) {
    optional_eol(data);
    if(ruleStat(data)) {
        if(!load_and_compare(data, TOKEN_EOL, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);
        ruleStList(data);
    }
}

bool ruleStat(ParserData *data) {
    load_token(data);

    if(data->token.type == TOKEN_IDENTIFIER) {
        ruleStatBody(data, data->token);
        return true;
    } else if(data->token.type == TOKEN_KEYWORD && data->token.value.k == KEYWORD_IF) {
        expResult result = ruleExp(data, false, false);
        
        /** There needs to be bool expression in the condition */
        if(getVarType(getSymTableForVar(data->scopes, result.result.ptr), result.result.ptr) != BOOL)
            throw_error_parser(data, INCOMPATIBLE_EXPRESSION_ERROR, "%s", "Incompatible expression error");

        if_start(result.result.ptr, data->scopes);

        if(!load_and_compare(data, TOKEN_BRACE_LEFT, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACE_LEFT, got token type %d", data->token.type);
        if(!load_and_compare(data, TOKEN_EOL, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);
        optional_eol(data);
        /** Create new scope for if body */
        insertLocalSymTable(data->scopes);
        ruleStList(data);
        /** Remove if body scope */
        removeLocalSymTable(data->scopes);
        if(!load_and_compare(data, TOKEN_BRACE_RIGHT, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);

        if(!load_and_compare(data, TOKEN_KEYWORD, false) || data->token.value.k != KEYWORD_ELSE)
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_KEYWORD - ELSE, got token type %d", data->token.type);

        if(!load_and_compare(data, TOKEN_BRACE_LEFT, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACE_LEFT, got token type %d", data->token.type);
        if(!load_and_compare(data, TOKEN_EOL, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);

        if_core();

        optional_eol(data);
        /** Create new scope for else body */
        insertLocalSymTable(data->scopes);
        ruleStList(data);
        /** Remove else body scope */
        removeLocalSymTable(data->scopes);
        if(!load_and_compare(data, TOKEN_BRACE_RIGHT, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);

        if_end();
        return true;
    } else if(data->token.type == TOKEN_KEYWORD && data->token.value.k == KEYWORD_FOR) {
        /** Create new scope for header of for */
        insertLocalSymTable(data->scopes);
        ruleForDef(data);
        if(!load_and_compare(data, TOKEN_SEMICOLON, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_SEMICOLON, got token type %d", data->token.type);

        for_start();

        expResult result = ruleExp(data, false, false);
        /** There needs to be bool expression in the condition */ 
        if(getVarType(getSymTableForVar(data->scopes, result.result.ptr), result.result.ptr) != BOOL)
            throw_error_parser(data, INCOMPATIBLE_EXPRESSION_ERROR, "%s", "Incompatible expression error");
        if(!load_and_compare(data, TOKEN_SEMICOLON, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_SEMICOLON, got token type %d", data->token.type);

        Var exp_result = {.name = result.result, .frame = TEMP_FRAME};
        for_expression(exp_result, data->scopes);
        for_assign_start();
        ruleForAssign(data);
        for_body();

        if(!load_and_compare(data, TOKEN_BRACE_LEFT, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACE_LEFT, got token type %d", data->token.type);
        if(!load_and_compare(data, TOKEN_EOL, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);
        optional_eol(data);
        /** Create new scope for the body of for */
        insertLocalSymTable(data->scopes);
        ruleStList(data);
        /** Remove for body scope */
        removeLocalSymTable(data->scopes);
        /** Remove for header scope */
        removeLocalSymTable(data->scopes);

        if(!load_and_compare(data, TOKEN_BRACE_RIGHT, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);

        for_end();
        return true;
    } else if(data->token.type == TOKEN_KEYWORD && data->token.value.k == KEYWORD_RETURN) {
        ruleReturnExp(data);
        return true;
    } else {
        push_token(data, data->token);
        return false;
    }
}

void ruleStatBody(ParserData *data, Token id) {
    load_token(data);

    htab_t *scope = getLocalSymTable(data->scopes);
    if(data->token.type == TOKEN_DECLARATION) {
        if(string_compare(&id.value.s, "_")) 
            throw_error_parser(data, OTHER_SEMANTIC_ERROR, "%s", "Can not define variable with name _");

        if(isVarUserDefined(scope, id.value.s.ptr))
            throw_error_parser(data, DEFINITION_ERROR, "Variable %s already defined", id.value.s.ptr);

        expResult result = ruleExp(data, false, false);
        htab_t *exp_scope = getSymTableForVar(data->scopes, result.result.ptr);

        if(getVarType(exp_scope, result.result.ptr) == BOOL)
            throw_error_parser(data, TYPE_DEFINITION_ERROR, "%s", "Bool variable declaration not supported");

        TokenValue value;
        if(isVarConst(exp_scope, result.result.ptr))
            value = getVarValue(exp_scope, result.result.ptr);
        else
            value.i = 0;

        defineUserVar(scope, id.value.s.ptr, getVarType(exp_scope, result.result.ptr), value, isVarConst(exp_scope, result.result.ptr));

        Var target = {.name = id.value.s, .frame = LOCAL_FRAME};
        Var source = {.name = result.result, .frame = LOCAL_FRAME};
        Symb symbol = {.isVar = true, .var = source};
        define_var(target, symbol, data->scopes);
    } else if(data->token.type == TOKEN_BRACKET_LEFT) {
        Vector *types = vectorInit();
        Vector *names = vectorInit();

        optional_eol(data);

        ruleCallParams(data, names, types);

        if(!load_and_compare(data, TOKEN_BRACKET_RIGHT, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACKET_RIGHT, got token type %d", data->token.type);

        if(!isFuncDefined(data->table, id.value.s.ptr))
            throw_error_parser(data, DEFINITION_ERROR, "Function %s not defined", id.value.s.ptr);

        if(getSymTableForVar(data->scopes, id.value.s.ptr) != NULL)
            throw_error_parser(data, OTHER_SEMANTIC_ERROR, "Function %s is shadowed by variable", id.value.s.ptr);

        if(getFuncReturnTypes(data->table, id.value.s.ptr)->length != 0)
            throw_error_parser(data, FUNCTION_DEFINITION_ERROR, "Function %s cannot be called as a void function", id.value.s.ptr);

        /** We do not support variadic functions, so print is handled differently */
        if(string_compare(&id.value.s, "print")) {
            for(unsigned i = 0; i < names->length; i++) {
                Var source = {.name.ptr = (char *) vectorGet(names, i), .frame = LOCAL_FRAME};
                Symb symbol = {.isVar = true, .var = source};
                PRINT(symbol, data->scopes);
            }
        } else {
            if(!checkTypes(getFuncParamTypes(data->table, id.value.s.ptr), types))
                throw_error_parser(data, FUNCTION_DEFINITION_ERROR, "%s", "Incorrect data types in function call");

            call_function(id.value.s.ptr, names, vectorInit(), data->scopes);
        }
    } else {
        push_token(data, data->token);

        Vector *ltypes = vectorInit();
        Vector *lnames = vectorInit();

        vectorPush(lnames, (void *) id.value.s.ptr);

        ruleIdN(data, lnames);

        if(!load_and_compare(data, TOKEN_ASSIGNMENT, false))
            throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_ASSIGNMENT, got token type %d", data->token.type);

        /** Get types of loaded variables */
        for(unsigned i = 0; i < lnames->length; i++) {
            char *name = (char *) vectorGet(lnames, i);
            if(strcmp(name, "_") == 0) {
                addFuncType(ltypes, NONE);
            } else {
                htab_t *scope = getSymTableForVar(data->scopes, name);
                if(scope == NULL)
                    throw_error_parser(data, DEFINITION_ERROR, "Variable %s not defined", name);

                addFuncType(ltypes, getVarType(scope, name));
            }
        }

        expResult result = ruleExp(data, false, true);

        /** There is function on the right side */
        if(result.isFunc) {
            if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
                throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);

            String function = data->token.value.s;

            if(!load_and_compare(data, TOKEN_BRACKET_LEFT, false))
                throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACKET_LEFT, got token type %d", data->token.type);

            optional_eol(data);

            Vector *types = vectorInit();
            Vector *names = vectorInit();

            ruleCallParams(data, names, types);

            if(!load_and_compare(data, TOKEN_BRACKET_RIGHT, false))
                throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_BRACKET_RIGHT, got token type %d", data->token.type);

            if(!load_and_compare(data, TOKEN_EOL, true))
                throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);

            push_token(data, data->token);

            if(!isFuncDefined(data->table, function.ptr))
                throw_error_parser(data, DEFINITION_ERROR, "Function %s not defined", id.value.s.ptr);
            if(getSymTableForVar(data->scopes, function.ptr) != NULL)
                throw_error_parser(data, OTHER_SEMANTIC_ERROR, "Function %s is shadowed by variable", id.value.s.ptr);
            if(!checkTypes(getFuncParamTypes(data->table, function.ptr), types))
                throw_error_parser(data, FUNCTION_DEFINITION_ERROR, "%s", "Incorrect data types in function call");
            if(!checkTypes(getFuncReturnTypes(data->table, function.ptr), ltypes))
                throw_error_parser(data, FUNCTION_DEFINITION_ERROR, "%s", "Incorrect data types in function call return");

            /** We do not know what function returns, so we need to set variables on the left side as non-constant */
            for(unsigned i = 0; i < lnames->length; i++) {
                char *target_name = (char *) vectorGet(lnames, i);

                if(strcmp(target_name, "_") != 0) {
                    htab_t *scope = getSymTableForVar(data->scopes, target_name);
                    setVarConst(scope, target_name, false);
                }
            }

            call_function(function.ptr, names, lnames, data->scopes);
        } else {
            Vector *rtypes = vectorInit();
            Vector *rnames = vectorInit();

            vectorPush(rnames, (void *) result.result.ptr);
            addFuncType(rtypes, getVarType(getSymTableForVar(data->scopes, result.result.ptr), result.result.ptr));

            ruleExpN(data, rnames, rtypes);

            if(!checkTypes(ltypes, rtypes))
                throw_error_parser(data, OTHER_SEMANTIC_ERROR, "%s", "Type mismatch");

            for(unsigned i = 0; i < lnames->length; i++) {
                char *target_name = (char *) vectorGet(lnames, i);
                char *exp_name = (char *) vectorGet(rnames, i);

                if(strcmp(target_name, "_") != 0) {
                    htab_t *scope = getSymTableForVar(data->scopes, target_name);
                    htab_t *exp_scope = getSymTableForVar(data->scopes, exp_name);

                    bool isConst = isVarConst(exp_scope, exp_name);
                    setVarConst(scope, target_name, isConst);
                    
                    if(isConst)
                        setVarValue(scope, target_name, getVarValue(exp_scope, exp_name));

                    Var target = {.name.ptr = target_name, .frame = LOCAL_FRAME};
                    Var source = {.name.ptr = exp_name, .frame = LOCAL_FRAME};
                    Symb symbol = {.isVar = true, .var = source};
                    MOVE(target, symbol, data->scopes);
                }
            }
        }
    }
}

void ruleIdN(ParserData *data, Vector *names) {
    if(!load_and_compare(data, TOKEN_COMA, true))
        return;
    if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);

    vectorPush(names, (void *) data->token.value.s.ptr);

    ruleIdN(data, names);
}

void ruleExpN(ParserData *data, Vector *names, Vector *types) {
    if(!load_and_compare(data, TOKEN_COMA, true))
        return;
    expResult result = ruleExp(data, false, false);

    vectorPush(names, (void *) result.result.ptr);
    addFuncType(types, getVarType(getSymTableForVar(data->scopes, result.result.ptr), result.result.ptr));

    ruleExpN(data, names, types);
}

void ruleReturnExp(ParserData *data) {
    Vector *types = vectorInit();
    Vector *names = vectorInit();

    expResult result = ruleExp(data, true, false);

    if(!result.isEmpty) {
        vectorPush(names, (void *) result.result.ptr);
        addFuncType(types, getVarType(getSymTableForVar(data->scopes, result.result.ptr), result.result.ptr));
    } else if(load_and_compare(data, TOKEN_COMA, true)) {
        /** First expression can be empty only when it is return of void function */
        throw_error_parser(data, SYNTAX_ERROR, "%s", "Expected non-empty expression, got empty");
    }

    ruleExpN(data, names, types);

    if(!checkTypes(getFuncReturnTypes(data->table, data->function.ptr), types))
        throw_error_parser(data, FUNCTION_DEFINITION_ERROR, "%s", "Type mismatch");

    data->returned = true;
    /** Return in main is treated differently then in functions */
    if(string_compare(&data->function, "main"))
        main_end();
    else
        return_function(names, data->scopes);
}

void ruleCallParams(ParserData *data, Vector *names, Vector *types) {
    if(ruleValues(data, names, types)) {
        ruleCallParamsN(data, names, types);
    } else {
        push_token(data, data->token);
    }
}

void ruleCallParamsN(ParserData *data, Vector *names, Vector *types) {
    if(!load_and_compare(data, TOKEN_COMA, true))
        return;

    optional_eol(data);

    if(ruleValues(data, names, types)) {
        ruleCallParamsN(data, names, types);
    } else {
        throw_error_parser(data, SYNTAX_ERROR, "Expected VALUE, got token type %d", data->token.type);
    }
}

bool ruleValues(ParserData *data, Vector *names, Vector *types) {
    load_token(data);

    if(data->token.type != TOKEN_IDENTIFIER && data->token.type != TOKEN_NUMBER_FLOAT && data->token.type != TOKEN_NUMBER_INT && data->token.type != TOKEN_STRING)
        return false;
    else {
        if(data->token.type == TOKEN_IDENTIFIER) {
            if(string_compare(&data->token.value.s, "_"))
                throw_error_parser(data, DEFINITION_ERROR, "%s", "Cannot use _ in function call");

            htab_t *scope = getSymTableForVar(data->scopes, data->token.value.s.ptr);
            if(scope == NULL)
                throw_error_parser(data, DEFINITION_ERROR, "Variable %s not defined", data->token.value.s.ptr);

            vectorPush(names, (void *) data->token.value.s.ptr);
            addFuncType(types, getVarType(scope, data->token.value.s.ptr));
        } else {
            varDataType type;

            if(data->token.type == TOKEN_STRING) type = STRING;
            else if(data->token.type == TOKEN_NUMBER_FLOAT) type = FLOAT;
            else if(data->token.type == TOKEN_NUMBER_INT) type = INTEGER;

            String name = defineCompilerVar(getLocalSymTable(data->scopes), type, data->token.value, true);
            vectorPush(names, (void *) name.ptr);
            addFuncType(types, type);

            Var target = {.name = name, .frame = LOCAL_FRAME};
            Symb symbol = {.isVar = false, .token = data->token};
            define_var(target, symbol, data->scopes);
        }
        return true;
    }
}

void ruleForDef(ParserData *data) {
    if(!load_and_compare(data, TOKEN_IDENTIFIER, true))
        return;

    Token id = data->token;
    
    if(!load_and_compare(data, TOKEN_DECLARATION, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_DECLARATION, got token type %d", data->token.type);

    if(string_compare(&id.value.s, "_")) 
        throw_error_parser(data, OTHER_SEMANTIC_ERROR, "%s", "Can not define variable with name _");

    expResult result = ruleExp(data, false, false);
    htab_t *exp_scope = getSymTableForVar(data->scopes, result.result.ptr);

    if(getVarType(exp_scope, result.result.ptr) == BOOL)
        throw_error_parser(data, TYPE_DEFINITION_ERROR, "%s", "Bool variable declaration not supported");

    TokenValue value;
    if(isVarConst(exp_scope, result.result.ptr))
        value = getVarValue(exp_scope, result.result.ptr);
    else
        value.i = 0;

    defineUserVar(getLocalSymTable(data->scopes), id.value.s.ptr, getVarType(exp_scope, result.result.ptr), value, isVarConst(exp_scope, result.result.ptr));

    Var target = {.name = id.value.s, .frame = LOCAL_FRAME};
    Var source = {.name = result.result, .frame = LOCAL_FRAME};
    Symb symbol = {.isVar = true, .var = source};
    define_var(target, symbol, data->scopes);
}

void ruleForAssign(ParserData *data) {
    if(!load_and_compare(data, TOKEN_IDENTIFIER, true))
        return;

    Vector *ltypes = vectorInit();
    Vector *lnames = vectorInit();

    vectorPush(lnames, (void *) data->token.value.s.ptr);

    ruleIdN(data, lnames);

    if(!load_and_compare(data, TOKEN_ASSIGNMENT, false))
        throw_error_parser(data, SYNTAX_ERROR, "Expected TOKEN_ASSIGNMENT, got token type %d", data->token.type);

    /** Get types of loaded variables */
    for(unsigned i = 0; i < lnames->length; i++) {
        char *name = (char *) vectorGet(lnames, i);
        if(strcmp(name, "_") == 0) {
            addFuncType(ltypes, NONE);
        } else {
            htab_t *scope = getSymTableForVar(data->scopes, name);
            if(scope == NULL)
                throw_error_parser(data, DEFINITION_ERROR, "Variable %s not defined", name);

            addFuncType(ltypes, getVarType(scope, name));
        }
    }

    Vector *rtypes = vectorInit();
    Vector *rnames = vectorInit();

    expResult result = ruleExp(data, false, false);
    vectorPush(rnames, (void *) result.result.ptr);
    addFuncType(rtypes, getVarType(getSymTableForVar(data->scopes, result.result.ptr), result.result.ptr));

    ruleExpN(data, rnames, rtypes);

    if(!checkTypes(ltypes, rtypes))
        throw_error_parser(data, OTHER_SEMANTIC_ERROR, "%s", "Type mismatch");

    for(unsigned i = 0; i < lnames->length; i++) {
        char *target_name = (char *) vectorGet(lnames, i);
        char *exp_name = (char *) vectorGet(rnames, i);

        if(strcmp(target_name, "_") != 0) {
            htab_t *scope = getSymTableForVar(data->scopes, target_name);
            htab_t *exp_scope = getSymTableForVar(data->scopes, exp_name);

            bool isConst = isVarConst(exp_scope, exp_name);
            setVarConst(scope, target_name, isConst);
            
            if(isConst)
                setVarValue(scope, target_name, getVarValue(exp_scope, exp_name));

            Var target = {.name.ptr = target_name, .frame = LOCAL_FRAME};
            Var source = {.name.ptr = exp_name, .frame = LOCAL_FRAME};
            Symb symbol = {.isVar = true, .var = source};
            MOVE(target, symbol, data->scopes);
        }
    }
}

expResult ruleExp(ParserData *data, bool allowEmpty, bool allowFunc) {
    expResult result = expression(data->scopes, data->table);

    if(!allowEmpty && result.isEmpty)
        throw_error_parser(data, SYNTAX_ERROR, "%s", "Expected non-empty expression, got empty");

    if(!allowFunc && result.isFunc)
        throw_error_parser(data, SYNTAX_ERROR, "%s", "Expected expression, got func");

    /** Add string to vector of allocated string so we can free them later */
    if(result.newToken.type == TOKEN_STRING || result.newToken.type == TOKEN_IDENTIFIER)
        vectorPush(data->strings, (void *) result.newToken.value.s.ptr);

    push_token(data, result.newToken);
    return result;
}

void register_functions(ParserData *data) {
    /** func inputs() (string,int) */
    Vector *inputs_returns = vectorInit();
    addFuncType(inputs_returns, STRING);
    addFuncType(inputs_returns, INTEGER); 
    defineFunc(data->table, "inputs", vectorInit(), inputs_returns);

    /** func inputi() (int,int) */
    Vector *inputi_returns = vectorInit();
    addFuncType(inputi_returns, INTEGER);
    addFuncType(inputi_returns, INTEGER);
    defineFunc(data->table, "inputi", vectorInit(), inputi_returns);

    /** func inputf() (float64,int) */
    Vector *inputf_returns = vectorInit();
    addFuncType(inputf_returns, FLOAT);
    addFuncType(inputf_returns, INTEGER);
    defineFunc(data->table, "inputf", vectorInit(), inputf_returns);

    /** func int2float(i int) (float64) */
    Vector *int2float_params = vectorInit();
    Vector *int2float_returns = vectorInit();
    addFuncType(int2float_params, INTEGER);
    addFuncType(int2float_returns, FLOAT);
    defineFunc(data->table, "int2float", int2float_params, int2float_returns);

    /** func float2int(f float64) (int) */
    Vector *float2int_params = vectorInit();
    Vector *float2int_returns = vectorInit();
    addFuncType(float2int_params, FLOAT);
    addFuncType(float2int_returns, INTEGER);
    defineFunc(data->table, "float2int", float2int_params, float2int_returns);

    /** func len(𝑠 string) (int) */
    Vector *len_params = vectorInit();
    Vector *len_returns = vectorInit();
    addFuncType(len_params, STRING);
    addFuncType(len_returns, INTEGER);
    defineFunc(data->table, "len", len_params, len_returns);

    /** func substr(s string, i int, n int) (string, int) */
    Vector *substr_params = vectorInit();
    Vector *substr_returns = vectorInit();
    addFuncType(substr_params, STRING);
    addFuncType(substr_params, INTEGER);
    addFuncType(substr_params, INTEGER);
    addFuncType(substr_returns, STRING);
    addFuncType(substr_returns, INTEGER);
    defineFunc(data->table, "substr", substr_params, substr_returns);

    /** func ord(s string, i int) (int, int) */
    Vector *ord_params = vectorInit();
    Vector *ord_returns = vectorInit();
    addFuncType(ord_params, STRING);
    addFuncType(ord_params, INTEGER);
    addFuncType(ord_returns, INTEGER);
    addFuncType(ord_returns, INTEGER);
    defineFunc(data->table, "ord", ord_params, ord_returns);

    /** func chr(i int) (string, int) */
    Vector *chr_params = vectorInit();
    Vector *chr_returns = vectorInit();
    addFuncType(chr_params, INTEGER);
    addFuncType(chr_returns, STRING);
    addFuncType(chr_returns, INTEGER);
    defineFunc(data->table, "chr", chr_params, chr_returns);

    /** func print ( term1 , term2 , …, termn ) */
    defineFunc(data->table, "print", vectorInit(), vectorInit());
}

void parse() {
    ParserData data;

    /** Load stdin */
    data.file = arrInit();
    copyStdinToArr(data.file);
    
    /** Setup first scan */
    scanner_set_file(data.file);
    data.isFirstScan = true;
    data.scopes = vectorInit();
    data.strings = vectorInit();
    data.table = htab_init(10);
    data.stack = stackInit();
    register_functions(&data);

    /** Run first scan */
    ruleProgram(&data);

    /** Cleanup after first scan */
    free_strings(&data);
    
    /** Setup second scan */
    gen_init();
    data.isFirstScan = false;
    arrSeekStart(data.file);

    /** Run second scan */
    ruleProgram(&data);
    program_end();

    /** Free allocated resources */
    free_parser(&data);
}