// IFJ20
// Autors:
// Mario Harvan, xharva03
// Juraj Marticek, xmarti97
// Michal Slesar, xslesa01
// Erik Belko, xbelko02

#include <stdio.h>

#include "symtable.h"
#include "dataTypes.h"
#include "error.h"
#include "scanner.h"
#include "string.h"

int main()
{
    scanner_set_file(stdin);

    while(1) {
        Token token;
        Result result = scanner_get_token(&token);

        if(result == RESULT_ERROR) {
            printf("Lexical error\n");
            throw_error_fatal(LEXICAL_ERROR, "%s", "Lexical error");
            break;
        }

        if(token.type == TOKEN_STRING || token.type == TOKEN_IDENTIFIER) {
            printf("Token Type: %d, Value: %s\n", (int) token.type, token.value.s.ptr);
            string_free(&token.value.s);
        } else if(token.type == TOKEN_NUMBER_FLOAT) {
            printf("Token Type: %d, Value: %f\n", (int) token.type, token.value.d);
        } else if(token.type == TOKEN_NUMBER_INT) {
            printf("Token Type: %d, Value: %d\n", (int) token.type, token.value.i);
        } else 
            printf("Token Type: %d\n", (int) token.type);

        if(token.type == TOKEN_EOF)
            break;
    }

    return 0;
}
