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
#include "expression.h"
#include "vector.h"
#include "symtable.h"
#include "code_gen.h"
#include "semantic_analysis.h"

#include "scanner.h"
#include "parser.h"

#include "scanner.h"
#include "parser.h"
#include "file.h"

int main(int argc, char **argv)
{
    parse();
    /*gen_init();
    Vector *varScopeVec = vectorInit();
    insertLocalSymTable(varScopeVec);
    TokenValue sampleStringVal;
    sampleStringVal.i = 1;
    defineUserVar(getLocalSymTable(varScopeVec), "sampleString", INTEGER, sampleStringVal, true);


    // sampleString := 1;
    String sampleString;
    string_init(&sampleString);
    string_append_string(&sampleString, "sampleString");
    Var dest = {.name = sampleString, .frame = LOCAL_FRAME};
    Symb val = {.isVar = false, .token.type = TOKEN_NUMBER_INT, .token.value.i = 1};
    define_var(dest, val, varScopeVec);

    removeLocalSymTable(varScopeVec);

    // funkcie

    insertLocalSymTable(varScopeVec);
    defineUserVar(getLocalSymTable(varScopeVec), "a", INTEGER, sampleStringVal, false);
    defineUserVar(getLocalSymTable(varScopeVec), "b", INTEGER, sampleStringVal, false);
    defineUserVar(getLocalSymTable(varScopeVec), "c", INTEGER, sampleStringVal, false);

    Vector *input = vectorInit();
    Vector *output = vectorInit();
    vectorPush(input, "a");
    vectorPush(input, "b");
    vectorPush(input, "c");
    func f (int a, int b, int c) (int, int) {
    }
    */
    /*declare_function("f", input, varScopeVec);

    insertLocalSymTable(varScopeVec);
    defineUserVar(getLocalSymTable(varScopeVec), "int1", INTEGER, sampleStringVal, false);
    defineUserVar(getLocalSymTable(varScopeVec), "int2", INTEGER, sampleStringVal, false);
    insertLocalSymTable(varScopeVec);
    defineUserVar(getLocalSymTable(varScopeVec), "int1", INTEGER, sampleStringVal, false);

    vectorPush(output, "int1");
    vectorPush(output, "int2");
    return_function(output, varScopeVec);

    removeLocalSymTable(varScopeVec);
    removeLocalSymTable(varScopeVec);
    removeLocalSymTable(varScopeVec);

    Vector *returnParams = vectorInit();
    vectorPush(returnParams, "ret1");
    vectorPush(returnParams, "ret2");

    insertLocalSymTable(varScopeVec);
    defineUserVar(getLocalSymTable(varScopeVec), "a", INTEGER, sampleStringVal, false);
    defineUserVar(getLocalSymTable(varScopeVec), "b", INTEGER, sampleStringVal, false);
    defineUserVar(getLocalSymTable(varScopeVec), "c", INTEGER, sampleStringVal, false);
    insertLocalSymTable(varScopeVec);
    defineUserVar(getLocalSymTable(varScopeVec), "ret1", INTEGER, sampleStringVal, false);
    defineUserVar(getLocalSymTable(varScopeVec), "ret2", INTEGER, sampleStringVal, false);

    // int ret1, ret2 = f(a, b, c)
    call_function("f", input, returnParams, varScopeVec);

    removeLocalSymTable(varScopeVec);
    if(res) {
        c = a + b;
    } else {
        c = a - b;
    }
    */
    /*String tmpVar = defineCompilerVar(getLocalSymTable(varScopeVec), INTEGER, sampleStringVal, false);

    if_start(tmpVar.ptr, varScopeVec);
    printf("ADD c a b\n");
    if_core();
    printf("SUB c a b\n");
    if_end();
    for i := 0; i < j; i++ {
        sum += i;
    }
    */
    /*defineUserVar(getLocalSymTable(varScopeVec), "sampleString", INTEGER, sampleStringVal, true);
    insertLocalSymTable(varScopeVec);
    for_start();
    tmpVar = defineCompilerVar(getLocalSymTable(varScopeVec), INTEGER, sampleStringVal, false);
    Var res;
    res.name.ptr = tmpVar.ptr;

    print_i("CREATEFRAME");
    print_i("DEFINE TF@res");
    print_i("LT TF@res LF@i LF@j");
    for_body(res, varScopeVec);
    // aa = 0.111111111111 + 0.037037037037037
    Symb op1 = {.isVar = false, .token.type = TOKEN_NUMBER_FLOAT, .token.value.d = 1.0 / 9};
    Symb op2 = {.isVar = false, .token.type = TOKEN_NUMBER_FLOAT, .token.value.d = 1.0 / 27};
    ADD(dest, op1, op2, false, varScopeVec);
    for_end();*/
    return 0;
}
