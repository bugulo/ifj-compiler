// IFJ20
// Autors:
// Mario Harvan, xharva03
// Juraj Marticek, xmarti97
// Michal Slesar, xslesa01
// Erik Belko, xbelko02
#ifndef BITSET_H_
#define BITSET_H_


#define ASSERT_EQ(x, y) ( (x == y) ? (printf("\033[0;32m"), printf("Test OK\n\n"), printf("\033[0m"), okCnt++ ): \
    (fprintf(stderr, "\033[0;31mTest FAILED\n\n%d != %d \033[0m\n\n", x, y),failedCnt++))

#define ASSERT_EQ_CHAR(x, y) ( (x == y) ? (printf("\033[0;32m"), printf("Test OK\n\n"), printf("\033[0m"), okCnt++ ): \
    (fprintf(stderr, "\033[0;31mTest FAILED\n\n%c != %c \033[0m\n\n", x, y), failedCnt++))

#define ASSERT_EQ_double(x, y) ( (x == y) ? (printf("\033[0;32m"), printf("Test OK\n\n"), printf("\033[0m"), okCnt++ ): \
    (fprintf(stderr, "\033[0;31mTest FAILED\n\n%f != %f \033[0m\n\n", x, y),failedCnt++))

#define ASSERT_EQ_ARR(x, y, j)\
    do{\
    for(int i = 0; i < j; i++){\
        if(x[i] != y[i]) {fprintf(stderr, "\033[0;31mTest FAILED\n\nData on index %d are not equal\033[0m\n\n", i); failedCnt++; break;}\
        if((i + 1) == j) {printf("\033[0;32m"); printf("Test OK\n\n"); printf("\033[0m"); okCnt++;}\
    }\
    }while(0)\

void beginTest(char* str);

void testData();

void endTests();

#endif