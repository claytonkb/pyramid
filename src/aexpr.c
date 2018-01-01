// aexpr.c
//

#include "pyramid.h"
#include "aexpr.h"
#include "list.h"
#include "mem.h"
#include "xbar.h"
#include "array.h"
#include "io.h"
#include "introspect.h"
#include "util.h"
#include "string.h"
#include "pearson.h"
#include "bstruct.h"

// XXX WIP [convert sexpr to eval-ready aexpr's] XXX //

//
//
aexpr aexpr_from_sexpr(pyr_cache *this_pyr, sexpr s){ // aexpr_from_sexpr#

    bstruct xbar_entry;
    blob aexpr;
    blob aexpr_body;

    mword sexpr_length = list_len(this_pyr, s);

    if(sexpr_length==0) return nil;

    bstr label = array_to_string(this_pyr, list_ith(this_pyr, s, 0));

    ls l = pcdr(s);
    aexpr_body = mem_new_ptr(this_pyr, (sexpr_length-1));

    if(array8_eq(this_pyr, label, C2B("val"))){ // label == "val"
        return list_to_val_array(this_pyr, l);
    }
    else if(array8_eq(this_pyr, label, C2B("ptr"))){ // label == "ptr"
        aexpr = aexpr_body;
    }
    else{

        xbar_entry = xbar_search_string(this_pyr,
                                array8_cat(this_pyr,
                                    C2B("/pyramid/sym/"),
                                    label));

        if(is_nil(xbar_entry)){ // User-defined tag
            blob temp = mem_new_ptr(this_pyr, 3);
            ldp(temp,0) = pearson_hash8(this_pyr, label);
            ldp(temp,1) = bstruct_cp(this_pyr, label);
            ldp(temp,2) = aexpr_body;
            aexpr = mem_new_tptr(this_pyr, global_irt->tags->PYR_SYM_USER, temp);
        }
        else{
            aexpr = mem_new_tptr(this_pyr, rdp(xbar_entry,0), aexpr_body);
        }

    }

    int i=0;

    while(!is_nil(l)){
        if(is_ptr(pcar(l))){
            ldp(aexpr_body,i) = aexpr_from_sexpr(this_pyr, pcar(l));
        }
        else{
            ldp(aexpr_body,i) = pcar(l);
        }
        l = pcdr(l);
        i++;
    }

    return aexpr;

}


//
//
aexpr aexpr_from_sexpr_r(pyr_cache *this_pyr, sexpr s){ // aexpr_from_sexpr_r#

    // lookup first element of list in xbar to translate from string to hash
    //      use hash to create tptr
    // get remaining length of list
    // allocate mem_new_ptr() of given size
    // recurse for each entry of mem_new_ptr()

    return nil;

}


// Clayton Bauman 2017

