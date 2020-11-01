// IFJ20
// Autors:
// Mario Harvan, xharva03
// Juraj Marticek, xmarti97
// Michal Slesar, xslesa01
// Erik Belko, xbelko02



#include "dataTypes.h"
#include "symtable.h"
#include <stdio.h>
#include <assert.h>
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
    //koncept ako bude prebiehat test

    //test 1
    beginTest("Test funkcie placeHolder, ci naozaj holdujePlacer");
    ASSERT_EQ(placeHolder(data1), 320);

    //test 2
    beginTest("Test funkcie placeHolder, ci  neholdujePlacer");
    ASSERT_EQ(placeHolder(data1), 420);

    //test 3
    beginTest("Test zhody poli");
    ASSERT_EQ_ARR(data2, data3, 5);


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