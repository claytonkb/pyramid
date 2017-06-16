// xbar.c
//

#include "pyramid.h"
#include "xbar.h"
#include "array.h"
#include "introspect.h"
#include "io.h"

// Initialization in interp.c
//   Idea: Set these up in the relevant X-macros, and add a few padding fields
//              just in case we decide to add another field later on
// tag       --> e.g. [hash "/babel/tag/list"]
// string    --> e.g. [val "/babel/tag/list"]
// verb_fn   --> (if applicable, such as for an operator-tag)
// blob_type --> [noun|verb]
// template  --> (if applicable, such as for a call_stack_frame)
// to_verb   --> if blob_type==noun, points to verbs that can operate on this noun (else nil)
// to_noun   --> if blob_type==verb, points to applicable nouns for this verb
// cacher    --> used only for cache-able nouns
// uncacher  -->  "          "              "
//
// check blind spots: threading, alternation, laziness

//    X(c_name                    , "/pyramid/tag/<name>"         ,    verb_fn      ,
//          blob_type             , template                      ,    to_verb      ,
//          to_noun               , cacher                        ,    unchacher    ,
//          reserved              , reserved                      ,    reserved     );

//    X(PYR_TAG_PAGED_ARRAY       , "/pyramid/tag/paged_array"    ,    UNINIT_FN_PTR,
//          UNINIT_VAL            , UNINIT_PTR                    ,    UNINIT_PTR   ,
//          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR,
//          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   );

//
//
void xbar_new(pyr_cache *this_pyr){

    _d(nil);

}


//
//
mword *xbar_search(pyr_cache *this_pyr, mword *tag){ // xbar_search#

    mword offset = array_search(this_pyr, global_irt->xbar, tag, LEX_MWORD);

    if(offset == -1){
        return nil;
    }

    return rdp(global_irt->xbar,offset);

}


//
//
mword *xbar_tag_to_string(pyr_cache *this_pyr, mword *tag){ // xbar_tag_to_string#

//    mword offset = array_search(this_pyr, global_irt->xbar, tag, LEX_MWORD);
//
//    if(offset == -1){
//        return nil;
//    }
//
//    return rdp( pcdr(rdp(global_irt->xbar, offset)), 1);

    return rdp( pcdr( xbar_search(this_pyr, tag) ), 1); // FIXME: Blows up if xbar search returns nil...

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

