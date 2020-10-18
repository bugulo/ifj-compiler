// IFJ20
// Autors:
// Mario Harvan, xharva03
// Juraj Marticek, xmarti97
// Michal Slesar, xslesa01
// Erik Belko, xbelko02

#include "symtable.h"
#include <stdio.h>

//tento modul bol definovany v zadani, str. 20


/**
 * @brief Príklad dokumentácie funkcie
 * @param holder Prvé číslo operácie
 * @return Funkcia vracia prazdno
*/
int placeHolder(int holder){
    holder = 320;
    //debugovacie spravy:
    #ifdef DEBUG
        fprintf(stderr, "Testovaci debug vypis %d\n", holder);
        //debugovacie spravy vypisujte na stderr, kedze vystup prekladaca pojde na stdout
    #endif

    return holder;
}