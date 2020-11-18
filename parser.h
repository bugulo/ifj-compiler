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

Stack *stack;

void parser_main();
Token getToken();
bool ruleProgram();
bool ruleBody();
bool ruleFuncN();
bool ruleFunc();

#endif