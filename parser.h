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
void ruleProgram();
void ruleBody();
void ruleFuncN();
void ruleFunc();
//void ruleParams();
//void ruleParamsN();
//void ruleType();
//void ruleRetTypes();
//void ruleRetTypesN();
//void ruleStList();
//void ruleStat();
//void ruleStatBody();
//void ruleAssign();
//void ruleIdN();
//void ruleExpressionN();
//void ruleReturn();
//void ruleReturnExp();
//void ruleExpression();

#endif