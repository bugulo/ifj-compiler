/* IFJ20 - Parser
 * Authors:
 * Erik Belko, xbelko02
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

void parser_main();
void ruleProgram();
void ruleBody(htab_t *table);
void ruleFuncN(htab_t *table);
void ruleFunc(htab_t *table);
void ruleParams(Vector *params);
void ruleParamsN(Vector *params);
void ruleType(Vector *types);
void ruleRetTypes(Vector *returns);
void ruleRetTypesN(Vector *returns);
void ruleStList();
bool ruleStat();
void ruleStatBody();
void ruleAssign();
void ruleIdN();
void ruleExpN();
void ruleReturnExp();
void ruleCallParams();
void ruleCallParamsN();
bool ruleValues();
void ruleForDef();
void ruleForAssign();
void ruleExp(bool allowEmpty, bool allowFunc);

#endif