/* IFJ20 - Dynamic char array
 * Authors:
 * Mario Harvan, xharva03
 */

#ifndef file_h
#define file_h

#define DEFAULT_ARR_SIZE 1000
#define ARR_ERROR -2

typedef struct{
    unsigned length;
    unsigned size;
    unsigned position;
    char* arr;
} dynamicArr;

/**
 * @brief Allocate new dynamic array
 * @return Pointer to array, or null when fail
*/
dynamicArr* arrInit();

/**
 * @brief Same as fputc
 * @param Pointer to dynamic arr
 * @param Char 
*/
void arrPutc(dynamicArr* arr, char c);

/**
 * @Brief Same as fgetc
 * @param Pointer do dynamic arr
 * @return Char or EOF when end of arr
*/
int arrGetc(dynamicArr* arr);

/**
 * @brief Same as ungetc
 * @param Pointer to dynamic arr
*/
void arrUnGetc(dynamicArr* arr);

/**
 * @brief Function will reset read position to start of arr
 * @param Pointer to dynamic arr
*/
void arrSeekStart(dynamicArr* arr);

/**
 * @brief Function will free dynamic array
 * @param Pointer to dynamic arr
*/
void arrFree(dynamicArr* arr);

/**
 * @brief Function will copy stdin to dynamic array
 * @param Pointer to dynamic arr
*/
void copyStdinToArr(dynamicArr* arr);

#endif