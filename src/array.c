// array.c
//

#include "pyramid.h"
#include "array.h"
#include "mem.h"

//#include "bstruct.h"
//#include "operator.h"
//#include "pbp.h"
//#include "introspect.h"
//#include "string.h"
//#include "lib_babel.h"
//#include "logic.h"
//#include "list.h"


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

    mword alignment = (mword)-1;

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
    else{ //FIXME: Throw an exception
        _fatal("cannot concatenate leaf array and interior array");
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
        _fatal("cannot byte-concatenate ptr-array");
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


#define array_cmp_size_check(x,y)       \
    if(x > y){                          \
        return 1;                       \
    }                                   \
    else if(x < y){                     \
        return -1;                      \
    }
    

//
//
int array_cmp_lex(mword *left, mword *right){ // array_cmp_lex#

    mword left_size  = size(left);
    mword right_size = size(right);

    array_cmp_size_check(left_size, right_size);

    return memcmp(left, right, UNITS_MTO8(left_size));

}


//
//
int array8_cmp_lex(pyr_cache *this_pyr, mword *left, mword *right){ // array8_cmp_lex#

    mword left_size  = array8_size(this_pyr, left);
    mword right_size = array8_size(this_pyr, right);

    array_cmp_size_check(left_size, right_size);

    return memcmp(left, right, left_size);

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

    if(access_size == MWORD_ASIZE){

        memmove( dest+dest_index, src+src_index, (size_t)UNITS_MTO8(final_size) );

    }
    else{ // access_size = BYTE_ASIZE

        memmove( ((char*)dest+dest_index), ((char*)src+src_index), (size_t)final_size );

    }

}


// comp --> "complementary modulus", i.e. comp(modulus) = (MWORD_BIT_SIZE - modulus)
//
void array1_move(pyr_cache *this_pyr, mword *dest, mword dest_begin, mword *src, mword size_arg){ // array1_move#
//void array1_move(pyr_cache *this_pyr, mword *dest, mword dest_begin, mword *src, src_begin, mword size_arg, align_sel who_is_aligned){ // array1_move#

    // FIXME: dest_begin could be > MWORD_BIT_SIZE
    // Fix this by adjusting dest to point at the first mword of the move and
    // adjusting dest_begin accordingly

    if((src == dest) // src and dest must not overlap
           || (!size_arg)) // nothing to move
        return;

//    if(    (  dest_mod % BITS_PER_BYTE == 0)
//        && (  size_arg % BITS_PER_BYTE == 0) ){
//        array_move(this_pyr, dest, dest_mod, src, 0, UNITS_1TO8(size_arg), BYTE_ASIZE);
//    }

//    mword src_begin = 0;
//    mword src_mod = 0;
//    mword src_comp = MWORD_BIT_SIZE;

    mword dest_begin_mod = dest_begin % MWORD_BIT_SIZE;
    mword dest_begin_comp = MWORD_BIT_SIZE-dest_begin_mod;

//    mword dest_end = dest_begin + size_arg;
//    mword dest_end_mod = dest_end % MWORD_BIT_SIZE;
//    mword dest_end_comp = MWORD_BIT_SIZE-dest_end_mod;

//    mword size_mod  = size_arg % MWORD_BIT_SIZE;
//    mword size_comp = MWORD_BIT_SIZE-size_mod;

//    _dd(src_begin);
//    _dd(src_mod);
//    _dd(src_comp);
//
//    _dd(dest_begin);
//    _dd(dest_begin_mod);
//    _dd(dest_begin_comp);
//
//    _dd(dest_end);
//    _dd(dest_end_mod);
//    _dd(dest_end_comp);
//
//    _dd(size_arg);
//    _dd(size_mod);
//    _dd(size_comp);

    mword num_dest_boundaries_crossed = 0;

    if(size_arg <= (2*MWORD_BIT_SIZE)){
        num_dest_boundaries_crossed = (dest_begin_comp < size_arg);
    }
    else{
        num_dest_boundaries_crossed = 2;
    }

   _dd(num_dest_boundaries_crossed);

    if(num_dest_boundaries_crossed == 0){
        array1_move_single(this_pyr, dest, dest_begin, src, size_arg);
    }
    else if(num_dest_boundaries_crossed == 1){
        array1_move_double(this_pyr, dest, dest_begin, src, size_arg);
    }
    else{ // num_dest_boundaries_crossed > 1
        array1_move_n(this_pyr, dest, dest_begin, src, size_arg);
    }

}



// prereqs: 
//      dest_mod < MWORD_BIT_SIZE
//      size_arg <= MWORD_BIT_SIZE-dest_mod
//
void array1_move_single(pyr_cache *this_pyr, mword *dest, mword dest_mod, mword *src, mword size_arg){

    // single move operation
    mword read_mask  = NBIT_LO_MASK(size_arg);
    mword write_mask = read_mask << dest_mod;
    mword write_val = (rdv(src,0) & read_mask) << dest_mod;

    ldv(dest,0) = MWORD_MUX(write_val, rdv(dest,0), write_mask);

//    mword result = ldv(dest,0);
//    _d(result);

}


//
//
void array1_move_double(pyr_cache *this_pyr, mword *dest, mword dest_mod, mword *src, mword size_arg){

    // |----------------|----------------|
    //       |-----------------|
    // |.....^..........^......^.........|
    //            B        A

    // two move operations (in dest)
//    _trace;
//
//    _dd(dest_mod);
//    _dd(size_arg);

    mword dest_comp = MWORD_BIT_SIZE-dest_mod;
    mword size_argA, size_argB, size_argC;
    mword read_maskA, read_maskB, read_maskC;
    mword write_maskA, write_maskB, write_maskC;
    mword write_valA, write_valB, write_valC;

    if(size_arg <= MWORD_BIT_SIZE){

        size_argA = dest_comp;
        size_argB = size_arg-size_argA;

        read_maskA  = NBIT_LO_MASK(size_argA);
        read_maskB  = NBIT_LO_MASK(size_argB) << size_argA;

        write_maskA = read_maskA << dest_mod;
        write_maskB = NBIT_LO_MASK(size_argB);

        write_valA = (rdv(src,0) & read_maskA) << dest_mod;
        write_valB = (rdv(src,0) & read_maskB) >> size_argA;

//        _dd(size_argA);
//        _dd(size_argB);
//
//        _d(read_maskA);
//        _d(read_maskB);
//
//        _d(write_maskA);
//        _d(write_maskB);
//
//        _d(write_valA);
//        _d(write_valB);

        ldv(dest,0) = MWORD_MUX(write_valA, rdv(dest,0), write_maskA);
        ldv(dest,1) = MWORD_MUX(write_valB, rdv(dest,1), write_maskB);

//        mword result0 = ldv(dest,0);
//        mword result1 = ldv(dest,1);
//
//        _d(result0);
//        _d(result1);

    }
    // |----------------|----------------|
    //    |--|-----------------|
    // |..^..^..........^......^.........|
    //     C      B        A
    else{

        size_argA = dest_comp;
        size_argC = size_arg-MWORD_BIT_SIZE;
        size_argB = size_arg-(size_argA+size_argC);

        read_maskA  = NBIT_LO_MASK(size_argA);
        read_maskB  = NBIT_LO_MASK(size_argB) << size_argA;
        read_maskC  = NBIT_LO_MASK(size_argC);

        write_maskA = read_maskA << dest_mod;
        write_maskB = NBIT_LO_MASK(size_argB);
        write_maskC = NBIT_LO_MASK(size_argC) << size_argB;

        write_valA = (rdv(src,0) & read_maskA) << dest_mod;
        write_valB = (rdv(src,0) & read_maskB) >> size_argA;
        write_valC = (rdv(src,1) & read_maskC) << size_argB;

//        _dd(size_argA);
//        _dd(size_argB);
//        _dd(size_argC);
//
//        _d(read_maskA);
//        _d(read_maskB);
//        _d(read_maskC);
//
//        _d(write_maskA);
//        _d(write_maskB);
//        _d(write_maskC);
//
//        _d(write_valA);
//        _d(write_valB);
//        _d(write_valC);

        ldv(dest,0) = MWORD_MUX(write_valA, rdv(dest,0), write_maskA);
        ldv(dest,1) = MWORD_MUX(write_valB, rdv(dest,1), write_maskB);
        ldv(dest,1) = MWORD_MUX(write_valC, rdv(dest,1), write_maskC);

//        mword result0 = ldv(dest,0);
//        mword result1 = ldv(dest,1);
//
//        _d(result0);
//        _d(result1);

    }

//    mword result = ldv(dest,0);
//    _d(result);

}


// prereq: dest_begin < MWORD_BIT_SIZE
//
void array1_move_n(pyr_cache *this_pyr, mword *dest, mword dest_begin, mword *src, mword size_arg){

    // calculate A and B masks
    // move body
    // final move operation
    _trace;

    mword dest_comp = MWORD_BIT_SIZE-dest_begin;

    mword read_mask  = NBIT_HI_MASK(dest_comp);
    mword write_mask = read_mask >> dest_begin;
//    mword write_val  = (rdv(src,0) & read_mask) >> dest_begin;

_dd(dest_comp);
_d(write_mask);

//    ldv(dest,0) = MWORD_MUX(write_val, rdv(dest,0), write_mask);

    // body of move loop ...

    mword dest_end = dest_begin + size_arg;
    mword dest_end_mod = dest_end % MWORD_BIT_SIZE;

_dd(dest_end_mod);

    mword body_size = UNITS_1TOM(size_arg-(dest_comp+dest_end_mod));

    //|ffffffff|fffffff f|fffffff f|fffffff f|ffffffff|
    //      |ff fffffff|f fffffff|f fffffff|f fffffff|
    //         |                                     |
    //                                      ^ ^^^^^^^
    //                                      B    A

    int i;
    for(i=0; i<body_size; i++){

    }

_dd(body_size);

    if(dest_end_mod){

//    read_mask  = NBIT_LO_MASK(dest_end_mod);
//    write_mask = read_mask >> SOMETHING;
//    write_val  = (rdv(src,SOMETHING) & read_mask) >> SOMETHING;
//
//    ldv(dest,SOMETHING) = MWORD_MUX(write_val, rdv(dest,SOMETHING), write_mask);

    }

}


//// src is always aligned
//// Note: to move from unaligned src to unaligned dest, use an intermediate buffer
////
//void array1_move(pyr_cache *this_pyr, mword *dest, mword dest_index, mword *src, mword size_arg){ // array1_move#
//
//    int i;
//
//    if(src == dest) // src and dest must not overlap
//        return;
//
////    mword src_size  = array1_size(this_pyr, src );
////    mword dest_size = array1_size(this_pyr, dest);
//
//    // Call array_move() for byte-aligned moves...
////    if(    (dest_index % BITS_PER_BYTE == 0)
////        && (  size_arg % BITS_PER_BYTE == 0) ){
////        array_move(this_pyr, dest, dest_index, src, 0, UNITS_1TO8(size_arg), BYTE_ASIZE);
////    }
//
//    mword size_arg_mwords = array1_mword_size(this_pyr, size_arg) - 1;
//    mword dest_index_mod  = dest_index % MWORD_BIT_SIZE;
//    mword dest_end_mod    = (dest_index+size_arg) % MWORD_BIT_SIZE;
//
//    mword src_curr_mword;
//    mword final_mword_mask;
//
//    mword bits_remaining = size_arg;
//
//    mword src_hi_mask = NBIT_HI_MASK(dest_index_mod);
//    mword src_lo_mask = NBIT_LO_MASK(dest_index_mod);
//
//    mword prev_lo_split=0;
//
//    mword dest_write;
//
//    if(dest_index_mod == 0){
//
//        _say("aligned move");
//
//        // just copy mwords until we get to the last mword, then mask
//        // return
//        for(i=0; (i<size_arg_mwords) & (bits_remaining >= MWORD_BIT_SIZE); i++){
//            src_curr_mword = rdv(src,i);
//            _d(src_curr_mword);
//            bits_remaining -= MWORD_BIT_SIZE; // FIXME: PERF YUCK!
//        }
//
//        if(bits_remaining){
//            if(bits_remaining <= (MWORD_BIT_SIZE-dest_end_mod)){
//_trace;
//                final_mword_mask = NBIT_LO_MASK(bits_remaining);
//                src_curr_mword = (rdv(src,i) & final_mword_mask);
//                ldv(dest,i) = src_curr_mword;
//                _d(src_curr_mword);
//            }
//            else{
//_trace;
//                final_mword_mask = NBIT_HI_MASK(bits_remaining);
//                final_mword_mask = NBIT_HI_MASK(MWORD_BIT_SIZE-dest_index_mod);
//                src_curr_mword = rdv(src,i) & final_mword_mask;
//                _d(src_curr_mword);
//                final_mword_mask = NBIT_LO_MASK(bits_remaining-(MWORD_BIT_SIZE-dest_index_mod));
//                src_curr_mword = rdv(src,i) & final_mword_mask;
//                _d(src_curr_mword);
//            }
////            _d(final_mword_mask);
//        }
//
//    }
//    else{
//
//        _say("unaligned move");
//
//        for(i=0; (i<size_arg_mwords) & (bits_remaining >= MWORD_BIT_SIZE); i++){
//            src_curr_mword = rdv(src,i);
//            dest_write = ((src_curr_mword & src_lo_mask) << dest_index) | prev_lo_split;
//            prev_lo_split = (src_curr_mword & src_hi_mask) >> (MWORD_BIT_SIZE-dest_index);
//            _d(src_curr_mword);
//            _d(dest_write);
//            bits_remaining -= MWORD_BIT_SIZE;
//        }
//
//        if(bits_remaining){ // UGH!!
//            final_mword_mask = NBIT_LO_MASK(bits_remaining);
//            _d(final_mword_mask);
//        }
//
//    }
//
//
//}


// NB: Do not use on tptr's
//
mword *array_slice(pyr_cache *this_pyr, mword *array, mword start, mword end){ // array_slice#

    mword *result=nil;

    mword arr_size = size(array);
    end = MAX(end, arr_size);

    if(end>start){

        if(is_val(array)){
            result = mem_new_val(this_pyr, end-start, 0);
        }
        else{
            result = mem_new_ptr(this_pyr, end-start);
        }

        array_move(this_pyr, result, 0, array, start, end-start, MWORD_ASIZE);

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

//      Use src_split and dest_split
//      If src_split == 0 (MWORD-aligned): 
//          memcpy
//      Else
//          read 2 mwords, mask each at src_split
//          if first iteration:
//              discard lower bits of mwords[0]
//          dest_result = ((mword[0]&upper_src_split) >> dest_split)
//                          | ((mword[1]&lower_src_split) << (MWORD_BIT_SIZE-dest_split))
//          prime dest_result for next iteration from (mword[1] & upper_src_split)
//

    // XXX put start and end checks HERE XXX
    if(start >= end)
        return nil;

    mword bit_length = end-start;
//_d(bit_length);

    mword mword_length = UNITS_1TOM(bit_length-1)+1;
//_d(mword_length);

    mword src_start_mword = UNITS_1TOM(start);
//_d(src_start_mword);

    mword src_end_mword = UNITS_1TOM(end-1);
//_d(src_end_mword);

    mword src_mword_length = (src_end_mword-src_start_mword)+1;
//_d(src_mword_length);

    mword *result = _newbits(this_pyr, bit_length);

//    mword src_start_modulus;
//    mword src_end_modulus;

    mword src_start_modulus = MODULO_MTO1(start);
//    mword src_end_modulus   = MODULO_MTO1(end);

//    mword src_start_split    = MWORD_BIT_SIZE - src_start_modulus;
//    mword src_end_split      = MWORD_BIT_SIZE - src_end_modulus;

//      mword dest_start_modulus = 0;
//      mword dest_end_modulus   = modulo_mword_size(bit_length);
//
//      mword dest_start_split   = MWORD_BIT_SIZE;
//      mword dest_end_split     = 


//    mword src_start_mask;
//    mword src_end_mask;

//    if(bit_length < MWORD_BIT_SIZE){
    if(mword_length == 1){
        if(src_mword_length == 1){
            _say("Case 1a");
            // Case 1a: All bits in single mword
            //
            //  |.###....|
            //    |||
            //    +++-+++
            //        |||
            //  |.....###|

            ldv(result,0) = rdv(array, src_start_mword) >> src_start_modulus;
            ldv(result,0) = rdv(result,0) & ~rdv(result,1);

        }
        else{ // crosses mword boundary
            _say("Case 1b");
            // Case 1b: Bits cross single mword-boundary, size(result) <= MWORD_BIT_SIZE
            //
            //         11 1
            //         45 6
            //  |......##|#.......|
            //         || |
            //         ++-+----+++
            //                 |||
            //           |.....###|
//            src_start_modulus = MODULO_MTO1(start);
//            src_end_modulus   = MODULO_MTO1(end);
//            mword mask = mask_split_low(bit_length) << start;
//            ldv(result,0) = (rdv(array,0) & mask) >> start;

            // src_mword1_mask = FMAX & [MWORD_BIT_SIZE:start_modulus]
            // src_mword2_mask = FMAX & [end_modulus:0]
            //
            // dest_lo_mask = FMAX >> [MWORD_BIT_SIZE-start_modulus]
            // dest_hi_mask = ~dest_lo_mask
            //
            // XXX USE ARRAY1_ENC_ALIGN TO GENERATE MASKS!!! XXX

        }
    }
//    else if(bit_length < (2*MWORD_BIT_SIZE)){
    else if(mword_length == 2){
        if(src_mword_length == 2){
            _say("Case 2a");
            // Case 2a: Bits cross single mword-boundary, size(result) > MWORD_BIT_SIZE
            //
            //         11 1
            //         45 6
            //  |.#######|##......|
            //    ||||||| ||
            //    +++++++-++++++++
            //          | ||||||||
            //  |.......#|########|
        }
        else{ //src_mword_length = 3
            _say("Case 2b");
            // Case 2b: Bits cross more than one mword-boundary; size(excess bits) <= MWORD_BIT_SIZE
            //
            //  |......##|########|#.......|
            //         || |||||||| |
            //         ++-++++++++-++++++++
            //                 ||| ||||||||
            //           |.....###|########|
        }
    }
    else{ // mword_length > 2
        _say("Case 3");
        // Case 3: Bits cross more than one mword-boundary; size(excess bits) > MWORD_BIT_SIZE
        //
        //  |...#####|########|####....|
        //      ||||| |||||||| ||||
        //      +++++-++++++++-++++++++
        //          | |||||||| ||||||||
        //  |.......#|########|########|
    }

    return result;

}


// ETC:

// mword *array_heapify(pyr_cache *this_pyr, ...
// mword *array_sort(pyr_cache *this_pyr, ...
// mword *array_find_val(pyr_cache *this_pyr, ...
// mword *array_find_ptr(pyr_cache *this_pyr, ...
// mword *array_to_list(pyr_cache *this_pyr, ...


//
//
////
////
////
////
//mword _cxr1(pyr_cache *this_pyr, mword *arr, mword off){ // _cxr1#
//
//    mword mword_select = off / MWORD_BIT_SIZE;
//    mword bit_offset   = off % MWORD_BIT_SIZE;
//
//    if(mword_select > (size(arr)-1)){
//        _fatal("_cxr1 error");
//    }
//
//    mword result = (rdv(arr,mword_select) & (1<<bit_offset)) >> bit_offset;
//
////    return (rdv(arr,mword_select) & (1<<bit_offset)) >> bit_offset;
//    return result;
//
//}
//
//
//// In-place mergesort of an array
////
//void _msort(pyr_cache *this_pyr, mword *array){ // _msort#
//
//    _rmsort(this_pyr, 0, size(array), array);
//
//}
//
//
////
////
//void _rmsort(pyr_cache *this_pyr, mword left, mword right, mword *array){ // _rmsort#
//
//    // base case, array is already sorted
//    if (right - left <= 1){
//        return;
//    }
//
//    // set up bounds to slice array into
//    mword left_start  = left;
//    mword left_end    = (left+right)/2;
//    mword right_start = left_end;
//    mword right_end   = right;
//
//    // sort left half
//    _rmsort(this_pyr, left_start, left_end, array);
//
//    // sort right half
//    _rmsort(this_pyr, right_start, right_end, array);
//
//    // merge sorted halves back together
//    _merge(this_pyr, array, left_start, left_end, right_start, right_end);
//
//}
//
//
////
////
//void _merge(pyr_cache *this_pyr, mword *array, mword left_start, mword left_end, mword right_start, mword right_end){ // _merge#
//
//    // calculate temporary array sizes
//    mword left_length  = left_end  - left_start;
//    mword right_length = right_end - right_start;
//
//    // declare temporary arrays
//    mword *left_half  = mem_new_val(this_pyr,left_length,0);
//    mword *right_half = mem_new_val(this_pyr,right_length,0);
//
//    mword r = 0; // right_half index
//    mword l = 0; // left_half index
//    mword i = 0; // array index
//
//    // copy left half of array into left_half
//    for (i = left_start; i < left_end; i++, l++){
//        left_half[l] = array[i];
//    }
//
//    // copy right half of array into right_half
//    for (i = right_start; i < right_end; i++, r++){
//        right_half[r] = array[i];
//    }
//
//    // merge left_half and right_half back into array
//    for ( i = left_start, r = 0, l = 0; l < left_length && r < right_length; i++){
//
//        if ( left_half[l] < right_half[r] ){
//            array[i] = left_half[l++];
//        }
//        else {
//            array[i] = right_half[r++];
//        }
//    }
//
//    // copy over leftovers of whichever temporary array hasn't finished
//    for ( ; l < left_length; i++, l++){
//        array[i] = left_half[l];
//    }
//
//    for ( ; r < right_length; i++, r++){
//        array[i] = right_half[r];
//    }
//
//}
//
//
//// In-place mergesort of a ptr-array
////
//void array_ptr_sort(pyr_cache *this_pyr, mword *array, mword *comparator){ // array_ptr_sort#
//
//    mword *temp;
//
//    if(is_val(array)){
//
//        array_rptr_sort(this_pyr, 0, size(array), array, nil, ARRAY_SORT_TYPE_LEAF);
//
//    }
//    else{
//
//        temp = _cp(this_pyr, comparator);
//        _append_direct(this_pyr, temp, _cons(this_pyr, _cp(this_pyr, bpdl_return_opcode), nil));
//        array_rptr_sort(this_pyr, 0, size(array), array, lib_code_to_pyr(this_pyr, temp, this_pyr->soft_root), ARRAY_SORT_TYPE_NON_LEAF);
//
//    }
//
//}
//
//
////
////
//void array_rptr_sort(pyr_cache *this_pyr, mword left, mword right, mword *array, mword *comparator, mword sort_type){ // array_rptr_sort#
//
//    // base case, array is already sorted
//    if(right - left <= 1){
//        return;
//    }
//
//    // set up bounds to slice array into
//    mword left_start  = left;
//    mword left_end    = (left+right)/2;
//    mword right_start = left_end;
//    mword right_end   = right;
//
//    // sort left half
//    array_rptr_sort(this_pyr, left_start, left_end, array, comparator, sort_type);
//
//    // sort right half
//    array_rptr_sort(this_pyr, right_start, right_end, array, comparator, sort_type);
//
//    // merge sorted halves back together
//    array_ptr_sort_merge(this_pyr, array, left_start, left_end, right_start, right_end, comparator, sort_type);
//
//}
//
//
////
////
//void array_ptr_sort_merge(pyr_cache *this_pyr, mword *array, mword left_start, mword left_end, mword right_start, mword right_end, mword *comparator, mword sort_type){ // array_ptr_sort_merge#
//
//    // calculate temporary array sizes
//    mword left_length  = left_end  - left_start;
//    mword right_length = right_end - right_start;
//
//    // declare temporary arrays
//    mword *left_half  = _newin(this_pyr,left_length);
//    mword *right_half = _newin(this_pyr,right_length);
//
//    mword r = 0; // right_half index
//    mword l = 0; // left_half index
//    mword i = 0; // array index
//
//    // copy left half of array into left_half
////    for (i = left_start; i < left_end; i++, l++){
////        left_half[l] = array[i];
////    }
////
////    // copy right half of array into right_half
////    for (i = right_start; i < right_end; i++, r++){
////        right_half[r] = array[i];
////    }
//
//    memcpy(left_half,  (array+left_start),  UNITS_MTO8(left_length));  // XXX WAIVER(memcpy) XXX //
//    memcpy(right_half, (array+right_start), UNITS_MTO8(right_length)); // XXX WAIVER(memcpy) XXX //
//
//    mword *comparison_result;
//    mword *arg_list;
//
//    if(sort_type == ARRAY_SORT_TYPE_LEAF){
//
//        // merge left_half and right_half back into array
//        for( i = left_start, r = 0, l = 0; l < left_length && r < right_length; i++){
//
//            if ( left_half[l] < right_half[r] ){
//                array[i] = left_half[l++];
//            }
//            else {
//                array[i] = right_half[r++];
//            }
//
//        }
//
//    }
//    else{ // if(sort_type == ARRAY_SORT_TYPE_NON_LEAF)
//
//        // merge left_half and right_half back into array
//        for( i = left_start, r = 0, l = 0; l < left_length && r < right_length; i++){
//
//            arg_list = _mkls(this_pyr, 2, left_half[l], right_half[r]);
//            this_pyr->flags->MC_GC_INTERP_BLOCKING = FLAG_SET;
//            comparison_result = lib_babelr(this_pyr, comparator, arg_list);
//            this_pyr->flags->MC_GC_INTERP_BLOCKING = FLAG_CLR;
//
//            if( is_false(comparison_result) ){
//                array[i] = left_half[l++];
//            }
//            else {
//                array[i] = right_half[r++];
//            }
//
//        }
//
//    }
//
//    if(l<left_length){
//        memcpy((array+i), (left_half+l), UNITS_MTO8(left_length-l));   // XXX WAIVER(memcpy) XXX //
//    }
//
//    if(r<right_length){
//        memcpy((array+i), (right_half+r), UNITS_MTO8(right_length-r)); // XXX WAIVER(memcpy) XXX //
//    }
//
//}
//
//
//
//
//
//
///*****************************************************************************
// *                                                                           *
// *                                ARRAY-M                                    *
// *                                                                           *
// ****************************************************************************/
//
//
////
////
//// FIXME: cut --> reimplement as two _slice's (left and right)
//
//
////
////
//mword *_arcat(pyr_cache *this_pyr, mword *left, mword *right){ // _arcat#
//
//    mword *result;
//
//    mword size_left  = size(left);
//    mword size_right = size(right);
//
//    if      ( is_val(right)  &&  is_val(left) ){
//        result = mem_new_val(this_pyr,  size_left + size_right, 0 );
//    }
//    else if ( is_ptr(right)  &&  is_ptr(left) ){
//        result = _newin(this_pyr,  size_left + size_right );
//    }
//    else{ //FIXME: Throw an exception
//        _fatal("cannot concatenate leaf array and interior array");
//    }
//
//    mword i,j;
//    for(    i=0;
//            i<size_left;
//            i++
//        ){
//
//        ldp((mword*)result,i) = rdp(left,i);
//
//    }
//
//    for(    i=0,j=size_left;
//            i<size_right;
//            i++,j++
//        ){
//
//        ldp(result,j) = rdp(right,i);
//
//    }
//
//    return result;
//
//}
//
//
////// Babelized wrapper around memmove()
//////
////void move(mword *dest, mword dest_index, mword *src, mword src_index, mword size){  // move#
////
////    mword  src_size = size(src );
////    mword dest_size = size(dest);
////
////     src_index =  src_index %  src_size;
////    dest_index = dest_index % dest_size;
////
////    mword  src_headroom = ( src_size -  src_index);
////    mword dest_headroom = (dest_size - dest_index);
////
////    mword size_limit = (src_headroom < dest_headroom) ? src_headroom : dest_headroom;
////
////    size = (size < size_limit) ? size : size_limit;
////
////    //void * memmove( void * destination, const void * source, size_t num );
////    memmove( dest+dest_index, src+src_index, (size_t)UNITS_MTO8(size) );
////
////}
//
//
////
////
//mword *_ar2bytes(pyr_cache *this_pyr, mword *array){ // _ar2bytes#
//
//    if(is_nil(array)){
//        return mem_new_val(this_pyr, 1, 0);
//    }
//
//    mword arsize = size(array);
//
//    char *result = (char*)_newstr(this_pyr, arsize, ' ');
//
//    int i;
//    for(i=0;i<arsize;i++){
//        result[i] = (rdv(array,i) & 0xff);
//    }
//
//    return (mword *)result;
//
//}
//
//
//// XXX The return-value from this function contains unsafe pointers!!! XXX
//// XXX internal interp use ONLY                                        XXX
//// XXX If you pass tag=nil, returns ALL tags in bs                     XXX
//// XXX PERF: A _tags2ar (like _bs2ar) would be more efficient          XXX
////
//mword *array_find_val(pyr_cache *this_pyr, mword *bs, mword *val){ // array_find_val#
//
//    mword *span_array     = _bs2ar(this_pyr, bs);
//    mword size_span_array = size(span_array);
//    mword size_inte;
//    mword *val_list       = nil;
//    mword *curr_span_elem;
//    mword *curr_inte_elem;
//
//
////_dump(span_array);
//
//    int i,j;
//    for(i=0; i<size_span_array; i++){
//
//        curr_span_elem = rdp(span_array,i);
//
//        if(is_ptr(curr_span_elem)){ // check each element
//
//            size_inte = size(curr_span_elem);
//
//            for(j=0;j<size_inte;j++){
//
//                curr_inte_elem = rdp(curr_span_elem,j);
//
//                if(is_nil(curr_inte_elem)){
//                    continue;
//                }
//
//                if(is_val(curr_inte_elem)){
//
//                    if( !_arcmp(curr_inte_elem, val) ){
//
//                        // push onto val_list
//                        if(is_nil(val_list)){
//                            val_list = _cons(this_pyr, (curr_span_elem+j), nil);
//                        }
//                        else{
//                            _unshift(this_pyr, val_list, (curr_span_elem+j));
//                        }
//
//                    }
//
//                }
//
//            }
//
//        }
//
//    }
//
//    return val_list;
//
//}
//
//
//// XXX The return-value from this function contains unsafe pointers!!! XXX
//// XXX internal interp use ONLY                                        XXX
//// XXX If you pass tag=nil, returns ALL tags in bs                     XXX
//// XXX PERF: A _tags2ar (like _bs2ar) would be more efficient          XXX
////
//mword *array_find_ptr(pyr_cache *this_pyr, mword *bs, mword *ptr){ // array_find_ptr#
//
//    mword *span_array     = _bs2ar(this_pyr, bs);
//    mword size_span_array = size(span_array);
//    mword size_inte;
//    mword *ptr_list       = nil;
//    mword *curr_span_elem;
//    mword *curr_inte_elem;
//
//    int i,j;
//    for(i=0; i<size_span_array; i++){
//
//        curr_span_elem = rdp(span_array,i);
//
//        if(is_ptr(curr_span_elem)){ // check each element
//
//            size_inte = size(curr_span_elem);
//
//            for(j=0;j<size_inte;j++){
//
//                curr_inte_elem = rdp(curr_span_elem,j);
//
//                if(is_nil(curr_inte_elem)){
//                    continue;
//                }
//
//                if(is_ptr(curr_inte_elem)){
//
//                    if( !_arcmp(curr_inte_elem, ptr) ){
//
//                        // push onto ptr_list
//                        if(is_nil(ptr_list)){
//                            ptr_list = _cons(this_pyr, (curr_span_elem+j), nil);
//                        }
//                        else{
//                            _unshift(this_pyr, ptr_list, (curr_span_elem+j));
//                        }
//
//                    }
//
//                }
//
//            }
//
//        }
//
//    }
//
//    return ptr_list;
//
//}

// Clayton Bauman 2016

