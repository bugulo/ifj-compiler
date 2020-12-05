/* IFJ20 - String Library
 * Authors:
 * Michal Slesar, xslesa01
 */

#include "string.h"
#include "error.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

bool string_init(String *string) {
    string->ptr = malloc(STRING_BLOCK_SIZE);

    if(string->ptr == NULL)
        throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");

    string->length = 0;
    string->size = STRING_BLOCK_SIZE;
    string->ptr[0] = '\0';

    return true;
}

bool string_free(String *string) {
    if(string->ptr == NULL)
        return false;
    free(string->ptr);
    string->ptr = NULL;
    string->length = 0;
    string->size = 0;
    return true;
}

bool string_clear(String *string) {
    if(string->ptr == NULL)
        return false;

    string->ptr[0] = '\0';
    string->length = 0;
    return true;
}

bool string_append_string(String *string, const char *source) {
    if(string->ptr == NULL || source == NULL)
        return false;

    size_t source_length = strlen(source);

    /** We need to allocate more blocks if needed */
    if(string->length + source_length >= string->size) {
        size_t new_size = ((string->length + source_length) / STRING_BLOCK_SIZE + 1) * STRING_BLOCK_SIZE;
        string->ptr = realloc(string->ptr, new_size);

        if(string->ptr == NULL) {
            string->length = 0;
            string->size = 0;
            throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
        }

        string->size = new_size;
    }

    for(size_t i = 0; i < source_length + 1; i++)
        string->ptr[string->length + i] = source[i];

    string->length += source_length;
    return true;
}

bool string_append_char(String *string, char c) {
    if(string->ptr == NULL)
        return false;

    /** We need to allocate more blocks if needed */
    if(string->length + 1 >= string->size) {
        size_t new_size = string->size + STRING_BLOCK_SIZE;
        string->ptr = realloc(string->ptr, new_size);

        if(string->ptr == NULL) {
            string->length = 0;
            string->size = 0;
            throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation error");
        }

        string->size = new_size;
    }

    string->ptr[string->length] = c;
    string->length++;
    string->ptr[string->length] = '\0';
    return true;
}

bool string_compare(String *string1, const char *string2) {
    return strcmp(string1->ptr, string2) == 0;
}