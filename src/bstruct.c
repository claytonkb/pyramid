// bstruct.c
//

#include "pyramid.h"
#include "bstruct.h"
#include "mem.h"
#include "array.h"
#include "tptr.h"


// recursively cleans a bstruct after traversal
//
void _rclean(pyr_cache *this_pyr, mword *bs){ // _rclean#

    int i;

    if( !(sfield(bs) & CTL_MASK) ){ //Already cleaned
        return;
    }

    sfield(bs) = sfield(bs) & ~CTL_MASK; //Mark clean

    if( is_ptr(bs) ){
        int num_elem = size(bs);
        for(i=0; i<num_elem; i++){
            _rclean(this_pyr, (mword *)*(bs+i));
        }
    }
    else if(is_tptr(bs)){
        _rclean(this_pyr, bs+HASH_SIZE+1);
    }

}


//
//
void _recurse(pyr_cache *this_pyr, mword *bs, bstruct_op_fn_ptr bfn, void *v){ // _recurse#

    _fn_recurse(this_pyr, bs, bfn, v);
    _rclean(this_pyr, bs);

}


//
//
mword _fn_recurse(pyr_cache *this_pyr, mword *bs, bstruct_op_fn_ptr bfn, void *v){ // _fn_recurse#

    int i;

    if( is_traversed_U(bs) ){
        return 1;
    }

    if( !bfn(this_pyr, bs,v) ){
        return 0;
    }

    if( is_ptr(bs) ){
        int num_elem = size(bs);
        mark_traversed_U(bs);
        for(i=0; i<num_elem; i++){
            if(!_fn_recurse(this_pyr, (mword *)*(bs+i),bfn,v)){
                return 0;
            }
        }
        return 1;
    }
    else if( is_tptr(bs) ){
        mword *tptr_ptr = (bs+TPTR_PTR_OFFSET);

        mark_traversed_U(bs);

        mark_traversed_U(tptr_ptr);

        _fn_recurse(this_pyr, (mword*)*tptr_ptr, bfn, v);
    }
    else{
        mark_traversed_U(bs);
    }
    return 1;

}


// _mu -> memory usage (mnemonic: *nix du)
// _mu(this_pyr, x) = 
//      + _nlf(this_pyr, x) 
//      + _nva(this_pyr, x)
//      _nin(this_pyr, x) 
//      + _nptr(this_pyr, x) 
//      + _ntag(this_pyr, x)*(HASH_SIZE+1)
//      
//  Don't forget that nil will add a "silent" HASH_SIZE+1 to 
//  your bstruct if anything in your bstruct points to it...
mword _mu(pyr_cache *this_pyr, mword *bs){ // _mu#

    mword counter=0;
    _recurse(this_pyr, bs, _rmu, &counter);
    return counter;

}


//
//
mword _rmu(pyr_cache *this_pyr, mword *bs, void *v){ // _rmu#

    if( is_tptr(bs) ){
        *(mword*)v += HASH_SIZE+3;
    }
    else if( is_val(bs) ){
        *(mword*)v += size(bs)+1;
    }
    else{
        *(mword*)v += size(bs)+1;
    }

    return 1;

}


// _nar -> number of leaf-arrays
//
mword _nar(pyr_cache *this_pyr, mword *bs){ // _nar#

    mword counter=0;
    _recurse(this_pyr, bs, _rnar, &counter);
    return counter;

}


//
//
mword _rnar(pyr_cache *this_pyr, mword *bs, void *v){ // _rnar#

    *(mword*)v += 1;

    return 1;

}


// _nlf -> number of leaf-arrays
//
mword _nlf(pyr_cache *this_pyr, mword *bs){ // _nlf#

    mword counter=0;
    _recurse(this_pyr, bs, _rnlf, &counter);
    return counter;

}


//
//
mword _rnlf(pyr_cache *this_pyr, mword *bs, void *v){ // _rnlf#

    if( is_val(bs) ){
        *(mword*)v += 1;
    }

    return 1;

}


// _nin -> number of interior-arrays
//
mword _nin(pyr_cache *this_pyr, mword *bs){ // _nin#

    mword counter=0;
    _recurse(this_pyr, bs, _rnin, &counter);
    return counter;

}


//
//
mword _rnin(pyr_cache *this_pyr, mword *bs, void *v){ // _rnin#

    if( is_ptr(bs) ){
        *(mword*)v += 1;
    }

    return 1;

}


// _ntag -> number of tagged-pointers
//
mword _ntag(pyr_cache *this_pyr, mword *bs){ // _ntag#

    mword counter=0;
    _recurse(this_pyr, bs, _rntag, &counter);
    return counter;

}


//
//
mword _rntag(pyr_cache *this_pyr, mword *bs, void *v){ // _rntag#

    if( is_tptr(bs) ){
        *(mword*)v += 1;
    }

    return 1;

}


// _nva -> number of values
//
mword _nva(pyr_cache *this_pyr, mword *bs){ // _nva#

    mword counter=0;
    _recurse(this_pyr, bs, _rnva, &counter);
    return counter;

}


//
//
mword _rnva(pyr_cache *this_pyr, mword *bs, void *v){ // _rnva#

    if( is_val(bs) ){
        *(mword*)v += size(bs);
    }

    return 1;

}


// _nptr -> number of pointers
//
mword _nptr(pyr_cache *this_pyr, mword *bs){ // _nptr#

    mword counter=0;
    _recurse(this_pyr, bs, _rnptr, &counter);
    return counter;

}


//
//
mword _rnptr(pyr_cache *this_pyr, mword *bs, void *v){ // _rnptr#

    if( is_ptr(bs) ){
        *(mword*)v += size(bs);
    }

    return 1;

}


//
//
mword *bstruct_cp(pyr_cache *this_pyr, mword *bs){

    mword *temp = bstruct_unload(this_pyr, bs);

    bs = bstruct_load(this_pyr, temp, size(temp));

    return bs;

}


//
//
mword *bstruct_load(pyr_cache *this_pyr, mword *bs, mword size){ // bstruct_load#

    mword *curr_ptr = bs+1;
    mword *base_ptr = curr_ptr;

    mword curr_size;
    int i;

    mword *bs_last_mword = bs+size-1;

    while(curr_ptr < bs_last_mword){

        if(is_ptr(curr_ptr)){

            curr_size = size(curr_ptr);

            for(i=0;i<curr_size;i++){
                ldv(curr_ptr,i) = rdv(curr_ptr,i) + (mword)bs;
            }

        }
        else if(is_tptr(curr_ptr)){

            ldv(curr_ptr,TPTR_PTR_OFFSET) = rdv(curr_ptr,TPTR_PTR_OFFSET) + (mword)bs;

        }
        // else is_val, do nothing

        curr_ptr += alloc_size(curr_ptr);

    }

    return base_ptr;

}


//
mword *bstruct_unload(pyr_cache *this_pyr, mword *bs){

    mword *dest;

    if(is_val(bs)){
        dest = mem_new_val(this_pyr, alloc_size(bs), 0);
        memcpy(dest,(bs-1),(size_t)UNITS_MTO8(size(dest)));
        return dest;
    }

    dest         = mem_new_val(this_pyr, _mu(this_pyr, bs), 0);

    mword *span_array = bstruct_to_array(this_pyr, bs);
    array_sort(this_pyr, span_array, VAL);

    mword *offset_array = mem_new_val(this_pyr, size(span_array), 0xff);

    mword dest_offset = 0;

    bstruct_unload_r(this_pyr, bs, dest, &dest_offset, span_array, offset_array);
    _rclean(this_pyr, bs);

    return dest;

}


//
//
mword bstruct_unload_r(
        pyr_cache   *this_pyr, 
        mword       *bs, 
        mword       *dest, 
        mword       *dest_offset, 
        mword       *span_array, 
        mword       *offset_array){ // bstruct_unload_r#

    int i;

    if( is_traversed_U(bs) ){
//        return offset_array[get_offset_from_ptr(this_pyr, span_array, bs)];
        return offset_array[array_search(this_pyr, span_array, (mword*)(&bs), VAL)];
    }

    int num_elem = size(bs);

    *(dest+(*dest_offset))   = sfield(bs);
    *dest_offset             = *dest_offset+1;

    mword local_offset = *dest_offset;
    mword this_offset  = (*dest_offset)*MWORD_SIZE;

    set_offset_for_ptr(this_pyr, span_array, bs, offset_array, this_offset);

    if(is_tptr(bs)){ // is_tptr

        mark_traversed_U(bs);

        for(i=0; i<=HASH_SIZE; i++){
            ldv(dest,(*dest_offset)) = rdv(bs,i);
            *dest_offset = *dest_offset+1;
        }

        mword *tptr_ptr = (bs+TPTR_PTR_OFFSET);

        mark_traversed_U(tptr_ptr); //FIXME: Does this actually need to be here??

        mword tptr_offset = *dest_offset;

        *dest_offset = *dest_offset + 1;

        ldv(dest,tptr_offset)
            = bstruct_unload_r(this_pyr, (mword*)*tptr_ptr, dest, dest_offset, span_array, offset_array);

    }
    else if(is_ptr(bs)){

        mark_traversed_U(bs);

        *dest_offset = *dest_offset + num_elem;

        for(i=0; i<num_elem; i++){
            ldv(dest,local_offset+i) 
                = bstruct_unload_r(this_pyr, rdp(bs,i), dest, dest_offset, span_array, offset_array);
        }

    }
    else{// if(is_val(bs))

        mark_traversed_U(bs);

//_d(sfield(bs));
//_bounds_check(this_pyr, bs);

        for(i=0; i<num_elem; i++){
            ldv(dest,(*dest_offset)) = rdv(bs,i);
            *dest_offset = *dest_offset+1;
        }

    }

    return this_offset;

}


////
////
//mword get_offset_from_ptr(pyr_cache *this_pyr, mword *span_array, mword *ptr){ // get_offset_from_ptr#
//
//    return array_search(this_pyr, span_array, _val(this_pyr, (mword)ptr), VAL);
//
////    return array_search_direct(this_pyr, span_array, (mword)ptr);
//
////    int span_array_size = size(span_array);
////    int i;
////
////    if(span_array_size < 4){ // linear search
////        for(i=0; i<span_array_size; i++){
////            if(rdp(span_array,i) == ptr){
////                return i;
////            }
////        }
////        return -1;
////    }
////
////    int shift       = span_array_size >> 1;
////    int guess_index = shift;
////
////    shift >>= 1;
////
////    mword last_guess_index = -1;
////
////    while(1){ // binary search sweetness...
////        if(guess_index == last_guess_index
////                || guess_index < 0
////                || guess_index >= span_array_size){
////            _fatal("ptr not found in span_array");
////        }
////        mword *guess = rdp(span_array,guess_index);
////        last_guess_index = guess_index;
////        if(guess < ptr){
////            guess_index += shift;
////        }
////        else if(guess > ptr){
////            guess_index -= shift;
////        }
////        else if(guess == ptr){
////            return guess_index;
////        }
////        shift >>= 1;
////        shift = (shift == 0) ? 1 : shift;
////    }
////
////    _die; // FIXME(throw_exception)
////    return -1; //error
////
//}


//
void set_offset_for_ptr(
        pyr_cache *this_pyr, 
        mword     *span_array,      mword *ptr, 
        mword     *offset_array,    mword this_offset){ // set_offset_for_ptr#

//    mword span_offset = get_offset_from_ptr(this_pyr, span_array, ptr);
    mword span_offset = array_search(this_pyr, span_array, (mword*)(&ptr), VAL);

    offset_array[span_offset] = this_offset;

}


//Creates an interior array with one pointer to each array in a bstruct
//
mword *bstruct_to_array(pyr_cache *this_pyr, mword *bs){  // bstruct_to_array#

//    mword num_arrays  = _nin(this_pyr, bs);
//          num_arrays += _nlf(this_pyr, bs);
//          num_arrays += _ntag(this_pyr, bs);

    mword num_arrays = _nar(this_pyr, bs);

    mword *arr_list = mem_new_ptr(this_pyr, num_arrays);
    mword offset = 0;

    bstruct_to_array_r(this_pyr, bs, arr_list, &offset);
    _rclean(this_pyr, bs);

    return arr_list;

}


//
//
void bstruct_to_array_r(pyr_cache *this_pyr, mword *bs, mword *arr_list, mword *offset){ // bstruct_to_array_r#

    int i;

    if( is_traversed_U(bs) ){ //Already dumped
        return;
    }

    int num_elem = size(bs);

    ldp(arr_list,*offset) = bs;
    *offset = *offset+1;

    if(is_tptr(bs)){

        mark_traversed_U(bs);

        mword *tptr_ptr = (bs+TPTR_PTR_OFFSET);

        mark_traversed_U(tptr_ptr);

//        bstruct_to_array_r(this_pyr, (mword*)*tptr_ptr, arr_list, offset);
        bstruct_to_array_r(this_pyr, tcar(bs), arr_list, offset);

    }
    else if(is_ptr(bs)){

        mark_traversed_U(bs);

        for(i=0; i<num_elem; i++){
            bstruct_to_array_r(this_pyr, (mword*)rdp(bs,i), arr_list, offset);
        }

    }
    //else(is_val(bs))

    mark_traversed_U(bs);

}


//
//
mword *bstruct_find(pyr_cache *this_pyr, mword *target_bs, mword *find_bs){ // bstruct_find#

    mword *result;

    if(is_tptr(find_bs)){
        result = tptr_find_tag_unsafe(this_pyr, target_bs, find_bs);
    }
    else if(is_val(find_bs)){
        result = array_find_val(this_pyr, target_bs, find_bs);
    }
    else{ // is_ptr(find_bs)
        result = array_find_ptr(this_pyr, target_bs, find_bs);
    }

    return result;

}


// Clayton Bauman 2017

