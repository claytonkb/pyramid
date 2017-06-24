// array.c
//

#include "pyramid.h"
#include "array.h"
#include "mem.h"
#include "utf8.h"
#include "bstruct.h"
#include "list.h"

//#include "operator.h"
//#include "pbp.h"
//#include "introspect.h"
//#include "string.h"
//#include "lib_babel.h"
//#include "logic.h"

#define  key_aopv(x,y) vcar(pcar(rdp(x,y)))
#define data_aopv(x,y) vcar(pcar(rdp(x,y)))

#define  key_aop(x,y) pcar(rdp(x,y))
#define data_aop(x,y) pcar(rdp(x,y))


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


// FIXME The following functions do not need pyr_cache* !!! FIXME

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


#define array_cmp_size_check(x,y)       \
    if(x > y){                          \
        return 1;                       \
    }                                   \
    else if(x < y){                     \
        return -1;                      \
    }
    

//
//
int array_cmp_lex(pyr_cache *this_pyr, mword *left, mword *right, access_size_sel access_size){ // array_cmp_lex#

    mword left_size;
    mword right_size;

    if(access_size == MWORD_ASIZE){
        left_size  = UNITS_MTO8(size_special(left));
        right_size = UNITS_MTO8(size_special(right));
//        left_size  = UNITS_MTO8(size(left));
//        right_size = UNITS_MTO8(size(right));
    }
    else{ // access_size == BYTE_ASIZE
        left_size  = array8_size(this_pyr, left);
        right_size = array8_size(this_pyr, right);
    }

//    array_cmp_size_check(left_size, right_size);

    if(left_size > right_size){
        return 1;
    }
    else if(left_size < right_size){
        return -1;
    }

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


// Multi-word numerical comparison
//
int array_cmp_num(mword *left, mword *right){ // array_cmp_num#

    mword acc = 0;
    mword left_size  = size(left);
    mword right_size = size(right);

    int i = left_size-1;

    if(left_size > right_size){
        for(i=left_size-1; i>=right_size; i--){ // Ignore leading zeroes...
            acc += (rdp(left,i) != 0);
        }
        if(acc != 0){
            return 1;
        }
    }
    else if(left_size < right_size){
        for(i=right_size-1; i>=left_size; i--){ // Ignore leading zeroes...
            acc += (rdp(right,i) != 0);
        }
        if(acc != 0){
            return -1;
        }
    }

    for(;i>=0;i--){
        if(rdp(left,i) > rdp(right,i)){ 
            return 1;
        }
        if(rdp(left,i) < rdp(right,i)){ 
            return -1;
        }
    }

    return 0; // They are numerically equal

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

// ETC:

// In-place mergesort of an array
// st can be one of: {UNSIGNED, SIGNED, ALPHA_MWORD, ALPHA_BYTE, LEX_MWORD, LEX_BYTE, VAL, CUSTOM}
//
void array_sort(pyr_cache *this_pyr, mword *array, sort_type st){ // array_sort#

    if(is_val(array) || (st == VAL)){ // ((st == VAL) && (!is_val(array))) means "sort ptr-array as a val-array"
_trace;
        array_sort_r(this_pyr, 0, size(array), array, st);
    }
    else{
_trace;
        array_sort_aop(this_pyr, array, 0, st);
    }

}

#define merge_pre_sort                                                      \
    mword left_length  = left_end  - left_start;                            \
    mword right_length = right_end - right_start;                           \
                                                                            \
    mword *left_half  = mem_new_val(this_pyr,left_length,0);                \
    mword *right_half = mem_new_val(this_pyr,right_length,0);               \
                                                                            \
    mword r = 0;                                                            \
    mword l = 0;                                                            \
    mword i = 0;                                                            \
                                                                            \
    for(i = left_start; i<left_end; i++, l++){                              \
        left_half[l] = array[i];                                            \
    }                                                                       \
                                                                            \
    for(i = right_start; i<right_end; i++, r++){                            \
        right_half[r] = array[i];                                           \
    }

#define merge_post_sort                                                     \
    for(; l<left_length; i++, l++){                                         \
        array[i] = left_half[l];                                            \
    }                                                                       \
                                                                            \
    for (; r<right_length; i++, r++){                                       \
        array[i] = right_half[r];                                           \
    }                                                                       \
                                                                            \


//
//
void array_sort_r(pyr_cache *this_pyr, mword left, mword right, mword *array, sort_type st){ // array_sort_r#

    // base case, array is already sorted
    if (right - left <= 1){
        return;
    }

    // set up bounds to slice array into
    mword left_start  = left;
    mword left_end    = (left+right)/2;
    mword right_start = left_end;
    mword right_end   = right;

    // sort left half
    array_sort_r(this_pyr, left_start, left_end, array, st);

    // sort right half
    array_sort_r(this_pyr, right_start, right_end, array, st);

    // merge sorted halves back together:
    merge_pre_sort;

    // merge left_half and right_half back into array
    for(i=left_start, r=0, l=0; l<left_length && r<right_length; i++){
        if(left_half[l] < right_half[r]){
            array[i] = left_half[l++];
        }
        else{
            array[i] = right_half[r++];
        }
    }

    merge_post_sort;

}


//array_sort_aop(this_pyr, size(array), array, st);
void array_sort_aop(pyr_cache *this_pyr, mword *array, mword key_index, sort_type st){ // array_sort_aop#

    if(st == UNSIGNED){
        array_sort_aop_unsigned(this_pyr, 0, size(array), array, key_index);
    }
    else if(st == SIGNED){
        array_sort_aop_signed(this_pyr, 0, size(array), array, key_index);
    }
    else if(st == ALPHA_MWORD){
        array_sort_aop_alpha_mword(this_pyr, 0, size(array), array, key_index);
    }
    else if(st == ALPHA_BYTE){
        array_sort_aop_alpha_byte(this_pyr, 0, size(array), array, key_index);
    }
    else if(st == LEX_MWORD){
        array_sort_aop_lex_mword(this_pyr, 0, size(array), array, key_index);
    }
    else if(st == LEX_BYTE){
        array_sort_aop_lex_byte(this_pyr, 0, size(array), array, key_index);
    }
    else{
        _pigs_fly;
    }


}


//
//
void array_sort_aop_unsigned(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index){ // array_sort_aop_unsigned#

    // base case, array is already sorted
    if (right - left <= 1){
        return;
    }

    // set up bounds to slice array into
    mword left_start  = left;
    mword left_end    = (left+right)/2;
    mword right_start = left_end;
    mword right_end   = right;

    // sort left half
    array_sort_aop_unsigned(this_pyr, left_start, left_end, array, key_index);

    // sort right half
    array_sort_aop_unsigned(this_pyr, right_start, right_end, array, key_index);

    // merge sorted halves back together:
    merge_pre_sort;

    // merge left_half and right_half back into array
    for(i=left_start, r=0, l=0; l<left_length && r<right_length; i++){
        if(val_lt(key_aop(left_half,l), key_aop(right_half,r))){ // numeric
            array[i] = left_half[l++];
        }
        else{
            array[i] = right_half[r++];
        }
    }

    merge_post_sort;

}


//
//
void array_sort_aop_signed(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index){ // array_sort_aop_signed#

    // base case, array is already sorted
    if (right - left <= 1){
        return;
    }

    // set up bounds to slice array into
    mword left_start  = left;
    mword left_end    = (left+right)/2;
    mword right_start = left_end;
    mword right_end   = right;

    // sort left half
    array_sort_aop_signed(this_pyr, left_start, left_end, array, key_index);

    // sort right half
    array_sort_aop_signed(this_pyr, right_start, right_end, array, key_index);

    // merge sorted halves back together:
    merge_pre_sort;

    // merge left_half and right_half back into array
    for(i=left_start, r=0, l=0; l<left_length && r<right_length; i++){
        if((array_cmp_num_signed(key_aop(left_half,l), key_aop(right_half,r)) < 0)){
            array[i] = left_half[l++];
        }
        else{
            array[i] = right_half[r++];
        }
    }

    merge_post_sort;

}


//
//
void array_sort_aop_alpha_mword(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index){ // array_sort_aop_alpha_mword#

    // base case, array is already sorted
    if (right - left <= 1){
        return;
    }

    // set up bounds to slice array into
    mword left_start  = left;
    mword left_end    = (left+right)/2;
    mword right_start = left_end;
    mword right_end   = right;

    // sort left half
    array_sort_aop_alpha_mword(this_pyr, left_start, left_end, array, key_index);

    // sort right half
    array_sort_aop_alpha_mword(this_pyr, right_start, right_end, array, key_index);

    // merge sorted halves back together:
    merge_pre_sort;

    for(i=left_start, r=0, l=0; l<left_length && r<right_length; i++){
        if((array_cmp_alpha(this_pyr, key_aop(left_half,l), key_aop(right_half,r), MWORD_ASIZE) < 0)){
            array[i] = left_half[l++];
        }
        else{
            array[i] = right_half[r++];
        }
    }

    merge_post_sort;

}


//
//
void array_sort_aop_alpha_byte(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index){ // array_sort_aop_alpha_byte#

    // base case, array is already sorted
    if (right - left <= 1){
        return;
    }

    // set up bounds to slice array into
    mword left_start  = left;
    mword left_end    = (left+right)/2;
    mword right_start = left_end;
    mword right_end   = right;

    // sort left half
    array_sort_aop_alpha_byte(this_pyr, left_start, left_end, array, key_index);

    // sort right half
    array_sort_aop_alpha_byte(this_pyr, right_start, right_end, array, key_index);

    // merge sorted halves back together:
    merge_pre_sort;

    for(i=left_start, r=0, l=0; l<left_length && r<right_length; i++){
        if((array_cmp_alpha(this_pyr, key_aop(left_half,l), key_aop(right_half,r), BYTE_ASIZE) < 0)){
            array[i] = left_half[l++];
        }
        else{
            array[i] = right_half[r++];
        }
    }

    merge_post_sort;

}



//
//
void array_sort_aop_lex_mword(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index){ // array_sort_aop_lex_mword#

    // base case, array is already sorted
    if (right - left <= 1){
        return;
    }

    // set up bounds to slice array into
    mword left_start  = left;
    mword left_end    = (left+right)/2;
    mword right_start = left_end;
    mword right_end   = right;

    // sort left half
    array_sort_aop_lex_mword(this_pyr, left_start, left_end, array, key_index);

    // sort right half
    array_sort_aop_lex_mword(this_pyr, right_start, right_end, array, key_index);

    // merge sorted halves back together:
    merge_pre_sort;

    for(i=left_start, r=0, l=0; l<left_length && r<right_length; i++){
        if((array_cmp_lex(this_pyr, key_aop(left_half,l), key_aop(right_half,r), MWORD_ASIZE) < 0)){
            array[i] = left_half[l++];
        }
        else{
            array[i] = right_half[r++];
        }
    }

    merge_post_sort;

}


//
//
void array_sort_aop_lex_byte(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index){ // array_sort_aop_lex_byte#

    // base case, array is already sorted
    if (right - left <= 1){
        return;
    }

    // set up bounds to slice array into
    mword left_start  = left;
    mword left_end    = (left+right)/2;
    mword right_start = left_end;
    mword right_end   = right;

    // sort left half
    array_sort_aop_lex_byte(this_pyr, left_start, left_end, array, key_index);

    // sort right half
    array_sort_aop_lex_byte(this_pyr, right_start, right_end, array, key_index);

    // merge sorted halves back together:
    merge_pre_sort;

    for(i=left_start, r=0, l=0; l<left_length && r<right_length; i++){
        if((array_cmp_lex(this_pyr, key_aop(left_half,l), key_aop(right_half,r), BYTE_ASIZE) < 0)){
            array[i] = left_half[l++];
        }
        else{
            array[i] = right_half[r++];
        }
    }

    merge_post_sort;

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
    //tptrs can't be trunc'd

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

    //free'd below
    char *temp_buffer = mem_sys_alloc( temp_buffer_size );
    
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


#define ARRAY_LINEAR_THRESH 2

// array must be in sorted order (non-decreasing)
// XXX smoke-tested ONLY XXX
//
//mword array_search_rewrite(pyr_cache *this_pyr, mword *array, mword *target, sort_type st){ // array_search_rewrite#
mword array_search(pyr_cache *this_pyr, mword *array, mword *target, sort_type st){ // array_search#

    int array_size = size(array);

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

