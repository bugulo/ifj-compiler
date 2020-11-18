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
#include "parser.h"

int main(int argc, char **argv)
{
#ifdef DEBUG
    print("%s", "debug_print z mainu\n");
    throw_error_fatal(LEXICAL_ERROR, "%s", "uhoh");
#endif

    FILE *f = fopen("test.go", "r");

    scanner_set_file(f);

    parser_main();

    fclose(f);

    return 0;
}
