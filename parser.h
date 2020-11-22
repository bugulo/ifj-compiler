/* IFJ20 - Parser
 * Authors:
 * Erik Belko, xbelko02
 * Michal Slesar, xslesa01
 */

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "stack.h"
#include <stdio.h>
#include <stdbool.h>
#include "error.h"
#include "vector.h"
#include "semantic_analysis.h"
#include "symtable.h"
#include "expression.h"
#include "string.h"

typedef struct {
    bool isFirstScan;
    htab_t *table;
    Vector *scopes;
    Token token;
    Stack *stack;
    Stack *tokens;
    bool inFunction;
    String function;
} ParserData;

void parse();

void ruleProgram(ParserData *data);
void ruleBody(ParserData *data);
void ruleFuncN(ParserData *data);
void ruleFunc(ParserData *data);
void ruleParams(ParserData *data, Vector *params);
void ruleParamsN(ParserData *data, Vector *params);
varDataType ruleType(ParserData *data, Vector *types);
void ruleRetTypes(ParserData *data, Vector *returns);
void ruleRetTypesN(ParserData *data, Vector *returns);
void ruleStList(ParserData *data);
bool ruleStat(ParserData *data);
void ruleStatBody(ParserData *data, Token id);

void ruleIdN(ParserData *data, Vector *types, Vector *names);
void ruleExpN(ParserData *data, Vector *names, Vector *types);
void ruleReturnExp(ParserData *data);
void ruleCallParams(ParserData *data, Vector *names, Vector *types);
void ruleCallParamsN(ParserData *data, Vector *names, Vector *types);
bool ruleValues(ParserData *data, Vector *names, Vector *types);
void ruleForDef(ParserData *data);
void ruleForAssign(ParserData *data);
expResult ruleExp(ParserData *data, bool allowEmpty, bool allowFunc);

#endif