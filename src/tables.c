// tables.c
//

#include "pyramid.h"
#include "tables.h"


//
//
void tables_new(pyr_cache *this_pyr){

    tables_new_verb_table(this_pyr);
    tables_new_noun_table(this_pyr);

}


//
//
void tables_new_verb_table(pyr_cache *this_pyr){

// Build verb_table:
//  verb_table = mem_new_ptr(this_pyr, num_verbs);
//  for each tag, fn_ptr:
//      cons(tag, fn_ptr)
//      add to verb_table
//  update global_irt->verb_table

}


//
//
void tables_new_noun_table(pyr_cache *this_pyr){
}


// Clayton Bauman 2017

