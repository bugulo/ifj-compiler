/* IFJ20 - Scanner
 * Authors:
 * Michal Slesar, xslesa01
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

typedef enum {
    KEYWORD_PACKAGE,
    KEYWORD_FUNC,
    KEYWORD_RETURN,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_FOR,
    KEYWORD_FLOAT64,
    KEYWORD_STRING,
    KEYWORD_INT
} KeywordType;

typedef enum {
    TOKEN_EOF,              // End of file
    TOKEN_EOL,              // End of line

    TOKEN_IDENTIFIER,       // Identifier
    TOKEN_KEYWORD,          // Keyword

    TOKEN_COMA,             // Coma character, ',', 0x2C
    TOKEN_SEMICOLON,        // Semicolon character, ';', 0x3B

    // Brackets
    TOKEN_BRACKET_LEFT,     // Left round bracket, (
    TOKEN_BRACKET_RIGHT,    // Right round bracket, )
    TOKEN_BRACE_LEFT,       // Left curly bracket, {
    TOKEN_BRACE_RIGHT,      // Right curly bracket, }

    // Assignment operators
    TOKEN_DECLARATION,      // Short variable declaration, :=
    TOKEN_ASSIGNMENT,       // Variable assignment, =

    // Arithmetic operators
    TOKEN_PLUS,             // Addition operator, +
    TOKEN_MINUS,            // Subtraction operator, -
    TOKEN_MUL,              // Multiplaction operator, *
    TOKEN_DIV,              // Division operator, /

    // Relational operators
    TOKEN_LESS,             // Less than, <
    TOKEN_LESS_EQ,          // Less than or equal to, <=
    TOKEN_GREATER,          // Greater than, >
    TOKEN_GREATER_EQ,       // Greater than or equal to, >=
    TOKEN_EQUALS,           // Equal to, ==
    TOKEN_NOT_EQUALS,       // Not equal to, !=

    // Literals
    TOKEN_NUMBER_INT,       // Integer number, 5465
    TOKEN_NUMBER_FLOAT,     // Floating point number, 0.45454
    TOKEN_STRING            // String, "string"
} TokenType;

typedef union {
    int i;
    char* s;
    double d;
    KeywordType k;
} TokenValue;

typedef struct {
    TokenType type;
    TokenValue value;
} Token;

/**
 * Set source file that should be parsed by scanner
 * 
 * @param file File pointer
 */
void set_file(FILE *file);

/**
 * Parse and retrieve token
 * 
 * @param token Token pointer
 * 
 * @return Error code
 */
int get_token(Token *token);

#endif