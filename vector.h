/* IFJ20 - Vector (dynamic array) library
 * Authors:
 * Juraj Marticek, xmarti97
 */

#ifndef VECTOR_H
#define VECTOR_H
#include <stdlib.h>
#include <stdio.h>
#define DEFAULT_VECTOR_SIZE 20

typedef struct
{
    unsigned length;
    unsigned currentMaxLength;
    const void **items;
} Vector;

Vector *vectorInit();

unsigned vectorLength(Vector *vector);

void vectorPush(Vector *vector, const void *item);

const void *vectorPop(Vector *vector);

void vectorInsert(Vector *vector, const void *item, unsigned index);

void vectorRemove(Vector *vector, unsigned index);

const void *vectorGet(Vector *vector, unsigned index);

void vectorFree(Vector *vector);
#endif