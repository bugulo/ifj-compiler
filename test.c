// IFJ20
// Autors:
// Mario Harvan, xharva03
// Juraj Marticek, xmarti97
// Michal Slesar, xslesa01
// Erik Belko, xbelko02


//Tu sa budu pisat testy ku vsetkym funkciam, a modulom. Sluzi to hlavne na to aby sme mohli testovat casti programu aj kym nebudeme mat funkcny prekladac

#include "dataTypes.h"
#include "symtable.h"
#include <stdio.h>
#include <assert.h>
#include "test.h"

//global variables
int totalCnt = 0;
int okCnt = 0;
int failedCnt = 0;

int main(){
    //koncept ako bude prebiehat test

    //test 1
    beginTest("Test funkcie placeHolder, ci naozaj holdujePlacer");
    ASSERT_EQ(placeHolder(10), 320);

    //test 2
    beginTest("Test funkcie placeHolder, ci  neholdujePlacer");
    ASSERT_EQ(placeHolder(10), 420);



    endTests();
    return 0;
}

void beginTest(char* str){
    printf("################# Test number: %d #################\n", totalCnt);
    printf("%s\n\n", str);
    totalCnt++;
}

void endTests(){
    printf("################# END OF TESTING #################\n\n");
    printf("Total tests: %d\nOk tests: %d\nFailed tests: %d\n", totalCnt, okCnt, failedCnt);
}