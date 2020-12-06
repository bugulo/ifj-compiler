/* IFJ20 - Stack library
 * Authors:
 * Mario Harvan, xharva03
 */

#ifndef STACK_H
#define STACK_H

#include "scanner.h"
#include <stdbool.h>

typedef struct{
    unsigned int size;          //size of array
    unsigned int itemCnt;       //number of items in stack
    unsigned int stackPosition; //position of last item in stack
    Token* arrPtr;
} Stack;

/**
 * @brief Function will init empty stack
 * @return Pointer to stack or null when init fails
*/
Stack* stackInit();

/**
 * @brief Function will check if stack is empty
 * @param Pointer to stack
 * @return true when stack is empty, or false if its not
*/
bool stackIsEmpty(Stack *stack);

/**
 * @brief Function will return token from top of stack
 * @param Pointer to stack
 * @return Token on top of stack, or empty token when stack is empty
*/
Token stackPeek(Stack *stack);

/**
 * @brief Funtion will add token at top of stack
 * @param Pointer to stack
 * @param Token to push
*/
void stackPush(Stack *stack, Token token);

/**
 * @brief Funtion will remove token from top of stack and returns it
 * @param Pointer to stack
 * @return Token from top of stack, or empty token when stack is empty
*/
Token stackPop(Stack *stack);

/**
 * @brief Function will free stack
 * @param Pointer to stack
*/
void stackFree(Stack *stack);



#endif