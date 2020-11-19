/* IFJ20 - Vector (dynamic array) library
 * Authors:
 * Juraj Marticek, xmarti97
 */
#include "vector.h"
#include "error.h"

Vector *vectorInit()
{
    Vector *vector = malloc(sizeof(Vector));
    if (vector == NULL)
        throw_error_fatal(INTERNAL_ERROR, "%s", "Couldn't allocate memory!");
    vector->items = malloc(DEFAULT_VECTOR_SIZE * sizeof(void *));
    if (vector->items == NULL)
        throw_error_fatal(INTERNAL_ERROR, "%s", "Couldn't allocate memory!");
    vector->currentMaxLength = DEFAULT_VECTOR_SIZE;
    vector->length = 0;
    return vector;
}

unsigned vectorLength(Vector *vector)
{
    return vector->length;
}

void reallocIfNeeded(Vector *vector)
{
    if (vector->length == vector->currentMaxLength)
    {
        // Double the size
        vector->items = realloc(vector->items, vector->currentMaxLength * 2 * sizeof(void *));
        if (vector->items == NULL)
            throw_error_fatal(INTERNAL_ERROR, "%s", "Couldn't allocate memory!");
        vector->currentMaxLength *= 2;
    }
}

void vectorPush(Vector *vector, const void *item)
{
    reallocIfNeeded(vector);
    if (item == NULL)
        throw_error(INTERNAL_ERROR, "%s", "Pushing NULL to vector.");
    vector->items[vector->length++] = item;
}

const void *vectorPop(Vector *vector) {
    const void *item = vector->items[--vector->length];
    vector->items[vector->length] = NULL;
    return item;
}

void vectorInsert(Vector *vector, const void *item, unsigned index) {
    if(index > vector->length) {
        throw_error_fatal(INTERNAL_ERROR, "%s", "Trying to insert value out of vector boundaries.");
    }
    reallocIfNeeded(vector);
    vector->items[index] = item;
}

void vectorRemove(Vector *vector, unsigned index) {
    if(index > vector->length) {
        throw_error_fatal(INTERNAL_ERROR, "%s", "Trying to access value out of vector boundaries.");
    }
    vector->items[index] = NULL;
}

const void *vectorGet(Vector *vector, unsigned index) {
    if(index > vector->length) {
        throw_error_fatal(INTERNAL_ERROR, "%s", "Trying to access value out of vector boundaries.");
    }
    return vector->items[index];
}

void vectorFree(Vector *vector) {
    vector->currentMaxLength = 0;
    vector->length = 0;
    free(vector->items);
    free(vector);
}
