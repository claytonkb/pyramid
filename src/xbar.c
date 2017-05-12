// xbar.c
//

#include "pyramid.h"
#include "xbar.h"
#include "array.h"
#include "introspect.h"
#include "io.h"


//
//
void xbar_new(pyr_cache *this_pyr){

    _d(nil);

}


//
//
mword *xbar_tag_to_string(pyr_cache *this_pyr, mword *tag){ // xbar_tag_to_string#

    mword offset = array_search(this_pyr, global_irt->xbar, tag, LEX_MWORD);
//    mword offset = array_search(this_pyr, global_irt->tags_strings, tag, LEX_MWORD);

    if(offset == -1){
        return nil;
    }

    return rdp( pcdr(rdp(global_irt->xbar, offset)), 1);
    //    return pcdr( rdp(global_irt->tags_strings, offset) );

}


//
//
mword *xbar_tag_to_fn(pyr_cache *this_pyr, mword *tag){ // xbar_tag_to_fn#

    mword offset = array_search(this_pyr, global_irt->xbar, tag, LEX_MWORD);
//    mword offset = array_search(this_pyr, global_irt->tags_fns, tag, LEX_MWORD);

    if(offset == -1){
        return nil;
    }

    return rdp( pcdr(rdp(global_irt->xbar, offset)), 0);
//    return pcdr( rdp(global_irt->tags_fns, offset) );

}


// Clayton Bauman 2017

