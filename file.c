/* IFJ20 - Dynamic char array
 * Authors:
 * Mario Harvan, xharva03
 */

#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include "error.h"

void resizeIfNeeded(dynamicArr *arr)
{
    if (arr != NULL)
    {
        if (arr->arr != NULL)
            if (arr->length == arr->size)
            {
                arr->arr = realloc(arr->arr, sizeof(char) * arr->size * 2);
                if (arr->arr == NULL)
                    throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
                arr->size *= 2;
            }
    }
}

dynamicArr *arrInit()
{
    dynamicArr *tmp = malloc(sizeof(dynamicArr));
    if (tmp == NULL)
        throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
    tmp->arr = malloc(sizeof(char) * DEFAULT_ARR_SIZE);
    if (tmp->arr == NULL)
        throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
    tmp->size = DEFAULT_ARR_SIZE;
    tmp->length = 0;
    tmp->position = 0;
    return tmp;
}

void arrPutc(dynamicArr *arr, char c)
{
    if (arr != NULL)
    {
        if (arr->arr != NULL)
        {
            resizeIfNeeded(arr);
            arr->arr[arr->length++] = c;
        }
    }
}

int arrGetc(dynamicArr *arr)
{
    if (arr != NULL)
    {
        if (arr->arr != NULL)
        {
            if (arr->position < arr->length)
            {
                return arr->arr[arr->position++];
            }

            else
            {
                arr->position++;
                return EOF;
            }
                
        }
    }
    return ARR_ERROR;
}

void arrUnGetc(dynamicArr *arr)
{
    if (arr != NULL)
    {
        if (arr->arr != NULL)
        {
            if (arr->position > 0)
                arr->position--;
        }
    }
}

void arrSeekStart(dynamicArr *arr)
{
    if (arr != NULL)
    {
        if (arr->arr != NULL)
        {
            arr->position = 0;
        }
    }
}

void arrFree(dynamicArr *arr)
{
    if (arr != NULL)
    {
        if (arr->arr != NULL)
        {
            free(arr->arr);
            free(arr);
        }
    }
}

void copyStdinToArr(dynamicArr *arr)
{
    int c = fgetc(stdin);
    while (c != EOF)
    {
        arrPutc(arr, c);
        c = fgetc(stdin);
    }
}