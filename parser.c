/* IFJ20 - Parser
 * Authors:
 * Erik Belko, xbelko02
 * Michal Slesar, xslesa01
 */

#include "parser.h"
#include "file.h"
#include "code_gen.h"

/**
 * FIXNUT CISLOVANIE PRAVIDIEL + KONTROLA
 * FIXNUT EOLY
 */

void load_token(ParserData *data) {
    if(stackIsEmpty(data->stack)) {
        scanner_get_token(&data->token);
    } else {
        data->token = stackPop(data->stack);
    }
}

void push_token(ParserData *data, Token token) {
    stackPush(data->stack, token);
}

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

void optional_eol(ParserData *data) {
    while(true) {
        load_token(data);
        if(data->token.type != TOKEN_EOL) {
            push_token(data, data->token);
            break;
        }
    }
}

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

/*  1: <program> -> package id EOL <body> EOF  */
void ruleProgram(ParserData *data) {
    optional_eol(data);
    if(!load_and_compare(data, TOKEN_KEYWORD, false) || data->token.value.k != KEYWORD_PACKAGE)
        throw_error_fatal(SYNTAX_ERROR, "%s", "Missing prolog");

    if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);
    if(!string_compare(&data->token.value.s, "main"))
        throw_error_fatal(SYNTAX_ERROR, "%s", "Wrong package name");

    if(!load_and_compare(data, TOKEN_EOL, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);

    optional_eol(data);
    ruleBody(data);
    optional_eol(data);
    
    if(!load_and_compare(data, TOKEN_EOF, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_EOF, got token type %d", data->token.type);

    if(data->isFirstScan) {
        if(!isFuncDefined(data->table, "main"))
            throw_error_fatal(DEFINITION_ERROR, "%s", "Missing function main");
        else {
            Vector *empty = vectorInit();
            Vector *params = getFuncParamTypes(data->table, "main");
            Vector *returns = getFuncReturnTypes(data->table, "main");

            if(!checkTypes(params, empty))
                throw_error_fatal(FUNCTION_DEFINITION_ERROR, "%s", "Invalid number of arguments for main function");
            if(!checkTypes(returns, empty))
                throw_error_fatal(FUNCTION_DEFINITION_ERROR, "%s", "Invalid number of return parameters for main function");
        }
    }
}

/*  2: <body> -> <func> <func_n>  */
void ruleBody(ParserData *data) {
    ruleFunc(data);
    ruleFuncN(data);
}

/*  3: <func_n> -> EOL <func> <func_n>
    4: <func_n> -> eps  */
void ruleFuncN(ParserData *data) {
    if(!load_and_compare(data, TOKEN_EOL, true))
        return;
    optional_eol(data);
    if(!load_and_compare(data, TOKEN_KEYWORD, true)) 
        return;

    if(data->token.value.k != KEYWORD_FUNC)
        throw_error_fatal(SYNTAX_ERROR, "Expected KEYWORD_FUNC, got token type %d", data->token.type);

    push_token(data, data->token);
    ruleFunc(data);
    ruleFuncN(data);
}

/*  5: <func> -> func id ( <params> ) <ret_types> { EOL <st_list> }  */
void ruleFunc(ParserData *data) {
    if(!load_and_compare(data, TOKEN_KEYWORD, false) || data->token.value.k != KEYWORD_FUNC)
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_KEYWORD - FUNC, got token type %d", data->token.type);

    if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);

    insertLocalSymTable(data->scopes);

    String func_name = data->token.value.s;

    Vector *params = vectorInit();
    Vector *names = vectorInit();
    Vector *returns = vectorInit();

    if(!load_and_compare(data, TOKEN_BRACKET_LEFT, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACKET_LEFT, got token type %d", data->token.type);
    ruleParams(data, params, names);
    if(!load_and_compare(data, TOKEN_BRACKET_RIGHT, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACKET_RIGHT, got token type %d", data->token.type);

    ruleRetTypes(data, returns);

    if(data->isFirstScan)
        defineFunc(data->table, func_name.ptr, params, returns);
    else
        declare_function(func_name.ptr, names, data->scopes);

    if(!load_and_compare(data, TOKEN_BRACE_LEFT, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACE_LEFT, got token type %d", data->token.type);

    data->inFunction = true;
    data->returned = false;
    data->function = func_name;

    if(data->isFirstScan) {
        if(!wait_for_scope_end(data, true))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);
    } else {
        ruleStList(data);

        if(!data->returned) {
            if(getFuncReturnTypes(data->table, data->function.ptr)->length == 0)
                return_function(vectorInit(), data->scopes);
            else
                throw_error_fatal(FUNCTION_DEFINITION_ERROR, "Missing return in function %s", data->function.ptr);
        }

        if(string_compare(&data->function, "main"))
            main_end();
    }

    removeLocalSymTable(data->scopes);

    data->inFunction = false;

    if(!load_and_compare(data, TOKEN_BRACE_RIGHT, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);
}

/*  6: <params> -> id <type> <params_n>
    7: <params> -> eps  */
void ruleParams(ParserData *data, Vector *params, Vector *names) {
    if(!load_and_compare(data, TOKEN_IDENTIFIER, true)) 
        return;

    String name = data->token.value.s;
    varDataType type = ruleType(data, params);

    if(!data->isFirstScan) {
        TokenValue value;
        vectorPush(names, (void *) name.ptr);
        defineUserVar(getLocalSymTable(data->scopes), name.ptr, type, value, false);
    }

    ruleParamsN(data, params, names);
}

/*  8: <params_n> -> , id <type> <params_n>
    9: <params_n> -> eps  */
void ruleParamsN(ParserData *data, Vector *params, Vector *names) {
    if(!load_and_compare(data, TOKEN_COMA, true)) 
        return;
    if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);

    String name = data->token.value.s;
    varDataType type = ruleType(data, params);

    if(!data->isFirstScan) {
        TokenValue value;
        vectorPush(names, (void *) name.ptr);
        defineUserVar(getLocalSymTable(data->scopes), name.ptr, type, value, false);
    }

    ruleParamsN(data, params, names);
}

/*  10: <type> -> int
    11: <type> -> float64
    12: <type> -> string  */
varDataType ruleType(ParserData *data, Vector *types) {
    if(!load_and_compare(data, TOKEN_KEYWORD, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_KEYWORD, got token type %d", data->token.type);

    if ((data->token.value.k == KEYWORD_INT) || (data->token.value.k == KEYWORD_FLOAT64) || (data->token.value.k == KEYWORD_STRING)) {
        varDataType type;

        if(data->token.value.k == KEYWORD_INT) { type = INTEGER; }
        else if(data->token.value.k == KEYWORD_FLOAT64) { type = FLOAT; }
        else if(data->token.value.k == KEYWORD_STRING) { type = STRING; }

        addFuncType(types, type);
        return type;
    } else {
        throw_error_fatal(SYNTAX_ERROR, "Expected KEYWORD_INT|KEYWORD_FLOAT64|KEYWORD_STRING, got token type %d", data->token.type);
    }
}

/*  13: <ret_types> -> ( <type> <ret_types_n> )
    14: <ret_types> -> eps */
void ruleRetTypes(ParserData *data, Vector *returns) {
    if(!load_and_compare(data, TOKEN_BRACKET_LEFT, true))
        return;
    if(load_and_compare(data, TOKEN_BRACKET_RIGHT, true))
        return;

    ruleType(data, returns);
    ruleRetTypesN(data, returns);

    if(!load_and_compare(data, TOKEN_BRACKET_RIGHT, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACKET_RIGHT, got token type %d", data->token.type);
}

/*  15: <ret_types_n> -> , <type> <ret_types_n>
    16: <ret_types_n> -> eps */
void ruleRetTypesN(ParserData *data, Vector *returns) {
    if(!load_and_compare(data, TOKEN_COMA, true))
        return;

    ruleType(data, returns);
    ruleRetTypesN(data, returns);
}

/*  17: <st_list> -> <stat> EOL <st_list>
    18: <st_list> -> eps */
void ruleStList(ParserData *data) {
    optional_eol(data);
    if(ruleStat(data)) {
        if(!load_and_compare(data, TOKEN_EOL, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);
        ruleStList(data);
    }
}

/*  19: <stat> -> id <stat_body>
    30: <stat> -> if <expression> { <st_list> } else { <st_list> }
    31: <stat> -> for <for_def> ; <expression> ; <for_assign> { <st_list> }
    36: <stat> -> return <return_exp> */
bool ruleStat(ParserData *data) {
    load_token(data);

    if(data->token.type == TOKEN_IDENTIFIER) {
        ruleStatBody(data, data->token);
        return true;
    } else if(data->token.type == TOKEN_KEYWORD && data->token.value.k == KEYWORD_IF) {
        expResult result = ruleExp(data, false, false);
        if(getVarType(getLocalSymTable(data->scopes), result.result.ptr) != BOOL)
            throw_error_fatal(INCOMPATIBLE_EXPRESSION_ERROR, "%s", "Incompatible expression error");

        if_start(result.result.ptr, data->scopes);

        if(!load_and_compare(data, TOKEN_BRACE_LEFT, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACE_LEFT, got token type %d", data->token.type);
        optional_eol(data);
        insertLocalSymTable(data->scopes);
        ruleStList(data);
        removeLocalSymTable(data->scopes);
        if(!load_and_compare(data, TOKEN_BRACE_RIGHT, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);

        if(!load_and_compare(data, TOKEN_KEYWORD, false) || data->token.value.k != KEYWORD_ELSE)
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_KEYWORD - ELSE, got token type %d", data->token.type);

        if(!load_and_compare(data, TOKEN_BRACE_LEFT, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACE_LEFT, got token type %d", data->token.type);

        if_core();

        optional_eol(data);
        insertLocalSymTable(data->scopes);
        ruleStList(data);
        removeLocalSymTable(data->scopes);
        if(!load_and_compare(data, TOKEN_BRACE_RIGHT, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);

        if_end();
        return true;
    } else if(data->token.type == TOKEN_KEYWORD && data->token.value.k == KEYWORD_FOR) {
        insertLocalSymTable(data->scopes);
        ruleForDef(data);
        if(!load_and_compare(data, TOKEN_SEMICOLON, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_SEMICOLON, got token type %d", data->token.type);

        for_start();

        expResult result = ruleExp(data, false, false);
        if(getVarType(getLocalSymTable(data->scopes), result.result.ptr) != BOOL)
            throw_error_fatal(INCOMPATIBLE_EXPRESSION_ERROR, "%s", "Incompatible expression error");
        if(!load_and_compare(data, TOKEN_SEMICOLON, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_SEMICOLON, got token type %d", data->token.type);

        Var exp_result = {.name = result.result, .frame = TEMP_FRAME};
        for_body(exp_result, data->scopes);

        ruleForAssign(data);

        if(!load_and_compare(data, TOKEN_BRACE_LEFT, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACE_LEFT, got token type %d", data->token.type);
        if(!load_and_compare(data, TOKEN_EOL, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);
        optional_eol(data);
        insertLocalSymTable(data->scopes);
        ruleStList(data);
        removeLocalSymTable(data->scopes);
        removeLocalSymTable(data->scopes);

        if(!load_and_compare(data, TOKEN_BRACE_RIGHT, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACE_RIGHT, got token type %d", data->token.type);

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

/*  20: <stat_body> -> <id_n> = <expression> <expression_n>
    21: <stat_body> -> := <expression>
    22: <stat_body> -> ( <call_params> ) */
void ruleStatBody(ParserData *data, Token id) {
    load_token(data);

    htab_t *scope = getLocalSymTable(data->scopes);
    if(data->token.type == TOKEN_DECLARATION) {
        if(string_compare(&id.value.s, "_")) 
            throw_error_fatal(OTHER_SEMANTIC_ERROR, "%s", "Can not define variable with name _");

        if(isVarUserDefined(scope, id.value.s.ptr))
            throw_error_fatal(DEFINITION_ERROR, "Variable %s already defined", id.value.s.ptr);

        expResult result = ruleExp(data, false, false);

        if(getVarType(scope, result.result.ptr) == BOOL)
            throw_error_fatal(TYPE_DEFINITION_ERROR, "%s", "Bool variable declaration not supported");

        TokenValue value;
        if(isVarConst(scope, result.result.ptr))
            value = getVarValue(scope, result.result.ptr);
        else
            value.i = 0;

        defineUserVar(scope, id.value.s.ptr, getVarType(scope, result.result.ptr), value, isVarConst(scope, result.result.ptr));

        Var target = {.name = id.value.s, .frame = LOCAL_FRAME};
        Var source = {.name = result.result, .frame = LOCAL_FRAME};
        Symb symbol = {.isVar = true, .var = source};
        define_var(target, symbol, data->scopes);
    } else if(data->token.type == TOKEN_BRACKET_LEFT) {
        Vector *types = vectorInit();
        Vector *names = vectorInit();

        ruleCallParams(data, names, types);

        if(!load_and_compare(data, TOKEN_BRACKET_RIGHT, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACKET_RIGHT, got token type %d", data->token.type);

        if(getSymTableForVar(data->scopes, id.value.s.ptr) != NULL || !isFuncDefined(data->table, id.value.s.ptr))
            throw_error_fatal(DEFINITION_ERROR, "Function %s not defined", id.value.s.ptr);

        if(string_compare(&id.value.s, "print")) {
            for(unsigned i = 0; i < names->length; i++) {
                Var source = {.name.ptr = (char *) vectorGet(names, i), .frame = LOCAL_FRAME};
                Symb symbol = {.isVar = true, .var = source};
                PRINT(symbol, data->scopes);
            }
        } else {
            if(!checkTypes(getFuncParamTypes(data->table, id.value.s.ptr), types))
                throw_error_fatal(FUNCTION_DEFINITION_ERROR, "%s", "Incorrect data types in function call");

            call_function(id.value.s.ptr, names, vectorInit(), data->scopes);
        }
    } else {
        push_token(data, data->token);

        Vector *ltypes = vectorInit();
        Vector *lnames = vectorInit();

        if(string_compare(&id.value.s, "_")) {
            vectorPush(lnames, (void *) id.value.s.ptr);
            addFuncType(ltypes, NONE);
        } else {
            htab_t *scope = getSymTableForVar(data->scopes, id.value.s.ptr);
            if(scope == NULL)
                throw_error_fatal(DEFINITION_ERROR, "Variable %s not defined", id.value.s.ptr);

            vectorPush(lnames, (void *) id.value.s.ptr);
            addFuncType(ltypes, getVarType(scope, id.value.s.ptr));
        }

        ruleIdN(data, ltypes, lnames);

        if(!load_and_compare(data, TOKEN_ASSIGNMENT, false))
            throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_ASSIGNMENT, got token type %d", data->token.type);

        expResult result = ruleExp(data, false, true);

        if(result.isFunc) {
            if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
                throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);

            String function = data->token.value.s;

            if(!load_and_compare(data, TOKEN_BRACKET_LEFT, false))
                throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACKET_LEFT, got token type %d", data->token.type);

            Vector *types = vectorInit();
            Vector *names = vectorInit();

            ruleCallParams(data, names, types);

            if(!load_and_compare(data, TOKEN_BRACKET_RIGHT, false))
                throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_BRACKET_RIGHT, got token type %d", data->token.type);

            if(!load_and_compare(data, TOKEN_EOL, true))
                throw_error_fatal(FUNCTION_DEFINITION_ERROR, "Expected TOKEN_EOL, got token type %d", data->token.type);

            push_token(data, data->token);

            if(getSymTableForVar(data->scopes, function.ptr) != NULL || !isFuncDefined(data->table, function.ptr))
                throw_error_fatal(DEFINITION_ERROR, "Function %s not defined", id.value.s.ptr);
            if(!checkTypes(getFuncParamTypes(data->table, function.ptr), types))
                throw_error_fatal(FUNCTION_DEFINITION_ERROR, "%s", "Incorrect data types in function call");
            if(!checkTypes(getFuncReturnTypes(data->table, function.ptr), ltypes))
                throw_error_fatal(FUNCTION_DEFINITION_ERROR, "%s", "Incorrect data types in function call return");

            call_function(function.ptr, names, lnames, data->scopes);
        } else {
            Vector *rtypes = vectorInit();
            Vector *rnames = vectorInit();

            vectorPush(rnames, (void *) result.result.ptr);
            addFuncType(rtypes, getVarType(getLocalSymTable(data->scopes), result.result.ptr));

            ruleExpN(data, rnames, rtypes);

            if(!checkTypes(ltypes, rtypes))
                throw_error_fatal(OTHER_SEMANTIC_ERROR, "%s", "Type mismatch");

            for(unsigned i = 0; i < lnames->length; i++) {
                char * target_name = (char *) vectorGet(lnames, i);
                char * exp_name = (char *) vectorGet(rnames, i);

                if(strcmp(target_name, "_") != 0) {
                    htab_t *scope = getSymTableForVar(data->scopes, target_name);

                    bool isConst = isVarConst(getLocalSymTable(data->scopes), exp_name);
                    setVarConst(scope, target_name, isConst);
                    
                    if(isConst)
                        setVarValue(scope, target_name, getVarValue(getLocalSymTable(data->scopes), exp_name));

                    Var target = {.name.ptr = target_name, .frame = LOCAL_FRAME};
                    Var source = {.name.ptr = exp_name, .frame = LOCAL_FRAME};
                    Symb symbol = {.isVar = true, .var = source};
                    MOVE(target, symbol, data->scopes);
                }
            }
        }
    }
}

/*  26: <id_n> -> , id <id_n>
    27: <id_n> -> eps  */
void ruleIdN(ParserData *data, Vector *types, Vector *names) {
    if(!load_and_compare(data, TOKEN_COMA, true))
        return;
    if(!load_and_compare(data, TOKEN_IDENTIFIER, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_IDENTIFIER, got token type %d", data->token.type);

    if(string_compare(&data->token.value.s, "_")) {
        vectorPush(names, (void *) data->token.value.s.ptr);
        addFuncType(types, NONE);
    } else {
        htab_t *scope = getSymTableForVar(data->scopes, data->token.value.s.ptr);
        if(scope == NULL)
            throw_error_fatal(DEFINITION_ERROR, "Variable %s not defined", data->token.value.s.ptr);

        vectorPush(names, (void *) data->token.value.s.ptr);
        addFuncType(types, getVarType(scope, data->token.value.s.ptr));
    }

    ruleIdN(data, types, names);
}

/*  28: <expression_n> -> , <expression> <expression_n>
    29: <expression_n> -> eps */
void ruleExpN(ParserData *data, Vector *names, Vector *types) {
    if(!load_and_compare(data, TOKEN_COMA, true))
        return;
    expResult result = ruleExp(data, false, false);

    vectorPush(names, (void *) result.result.ptr);
    addFuncType(types, getVarType(getLocalSymTable(data->scopes), result.result.ptr));

    ruleExpN(data, names, types);
}

/*  37: <return_exp> -> <expression> <expression_n>
    38: <return_exp> -> eps */
void ruleReturnExp(ParserData *data) {
    Vector *types = vectorInit();
    Vector *names = vectorInit();

    expResult result = ruleExp(data, true, false);

    if(!result.isEmpty) {
        vectorPush(names, (void *) result.result.ptr);
        addFuncType(types, getVarType(getLocalSymTable(data->scopes), result.result.ptr));
    }

    ruleExpN(data, names, types);

    if(!checkTypes(getFuncReturnTypes(data->table, data->function.ptr), types))
        throw_error_fatal(FUNCTION_DEFINITION_ERROR, "%s", "Type mismatch");

    if(!string_compare(&data->function, "main")) {
        return_function(names, data->scopes);
        data->returned = true;
    }
}

/*  23: <call_params> -> <values> <call_params_n>
    24: <call_params> -> eps    */
void ruleCallParams(ParserData *data, Vector *names, Vector *types) {
    if(ruleValues(data, names, types)) {
        ruleCallParamsN(data, names, types);
    } else {
        push_token(data, data->token);
    }
}

/*  <call_params_n> -> , <values> <call_params_n>
    <call_params_n> -> eps  */
void ruleCallParamsN(ParserData *data, Vector *names, Vector *types) {
    if(!load_and_compare(data, TOKEN_COMA, true))
        return;

    if(ruleValues(data, names, types)) {
        ruleCallParamsN(data, names, types);
    } else {
        throw_error_fatal(SYNTAX_ERROR, "Expected VALUE, got token type %d", data->token.type);
    }
}

/*  <values> -> value_int
    <values> -> value_float
    <values> -> value_string
    <values> -> id  */
bool ruleValues(ParserData *data, Vector *names, Vector *types) {
    load_token(data);

    if(data->token.type != TOKEN_IDENTIFIER && data->token.type != TOKEN_NUMBER_FLOAT && data->token.type != TOKEN_NUMBER_INT && data->token.type != TOKEN_STRING)
        return false;
    else {
        if(data->token.type == TOKEN_IDENTIFIER) {
            if(string_compare(&data->token.value.s, "_"))
                throw_error_fatal(OTHER_SEMANTIC_ERROR, "%s", "Cannot use _ in function call");

            htab_t *scope = getSymTableForVar(data->scopes, data->token.value.s.ptr);
            if(scope == NULL)
                throw_error_fatal(DEFINITION_ERROR, "Variable %s not defined", data->token.value.s.ptr);

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

/*  32: <for_def> -> id := <expression>
    33: <for_def> -> eps  */
void ruleForDef(ParserData *data) {
    if(!load_and_compare(data, TOKEN_IDENTIFIER, true))
        return;

    Token id = data->token;
    
    if(!load_and_compare(data, TOKEN_DECLARATION, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_DECLARATION, got token type %d", data->token.type);

    if(string_compare(&id.value.s, "_")) 
        throw_error_fatal(OTHER_SEMANTIC_ERROR, "%s", "Can not define variable with name _");

    expResult result = ruleExp(data, false, false);

    if(getVarType(getLocalSymTable(data->scopes), result.result.ptr) == BOOL)
        throw_error_fatal(TYPE_DEFINITION_ERROR, "%s", "Bool variable declaration not supported");

    TokenValue value;
    if(isVarConst(getLocalSymTable(data->scopes), result.result.ptr))
        value = getVarValue(getLocalSymTable(data->scopes), result.result.ptr);
    else
        value.i = 0;

    defineUserVar(getLocalSymTable(data->scopes), id.value.s.ptr, getVarType(getLocalSymTable(data->scopes), result.result.ptr), value, isVarConst(getLocalSymTable(data->scopes), result.result.ptr));

    Var target = {.name = id.value.s, .frame = LOCAL_FRAME};
    Var source = {.name = result.result, .frame = LOCAL_FRAME};
    Symb symbol = {.isVar = true, .var = source};
    define_var(target, symbol, data->scopes);
}

/*  34: <for_assign> -> id <id_n> = <expression> <expression_n>
    35: <for_assign> -> eps */
void ruleForAssign(ParserData *data) {
    if(!load_and_compare(data, TOKEN_IDENTIFIER, true))
        return;

    Vector *ltypes = vectorInit();
    Vector *lnames = vectorInit();

    if(string_compare(&data->token.value.s, "_")) {
        vectorPush(lnames, (void *) data->token.value.s.ptr);
        addFuncType(ltypes, NONE);
    } else {
        htab_t *scope = getSymTableForVar(data->scopes, data->token.value.s.ptr);
        if(scope == NULL)
            throw_error_fatal(DEFINITION_ERROR, "Variable %s not defined", data->token.value.s.ptr);

        vectorPush(lnames, (void *) data->token.value.s.ptr);
        addFuncType(ltypes, getVarType(scope, data->token.value.s.ptr));
    }

    ruleIdN(data, ltypes, lnames);

    if(!load_and_compare(data, TOKEN_ASSIGNMENT, false))
        throw_error_fatal(SYNTAX_ERROR, "Expected TOKEN_ASSIGNMENT, got token type %d", data->token.type);

    Vector *rtypes = vectorInit();
    Vector *rnames = vectorInit();

    expResult result = ruleExp(data, false, false);
    vectorPush(rnames, (void *) result.result.ptr);
    addFuncType(rtypes, getVarType(getLocalSymTable(data->scopes), result.result.ptr));

    ruleExpN(data, rnames, rtypes);

    if(!checkTypes(ltypes, rtypes))
        throw_error_fatal(OTHER_SEMANTIC_ERROR, "%s", "Type mismatch");

    for(unsigned i = 0; i < lnames->length; i++) {
        char * target_name = (char *) vectorGet(lnames, i);
        char * exp_name = (char *) vectorGet(rnames, i);

        if(strcmp(target_name, "_") != 0) {
            htab_t *scope = getSymTableForVar(data->scopes, target_name);

            bool isConst = isVarConst(getLocalSymTable(data->scopes), exp_name);
            setVarConst(scope, target_name, isConst);
            
            if(isConst)
                setVarValue(scope, target_name, getVarValue(getLocalSymTable(data->scopes), exp_name));

            Var target = {.name.ptr = target_name, .frame = LOCAL_FRAME};
            Var source = {.name.ptr = exp_name, .frame = LOCAL_FRAME};
            Symb symbol = {.isVar = true, .var = source};
            MOVE(target, symbol, data->scopes);
        }
    }
}

/*  39: <expression> -> expression */
expResult ruleExp(ParserData *data, bool allowEmpty, bool allowFunc) {
    expResult result = expression(data->scopes, data->table);

    if(!allowEmpty && result.isEmpty)
        throw_error_fatal(SYNTAX_ERROR, "%s", "Expected non-empty expression, got empty");

    if(!allowFunc && result.isFunc)
        throw_error_fatal(SYNTAX_ERROR, "%s", "Expected expression, got func");

    push_token(data, result.newToken);
    return result;
}

void register_functions(ParserData *data) {
    Vector *inputs_returns = vectorInit();
    addFuncType(inputs_returns, STRING);
    addFuncType(inputs_returns, INTEGER); 
    defineFunc(data->table, "inputs", vectorInit(), inputs_returns);

    Vector *inputi_returns = vectorInit();
    addFuncType(inputi_returns, INTEGER);
    addFuncType(inputi_returns, INTEGER);
    defineFunc(data->table, "inputi", vectorInit(), inputi_returns);

    Vector *inputf_returns = vectorInit();
    addFuncType(inputf_returns, FLOAT);
    addFuncType(inputf_returns, INTEGER);
    defineFunc(data->table, "inputf", vectorInit(), inputf_returns);

    Vector *int2float_params = vectorInit();
    Vector *int2float_returns = vectorInit();
    addFuncType(int2float_params, INTEGER);
    addFuncType(int2float_returns, FLOAT);
    defineFunc(data->table, "int2float", int2float_params, int2float_returns);

    Vector *float2int_params = vectorInit();
    Vector *float2int_returns = vectorInit();
    addFuncType(float2int_params, FLOAT);
    addFuncType(float2int_returns, INTEGER);
    defineFunc(data->table, "float2int", float2int_params, float2int_returns);

    Vector *len_params = vectorInit();
    Vector *len_returns = vectorInit();
    addFuncType(len_params, STRING);
    addFuncType(len_returns, INTEGER);
    defineFunc(data->table, "len", len_params, len_returns);

    Vector *substr_params = vectorInit();
    Vector *substr_returns = vectorInit();
    addFuncType(substr_params, STRING);
    addFuncType(substr_params, INTEGER);
    addFuncType(substr_params, INTEGER);
    addFuncType(substr_returns, STRING);
    addFuncType(substr_returns, INTEGER);
    defineFunc(data->table, "substr", substr_params, substr_returns);

    Vector *ord_params = vectorInit();
    Vector *ord_returns = vectorInit();
    addFuncType(ord_params, STRING);
    addFuncType(ord_params, INTEGER);
    addFuncType(ord_returns, INTEGER);
    addFuncType(ord_returns, INTEGER);
    defineFunc(data->table, "ord", ord_params, ord_returns);

    Vector *chr_params = vectorInit();
    Vector *chr_returns = vectorInit();
    addFuncType(chr_params, INTEGER);
    addFuncType(chr_returns, STRING);
    addFuncType(chr_returns, INTEGER);
    defineFunc(data->table, "chr", chr_params, chr_returns);

    defineFunc(data->table, "print", vectorInit(), vectorInit());
}

void parse() {
    dynamicArr *file = arrInit();
    copyStdinToArr(file);
    
    /*FILE *f = fopen("test.go", "r");
    int c = fgetc(f);
    while (c != EOF)
    {
        arrPutc(file, c);
        c = fgetc(f);
    }
    fclose(f);*/

    scanner_set_file(file);

    ParserData data;
    data.isFirstScan = true;
    data.scopes = vectorInit();
    data.table = htab_init(10);
    data.stack = stackInit();
    register_functions(&data);
    ruleProgram(&data);
    arrSeekStart(file);
    data.isFirstScan = false;
    gen_init();
    ruleProgram(&data);
    program_end();

    arrFree(file);
}