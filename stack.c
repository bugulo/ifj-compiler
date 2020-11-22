/* IFJ20 - Stack library
 * Authors:
 * Mario Harvan, xharva03
 */

#include "stack.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#define DEFAULT_STACK_SIZE 20

Stack *stackInit(){
    Stack *tmp = malloc(sizeof(Stack));
    if(tmp == NULL){
        throw_error_fatal(INTERNAL_ERROR, "%s", "Couldn't allocate memory!");
    }
    tmp->size = DEFAULT_STACK_SIZE;
    tmp->arrPtr = 0;
    tmp->stackPosition = 0;
    tmp->itemCnt = 0;
    tmp->arrPtr = malloc(sizeof(Token) * DEFAULT_STACK_SIZE);
    if(tmp->arrPtr == NULL){
        throw_error_fatal(INTERNAL_ERROR, "%s", "Couldn't allocate memory!");
    }
    return tmp;
}

bool stackIsEmpty(Stack *stack){
    if(stack != NULL){
        if(stack->itemCnt == 0) return true;
        else return false;
    }
    return false;
}

Token stackPeek(Stack *stack){
    Token empty;
    empty.type = TOKEN_EOF;
    if(stack != NULL){
        if(stackIsEmpty(stack) == false){
            return stack->arrPtr[stack->stackPosition];
        }
    }
    return empty;
}

void stackPush(Stack *stack, Token token){
    if(stack != NULL){
        if(stackIsEmpty(stack) == true){
            stack->arrPtr[0] = token;
            stack->itemCnt++;
        }
        else if(stack->size > stack->itemCnt){
            stack->stackPosition++;
            stack->arrPtr[stack->stackPosition] = token;
            stack->itemCnt++;
        }
        else{
            stack->arrPtr = realloc(stack->arrPtr, sizeof(Token) * stack->size * 2);
            if(stack->arrPtr == NULL){
                throw_error_fatal(INTERNAL_ERROR, "%s", "Couldn't allocate memory!");
            }
            stack->size = stack->size * 2;
            stack->stackPosition++;
            stack->arrPtr[stack->stackPosition] = token;
            stack->itemCnt++;
        }
    }
}

Token stackPop(Stack *stack){
    Token empty;
    empty.type = TOKEN_EOF;
    if(stack != NULL){
        if(stackIsEmpty(stack) == true){
            return empty;
        }
        else{
            stack->itemCnt--;
            if(stack->stackPosition != 0){
                stack->stackPosition--;
                return stack->arrPtr[stack->stackPosition + 1];
            }
            else{
                return stack->arrPtr[0];
            }
        }
    }
    return empty;
}

void stackFree(Stack *stack){
    if(stack != NULL){
        free(stack->arrPtr);
        free(stack);
        stack = NULL;
    }
}