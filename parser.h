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

void parser_main();
Token getToken();
bool ruleProgram();
bool ruleBody();
bool ruleFuncN();
bool ruleFunc();
//bool ruleParams();
//bool ruleParamsN();
//bool ruleType();
//bool ruleRetTypes();
//bool ruleRetTypesN();
//bool ruleStList();
//bool ruleStat();
//bool ruleStatBody();
//bool ruleAssign();
//bool ruleIdN();
//bool ruleExpressionN();
//bool ruleReturn();
//bool ruleReturnExp();
//bool ruleExpression();

#endif