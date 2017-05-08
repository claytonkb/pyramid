// xbar.c
//

#include "pyramid.h"
#include "xbar.h"
#include "array.h"


//
//
void xbar_new(pyr_cache *this_pyr){

    _d(nil);

}


//
//
mword *xbar_tag_to_string(pyr_cache *this_pyr, mword *tag){ // xbar_tag_to_string#

    mword offset = array_search(this_pyr, global_irt->tags_strings, tag, LEX_MWORD);
//    mword offset = array_search_rewrite(this_pyr, global_irt->tags_strings, tag, LEX_MWORD);

    if(offset == -1){
        return nil;
    }
    
    return pcdr( rdp(global_irt->tags_strings, offset) );

}


//
//
mword *xbar_tag_to_fn(pyr_cache *this_pyr, mword *tag){ // xbar_tag_to_fn#

    mword offset = array_search(this_pyr, global_irt->tags_fns, tag, LEX_MWORD);
//    mword offset = array_search_rewrite(this_pyr, global_irt->tags_strings, tag, LEX_MWORD);

    if(offset == -1){
        return nil;
    }

    return pcdr( rdp(global_irt->tags_fns, offset) );

}


// Clayton Bauman 2017

