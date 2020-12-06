/* IFJ20 - Hash table library (Tento modul bol prevzatý z predmetu IJC, projekt 2. Hlavičkový súbor bol súčasťou zadania a bol upravený autorom tohto súboru) 
 * Authors:
 * Mario Harvan, xharva03
 */

#ifndef SYMTABLE_H_
#define SYMTABLE_H_

#include <string.h>     // size_t
#include <stdbool.h>    // bool
#include "scanner.h"
#include "vector.h"

//table struct
struct htab {
    size_t size;
    size_t arr_size;
    struct htab_item *ptr[];
};


typedef struct htab htab_t;

typedef const char * htab_key_t;

typedef enum {
    INTEGER,
    FLOAT,
    STRING,
    BOOL,
    NONE
} varDataType;

//table item
struct htab_item {
    htab_key_t name;            //symbol name
    TokenType tokenType;        //token type
    //for variables
    bool isVar;                 //true when its var, false if its func
    varDataType varDataType;    //data type of variables
    TokenValue varValue;        //value of statically defined variable
    bool isVarUsedDefined;      //set to false when its compiler created variable
    bool isConst;               //set to false if its not constant anymore
    unsigned varCnt;
    //for functions
    Vector *returnTypes;        //return types from function
    Vector *paramTypes;         //params for functions
    
    struct htab_item *next;     //pointer to next item
};

//item iterator
typedef struct htab_iterator {
    struct htab_item *ptr;      // pointer to item
    const htab_t *t;            // pointer to table
    size_t idx;                 //internal ID
} htab_iterator_t;     

//hash function
size_t htab_hash_fun(htab_key_t str);

/**
 * @brief Function will initialize empty table
 * @param Size of array (will impact performance)
 * @return Return pointer to table
*/
htab_t *htab_init(size_t n);

/**
 * @brief Get number of items in table
 * @param Pointer to table
 * @return Number of items
*/
size_t htab_size(const htab_t * t);

/**
 * @brief Get size of array
 * @param Pointer to table
 * @return Size of array
*/
size_t htab_bucket_count(const htab_t * t);

/**
 * @brief Find symbol by its name
 * @param Pointer to table
 * @param Name of symbol
 * @return return iterator to item when found, or empty iterator
*/
htab_iterator_t htab_find(htab_t * t, htab_key_t name);

/**
 * @brief Function will insert new symbol to table, if symbol already exist, it will return empty iterator
 * @param Pointer to table
 * @param Name of symbol
 * @return Iterator to new, or existing item
*/
htab_iterator_t htab_insert(htab_t * t, htab_key_t name);

/**
 * @brief Erase symbol from table
 * @param Pointer to a table
 * @param Iterator with symbol that we want to delete
*/
void htab_erase(htab_t * t, htab_iterator_t it);

/**
 * @brief Set iterator to first symbol in table
 * @param Pointer to a table
 * @return Iterator to first item, NULL iterator when error
*/
htab_iterator_t htab_begin(const htab_t * t);

/**
 * @brief Set iterator after last symbol in table
 * @param Pointer to a table
 * @return Iterator after last item, NULL iterator when error
*/
htab_iterator_t htab_end(const htab_t * t); 

/**
 * @brief Set iterator to next item in table
 * @param Iterator to item
 * @return Iterator to next item, NULL iterator on error
*/
htab_iterator_t htab_iterator_next(htab_iterator_t it);

static inline bool htab_iterator_valid(htab_iterator_t it) { return it.ptr!=NULL; }

static inline bool htab_iterator_equal(htab_iterator_t it1, htab_iterator_t it2) {
  return it1.ptr==it2.ptr && it1.t == it2.t;
}

/**
 * @brief Remove all items from table
 * @param Pointer to table
*/
void htab_clear(htab_t * t); 

/**
 * @brief Remove table and all items
 * @param Pointer to a table
*/
void htab_free(htab_t * t);

#endif