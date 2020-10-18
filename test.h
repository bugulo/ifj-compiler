// IFJ20
// Autors:
// Mario Harvan, xharva03
// Juraj Marticek, xmarti97
// Michal Slesar, xslesa01
// Erik Belko, xbelko02
#ifndef BITSET_H_
#define BITSET_H_


#define ASSERT_EQ(x, y) ( (x == y) ? (printf("Test OK\n\n"), okCnt++ ): \
    (printf("\033[0;31m"),printf("Test FAILED\n\n%d != %d \n\n", x, y), printf("\033[0m"),failedCnt++))

#define ASSERT_EQ_CHAR(x, y) ( (x == y) ? (printf("Test OK\n\n"), okCnt++ ): \
    (printf("\033[0;31m"),printf("Test FAILED\n\n%c != %c \n\n", x, y), printf("\033[0m"),failedCnt++))

#define ASSERT_EQ_double(x, y) ( (x == y) ? (printf("Test OK\n\n"), okCnt++ ): \
    (printf("\033[0;31m"),printf("Test FAILED\n\n%f != %f \n\n", x, y), printf("\033[0m"),failedCnt++))

void beginTest(char* str);

void endTests();

#endif