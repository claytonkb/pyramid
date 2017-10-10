// array.c
//

#include "pyramid.h"
#include "array.h"
#include "mem.h"
#include "utf8.h"
#include "bstruct.h"
#include "list.h"
#include "util.h"

//#include "operator.h"
//#include "pbp.h"
//#include "introspect.h"
//#include "string.h"
//#include "lib_babel.h"
//#include "logic.h"

//#define  key_aopv(x,y) vcar(pcar(rdp(x,y)))
//#define data_aopv(x,y) vcar(pcar(rdp(x,y)))
//
//#define  key_aop(x,y) pcar(rdp(x,y))
//#define data_aop(x,y) pcar(rdp(x,y))
//

/*****************************************************************************
 *                                                                           *
 *                           ARRAY PRIMITIVES                                *
 *                                                                           *
 ****************************************************************************/


// XXX TESTED XXX
//
mword *array_th(pyr_cache *this_pyr, mword *bs, mword entry){ // array_th#

    if(is_val(bs)){
        return _val(this_pyr, rdv(bs, entry%size(bs)));
    }
    else if(is_tptr(bs)){ // ignores entry
        return tcar(bs);
    } 
    //else is_ptr(bs)

    return rdp(bs, entry%size(bs));

}


/*****************************************************************************
 *                                                                           *
 *                          ARRAY-8 PRIMITIVES                               *
 *                                                                           *
 ****************************************************************************/


#define array8_mask_generate(off,arr,mask,sel)      \
    mword sel         = (off / MWORD_SIZE);         \
    mword byte_offset = (off % MWORD_SIZE);         \
                                                    \
    if(sel > size(arr)-1){                          \
        _fatal("error");                            \
    }                                               \
                                                    \
    mword mask = (0xff<<UNITS_8TO1(byte_offset));


// XXX TESTED XXX
//
mword array8_read(mword *array, mword offset){ // array8_read#

    array8_mask_generate(offset, array, read_mask, mword_select);
    return ((rdv(array,mword_select) & read_mask) >> UNITS_8TO1(offset));

}


// XXX TESTED XXX
//
void array8_write(mword *array, mword offset, mword value){ // array8_write#

    array8_mask_generate(offset, array, write_mask, mword_select);
    ldv(array,mword_select) = (rdv(array,mword_select) & ~write_mask) | ((value & MASK_1_BYTE) << UNITS_8TO1(byte_offset));

}


// Returns a val containing the byte at val_entry[entry] (byte-wise addressing)
//
mword *array8_th(pyr_cache *this_pyr, mword *val_array, mword entry8){ // array8_th#

    mword *byte = _newstr(this_pyr, 1, 0);
    ldv(byte,0) = array8_read(val_array, entry8);

    return byte;

}


//
//
mword array8_size(pyr_cache *this_pyr, mword *string){ // array8_size#

    mword strsize = size(string) - 1;
    mword last_mword = rdv(string, strsize);

    if(last_mword){
        return (strsize * MWORD_SIZE) - (MWORD_SIZE - array8_dec_align(this_pyr, last_mword));
    }
    else{
        return (strsize * MWORD_SIZE);
    }

}

// FIXME The following functions do not need pyr_cache* !!! FIXME

// decodes the alignment word
//
mword array8_dec_align(pyr_cache *this_pyr, mword alignment_word){ // array8_dec_align#

    if(alignment_word == 0){
        return 0;
    }

    alignment_word = ~alignment_word;
    mword alignment = 0;

    while(alignment_word != 0){ //FIXME: PERF ... really inefficient
        alignment_word = alignment_word >> BITS_PER_BYTE;
        alignment++;
    }

    return alignment;

}


//Returns an alignment word based on size8
//
mword array8_enc_align(pyr_cache *this_pyr, mword size8){ // array8_enc_align#

    if(size8 % MWORD_SIZE == 0)
        return 0;

    mword alignment = (mword)-1;

    return alignment << ((size8 % MWORD_SIZE) * BITS_PER_BYTE);

}


//
//
mword array8_mword_size(pyr_cache *this_pyr, mword size8){ // array8_mword_size#

    mword size = (size8 / MWORD_SIZE);

    if(size8 % MWORD_SIZE != 0){ //XXX Assumes that int div rounds to floor
        size++;
    }

    return size+1; //for the alignment_word

}


/*****************************************************************************
 *                                                                           *
 *                          ARRAY-1 PRIMITIVES                               *
 *                                                                           *
 ****************************************************************************/


#define array1_mask_generate(off,arr,mask,sel)      \
    mword sel          = (off / MWORD_BIT_SIZE);    \
    mword bit_offset   = (off % MWORD_BIT_SIZE);    \
                                                    \
    if(sel > size(arr)-1){                 \
        _fatal("error");                            \
    }                                               \
                                                    \
    mword mask = (1<<bit_offset);
    

// XXX TESTED XXX
//
mword array1_read(mword *array, mword offset){ // array1_read#

    array1_mask_generate(offset, array, read_mask, mword_select);
    return ((rdv(array,mword_select) & read_mask) >> offset);

}


// XXX TESTED XXX
//
void array1_write(mword *array, mword offset, mword value){ // array1_write#

    array1_mask_generate(offset, array, write_mask, mword_select);
    ldv(array,mword_select) = (rdv(array,mword_select) & ~write_mask) | ((value<<bit_offset) & write_mask);

}


// Returns a val containing the bit at val_array[entry1] (bitwise addressing)
// XXX TESTED XXX
//
mword *array1_th(pyr_cache *this_pyr, mword *val_array, mword entry1){ // array1_th#

    mword *bit = _newbits(this_pyr, 1);
    ldv(bit,0) = array1_read(val_array, entry1);

    return bit;

}


//
//
mword array1_size(pyr_cache *this_pyr, mword *string){ // array1_size#

    mword strsize = size(string) - 1;
    mword last_mword = rdv(string, strsize);
    mword alignment = array1_dec_align(this_pyr, last_mword);

    if(last_mword){
        return  (strsize * MWORD_BIT_SIZE) - (MWORD_BIT_SIZE - alignment);
    }
    else{
        return (strsize * MWORD_BIT_SIZE);
    }

}


// decodes the alignment word
//
mword array1_dec_align(pyr_cache *this_pyr, mword alignment_word){ // array1_dec_align#

    if(alignment_word == 0){
        return 0;
    }

    alignment_word = ~alignment_word;
    mword alignment = 0;

    while(alignment_word != 0){ //FIXME: PERF ... really inefficient
        alignment_word = alignment_word >> 1;
        alignment++;
    }

    return alignment;

}


//Returns an alignment word based on size1
//
mword array1_enc_align(pyr_cache *this_pyr, mword size1){ // array1_enc_align#

    if((size1 % MWORD_BIT_SIZE) == 0)
        return 0;

    mword alignment = FMAX; //(mword)-1;

    return alignment << (size1 % MWORD_BIT_SIZE);

}


//
//
mword array1_mword_size(pyr_cache *this_pyr, mword size1){ // array1_mword_size#

    mword size = (size1 / MWORD_BIT_SIZE);

    if((size1 % MWORD_BIT_SIZE) != 0){ //XXX Assumes that int div rounds to floor
        size++;
    }

    return size+1; //for the alignment_word

}


/*****************************************************************************
 *                                                                           *
 *                            ARRAY CONVERSION                               *
 *                                                                           *
 ****************************************************************************/


// XXX TESTED XXX
//
mword *array_mwords_to_bytes(pyr_cache *this_pyr, mword *array){ // array_mwords_to_bytes#

    mword arr_size  = size(array);
    char *result = (char*)_newstr(this_pyr, arr_size, ' ');

    int i;

    for(i=0; i<arr_size; i++){
        result[i] = (char)(array[i] & 0xff);
    }

    return (mword*)result;

}


// XXX TESTED XXX
//
mword *array_bytes_to_mwords(pyr_cache *this_pyr, mword *array8){ // array_bytes_to_mwords#

    unsigned char *cast_array = (unsigned char *)array8;
    mword arr8_size = array8_size(this_pyr, array8);
    mword *result = mem_new_val(this_pyr, arr8_size, 0);

    int i;

    for(i=0; i<arr8_size; i++){
        result[i] = (mword)cast_array[i];
    }

    return result;

}


// XXX TESTED XXX
//
mword *array_mwords_to_bits(pyr_cache *this_pyr, mword *array){ // array_mwords_to_bits#

    mword arr_size  = size(array);
    mword *result   = _newbits(this_pyr, arr_size);

    int i;

    for(i=0; i<arr_size; i++){
        array1_write(result, i, (array[i] != 0));
    }

    return result;

}


// XXX TESTED XXX
//
mword *array_bits_to_mwords(pyr_cache *this_pyr, mword *array1){ // array_bits_to_mwords#

    mword arr1_size  = array1_size(this_pyr, array1);
    mword *result = mem_new_val(this_pyr, arr1_size, 0);

    int i;

    for(i=0; i<arr1_size; i++){

        result[i] = array1_read(array1, i);

    }

    return result;

}


/*****************************************************************************
 *                                                                           *
 *                          ARRAY CONCATENATION                              *
 *                                                                           *
 ****************************************************************************/


// XXX TESTED XXX
//
mword *array_cat(pyr_cache *this_pyr, mword *left, mword *right){ // array_cat#

    mword s_left;
    mword s_right;

    char *result;

    if(is_val(right) && is_val(left)){
        s_left  = sfield(left);
        s_right = sfield(right);
        result = (char*)mem_new_valz(this_pyr, UNITS_8TOM(s_left+s_right));
    }
    else if(is_ptr(right) && is_ptr(left)){
        s_left  = sfield(left);
        s_right = sfield(right);
        result = (char*)mem_new_ptr(this_pyr, UNITS_8TOM(s_left+s_right));
    }
    else{ // FIXME(throw_exception)
        _fatal("cannot concatenate val-array and ptr-array");
    }

    memcpy(result,        left,  s_left);
    memcpy(result+s_left, right, s_right);

    return (mword*)result;

}


// XXX TESTED XXX
//
mword *array8_cat(pyr_cache *this_pyr, mword *left, mword *right){ // array8_cat#

    mword size_left;
    mword size_right;

    char *result;

    if(!is_val(right) || !is_val(left)){
        _fatal("cannot byte-concatenate ptr-array");
    }

    size_left  = array8_size(this_pyr, left);
    size_right = array8_size(this_pyr, right);

    result = (char*)_newstr(this_pyr, size_left+size_right, '\0');

    memcpy(result,           left,  size_left);
    memcpy(result+size_left, right, size_right);

    return (mword*)result;

}


//
//
mword *array1_cat(pyr_cache *this_pyr, mword *left, mword *right){ // array1_cat#

    mword size_left;
    mword size_right;

    char *result;

    if(!is_val(right) || !is_val(left)){
        _fatal("cannot bit-concatenate ptr-array");
    }

    size_left  = array1_size(this_pyr, left);
    size_right = array1_size(this_pyr, right);

    if(   (size_left  % BITS_PER_BYTE == 0)
       && (size_right % BITS_PER_BYTE == 0)) // array8_cat is *much* faster
        return array8_cat(this_pyr, left, right);

    result = (char*)_newbits(this_pyr, size_left+size_right);

    // The hard part...

//    memcpy(result,           left,  size_left);
//    memcpy(result+size_left, right, size_right);

    return (mword*)result;


}


/*****************************************************************************
 *                                                                           *
 *                            ARRAY COMPARISON                               *
 *                                                                           *
 ****************************************************************************/


//
//
int array_cmp_lex(pyr_cache *this_pyr, mword *left, mword *right, access_size_sel access_size){ // array_cmp_lex#

    mword left_size;
    mword right_size;

    if(access_size == MWORD_ASIZE){
        left_size  = UNITS_MTO8(size_special(left));
        right_size = UNITS_MTO8(size_special(right));
    }
    else{ // access_size == BYTE_ASIZE
        left_size  = array8_size(this_pyr, left);
        right_size = array8_size(this_pyr, right);
    }

    if(left_size > right_size){
        return 1;
    }
    else if(left_size < right_size){
        return -1;
    }
    //else:

    return memcmp(left, right, left_size);

}


// LLLLLLLLLLL
//     RRRRRRRR
//     \---/
//       n
//
int array_ncmp(pyr_cache *this_pyr, mword *left, mword left_offset, mword *right, mword length, access_size_sel access_size){

    mword left_size;
    mword right_size;

    if(access_size == MWORD_ASIZE){
        left_size  = UNITS_MTO8(size(left) - left_offset);
        right_size = UNITS_MTO8(size(right));
        length     = UNITS_MTO8(length);
    }
    else{ // access_size == BYTE_ASIZE
        left_size  = array8_size(this_pyr, left) - left_offset;
        right_size = array8_size(this_pyr, right);
    }

    if( (length > left_size)
            ||
        (length > right_size)){
        return 0;
    }

    return (memcmp(((char*)left + left_offset), right, length) == 0);

}


//
//
int array_cmp_alpha(pyr_cache *this_pyr, mword *left, mword *right, access_size_sel access_size){ // array_cmp_alpha#

    mword left_size;
    mword right_size;
    mword result;

    if(access_size == MWORD_ASIZE){
        left_size  = size(left);
        right_size = size(right);
    }
    else{ // access_size == BYTE_ASIZE
        left_size  = array8_size(this_pyr, left);
        right_size = array8_size(this_pyr, right);
    }

    if(left_size == right_size){
        result = memcmp(left, right, left_size);
    }
    else{
        result = memcmp(left, right, MIN(left_size,right_size));
        if(result == 0){
            if(left_size > right_size){
                result = 1;
            }
            else{
                result = -1;
            }
        }
    }

    return result;

}


// Multi-word unsigned numerical comparison
//
int array_cmp_num(mword *left, mword *right){ // array_cmp_num#

    return array_cmp_num_range(left, (left+size(left)-1), right, (right+size(right)-1));

}


// Multi-word (range-based) unsigned numerical comparison
//
int array_cmp_num_range(mword *left, mword *left_end, mword *right, mword *right_end){ // array_cmp_num_range#

    mword left_size  = (left_end-left);
    mword right_size = (right_end-right);

    mword left_non_zero=0;
    mword right_non_zero=0;

    int i;

    for(i=right_size;i>=0;i--){
        if(right[i] != 0){
            right_non_zero=i;
            break;
        }
    }

    for(i=left_size;i>=0;i--){
        if(left[i] != 0){
            left_non_zero=i;
            break;
        }
    }

    if(left_non_zero > right_non_zero){
        return 1;
    }
    else if(left_non_zero < right_non_zero){
        return -1;
    }
    else{
        for(;i>=0;i--){ // i is in correct position from previous for-loop
            if(rdp(left,i) > rdp(right,i)){ 
                return 1;
            }
            if(rdp(left,i) < rdp(right,i)){ 
                return -1;
            }
        }
    }

    return 0; // left and right are numerically equal

}



// Multi-word signed numerical comparison
//
int array_cmp_num_signed(mword *left, mword *right){ // array_cmp_num_signed#

    mword left_size  = size(left);
    mword right_size = size(right);

    mword left_sign  = rdv(left,left_size-1)   & MSB_MASK;
    mword right_sign = rdv(right,right_size-1) & MSB_MASK;

    if(left_sign && !right_sign){ // left is negative, right is non-negative
        return -1;
    }
    else if(!left_sign && right_sign){ // left is non-negative, right is negative
        return 1;
    }
    else{ // both left and right have the same sign
        return array_cmp_num(left, right);
    }

}



/*****************************************************************************
 *                                                                           *
 *                           ARRAY MOVE/SLICE                                *
 *                                                                           *
 ****************************************************************************/


// Babelized wrapper around memmove()
//
void array_move(pyr_cache *this_pyr, mword *dest, mword dest_index, mword *src, mword src_index, mword size_arg, access_size_sel access_size){ // array_move#

    mword src_size;
    mword dest_size;
    mword src_headroom;
    mword dest_headroom;
    mword size_limit;
    mword final_size;

    if(access_size == MWORD_ASIZE){

        src_size  = size(src );
        dest_size = size(dest);

    }
    else{ // access_size = BYTE_ASIZE

        src_size  = array8_size(this_pyr, src );
        dest_size = array8_size(this_pyr, dest);

    }

    src_index  =  src_index %  src_size;
    dest_index = dest_index % dest_size;

    src_headroom  = ( src_size -  src_index);
    dest_headroom = (dest_size - dest_index);

    size_limit = (src_headroom < dest_headroom) ? src_headroom : dest_headroom;

    final_size = (size_arg < size_limit) ? size_arg : size_limit;

//    final_size = size_arg;

    if(access_size == MWORD_ASIZE){

        memmove( dest+dest_index, src+src_index, (size_t)UNITS_MTO8(final_size) );

    }
    else{ // access_size = BYTE_ASIZE

        memmove( ((char*)dest+dest_index), ((char*)src+src_index), (size_t)final_size );

    }

}


// NB: Do not use on tptr's
//
mword *array_slice(pyr_cache *this_pyr, mword *array, mword start, mword end){ // array_slice#

    mword *result=nil;

    mword arr_size = size(array);
    end = MIN(end, arr_size);
    arr_size = end-start;

    if(end>start){

        if(is_val(array)){
            result = mem_new_val(this_pyr, arr_size, 0);
        }
        else{
            result = mem_new_ptr(this_pyr, arr_size);
        }

        array_move(this_pyr, result, 0, array, start, arr_size, MWORD_ASIZE);

    }

    return result;

}


//
//
mword *array8_slice(pyr_cache *this_pyr, mword *array, mword start, mword end){ // array8_slice#

    mword *result = _val(this_pyr, 0); // Empty-string

    mword size8 = array8_size(this_pyr, array);
    end = MAX(end, size8);

    if(end>start){

        result = _newstr(this_pyr, end-start, ' ');
        array_move(this_pyr, result, 0, array, start, end-start, BYTE_ASIZE);

    }

    return result;

}
 

//
//
mword *array1_slice(pyr_cache *this_pyr, mword *array, mword start, mword end){ // array1_slice#

    mword *result=nil;
    return result;

}


// XXX SMOKE-TESTED XXX
//
void array1_move(pyr_cache *this_pyr, mword *dest, int dest_begin, mword *src, int src_begin, mword size_arg){ // array1_move#

    mword dest_begin_mword = UNITS_1TOM(dest_begin);
    dest_begin -= UNITS_MTO1(dest_begin_mword);

    mword src_begin_mword = UNITS_1TOM(src_begin);
    src_begin -= UNITS_MTO1(src_begin_mword);

    array1_move_unsafe(this_pyr, (dest+dest_begin_mword), dest_begin, (src+src_begin_mword), src_begin, size_arg);

}

// comp --> "complementary modulus", i.e. comp(modulus) = (MWORD_BIT_SIZE - modulus)
// dest_begin and src_begin MUST BE less than MWORD_BIT_SIZE
// This function performs a full bitwise move from src to dest...
// XXX src and dest must not overlap... this is NOT checked in this function XXX
//
void array1_move_unsafe(pyr_cache *this_pyr, mword *dest, int dest_begin, mword *src, int src_begin, mword size_arg){ // array1_move_unsafe#

    // if src, dest & size_arg are aligned, memmove
    // if src is aligned, but dest is not aligned, this is a move
    // if src is not aligned, and dest is aligned, this is a slice

    if(!size_arg)  // nothing to move
        return;

    if(    (  dest_begin % BITS_PER_BYTE == 0)
        && (   src_begin % BITS_PER_BYTE == 0) 
        && (    size_arg % BITS_PER_BYTE == 0) ){
//        _notify("This was supposed to memmove...");
//        memmove( ((char*)dest+UNITS_1TO8(dest_begin)), (char*)src, (size_t)UNITS_1TO8(size_arg) );
//        return;
    }

    mword num_src_splits  = array1_calc_splits(src_begin,  size_arg);
    mword num_dest_splits = array1_calc_splits(dest_begin, size_arg);

    if((num_dest_splits==0) && (num_src_splits==0)){
//_say("splits - dest:0 src:0");
        array1_move_split_0_0(this_pyr, dest, dest_begin, src, src_begin, size_arg);
    }
    else if((num_dest_splits==1) && (num_src_splits==0)){
//_say("splits - dest:1 src:0");
        array1_move_split_1_0(this_pyr, dest, dest_begin, src, src_begin, size_arg);
    }
    else if((num_dest_splits==0) && (num_src_splits==1)){
//_say("splits - dest:0 src:1");
        array1_move_split_0_1(this_pyr, dest, dest_begin, src, src_begin, size_arg);
    }
    else if((num_dest_splits==1) && (num_src_splits==1)){
//_say("splits - dest:1 src:1");
        array1_move_split_1_1(this_pyr, dest, dest_begin, src, src_begin, size_arg);
    }
    else if((num_dest_splits==2) && (num_src_splits==1)){
//_say("splits - dest:2 src:1");
        array1_move_split_2_1(this_pyr, dest, dest_begin, src, src_begin, size_arg);
    }
    else if((num_dest_splits==1) && (num_src_splits==2)){
//_say("splits - dest:1 src:2");
        array1_move_split_1_2(this_pyr, dest, dest_begin, src, src_begin, size_arg);
    }
    else if((num_dest_splits==2) && (num_src_splits==2)){
//_say("splits - dest:n src:n");
        array1_move_split_n(this_pyr, dest, dest_begin, src, src_begin, size_arg);
    }
    else{
        _pigs_fly;
    }

}


//
//
mword array1_calc_splits(mword begin, mword size_arg){ // array1_calc_splits#

    mword begin_comp = MWORD_BIT_SIZE-begin;
    mword end_mod = (begin+size_arg) % MWORD_BIT_SIZE;

    mword num_splits = 0;

    if(size_arg <= MWORD_BIT_SIZE){ // 0 or 1 boundaries crossed
        num_splits = (begin_comp < size_arg);
    }
    else if(size_arg < (2*MWORD_BIT_SIZE)){ // 1 or 2 boundaries crossed
        if((size_arg+begin) <= (2*MWORD_BIT_SIZE)){
            num_splits = 1;
        }
        else{
            num_splits = 2;
        }
    }
    else if(size_arg == (2*MWORD_BIT_SIZE)){ // 1 or 2 boundaries crossed
        if( !begin && !end_mod ){
            num_splits = 1;
        }
        else{
            num_splits = 2;
        }
    }
    else{ // 2 or more boundaries crossed
        num_splits = 2;
    }

    return num_splits;

}


//
//
void array1_move_split_0_0(pyr_cache *this_pyr, mword *dest, int dest_begin, mword *src, int src_begin, mword size_arg){ // array1_move_split_0_0#

    // src    | .xxx.... |
    //          deadbeef
    // 
    // dest   | ...xxx.. |
    //          000ead00
    ldv(dest,0) = MWORD_MUX(
        (MWORD_SHIFT((rdv(src,0)),(dest_begin-src_begin))),
        (rdv(dest,0)),
        (BIT_RANGE((dest_begin+size_arg-1),dest_begin)));

}



// dest:0 src:1
//
void array1_move_split_0_1(pyr_cache *this_pyr, mword *dest, int dest_begin, mword *src, int src_begin, mword size_arg){ // array1_move_split_0_1#

    // src    |. . . . . . x x|x . . . . . . .|
    //         f a c e f e e d d e a d b e e f
    // 
    // dest   |. . . . x x x .|
    //         c c c c e d d c 

    mword src_mwordA = BIT_SELECT(rdv(src,0), MWORD_MSB, src_begin);
    mword src_mwordB = BIT_SELECT(rdv(src,1), (size_arg-(MWORD_MSB-src_begin))-1, 0);

    mword src_mword = (src_mwordA << dest_begin)
                        |
                      (src_mwordB << (dest_begin+(MWORD_MSB-src_begin)+1));

    ldv(dest,0) = MWORD_MUX(
                    src_mword,
                    rdv(dest,0),
                    BIT_RANGE((dest_begin+size_arg-1),dest_begin));

}



// dest:1 src:0
//
void array1_move_split_1_0(pyr_cache *this_pyr, mword *dest, int dest_begin, mword *src, int src_begin, mword size_arg){ // array1_move_split_1_0#

    // src    |. x x x . . . .|
    //         d e a d b e e f
    // 
    // dest   |. . . . . . x x|x . . . . . . .|
    //         c c c c c c e a d 0 0 0 0 0 0 0

    int shiftA = dest_begin-src_begin;
    int shiftB = -1*COMPLEMENT_MTO1(shiftA);

    mword src_mwordA = MWORD_SHIFT(rdv(src,0),shiftA);
    mword src_mwordB = MWORD_SHIFT(rdv(src,0),shiftB);

    ldv(dest,0) = MWORD_MUX(
                    src_mwordA,
                    rdv(dest,0),
                    BIT_RANGE(MWORD_MSB,dest_begin));

    ldv(dest,1) = MWORD_MUX(
                    src_mwordB,
                    rdv(dest,1),
                    BIT_RANGE(MODULO_MTO1(dest_begin+size_arg-1),0));

}


//
//
void array1_move_split_1_1(pyr_cache *this_pyr, mword *dest, int dest_begin, mword *src, int src_begin, mword size_arg){ // array1_move_split_1_1#

    mword src_comp, dest_comp;
    mword size_hi,  size_mid, size_lo;
    mword mask_hi,  mask_mid, mask_lo;
    mword src_val0, src_val1;
    mword buff0,    buff1;

    // size_arg <= MWORD_SIZE
    //               1                0
    //                  hi       lo
    // src:  |-----v----------|------v---------|
    //             |-----------------|
    //                   |-----------------|
    // dest: |...........^....|............^...|
    //                     hi       lo
    if(size_arg <= MWORD_BIT_SIZE){

        src_comp  = MWORD_BIT_SIZE-src_begin;
        dest_comp = MWORD_BIT_SIZE-dest_begin;

        size_hi = src_begin;

        mask_hi = NBIT_LO_MASK(size_arg-src_comp);

        src_val0 = rdv(src,0);
        src_val1 = rdv(src,1);

        buff0 = BIT_SELECT(src_val0, MWORD_MSB, src_begin)
                        |
                     ((src_val1 & mask_hi) << src_comp);

        size_lo = dest_comp;
        size_hi = size_arg-size_lo;

        mask_hi = NBIT_LO_MASK(size_hi);
        mask_lo = NBIT_HI_MASK(size_lo);

        ldv(dest,0) = MWORD_MUX(buff0 << dest_begin, rdv(dest,0), mask_lo);
        ldv(dest,1) = MWORD_MUX(buff0 >> dest_comp,  rdv(dest,1), mask_hi);

    }
    // size_arg > MWORD_SIZE
    //               1                0
    //           hi      mid     lo
    // src:  |--v--v----------|------v---------|
    //          |--|-----------------|
    //                |--|-----------------|
    // dest: |........^..^....|............^...|
    //                 hi  mid     lo
    else{

        src_comp  = MWORD_BIT_SIZE-src_begin;
        dest_comp = MWORD_BIT_SIZE-dest_begin;

        size_hi = size_arg-MWORD_BIT_SIZE;
        size_lo = src_comp;
        size_mid = size_arg-(size_hi+size_lo);

        src_val0 = rdv(src,0);
        src_val1 = rdv(src,1);

        if(size_mid){

            buff0 =
                //lo
                BIT_SELECT(src_val0, MWORD_MSB, src_begin)
                    |
                //mid
                (BIT_SELECT(src_val1, size_mid-1, 0) << size_lo);

        }
        else{

            //lo (no mid)
            buff0 = BIT_SELECT(src_val0, MWORD_MSB, src_begin);

        }

        //hi
        buff1 = BIT_SELECT(src_val1, size_hi+size_mid-1, size_mid);

//_d(buff0);
//_d(buff1);

        size_lo  = dest_comp;
        size_mid = size_arg-(size_hi+size_lo);

        mask_mid = NBIT_LO_MASK(size_mid);
        mask_lo  = NBIT_HI_MASK(size_lo);
        mask_hi  = BIT_RANGE((size_hi+size_mid-1),size_mid);

        ldv(dest,0) = MWORD_MUX(buff0 << dest_begin, rdv(dest,0), mask_lo);
        ldv(dest,1) = MWORD_MUX(buff0 >> dest_comp,  rdv(dest,1), mask_mid);
        ldv(dest,1) = MWORD_MUX(buff1 << size_mid,   rdv(dest,1), mask_hi);

    }

}


// dest:2 src:1
//
void array1_move_split_2_1(pyr_cache *this_pyr, mword *dest, int dest_begin, mword *src, int src_begin, mword size_arg){ // array1_move_split_2_1#

    //               1                0
    //             A      B       C
    // src:  |--v-----v-------|--------v-------|
    //          |-----|----------------|
    //                     |-----|----------------|
    // dest: |.............^..|..^.............|..^.............|
    //                      A  B        C       D

    mword src_comp, dest_comp;
    mword size_A, size_B, size_C;
    mword src_val0, src_val1;
    mword buff0, buff1;

    src_comp  = MWORD_BIT_SIZE-src_begin;
    dest_comp = MWORD_BIT_SIZE-dest_begin;

    size_A = size_arg-MWORD_BIT_SIZE;
    size_C = src_comp;
    size_B = size_arg-(size_A+size_C);

    src_val0 = rdv(src,0);
    src_val1 = rdv(src,1);

    if(size_B){

        buff0 =
            //lo
            BIT_SELECT(src_val0, MWORD_MSB, src_begin)
                |
            //mid
            (BIT_SELECT(src_val1, size_B-1, 0) << size_C);

    }
    else{

        //lo (no mid)
        buff0 = BIT_SELECT(src_val0, MWORD_MSB, src_begin);

    }

    //hi
    buff1 = BIT_SELECT(src_val1, size_A+size_B-1, size_B);

//_d(buff0);
//_d(buff1);

    ldv(dest,0) = MWORD_MUX(
                    (buff0<<dest_begin),
                    rdv(dest,0),
                    BIT_RANGE(MWORD_MSB,dest_begin));

    ldv(dest,1) = MWORD_MUX(
                    (buff0>>dest_comp),
                    rdv(dest,1),
                    BIT_RANGE(dest_begin-1,0));

    ldv(dest,1) = MWORD_MUX(
                    (buff1<<dest_begin),
                    rdv(dest,1),
                    BIT_RANGE(MWORD_MSB,dest_begin));

    ldv(dest,2) = MWORD_MUX(
                    (buff1>>dest_comp),
                    rdv(dest,2),
                    BIT_RANGE((size_arg-(MWORD_BIT_SIZE+dest_comp)-1),0));

}


// dest:1 src:2
//
void array1_move_split_1_2(pyr_cache *this_pyr, mword *dest, int dest_begin, mword *src, int src_begin, mword size_arg){ // array1_move_split_1_2#

    //               1                0
    //                      A  B        C       D
    // src:  |.............v..|..v.............|..v.............|
    //                     |-----|----------------|
    //          |-----|----------------|
    // dest: |--^-----^-------|--------^-------|
    //             A      B       C

    mword src_comp, dest_comp;
    mword size_A, size_B, size_C, size_D;
    mword src_val0, src_val1, src_val2;
    mword buff0, buff1;

    src_comp  = MWORD_BIT_SIZE-src_begin;
    dest_comp = MWORD_BIT_SIZE-dest_begin;

    size_D = src_comp;
    size_B = size_D;
    size_C = src_begin;
    size_A = size_arg-(size_B+size_C+size_D);

    src_val0 = rdv(src,0);
    src_val1 = rdv(src,1);
    src_val2 = rdv(src,2);

    buff0 =
        BIT_SELECT(src_val0, MWORD_MSB, src_begin)
            |
        (BIT_SELECT(src_val1, size_C-1, 0) << size_D);

    buff1 =
        BIT_SELECT(src_val1, MWORD_MSB, size_C)
            |
        (BIT_SELECT(src_val2, size_A-1, 0) << size_B);

//_d(buff0);
//_d(buff1);

    ldv(dest,0) = MWORD_MUX(
                    (buff0<<dest_begin),
                    rdv(dest,0),
                    BIT_RANGE(MWORD_MSB,dest_begin));

    ldv(dest,1) = MWORD_MUX(
                    (buff0>>dest_comp),
                    rdv(dest,1),
                    BIT_RANGE(dest_begin-1,0));

    ldv(dest,1) = MWORD_MUX(
                    (buff1<<dest_begin),
                    rdv(dest,1),
                    BIT_RANGE((size_arg-MWORD_BIT_SIZE+dest_begin-1),dest_begin));

}


// dest>=2, src>=2
//
void array1_move_split_n(pyr_cache *this_pyr, mword *dest, int dest_begin, mword *src, int src_begin, mword size_arg){ // array1_move_split_n#

    // Main loop (0 to n-1):
    //               1                0
    //                  hi       lo
    // src:  |-----v----------|------v---------|
    //             |-----------------|
    //                   |-----------------|
    // dest: |...........^....|............^...|
    //                     hi       lo
    //
    // Final loop: Use array1_move_split_1_1() to move last bits
    mword src_comp,     dest_comp;
    mword dest_size_hi, dest_size_lo;
    mword dest_mask_hi, dest_mask_lo;
    mword src_mask_hi;
    mword buff;

    int i, ctr;

    src_comp  = MWORD_BIT_SIZE-src_begin;
    dest_comp = MWORD_BIT_SIZE-dest_begin;

    src_mask_hi = NBIT_LO_MASK(src_begin);

    dest_size_lo = dest_comp;
    dest_size_hi = dest_begin;

    dest_mask_hi = NBIT_LO_MASK(dest_size_hi);
    dest_mask_lo = NBIT_HI_MASK(dest_size_lo);

    for(    i=size_arg,        ctr=0;
            i>MWORD_BIT_SIZE;
            i-=MWORD_BIT_SIZE, ctr++){

        buff = BIT_SELECT(rdv(src,ctr+0), MWORD_MSB, src_begin)
                        |
                     ((rdv(src,ctr+1) & src_mask_hi) << src_comp);

//_d(buff);

        ldv(dest,ctr+0) = MWORD_MUX(buff << dest_begin, rdv(dest,ctr+0), dest_mask_lo);
        ldv(dest,ctr+1) = MWORD_MUX(buff >> dest_comp,  rdv(dest,ctr+1), dest_mask_hi);

    }

//_dd(i);
//_dd(ctr);

    //clean up remaining bits
    array1_move_unsafe(this_pyr, (dest+ctr), dest_begin, (src+ctr), src_begin, i);

}


// XXX Can this be made into a macro?
//
void array_trunc(pyr_cache *this_pyr, mword *operand, mword new_size){ // array_trunc#

    if(is_val(operand)){
        sfield(operand) = (new_size*MWORD_SIZE);
    }
    else if(is_ptr(operand)){ //is_ptr
        sfield(operand) = (int)-1*(new_size*MWORD_SIZE);
    }
    //else: do nothing, tptrs can't be trunc'd

}


//
//
mword *array_to_string(pyr_cache *this_pyr, mword *array){ // array_to_string#

    if(is_nil(array)){
        return mem_new_val(this_pyr, 1, 0);
    }

    mword *result;
    #define MAX_UTF8_CHAR_SIZE 4

    mword arsize = size(array);
    int temp_buffer_size = MAX_UTF8_CHAR_SIZE * (arsize);

    // free'd below
    char *temp_buffer = mem_sys_alloc( temp_buffer_size ); // XXX WAIVER(mem_sys_alloc) XXX //
    
    mword utf8_length = (mword)u8_toutf8(temp_buffer, temp_buffer_size, (uint32_t *)array, arsize) - 1;

    mword arlength = (utf8_length / 4) + 1;

    if(utf8_length % 4){
        arlength++;
    }

    result = mem_new_val(this_pyr, arlength,0);
    memcpy(result, temp_buffer, utf8_length);
    mem_sys_free(temp_buffer);

    ldv(result,arlength-1) = array8_enc_align(this_pyr, utf8_length);

    return result;

}


// XXX The return-value from this function contains unsafe pointers!!! XXX
// XXX internal interp use ONLY                                        XXX
// XXX If you pass tag=nil, returns ALL tags in bs                     XXX
// XXX PERF: A _tags2ar (like bstruct_to_array) would be more efficient          XXX
//
mword *array_find_val(pyr_cache *this_pyr, mword *bs, mword *val){ // array_find_val#

    mword *span_array     = bstruct_to_array(this_pyr, bs);
    mword size_span_array = size(span_array);
    mword size_ptr;
    mword *val_list       = nil;
    mword *curr_span_elem;
    mword *curr_ptr_elem;

    int i,j;
    for(i=0; i<size_span_array; i++){

        curr_span_elem = rdp(span_array,i);

        if(is_ptr(curr_span_elem)){ // check each element

            size_ptr = size(curr_span_elem);

            for(j=0;j<size_ptr;j++){

                curr_ptr_elem = rdp(curr_span_elem,j);

                if(is_nil(curr_ptr_elem)){
                    continue;
                }

                if(is_val(curr_ptr_elem)){

                    if( !array_cmp_lex(this_pyr, curr_ptr_elem, val, MWORD_ASIZE) ){

                        // push onto val_list
                        if(is_nil(val_list)){
                            val_list = _cons(this_pyr, (curr_span_elem+j), nil);
                        }
                        else{
                            list_unshift(this_pyr, val_list, (curr_span_elem+j));
                        }

                    }

                }

            }

        }

    }

    return val_list;

}


// XXX The return-value from this function contains unsafe pointers!!! XXX
// XXX internal interp use ONLY                                        XXX
// XXX If you pass tag=nil, returns ALL tags in bs                     XXX
// XXX PERF: A _tags2ar (like bstruct_to_array) would be more efficient          XXX
//
mword *array_find_ptr(pyr_cache *this_pyr, mword *bs, mword *ptr){ // array_find_ptr#

    mword *span_array     = bstruct_to_array(this_pyr, bs);
    mword size_span_array = size(span_array);
    mword size_ptr;
    mword *ptr_list       = nil;
    mword *curr_span_elem;
    mword *curr_ptr_elem;

    int i,j;
    for(i=0; i<size_span_array; i++){

        curr_span_elem = rdp(span_array,i);

        if(is_ptr(curr_span_elem)){ // check each element

            size_ptr = size(curr_span_elem);

            for(j=0;j<size_ptr;j++){

                curr_ptr_elem = rdp(curr_span_elem,j);

                if(is_nil(curr_ptr_elem)){
                    continue;
                }

                if(is_ptr(curr_ptr_elem)){

                    if( !array_cmp_lex(this_pyr, curr_ptr_elem, ptr, MWORD_ASIZE) ){

                        // push onto ptr_list
                        if(is_nil(ptr_list)){
                            ptr_list = _cons(this_pyr, (curr_span_elem+j), nil);
                        }
                        else{
                            list_unshift(this_pyr, ptr_list, (curr_span_elem+j));
                        }

                    }

                }

            }

        }

    }

    return ptr_list;

}


//
//
mword *array_to_list(pyr_cache *this_pyr, mword *arr){ // array_to_list#

    mword *last_cons = nil;
    int i;
    mword *entry;

    if(is_ptr(arr)){
        for(i=size(arr)-1;i>=0;i--){
            last_cons = _cons(this_pyr, rdp(arr,i),last_cons);
        }
    }
    else{
        for(i=size(arr)-1;i>=0;i--){
            entry = mem_new_val(this_pyr, 1, 0);
            *entry = rdv(arr,i);
            last_cons = _cons(this_pyr, entry,last_cons);
        }
    }

    return last_cons;

}


//
//
void array_build_max_heap(mword *array){

    mword array_size = size(array);
    int i = array_size;
    i = (i%2) ? (i/2) : ((i/2)-1);

    for(;i>=0;i--){
        array_max_heapify(array, i, array_size);
    }

}


//
//
void array_max_heapify(mword *array, mword i, mword array_size){

    mword left  = 2*i+1;
    mword right = 2*i+2;
    mword largest = i;

    mword *temp;

    if( (left < array_size)
            && 
        val_gt(key_aop(array,left), key_aop(array,largest)) ){
        largest = left;
    }

    if( (right < array_size) 
            && 
        val_gt(key_aop(array,right), key_aop(array,largest)) ){
        largest = right;
    }

    if(largest != i){
        temp = rdp(array,i);
        ldp(array,i) = rdp(array,largest);
        ldp(array,largest) = temp;
        array_max_heapify(array, largest, array_size);
    }

}


// array must be in sorted order (non-decreasing)
// NOTE: It might be nice to know where an element *would* fit in the array
//       if it were inserted. Need two return values - whether the element was
//       found, and where the search found the closest match (return a 
//       boundary index)
//
mword array_search(pyr_cache *this_pyr, mword *array, mword *target, sort_type st){ // array_search#

    return array_search_binary(this_pyr, array, (mword*)(array+size(array)), target, st);

}


// XXX(INTERNAL_USE_ONLY)
//
mword array_search_binary(pyr_cache *this_pyr, mword *begin, mword *end, mword *target, sort_type st){ // array_search_binary#

    mword *array = begin;
    int array_size = (begin-end);

    if(array_size <= ARRAY_LINEAR_THRESH)
        return array_search_linear(this_pyr,
                    array,
                    0,
                    array_size,
                    target,
                    st);

    mword target_val;

    int shift       = array_size >> 1;
    int guess_index = shift;
        shift       >>= 1;

    int lower_bound = 0;
    int upper_bound = array_size;

    int local_array_size = array_size>>ARRAY_PERF_LINEAR_HOLDOFF;

    //////////////////////////// val-array //////////////////////////////////
    if(is_val(array) || (st == VAL)){

        mword guess;
        target_val = vcar(target);

        while(local_array_size){

            guess = rdv(array,guess_index);

            if(guess < target_val){
                lower_bound = guess_index;
                guess_index += shift;
            }
            else if(guess > target_val){
                upper_bound = guess_index;
                guess_index -= shift;
            }
            else if(guess == target_val){
                return guess_index;
            }
            else{
                _pigs_fly;
            }

            shift >>= 1;
            shift = (shift == 0) ? 1 : shift;

            local_array_size >>= 1;

        }

        return array_search_linear(this_pyr,
                    array,
                    lower_bound,
                    upper_bound+1,
                    target,
                    st);

    }
    else{

        mword *guess;

        ////////////////////////// numeric //////////////////////////////////
        if(st == SIGNED || st == UNSIGNED){

            while(local_array_size){

                guess = key_aop(array,guess_index);

                if(array_lt_num(guess, target)){
                    lower_bound = guess_index;
                    guess_index += shift;
                }
                else if(array_gt_num(guess, target)){
                    upper_bound = guess_index;
                    guess_index -= shift;
                }
                else if(array_eq_num(guess, target)){
                    return guess_index;
                }
                else{
                    _pigs_fly;
                }

                shift >>= 1;
                shift = (shift == 0) ? 1 : shift;

                local_array_size >>= 1;

            }

            return array_search_linear(this_pyr,
                        array,
                        lower_bound,
                        upper_bound+1,
                        target,
                        st);

        }
        ////////////////////////// aop-array ////////////////////////////////
        else if(st == ALPHA_MWORD || st == LEX_MWORD){

            while(local_array_size){

                guess = key_aop(array,guess_index);

                if(array_lt(this_pyr, guess, target)){
                    lower_bound = guess_index;
                    guess_index += shift;
                }
                else if(array_gt(this_pyr, guess, target)){
                    upper_bound = guess_index;
                    guess_index -= shift;
                }
                else if(array_eq(this_pyr, guess, target)){
                    return guess_index;
                }
                else{
                    _pigs_fly;
                }

                shift >>= 1;
                shift = (shift == 0) ? 1 : shift;

                local_array_size >>= 1;

            }

            return array_search_linear(this_pyr,
                        array,
                        lower_bound,
                        upper_bound+1,
                        target,
                        st);

        }
        ////////////////////////// aop-array8 ///////////////////////////////
        else if(st == ALPHA_BYTE || st == LEX_BYTE){

            while(local_array_size){

                guess = key_aop(array,guess_index);

                if(array8_lt(this_pyr, guess, target)){
                    lower_bound = guess_index;
                    guess_index += shift;
                }
                else if(array8_gt(this_pyr, guess, target)){
                    upper_bound = guess_index;
                    guess_index -= shift;
                }
                else if(array8_eq(this_pyr, guess, target)){
                    return guess_index;
                }
                else{
                    _pigs_fly;
                }


                shift >>= 1;
                shift = (shift == 0) ? 1 : shift;

                local_array_size >>= 1;

            }

            return array_search_linear(this_pyr,
                        array,
                        lower_bound,
                        upper_bound+1,
                        target,
                        st);

        }
        else{
            _give_up;
        }

//        return -1; // we didn't find what you were looking for...
        return ARRAY_SEARCH_NOT_FOUND;
    }

    _pigs_fly;

//    return -1; // we didn't find what you were looking for...
        return ARRAY_SEARCH_NOT_FOUND;

}


//
//
mword array_search_linear(pyr_cache *this_pyr, mword *array, int start, int end, mword *target, sort_type st){ // array_search_linear#

    start = (start < 0) ? 0 : start;
    end   = (end   > size(array)) ? size(array) : end;

    int i=start;
    mword target_val;

    if(is_val(array) || st == VAL){
        target_val = vcar(target);
        for(; i<end; i++){
            if(rdv(array,i) == target_val){
                return i;
            }
        }
        return -1; // we didn't find what you were looking for...
    }
    else if(st == SIGNED || st == UNSIGNED){ // numeric
        for(; i<end; i++){
            if(array_eq_num(key_aop(array,i), target)){
                return i;
            }
        }
    }
    else if(st == ALPHA_MWORD || st == LEX_MWORD){
        for(; i<end; i++){
            if(array_eq(this_pyr, key_aop(array,i), target)){
                return i;
            }
        }
    }
    else if(st == ALPHA_BYTE || st == LEX_BYTE){
        for(; i<end; i++){
            if(array8_eq(this_pyr, key_aop(array,i), target)){
                return i;
            }
        }
    }
    else{
        _give_up;
    }

    return -1; // we didn't find what you were looking for...

}


//array operators
//--------------
//


//
//
blob array_cat_pyr_op(pyr_cache *this_pyr, blob arrays){ // array_cat_pyr_op#

    char *result;

    mword num_arrays = size(arrays);
    int i;

    if(num_arrays == 1) return (blob)rdp(arrays,0);

    mword all_vals   = 1;
    mword all_ptrs   = 1;
    mword total_size = 0;
    mword *curr_array;

    for(i=0; i<num_arrays; i++){

        curr_array = rdp(arrays,i);

        all_vals = all_vals && is_val(curr_array);
        all_ptrs = all_ptrs && is_ptr(curr_array);

        total_size += size(curr_array);

    }

    mword curr_offset=0;
    mword curr_size;

    if(all_vals){
        result = (char*)mem_new_valz(this_pyr, total_size);
    }
    else if(all_ptrs){
        result = (char*)mem_new_ptr(this_pyr, total_size);
    }
    else{
        return (blob)nil;
//        return (blob)global_irt->tags->PYR_TAG_BAD_OPERANDS;
    }

    for(i=0; i<num_arrays; i++){
        curr_array = rdp(arrays,i);
        curr_size  = UNITS_MTO8(size(curr_array));
        memcpy(result+curr_offset, curr_array, curr_size);
        curr_offset+=curr_size;
    }

    return (blob)result;

}

// Clayton Bauman 2017

