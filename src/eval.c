// eval.c
//

#include "pyramid.h"
#include "eval.h"
#include "array.h"
#include "tptr.h"


//
//
mword *frobnicate(void){
    return nil;
}


//
//
blob eval_apply(pyr_cache *this_pyr, blob b){

    if(!is_tptr(b))
        return (blob)global_irt->tags->PYR_TAG_MALFORMED_EXPR;

    // FIXME: Update the call-stack HERE (exceptions, restart, etc.)

    mword xbar_index = array_search(this_pyr, global_irt->xbar, b, LEX_MWORD);

    if(xbar_index == ARRAY_SEARCH_NOT_FOUND){ // not a built-in

        if(0){ // exists_in_definitions_table
            // is it a user variable?
            // is it a user function?
            // is it a user macro?
            return (blob)nil;
        }
        else{
            return (blob)global_irt->tags->PYR_TAG_UNKNOWN_SYM; // We don't what you're talking about...
        }

    }
    else{ // it's a built-in

        mword *xbar_entry = rdp(global_irt->xbar,xbar_index);
        mword *a;
        mword num_args;
        int i;

        if(is_verb(xbar_entry)){ // function/operator

            a = tptr_detag(this_pyr,b);
            num_args = size(a);

            for(i=num_args-1; i>=0; i--){
                eval_apply(this_pyr, rdp(b,i));
            }

            // call function in xbar_entry and pass a to it...

            return (blob)nil;

        }
        else if(is_special_form(xbar_entry)){
            // (list ...) etc.

            a = tptr_detag(this_pyr,b);

            // call special-form handler in xbar_entry and pass a to it...

            return (blob)nil;

        }
        else{

            // We recognize the tag but we don't know what to do with it
            return (blob)global_irt->tags->PYR_TAG_KNOWN_SYM;

        }


    }

    return (blob)global_irt->tags->PYR_TAG_PIGS_FLY;

}


// Clayton Bauman 2017

