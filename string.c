#include "string.h"

#include <stdbool.h>
#include <string.h>
#include <malloc.h>

bool string_init(string *string) {
    string->ptr = malloc(STRING_BLOCK_SIZE);

    if(string->ptr == NULL)
        return false; 

    string->length = 0;
    string->size = STRING_BLOCK_SIZE;
    string->ptr[0] = '\0';

    return true;
}

bool string_free(string *string) {
    if(string->ptr == NULL)
        return false;

    free(string->ptr);
    string->ptr = NULL;
    string->length = 0;
    string->size = 0;
    return true;
}

bool string_clear(string *string) {
    if(string->ptr == NULL)
        return false;

    string->ptr[0] = '\0';
    string->length = 0;
    return true;
}

bool string_append(string *string, const char *source) {
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
            return false;
        }

        string->size = new_size;
    }

    for(size_t i = 0; i < source_length + 1; i++)
        string->ptr[string->length + i] = source[i];

    string->length += source_length;
    return true;
}

bool string_compare(string *string1, const char *string2) {
    return strcmp(string1->ptr, string2) == 0;
}