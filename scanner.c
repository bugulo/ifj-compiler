/* IFJ20 - String Library
 * Authors:
 * Michal Slesar, xslesa01
 */

#include "scanner.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "file.h"
#include "error.h"
#include "string.h"

/** File to read from */
dynamicArr *source = NULL;

void scanner_set_file(dynamicArr *file) {
    source = file;
}

typedef enum {
    STATE_START,
    STATE_IDENTIFIER,
    STATE_COMMENT_LINE,
    STATE_COMMENT_BLOCK,
    STATE_INTEGER,
    STATE_DECIMAL,
    STATE_EXPONENT_SIGN,
    STATE_EXPONENT_START,
    STATE_EXPONENT,
    STATE_STRING
} State;

/** Compare string with available keywords */
KeywordType get_keyword_type(String string) {
    if(string_compare(&string, "package")) return KEYWORD_PACKAGE;
    else if(string_compare(&string, "func")) return KEYWORD_FUNC;
    else if(string_compare(&string, "return")) return KEYWORD_RETURN;
    else if(string_compare(&string, "if")) return KEYWORD_IF;
    else if(string_compare(&string, "else")) return KEYWORD_ELSE;
    else if(string_compare(&string, "for")) return KEYWORD_FOR;
    else if(string_compare(&string, "float64")) return KEYWORD_FLOAT64;
    else if(string_compare(&string, "string")) return KEYWORD_STRING;
    else if(string_compare(&string, "int")) return KEYWORD_INT;
    else return KEYWORD_NONE;
}

/** Convert string to 64bit integer */
bool string_to_integer(String string, int64_t *number) {
    char *endptr;
	*number = strtoll(string.ptr, &endptr, 10);
    return *endptr == '\0';
}

/** Convert string to double */
bool string_to_double(String string, double *number) {
    char *endptr;
	*number = strtod(string.ptr, &endptr);
    return *endptr == '\0';
}

void scanner_get_token(Token *token) {
    Result result = RESULT_NONE;
    State state = STATE_START;
    String string;

    if(source == NULL || !string_init(&string))
        result = RESULT_ERROR;

    while(result == RESULT_NONE) {
        /** Get next character */
        int c = arrGetc(source);

        if(state == STATE_START) {
            if(c == '\n') {
                token->type = TOKEN_EOL;
                result = RESULT_OK;
            } else if(c == EOF) {
                token->type = TOKEN_EOF;
                result = RESULT_OK;
            } else if(c == ',') {
                token->type = TOKEN_COMA;
                result = RESULT_OK;
            } else if(c == ';') {
                token->type = TOKEN_SEMICOLON;
                result = RESULT_OK;
            } else if(c == '(') {
                token->type = TOKEN_BRACKET_LEFT;
                result = RESULT_OK;
            } else if(c == ')') {
                token->type = TOKEN_BRACKET_RIGHT;
                result = RESULT_OK;
            } else if(c == '{') {
                token->type = TOKEN_BRACE_LEFT;
                result = RESULT_OK;
            } else if(c == '}') {
                token->type = TOKEN_BRACE_RIGHT;
                result = RESULT_OK;
            } else if(c == '+') {
                token->type = TOKEN_PLUS;
                result = RESULT_OK;
            } else if(c == '-') {
                token->type = TOKEN_MINUS;
                result = RESULT_OK;
            } else if(c == '*') {
                token->type = TOKEN_MUL;
                result = RESULT_OK;
            } else if(c == ':') {
                char c1 = arrGetc(source);

                if(c1 == '=') {
                    token->type = TOKEN_DECLARATION;
                    result = RESULT_OK;
                } else 
                    result = RESULT_ERROR;
            } else if(c == '!') {
                char c1 = arrGetc(source);

                if(c1 == '=') {
                    token->type = TOKEN_NOT_EQUALS;
                    result = RESULT_OK;
                } else 
                    result = RESULT_ERROR;
            } else if(c == '=') {
                char c1 = arrGetc(source);

                if(c1 == '=') {
                    token->type = TOKEN_EQUALS;
                    result = RESULT_OK;
                } else {
                    token->type = TOKEN_ASSIGNMENT;
                    result = RESULT_OK;
                    arrUnGetc(source);
                }
            } else if(c == '<') {
                char c1 = arrGetc(source);

                if(c1 == '=') {
                    token->type = TOKEN_LESS_EQ;
                    result = RESULT_OK;
                } else {
                    token->type = TOKEN_LESS;
                    result = RESULT_OK;
                    arrUnGetc(source);
                }
            } else if(c == '>') {
                char c1 = arrGetc(source);

                if(c1 == '=') {
                    token->type = TOKEN_GREATER_EQ;
                    result = RESULT_OK;
                } else {
                    token->type = TOKEN_GREATER;
                    result = RESULT_OK;
                    arrUnGetc(source);
                }
            } else if(isalpha(c) || c == '_') {
                if(string_append_char(&string, c))
                    state = STATE_IDENTIFIER;
                else
                    result = RESULT_ERROR;
            } else if(c == '/') {
                char c1 = arrGetc(source);

                if(c1 == '/')
                    state = STATE_COMMENT_LINE;
                else if(c1 == '*')
                    state = STATE_COMMENT_BLOCK;
                else {
                    token->type = TOKEN_DIV;
                    result = RESULT_OK;
                    arrUnGetc(source);
                }
            } else if(isdigit(c)) {
                if(string_append_char(&string, c))
                    state = STATE_INTEGER;
                else 
                    result = RESULT_ERROR;
            } else if(c == '"') {
                state = STATE_STRING;
            } else if(!isspace(c)) {
                result = RESULT_ERROR;
            }
        } else if(state == STATE_IDENTIFIER) {
            if(isalnum(c) || c == '_') {
                if(!string_append_char(&string, c))
                    result = RESULT_ERROR;
            } else {
                /** Check if string is keyword or identifier */
                KeywordType keyword = get_keyword_type(string);
                if(keyword != KEYWORD_NONE) {
                    token->type = TOKEN_KEYWORD;
                    token->value.k = keyword;
                    result = RESULT_OK;
                } else {
                    if(string_init(&token->value.s) && string_append_string(&token->value.s, string.ptr)) {
                        token->type = TOKEN_IDENTIFIER;
                        result = RESULT_OK;
                    } else 
                        result = RESULT_ERROR;
                }
                arrUnGetc(source);
            }
        } else if(state == STATE_COMMENT_LINE) {
            if(c == '\n' || c == EOF) {
                state = STATE_START;
                arrUnGetc(source);
            }
        } else if(state == STATE_COMMENT_BLOCK) {
            char c1 = arrGetc(source);

            if(c == '*' && c1 == '/')
                state = STATE_START;
            else if(c == EOF || c1 == EOF)
                result = RESULT_ERROR;
            else
                arrUnGetc(source);
        } else if(state == STATE_INTEGER) {
            if(isdigit(c)) {
                if(!string_append_char(&string, c))
                    result = RESULT_ERROR;
            } else if(c == '.') {
                char c1 = arrGetc(source);

                /** Check trailing zeroes */
                if((string.ptr[0] != '0' || string_compare(&string, "0")) && isdigit(c1) && string_append_char(&string, c)) {
                    state = STATE_DECIMAL;
                    arrUnGetc(source);
                } else 
                    result = RESULT_ERROR;
            } else if(c == 'e' || c == 'E') {
                /** Check trailing zeroes */
                if((string.ptr[0] != '0' || string_compare(&string, "0")) && string_append_char(&string, c))
                    state = STATE_EXPONENT_SIGN;     
                else 
                    result = RESULT_ERROR;
            } else {
                int64_t number;
                /** Check trailing zeroes */
                if((string.ptr[0] != '0' || string_compare(&string, "0")) && string_to_integer(string, &number)) {
                    token->type = TOKEN_NUMBER_INT;
                    token->value.i = number;
                    result = RESULT_OK;
                    arrUnGetc(source);
                } else
                    result = RESULT_ERROR;
            }
        } else if(state == STATE_DECIMAL) {
            if(isdigit(c)) {
                if(!string_append_char(&string, c))
                    result = RESULT_ERROR;
            } else if(c == 'e' || c == 'E') {
                if(string_append_char(&string, c))
                    state = STATE_EXPONENT_SIGN;     
                else 
                    result = RESULT_ERROR;
            } else {
                double number;
                if(string_to_double(string, &number)) {
                    token->type = TOKEN_NUMBER_FLOAT;
                    token->value.d = number;
                    result = RESULT_OK;
                    arrUnGetc(source);
                } else
                    result = RESULT_ERROR;
            }
        } else if(state == STATE_EXPONENT_SIGN) {
            if(c == '+' || c == '-') {
                if(string_append_char(&string, c))
                    state = STATE_EXPONENT_START;
                else 
                    result = RESULT_ERROR;
            } else if(isdigit(c)) {
                state = STATE_EXPONENT_START;
                arrUnGetc(source);
            } else
                result = RESULT_ERROR;
        } else if(state == STATE_EXPONENT_START) {
            char c1 = arrGetc(source);

            if(isdigit(c) && c == '0' && !isdigit(c1)) {
                state = STATE_EXPONENT;
                arrUnGetc(source);
                arrUnGetc(source);
            } else if(isdigit(c) && c == '0' && isdigit(c1) && c != '0') {
                state = STATE_EXPONENT;
                arrUnGetc(source);
            } else if(isdigit(c) && c != '0') {
                state = STATE_EXPONENT;
                arrUnGetc(source);
                arrUnGetc(source);
            } else if(c == '0') {
                arrUnGetc(source);
            } else 
                result = RESULT_ERROR;
        } else if(state == STATE_EXPONENT) {
            if(isdigit(c)) {
                if(!string_append_char(&string, c))
                    result = RESULT_ERROR;
            } else {
                double number;
                if(string_to_double(string, &number)) {
                    token->type = TOKEN_NUMBER_FLOAT;
                    token->value.d = number;
                    result = RESULT_OK;
                    arrUnGetc(source);
                } else
                    result = RESULT_ERROR;
            }
        } else if(state == STATE_STRING) {
            if(c == '\\') {
                char c1 = arrGetc(source);

                if(c1 == '"' || c1 == '\\') {
                    if(string_append_char(&string, c1))
                        state = STATE_STRING;
                    else 
                        result = RESULT_ERROR;
                } else if(c1 == 'n') {
                    if(string_append_char(&string, '\n'))
                        state = STATE_STRING;
                    else 
                        result = RESULT_ERROR;
                } else if(c1 == 't') {
                    if(string_append_char(&string, '\t'))
                        state = STATE_STRING;
                    else 
                        result = RESULT_ERROR;
                } else if(c1 == 'x') {
                    char c2 = toupper(arrGetc(source));
                    char c3 = toupper(arrGetc(source));

                    /** Convert hexadecimal character to decimal */
                    if((isdigit(c2) || (c2 >= 'A' && c2 <= 'F')) && (isdigit(c3) || (c3 >= 'A' && c3 <= 'F'))) {
                        char combined = (c2 >= 'A' ? c2 - 'A' + 10 : c2 - '0') * 16 + (c3 >= 'A' ? c3 - 'A' + 10 : c3 - '0');

                        if(string_append_char(&string, combined))
                            state = STATE_STRING;
                        else
                            result = RESULT_ERROR;
                    } else
                        result = RESULT_ERROR;
                } else 
                    result = RESULT_ERROR;
            } else if(c == '"') {
                if(string_init(&token->value.s) && string_append_string(&token->value.s, string.ptr)) {
                    token->type = TOKEN_STRING;
                    result = RESULT_OK;
                } else 
                    result = RESULT_ERROR;
            } else if(c >= 32) {
                if(!string_append_char(&string, c))
                    result = RESULT_ERROR;
            } else {
                result = RESULT_ERROR;
            }
        }
    }

    string_free(&string);

    if(result != RESULT_OK)
        throw_error_fatal(LEXICAL_ERROR, "%s", "Lexical error");
}