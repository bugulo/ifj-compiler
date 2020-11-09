/* IFJ20 - String Library
 * Authors:
 * Michal Slesar, xslesa01
 */

#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Structure representing a string
 */
typedef struct {
    char* ptr; /** Raw pointer to string content */
    size_t length; /** Length of the string */
    size_t size; /** Number of allocated bytes */
} string;

/** Bytes to be allocated in advance */
#define STRING_BLOCK_SIZE 32

/**
 * Initialize empty string
 * @param string Target string pointer
 */
bool string_init(string *string);

/**
 * Free resources of string
 * @param string Target string pointer
 */
bool string_free(string *string);

/**
 * Clear content of string but keep allocated resources
 * @param string Target string pointer
 */
bool string_clear(string *string);

/**
 * Append to string
 * @param string Target string pointer
 * @param source String to be pushed
 */
bool string_append(string *string, const char *source);

/**
 * Compare two strings
 * @param string1 First string
 * @param string2 Second string
 */
bool string_compare(string *string1, const char *string2);

#endif