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

#endif