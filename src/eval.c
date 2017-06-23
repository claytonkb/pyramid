// eval.c
//

#include "pyramid.h"
#include "eval.h"
#include "array.h"
#include "tptr.h"
#include "introspect.h"
#include "io.h"
#include "mem.h"
#include "list.h"
#include "util.h"


//
//
blob eval_apply(pyr_cache *this_pyr, blob b){ // eval_apply#

//_mem(b);
//_say("=================");

    if(!is_tptr(b))
        return b;

    // XXX(PERF): This can be done faster with a giant switch-stmt using short-hashes
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
        mword *a = tptr_detag(this_pyr,b);
        mword num_args;
        mword *evald_args;
        int i;
        pyr_op p;

        if(is_verb(xbar_entry)){ // function/operator

            num_args = size(a);

            evald_args = mem_new_ptr(this_pyr, num_args);

            p = (pyr_op)(vcar(rdp(xbar_entry,XBAR_VERB_FN_OFF)));
//_d(list_append_pyr_op);
//_d(p);
//_die;
            // stash p and evald_args in rstack before recursing

            for(i=num_args-1; i>=0; i--){
                ldp(evald_args,i) = eval_apply(this_pyr, rdp(a,i));
            }

            // call function in xbar_entry and pass a to it...
            return p(this_pyr, evald_args);

        }
        else if(is_special_form(xbar_entry)){ // (list ...) etc.

            // call special-form handler in xbar_entry and pass a to it...
            blob temp;
            temp = eval_spec_form_handler(this_pyr, xbar_entry, b);
            return temp;

        }
        else{

            // We recognize the tag but we don't know what to do with it
            return (blob)global_irt->tags->PYR_TAG_KNOWN_SYM;

        }

    }

    return (blob)global_irt->tags->PYR_TAG_PIGS_FLY;

}


//
//
blob eval_spec_form_handler(pyr_cache *this_pyr, mword *xbar_entry, blob b){

    mword spec_form_select = vcar(rdp(xbar_entry,XBAR_HASH_OFF)); // NOTE: This is a short-hash

    switch(spec_form_select){
        case EVAL_SF_IGNORE:
            _fatal("ignore");
            break;
        case EVAL_SF_QUOTE:
            _fatal("quote");
            break;
        case EVAL_SF_SEXPR:
            _fatal("sexpr");
            break;
        case EVAL_SF_BYTES:
            _fatal("bytes");
            break;
        case EVAL_SF_LIST:
            return (blob)array_to_list(this_pyr, eval_apply(this_pyr, tcar(b)));
        case EVAL_SF_CODE:
            _fatal("code");
            break;
        case EVAL_SF_HASH:
            _fatal("hash");
            break;
        case EVAL_SF_OPER:
            _fatal("oper");
            break;
        case EVAL_SF_TPTR:
            _fatal("tptr");
            break;
        case EVAL_SF_VAL:
            _fatal("val");
            break;
        case EVAL_SF_PTR:
            _fatal("ptr");
            break;
        case EVAL_SF_TAG:
            _fatal("tag");
            break;
        case EVAL_SF_REF:
            _fatal("ref");
            break;
        case EVAL_SF_NIL:
            _fatal("nil");
            break;
        case EVAL_SF_SYM:
            _fatal("sym");
            break;
        case EVAL_SF_BS:
            _fatal("bs");
            break;
        case EVAL_SF_QW:
            _fatal("qw");
            break;
        case EVAL_SF_SQUOTE:
            _fatal("squote");
            break;
        case EVAL_SF_DQUOTE:
            _fatal("dquote");
            break;
    }

    return nil;

}



// Clayton Bauman 2017

