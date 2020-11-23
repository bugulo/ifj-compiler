/* IFJ20 - Error lib
 * Authors:
 * Juraj Marticek, xmarti97
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>
#include <stdlib.h>

typedef enum
{
    LEXICAL_ERROR = 1,
    SYNTAX_ERROR,
    DEFINITION_ERROR,
    TYPE_DEFINITION_ERROR,
    INCOMPATIBLE_EXPRESSION_ERROR,
    FUNCTION_DEFINITION_ERROR,
    OTHER_SEMANTIC_ERROR,
    ZERO_DIVISION_ERROR = 9,
    INTERNAL_ERROR = 99,
} ErrorTypes;

#define print(fmt, ...)                                                                                                        \
    do                                                                                                                         \
    {                                                                                                                          \
        printf("\n");                                                                                                          \
        fprintf(stderr, "#\033[32m[DEBUG]\033[1m %s:%d\033[0m In function \033[1m‘%s’\033[0m: ", __FILE__, __LINE__, __func__); \
        fprintf(stderr, fmt, ##__VA_ARGS__);                                                                                   \
        printf("\n");                                                                                                          \
    } while (0)

#define throw_error(err_no, fmt, ...)                                                                                                 \
    do                                                                                                                                \
    {                                                                                                                                 \
        printf("\n");                                                                                                                 \
        char *errorMsg = errTypeToString(err_no);                                                                                     \
        fprintf(stderr, "\033[31m[%s]\033[1m %s:%d\033[0m In function \033[1m‘%s’\033[0m: ", errorMsg, __FILE__, __LINE__, __func__); \
        fprintf(stderr, fmt, ##__VA_ARGS__);                                                                                          \
        printf("\n");                                                                                                                 \
    } while (0)

#define throw_error_fatal(err_no, fmt, ...)                                                                                                   \
    do                                                                                                                                        \
    {                                                                                                                                         \
        printf("\n");                                                                                                                         \
        char *errorMsg = errTypeToString(err_no);                                                                                             \
        fprintf(stderr, "\033[31m[%s (FATAL)]\033[1m %s:%d\033[0m In function \033[1m‘%s’\033[0m: ", errorMsg, __FILE__, __LINE__, __func__); \
        fprintf(stderr, fmt, ##__VA_ARGS__);                                                                                                  \
        printf("\n");                                                                                                                         \
        printf("Exiting...\n");                                                                                                               \
        exit(err_no);                                                                                                                              \
    } while (0)

char *errTypeToString(ErrorTypes err_no);

#endif