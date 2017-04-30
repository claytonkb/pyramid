// trie.c
//

#include "pyramid.h"
#include "trie.h"
#include "bstruct.h"
#include "tptr.h"
#include "pearson.h"
#include "string.h"
#include "list.h"
#include "array.h"
#include "introspect.h"
#include "mem.h"


mword *trie_new_cell(pyr_cache *this_pyr, mword *key, mword *secondary_key, mword *payload){  // trie_new_cell#

    return tptr_new(this_pyr, global_irt->tags->PYR_TAG_TRIE_ENTRY,
                    trie_new_entry(this_pyr, payload, secondary_key, key));

}


//
//
mword *trie_new(pyr_cache *this_pyr){ // trie_new#

#ifdef COMPAT_MODE
    return tptr_new(this_pyr, 
                    HASHC(this_pyr, "/babel/tag/trie"), 
                    _mkptr(this_pyr, 2, nil, nil));
#else
    return tptr_new(this_pyr, 
                    HASHC(this_pyr, "/pyramid/tag/trie"), 
                    _mkptr(this_pyr, 2, nil, nil));
#endif

}


//
//
void trie_insert(pyr_cache *this_pyr, mword *trie, mword *key, mword *secondary_key, mword *payload){ // trie_insert#

    if(is_tptr(trie)){ 
        trie = tcar(trie);  // XXX We don't check the tag
    }

    if( (is_nil(key) && is_nil(secondary_key))
            || !is_ptr(trie))
        return; // do nothing

    if(is_nil(key)){

        if(is_nil(secondary_key)){
            return; // do nothing
        }
        else{
            key = pearson_hash8(this_pyr, secondary_key);
        }

    }
    else{

        if(is_tptr(key)){ // FIXME: kludge due to single-tptr-copy bug in bstruct_unload()
            key = tptr_extract_hash(this_pyr, key);
        }
        else{
            key = bstruct_cp(this_pyr, key);
        }

    }

    if(!is_nil(secondary_key))
        secondary_key = bstruct_cp(this_pyr, secondary_key);

    rtrie_insert(this_pyr, trie, key, secondary_key, payload, 0);

}


//
//
void rtrie_insert(pyr_cache *this_pyr, mword *trie, mword *key, mword *secondary_key, mword *payload, mword level){ // rtrie_insert#

    mword cons_side   = array1_read(key, level);
    mword *next_level = rdp(trie, cons_side);

    // 1. cons_side = nil
    //      insert
    // 2. cons_side is inte AND size = 2
    //      recurse
    // 3. cons_side is inte AND size = HASH_ENTRY_SIZE
    //      split and insert

    if(is_ptr(next_level)){
        rtrie_insert(this_pyr, next_level, key, secondary_key, payload, level+1);
    }
    else if(is_nil(next_level)){
//trie_new_cell(this_pyr, key, secondary_key, payload)
//        ldp(trie,cons_side) = payload;
//        ldp(trie,cons_side) = trie_new_cell(this_pyr, key, secondary_key, payload);
        ldp(trie,cons_side) = trie_new_cell(this_pyr, payload, secondary_key, key);
    }
    else if(is_tptr(next_level)){ //XXX: We are ASSUMING it's a trie_entry...

        mword *entry     = tptr_detag(this_pyr, next_level);

        mword *entry_key = trie_entry_get_key(this_pyr, entry);

        if( tageq(entry_key, key, TAG_SIZE) ){ //already exists...  
            trie_entry_set_payload2(this_pyr, entry, payload); 
        }
        else{

            //mword *existing_key = trie_entry_get_key(this_pyr, entry);
            if( array1_read(entry_key, level+1) ){
                // note that ldp(trie, cons_side) is equivalent to next_level
                ldp(trie, cons_side) = _cons(this_pyr,  nil,         next_level  );
            }
            else{
                ldp(trie, cons_side) = _cons(this_pyr,  next_level,  nil         );
            }

            rtrie_insert(this_pyr, ldp(trie, cons_side), key, secondary_key, payload, level+1);

        }
    }
    else{ // is_val(next_level)
        return; // FIXME: Should throw an exception here due to mal-formed trie...
    }

}


//
//
mword *trie_lookup_binary(pyr_cache *this_pyr, mword *trie, mword *binary_key){ // trie_lookup_binary#

    return nil;

}


//
//
mword *trie_lookup_hash(pyr_cache *this_pyr, mword *trie, mword *hash_key, mword *string_key){ // trie_lookup_hash#

    if(is_tptr(trie)){ 
        trie = tcar(trie);  // XXX We don't check the tag
    }

    if(!is_ptr(trie))
        return nil; // return nothing

    if(is_nil(hash_key)){

        if(is_nil(string_key)){
            return nil;
        }
        else{
            hash_key = pearson_hash8(this_pyr, string_key);
        }

    }

    return rtrie_lookup(this_pyr, trie, hash_key, 0);

}


//
//
mword *rtrie_lookup(pyr_cache *this_pyr, mword *trie, mword *key, mword level){ // rtrie_lookup#

    mword cons_side   = array1_read(key, level);
    mword *next_level = rdp(trie, cons_side);

    if(is_ptr(next_level)){
        return rtrie_lookup(this_pyr, next_level, key, level+1);
    }
    if(is_nil(next_level)){ //dead-end
        return nil;
    }
    else if(is_tptr(next_level)){

        mword *trie_entry = tptr_detag(this_pyr, next_level); // XXX assumes well-formedness
        mword *entry_key = trie_entry_get_key(this_pyr, trie_entry);

        if(tageq(entry_key, key, TAG_SIZE)){ 
            return trie_entry; // found it!
        }
        else{
            return nil; //dead-end
        }

    }
    // else is_val(next_level)... FIXME should raise an exception

    return nil;

}


//
//
mword trie_remove(pyr_cache *this_pyr, mword *trie, mword *key, mword *secondary_key){ // trie_remove#

    if(is_tptr(trie)){ 
        trie = tcar(trie);  // XXX We don't check the tag
    }

    if(!is_ptr(trie))
        return 0; // nothing removed

    if(is_nil(key)){

        if(is_nil(secondary_key)){
            return 0;
        }
        else{
            key = pearson_hash8(this_pyr, secondary_key); 
        }

    }

    return rtrie_remove(this_pyr, trie, key, 0);

}


//
//
mword rtrie_remove(pyr_cache *this_pyr, mword *trie, mword *key, mword level){ // rtrie_remove#

    mword cons_side   = array1_read(key, level);
    mword *next_level = rdp(trie, cons_side);

    if(is_ptr(next_level)){
        if(rtrie_remove(this_pyr, rdp(trie,cons_side), key, level+1)){

            if(    is_nil( pcar(next_level) ) 
                && is_nil( pcdr(next_level) )){
                ldp(trie,cons_side) = nil; 
            }

            return 1;
        }
        return 0;
    }
    else if(is_nil(next_level)){ //dead-end
        return 0;
    }
    else if(is_tptr(next_level)){ // XXX assumes well-formed key-entry

        if(tageq(pcar(tcar(next_level)),key,TAG_SIZE)){ //match
            ldp(trie,cons_side) = nil;
            return 1;
        }
        else{
            return 0;
        }

    }

    return 0; // silence compiler warning

}


//
//
mword *trie_entries(pyr_cache *this_pyr, mword *trie){ // trie_entries#

    if(is_tptr(trie)){ 
        trie = tcar(trie);
    }

    if(is_nil(trie)){
        return nil;
    }
    else{
        return rtrie_entries(this_pyr, trie, 0);
    }

}


//
//
mword *rtrie_entries(pyr_cache *this_pyr, mword *trie, mword level){ // rtrie_entries#

    //FIXME: Check for level > HASH_BIT_SIZE

    if(is_nil(trie)){ //dead-end
        return nil;
    }
    else if(is_conslike(trie)){

        mword *list0 = rtrie_entries(this_pyr, pcar(trie), level+1);
        mword *list1 = rtrie_entries(this_pyr, pcdr(trie), level+1);

        if(is_nil(list0)){
            if(is_nil(list1)){
                return nil;
            }
            else{
                return list1;
            }
        }
        else{
            if(is_nil(list1)){
                return list0;
            }
            else{
                return list_append_direct(this_pyr, list0, list1);
            }
        }

    }
    else if(is_tptr(trie)){
        return _cons(this_pyr, tptr_hard_detag(this_pyr, trie), nil);
    }
    else{
        _fatal("unexpected element in hash-table"); //FIXME: except, not fatal
    }

}





// Clayton Bauman 2017

