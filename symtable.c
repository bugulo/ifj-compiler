// IFJ20
// Autors:
// Mario Harvan, xharva03
// Juraj Marticek, xmarti97
// Michal Slesar, xslesa01
// Erik Belko, xbelko02

#include "symtable.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "error.h"

//tento modul bol definovany v zadani, str. 20


size_t htab_hash_fun(htab_key_t str){
    uint32_t h=0;     // musí mít 32 bitů
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}

htab_t *htab_init(size_t n){
    if(n == 0) return NULL;
    //init array
    htab_t *tmp = malloc(sizeof(htab_t) + n * sizeof(struct htab_item));
    if (tmp == NULL){
        return NULL;
    }
    //set size
    tmp->arr_size = n;
    tmp->size = 0;
    //null array
    for(size_t i = 0; i < n; i++){
        tmp->ptr[i] = NULL;
    }
    return tmp;
}

size_t htab_size(const htab_t * t){
    if (t == NULL) {
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to table");
        return 0;
    }
    return t->size;
}

size_t htab_bucket_count(const htab_t * t){
    if (t == NULL) {
        if (t == NULL) {
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to table");
        return 0;
        }
    }
    return t->arr_size;
}

htab_iterator_t htab_find(htab_t * t, htab_key_t name){
    htab_iterator_t tmpIterator = {NULL, NULL, 0};
    //check if t is valid, if not return NULL iterator
    if(t == NULL){
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to table");
        return tmpIterator;
    }
    if(name == NULL){
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to name");
        return tmpIterator;
    }
    //get index from hash function
    int ptrIndex = (htab_hash_fun(name) % t->arr_size);
    //save pointer to first item in list
    struct htab_item *tmpItem = t->ptr[ptrIndex];
    bool stringFound = false;
    //loop while we are not at the end of list
    while(tmpItem != NULL){
        //check if we found searched key
        if(strcmp(name, tmpItem->name) == 0){
            stringFound = true;
            break;
        }
        //go to next item
        tmpItem = tmpItem->next;
    }
    //setup iterator
    if(stringFound == true){
        tmpIterator.ptr = tmpItem;
        tmpIterator.t = t;
        tmpIterator.idx = ptrIndex;
        return tmpIterator;
    }
    //if found failed, return end iterator
    return htab_end(t);
}

htab_iterator_t htab_insert(htab_t * t, htab_key_t name){
    htab_iterator_t tmpIterator = {NULL, NULL, 0};
    //check if t is valid, if not return NULL iterator
    if(t == NULL){
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to table");
        return tmpIterator;
    }
    if(name == NULL){
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to name");
        return tmpIterator;
    }
    //try to find item
    tmpIterator = htab_find(t, name);
    struct htab_item *tmpItem;
    struct htab_item *tmpLastItem;
    //if item wasnt found
    if(htab_iterator_equal(tmpIterator, htab_end(t)) == true){
        tmpItem = t->ptr[htab_hash_fun(name) % t->arr_size];
        tmpLastItem = tmpItem;
        //get to last last item in list
        while(tmpItem != NULL){
            tmpLastItem = tmpItem;
            tmpItem = tmpItem->next;
        }
        if(tmpItem == NULL){
            //malloc new item
            tmpItem = malloc(sizeof(struct htab_item));
            if(tmpItem == NULL){
                throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation failed");
            }
            //setup new item
            tmpItem->next = NULL;
            //prepare string
            char * nameArr = malloc((strlen(name) * sizeof(char)) + 1);
            if(nameArr == NULL){
                throw_error_fatal(INTERNAL_ERROR, "%s", "Memory allocation failed");
            }
            strcpy(nameArr, name);
            tmpItem->name = nameArr;
            //if its first item in list, save its pointer to arr
            if(tmpLastItem == NULL){
                t->ptr[htab_hash_fun(name) % t->arr_size] = tmpItem;
            }
            //save its pointer to last item->next pointer
            else{
                tmpLastItem->next = tmpItem;
            }
        }
        //setup iterator to return
        tmpIterator.ptr = tmpItem;
        tmpIterator.t = t;
        tmpIterator.idx = htab_hash_fun(name) % t->arr_size;
        //add size of list
        t->size += 1;
        return tmpIterator;
    }
    tmpIterator.ptr = NULL;
    tmpIterator.t = NULL;
    tmpIterator.idx = 0;
    return tmpIterator;
}

void htab_erase(htab_t * t, htab_iterator_t it){
    if(htab_iterator_valid(it) == false){
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to name");
        return;
    }
    if(t != it.t) {
        throw_error(INTERNAL_ERROR, "%s", "Iterator is from different table");
        return;
    }
    if(it.idx >= htab_bucket_count(t)){
        throw_error(INTERNAL_ERROR, "%s", "Iterator is not valid");
        return;
    }
    struct htab_item *tmp = t->ptr[it.idx];
    if(tmp == NULL) {
        throw_error(INTERNAL_ERROR, "%s", "Table is empty");
        return;
    }
    //if first pointer is our pointer
    if(tmp == it.ptr){
        //set first pointer to next list member
        t->ptr[it.idx] = it.ptr->next;
        //delete item
        free((char *)it.ptr->name);
        free(it.ptr);
        t->size -= 1;
        return;
    }
    //else, loop until end of list
    while(tmp->next != NULL) {
        //check if next list member is searched one
        if(tmp->next == it.ptr){
            //set tmp next, to deleted item next pointer
            tmp->next = it.ptr->next;
            //delete item
            free((char *)it.ptr->name);
            free(it.ptr);
            t->size -= 1;
            return;
        }
        tmp = tmp->next;
    }
    return;
}

htab_iterator_t htab_begin(const htab_t * t){
    htab_iterator_t tmp = {NULL, NULL, 0};
    //if t is null, return NULL iterator
    if(t == NULL){
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to table");
        return tmp;
    }
    //set iterator to end
    tmp = htab_end(t);
    for(size_t i = 0; i < t->arr_size; i++){
        //if we found array member, return it
        if(t->ptr[i] != NULL) {
            tmp.ptr = t->ptr[i];
            tmp.idx = i;
            tmp.t = t;
            return tmp;
        }
    }
    //if array is empty, return htab_end
    return tmp;
}

htab_iterator_t htab_end(const htab_t * t){
    //set iterator to NULLS
    htab_iterator_t tmp = {NULL, NULL, 0};
    if(t == NULL){
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to table");
        return tmp;
    }
    //return ending iterator
    tmp.ptr = NULL;
    tmp.t = t;
    tmp.idx = htab_bucket_count(t);
    return tmp;
}

htab_iterator_t htab_iterator_next(htab_iterator_t it){
    //check if iterator is valid
    if(htab_iterator_valid(it) == false) return htab_end(it.t);
    //if next list item is valid, return it
    if(it.ptr->next != NULL){
        it.ptr = it.ptr->next;
        return it;
    }
    else{
        //loop through array
        for(size_t i = (it.idx + 1); i < htab_bucket_count(it.t); i++){
            //if we found valid next item, return it
            if(it.t->ptr[i] != NULL){
                it.idx = i;
                it.ptr = it.t->ptr[i];
                return it;
            }
        }
    }
    //if we didnt find valid next item, return htab_end
    return htab_end(it.t);
}

void htab_clear(htab_t * t){
    if(t == NULL){
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to table");
        return;
    }
    //loop through whole arr
    for(size_t i = 0; i < htab_bucket_count(t); i++){
        struct htab_item *tmpIteratorItem = t->ptr[i];
        struct htab_item *tmpIteratorItemLast = t->ptr[i];;
        //loop through list, until whole list is cleared
        while(t->ptr[i] != NULL){
            //if next item is NULL, free actual item
            if(tmpIteratorItem->next == NULL){
                free((char * )tmpIteratorItem->name);
                free(tmpIteratorItem);
                //if its first item in list
                if(tmpIteratorItem == t->ptr[i]){
                    //null first pointer, break
                    t->ptr[i] = NULL;
                    break;
                }
                //set last item pointer to next to NULL
                tmpIteratorItemLast->next = NULL;
                //reset pointer and start from beginning
                tmpIteratorItem = t->ptr[i];
                tmpIteratorItemLast = t->ptr[i];
            }
            else {
                //if pointer to next is not null, go to next items
                tmpIteratorItemLast = tmpIteratorItem;
                tmpIteratorItem = tmpIteratorItem->next;
            }
        }
    }
    //set arr size to 0
    t->size = 0;
}

void htab_free(htab_t * t){
    if(t == NULL){
        throw_error(INTERNAL_ERROR, "%s", "Invalid pointer to table");
        return;
    }
    //first call clear
    htab_clear(t);
    //then free table
    free(t);
}