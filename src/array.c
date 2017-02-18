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

//    final_size = size_arg;

    if(access_size == MWORD_ASIZE){

        memmove( dest+dest_index, src+src_index, (size_t)UNITS_MTO8(final_size) );

    }
    else{ // access_size = BYTE_ASIZE

        memmove( ((char*)dest+dest_index), ((char*)src+src_index), (size_t)final_size );

    }

}


// comp --> "complementary modulus", i.e. comp(modulus) = (MWORD_BIT_SIZE - modulus)
// XXX dest_begin and size_arg are NOT safety-checked before they are used XXX
//
void array1_move(pyr_cache *this_pyr, mword *dest, mword dest_begin, mword *src, mword size_arg){ // array1_move#

    if((src == dest)        // src and dest must not overlap
           || (!size_arg))  // nothing to move
        return;

    if(    (  dest_begin % BITS_PER_BYTE == 0)
        && (  size_arg % BITS_PER_BYTE == 0) ){
        memmove( ((char*)dest+UNITS_1TO8(dest_begin)), (char*)src, (size_t)UNITS_1TO8(size_arg) );
        return;
    }

    mword dest_begin_mword = UNITS_1TOM(dest_begin);

    dest+=dest_begin_mword;
    dest_begin -= UNITS_MTO1(dest_begin_mword);

    mword dest_begin_comp = MWORD_BIT_SIZE-dest_begin;
    mword dest_end = dest_begin+size_arg;
    mword dest_end_mod = dest_end % MWORD_BIT_SIZE;

//_dd(dest_begin);
//_dd(dest_begin_comp);
//_dd(dest_end_mod);
//_dd(dest_end_comp);

    mword num_dest_boundaries_crossed = 0;

    if(size_arg <= MWORD_BIT_SIZE){ // 0 or 1 boundaries crossed
        num_dest_boundaries_crossed = (dest_begin_comp < size_arg);
    }
    else if(size_arg < (2*MWORD_BIT_SIZE)){
        if((size_arg+dest_begin) <= (2*MWORD_BIT_SIZE)){
            num_dest_boundaries_crossed = 1;
        }
        else{
            num_dest_boundaries_crossed = 2;
        }
    }
    else if(size_arg == (2*MWORD_BIT_SIZE)){
        if( !dest_begin && !dest_end_mod ){
            num_dest_boundaries_crossed = 1;
        }
        else{
            num_dest_boundaries_crossed = 2;
        }
    }
    else{ // 2 or more boundaries crossed
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
void array1_move_single(pyr_cache *this_pyr, mword *dest, mword dest_mod, mword *src, mword size_arg){ // array1_move_single#

    ldv(dest,0) = MWORD_MUX(    
                        rdv(src,0) << dest_mod, 
                        rdv(dest,0), 
                        NBIT_LO_MASK(size_arg) << dest_mod );

//mword result = ldv(dest,0);
//_d(result);

}


//
//
void array1_move_double(pyr_cache *this_pyr, mword *dest, mword dest_mod, mword *src, mword size_arg){ // array1_move_double#

    // |----------------|----------------|
    //       |-----------------|
    // |.....^..........^......^.........|
    //            B        A

    mword dest_comp = MWORD_BIT_SIZE-dest_mod;
    mword size_argA, size_argB, size_argC;
    mword read_maskA; //, read_maskB, read_maskC;
    mword write_maskA, write_maskB, write_maskC;
    mword src_val0;

    if(size_arg <= MWORD_BIT_SIZE){

        size_argA = dest_comp;
        size_argB = size_arg-size_argA;

        read_maskA  = NBIT_LO_MASK(size_argA);

        write_maskA = read_maskA << dest_mod;
        write_maskB = NBIT_LO_MASK(size_argB);

        src_val0 = rdv(src,0);

        ldv(dest,0) = MWORD_MUX(src_val0 << dest_mod,  rdv(dest,0), write_maskA);
        ldv(dest,1) = MWORD_MUX(src_val0 >> dest_comp, rdv(dest,1), write_maskB);

//mword result0 = ldv(dest,0);
//mword result1 = ldv(dest,1);
//
//_d(result0);
//_d(result1);

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

        write_maskA = read_maskA << dest_mod;
        write_maskB = NBIT_LO_MASK(size_argB);
        write_maskC = NBIT_LO_MASK(size_argC) << size_argB;

        src_val0 = rdv(src,0);

        ldv(dest,0) = MWORD_MUX(src_val0 << dest_mod, rdv(dest,0), write_maskA);
        ldv(dest,1) = MWORD_MUX(src_val0 >> size_argA, rdv(dest,1), write_maskB);
        ldv(dest,1) = MWORD_MUX(rdv(src,1) << size_argB, rdv(dest,1), write_maskC);

//mword result0 = ldv(dest,0);
//mword result1 = ldv(dest,1);
//
//_d(result0);
//_d(result1);

    }

//    mword result = ldv(dest,0);
//    _d(result);

}


// prereq: dest_begin < MWORD_BIT_SIZE
//
void array1_move_n(pyr_cache *this_pyr, mword *dest, mword dest_mod, mword *src, mword size_arg){ // array1_move_n#

    mword dest_comp = MWORD_BIT_SIZE-dest_mod;

    mword read_maskA  = NBIT_LO_MASK(dest_mod);
    mword read_maskB  = ~read_maskA;

    mword dest_end = dest_mod + size_arg;
    mword dest_end_mod = dest_end % MWORD_BIT_SIZE;
    mword dest_mword_count = UNITS_1TOM((dest_end-dest_end_mod)) - 1;


    ldv(dest,0) = MWORD_MUX(rdv(src,0) << dest_mod, rdv(dest,0), read_maskB);

    int i;

    for(i=0; i<dest_mword_count; i++){
        ldv(dest,i+1) = MWORD_MUX(rdv(src,i)   >> dest_comp, rdv(dest,i+1), read_maskA);
        ldv(dest,i+1) = MWORD_MUX(rdv(src,i+1) << dest_mod,  rdv(dest,i+1), read_maskB);
    }

    if(dest_end_mod){ // dest is aligned; src is unaligned

        if(dest_end_mod > dest_mod){

            read_maskA = NBIT_LO_MASK(dest_mod);
            read_maskB = NBIT_LO_MASK(dest_end_mod-dest_mod) << dest_mod;

            ldv(dest,i+1) = MWORD_MUX(rdv(src,i)   >> dest_comp, rdv(dest,i+1), read_maskA);
            ldv(dest,i+1) = MWORD_MUX(rdv(src,i+1) << dest_mod,  rdv(dest,i+1), read_maskB);

        }
        else{

            read_maskA = NBIT_LO_MASK(dest_end_mod);
            ldv(dest,i+1) = MWORD_MUX(rdv(src,i) >> dest_comp, rdv(dest,i+1), read_maskA);

        }

    }
//    else{ // XXX DEBUG ONLY; REMOVE XXX
//_trace;
//   }

//    mword result0 = ldv(dest,0);
//    mword result1 = ldv(dest,1);
//    mword result2 = ldv(dest,2);
//    mword result3 = ldv(dest,3);
//
//    _d(result0);
//    _d(result1);
//    _d(result2);
//    _d(result3);

}


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
 

// dest aligned, src un-aligned
//
mword *array1_slice(pyr_cache *this_pyr, mword *array, mword start, mword end){ // array1_slice#

    mword *result=nil;
    return result;

}


// prereqs: 
//      src_mod  <  MWORD_BIT_SIZE
//      size_arg <= MWORD_BIT_SIZE-src_mod
//
void array1_slice_single(pyr_cache *this_pyr, mword *dest, mword *src, mword src_mod, mword size_arg){ // array1_slice_single#

    ldv(dest,0) = MWORD_MUX(
                        rdv(src,0) >> src_mod, 
                        rdv(dest,0), 
                        NBIT_LO_MASK(size_arg) );

}

//    ldv(dest,0) = MWORD_MUX(    
//                        rdv(src,0) << dest_mod, 
//                        rdv(dest,0), 
//                        NBIT_LO_MASK(size_arg) << dest_mod );

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

