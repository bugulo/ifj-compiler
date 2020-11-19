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
void ruleParams();
void ruleParamsN();
void ruleType(bool param);
void ruleRetTypes();
void ruleRetTypesN();
void ruleStList();
bool ruleStat();
void ruleStatBody();
void ruleAssign();
void ruleIdN();
void ruleExpN();
void ruleReturnExp();
void ruleExp();

#endif