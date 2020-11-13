// IFJ20
// Autors:
// Mario Harvan, xharva03
// Juraj Marticek, xmarti97
// Michal Slesar, xslesa01
// Erik Belko, xbelko02

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "dataTypes.h"
#include "symtable.h"
#include "string.h"
#include "test.h"
#include "stack.h"

//global variables
int totalCnt = 0;
int okCnt = 0;
int failedCnt = 0;

//test data variables
int data1;
int data2[5];
int data3[5];

int main(){
    //Tests for symTable
    beginTest("Zakladny test na vkladanie a hladanie v symtable");
    htab_t *table = htab_init(20);
    char data[] = "test1";
    htab_lookup_add(table, data);
    htab_iterator_t tmp = htab_find(table, data);
    ASSERT_EQ_ARR(tmp.ptr->name, data, (int)sizeof(data));
    tmp = htab_find(table, "test");
    ASSERT_EQ(tmp.ptr, NULL);
    htab_free(table);


    //test 3
    beginTest("Test zhody poli");
    ASSERT_EQ_ARR(data2, data3, 5);

    beginTest("String init");
    String string;
    ASSERT_EQ(string_init(&string), true);

    beginTest("String push & compare");
    string_append_char(&string, 'a');
    string_append_string(&string, "a");
    ASSERT_EQ((int) string.length, 2);
    ASSERT_EQ((int) string.size, (int) STRING_BLOCK_SIZE);
    ASSERT_EQ(string_compare(&string, "aa"), true);
    string_append_string(&string, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ASSERT_EQ((int) string.length, 35);
    ASSERT_EQ((int) string.size, (int) (STRING_BLOCK_SIZE * 2));
    ASSERT_EQ(string_compare(&string, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), true);
    string_append_string(&string, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ASSERT_EQ((int) string.length, 68);
    ASSERT_EQ((int) string.size, (int) (STRING_BLOCK_SIZE * 3));
    ASSERT_EQ(string_compare(&string, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), true);

    beginTest("String clear");
    ASSERT_EQ(string_clear(&string), true);
    ASSERT_EQ((int) string.length, 0);

    beginTest("String free");
    ASSERT_EQ(string_free(&string), true);
    ASSERT_EQ((int) string.length, 0);
    ASSERT_EQ((int) string.size, 0);

    beginTest("Stack testing");
    Stack *tmpStack = stackInit();
    Token test1;
    test1.type = TOKEN_COMA;
    ASSERT_EQ(stackIsEmpty(tmpStack), true);
    stackPush(tmpStack, test1);
    ASSERT_EQ(stackIsEmpty(tmpStack), false);
    ASSERT_EQ(stackPeek(tmpStack).type, TOKEN_COMA);
    ASSERT_EQ(stackPop(tmpStack).type, TOKEN_COMA);
    stackFree(tmpStack);

    beginTest("Advanced stack testing");
    tmpStack = stackInit();
    Token test2;
    test2.type = TOKEN_DECLARATION;

    ASSERT_EQ(stackIsEmpty(tmpStack), true);
    for(int i = 0; i < 22; i++){
        if(i % 2 == 0){
            stackPush(tmpStack, test1);
        }
        else stackPush(tmpStack, test2);
    }

    for(int i = 21; i >= 0; i--){
        if(i % 2 == 0){
            ASSERT_EQ(stackPop(tmpStack).type, TOKEN_COMA);
        }
        else ASSERT_EQ(stackPop(tmpStack).type, TOKEN_DECLARATION);
    }
    
    

    ASSERT_EQ(stackIsEmpty(tmpStack), true);

    stackFree(tmpStack);
    endTests();
    return 0;
}

void beginTest(char* str){
    printf("################# Test number: %d #################\n", totalCnt);
    printf("%s\n\n", str);
    totalCnt++;
    testData();
}

void endTests(){
    printf("################# END OF TESTING #################\n\n");
    printf("Total tests: %d\nSuccessful tests: %d\nFailed tests: %d\n", totalCnt, okCnt, failedCnt);
}

void testData(){
    data1 = 10;
    data2[0] = 4;
    data2[1] = 3;
    data2[2] = 4;
    data2[3] = 3;
    data2[4] = 4;

    data3[0] = 4;
    data3[1] = 3;
    data3[2] = 5;
    data3[3] = 3;
    data3[4] = 4;
    
}