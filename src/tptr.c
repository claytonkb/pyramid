// tptr.c
//

#include "pyramid.h"
#include "tptr.h"
#include "mem.h"
#include "list.h"
#include "bstruct.h"

#define MAX_DETAG_DEPTH 1024


//
//
mword *tptr_new(pyr_cache *this_pyr, const mword *hash, mword *bs){ // tptr_new#

    mword *ptr = mem_alloc( this_pyr, TPTR_SFIELD );

    int i;
    for(i=0; i<HASH_SIZE; i++){ // FIXME: PERF... use memcpy
        ptr[i] = hash[i];
    }

    ldv(ptr,HASH_SIZE) = NEG_ONE*MWORD_SIZE;

    tptr_set_ptr(ptr,bs);

    return ptr;

}


// Safely, recursively de-references a tag
mword *tptr_detag(pyr_cache *this_pyr, mword *tptr){ // tptr_detag#

    static int livelock_detect=0;

    if(is_nil(tptr)){
        return nil;
    }

    if(is_tptr(tptr)){
        if(livelock_detect++ > MAX_DETAG_DEPTH){
            //cat_except(this_pyr);
            _fatal("FIXME: this should have been a cat_except...");
        }
        return tptr_detag(this_pyr, tcar(tptr));
    }
    else{
        livelock_detect=0;
        return tptr;
    }

}


//
//
mword *tptr_extract_hash(pyr_cache *this_pyr, mword *tptr){ // tptr_extract_hash#

    mword *ptr = mem_new_val(this_pyr, HASH_SIZE, 0);
    int i;

    for(i=0; i<HASH_SIZE; i++){// FIXME: PERF... use memcpy
        ptr[i] = rdv(tptr,i);
    }

    return ptr;

}


//
//
void tptr_update_hash(pyr_cache *this_pyr, mword *tptr, mword *hash){ // tptr_update_hash#

    int i;

    for(i=0; i<HASH_SIZE; i++){// FIXME: PERF... use memcpy
        tptr[i] = rdv(hash,i);
    }

}


//
//
mword *tptr_extract_ptr(pyr_cache *this_pyr, mword *tptr){ // tptr_extract_ptr#

    mword* temp = (mword*)(tptr+TPTR_PTR_OFFSET);

    if(is_tptr(temp)){
        return tptr_extract_ptr(this_pyr, temp);
    }
    else{
        return temp;
    }

}


// Hard de-references a tag (recursive)
//
mword *tptr_hard_detag(pyr_cache *this_pyr, mword *tptr){ // tptr_hard_detag#

    mword *temp = tcar(tptr);

    if(is_tptr(temp)){
        return tptr_hard_detag(this_pyr, temp);
    }
    else{
        return temp;
    }

}


// XXX The return-value from this function contains unsafe pointers!!! XXX
// XXX internal interp use ONLY                                        XXX
// XXX If you pass tag=nil, returns ALL tags in bs                     XXX
// XXX PERF: A _tags2ar (like bstruct_to_array) would be more efficient          XXX
//
mword *tptr_find_tag_unsafe(pyr_cache *this_pyr, mword *bs, mword *tag){ // tptr_find_tag_unsafe#

    mword *span_array     = bstruct_to_array(this_pyr, bs);
    mword size_span_array = size(span_array);
    mword size_inte;
    mword *tag_list       = nil;
    mword *curr_span_elem;
    mword *curr_inte_elem;


//_dump(span_array);

    int i,j;
    for(i=0; i<size_span_array; i++){

        curr_span_elem = rdp(span_array,i);

        if(is_ptr(curr_span_elem)){ // check each element

            size_inte = size(curr_span_elem);

            for(j=0;j<size_inte;j++){

                curr_inte_elem = rdp(curr_span_elem,j);

                if(is_nil(curr_inte_elem)){
                    continue;
                }

                if(is_tptr(curr_inte_elem)){

                    if(is_nil(tag)){

                        // push onto tag_list
                        if(is_nil(tag_list)){
                            tag_list = _cons(this_pyr, (curr_span_elem+j), nil);
                        }
                        else{
                            list_unshift(this_pyr, tag_list, (curr_span_elem+j));
                        }

                    }
                    else{

                        if( tageq(curr_inte_elem, tag, TAG_SIZE) ){

                            // push onto tag_list
                            if(is_nil(tag_list)){
                                tag_list = _cons(this_pyr, (curr_span_elem+j), nil);
                            }
                            else{
                                list_unshift(this_pyr, tag_list, (curr_span_elem+j));
                            }

                        }

                    }


                }

            }

        }

    }

    return tag_list;

}


//
//
mword *tptr_find_tag(pyr_cache *this_pyr, mword *search_bs, mword *tag){  // tptr_find_tag#

    if(is_nil(search_bs) || is_nil(tag)){
        return nil;
    }

    mword *search_tag;

    if(is_tptr(tag)){
        search_tag = tag;
    }
    else{
        search_tag = tptr_new(this_pyr, tag, nil);
    }

    mword *sub_list = bstruct_find(this_pyr, search_bs, search_tag);
    mword *result = nil;

    while(!is_nil(sub_list)){
        result = _cons(this_pyr, rdp(rdp(sub_list,0),0), result);
        sub_list = pcdr(sub_list);
    }

    return result;

}


// Clayton Bauman 2017

