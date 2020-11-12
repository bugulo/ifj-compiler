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
    ASSERT_EQ_ARR(tmp.ptr->name, data, sizeof(data));
    tmp = htab_find(table, "test");
    ASSERT_EQ(tmp.ptr, NULL);
    htab_free(table);


    //test 3
    beginTest("Test zhody poli");
    ASSERT_EQ_ARR(data2, data3, 5);

    beginTest("String init");
    string string1;
    ASSERT_EQ(string_init(&string1), true);

    beginTest("String push & compare");
    string_append(&string1, "a");
    string_append(&string1, "a");
    ASSERT_EQ((int) string1.length, 2);
    ASSERT_EQ((int) string1.size, (int) STRING_BLOCK_SIZE);
    ASSERT_EQ(string_compare(&string1, "aa"), true);
    string_append(&string1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ASSERT_EQ((int) string1.length, 35);
    ASSERT_EQ((int) string1.size, (int) (STRING_BLOCK_SIZE * 2));
    ASSERT_EQ(string_compare(&string1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), true);
    string_append(&string1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    ASSERT_EQ((int) string1.length, 68);
    ASSERT_EQ((int) string1.size, (int) (STRING_BLOCK_SIZE * 3));
    ASSERT_EQ(string_compare(&string1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), true);

    beginTest("String clear");
    ASSERT_EQ(string_clear(&string1), true);
    ASSERT_EQ((int) string1.length, 0);

    beginTest("String free");
    ASSERT_EQ(string_free(&string1), true);
    ASSERT_EQ((int) string1.length, 0);
    ASSERT_EQ((int) string1.size, 0);

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