/* IFJ20 - Stack library
 * Authors:
 * Mario Harvan, xharva03
 */

#ifndef STACK_H
#define STACK_H

#include "scanner.h"
#include <stdbool.h>

typedef struct{
    unsigned int size;
    unsigned int itemCnt;
    unsigned int stackPosition;
    Token* arrPtr;
} Stack;

Stack* stackInit();

bool stackIsEmpty(Stack *stack);

Token stackPeek(Stack *stack);

void stackPush(Stack *stack, Token token);

Token stackPop(Stack *stack);

void stackFree(Stack *stack);



#endif