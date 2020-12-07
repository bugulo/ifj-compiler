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

// Returns current number of items in vector
unsigned vectorLength(Vector *vector);

// Push new item into the vector
void vectorPush(Vector *vector, const void *item);

// Return and remove last item from vector
const void *vectorPop(Vector *vector);

// Insert item on given index
void vectorInsert(Vector *vector, const void *item, unsigned index);

// Remove item on given intex
void vectorRemove(Vector *vector, unsigned index);

// Return item on given index
const void *vectorGet(Vector *vector, unsigned index);

// Free allocated vector, and every item
void vectorFree(Vector *vector);
#endif