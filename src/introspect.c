// introspect.c
//

#include "pyramid.h"
#include "introspect.h"
#include "mem.h"
#include "array.h"

#define INTE_OPEN  "[ptr "
#define INTE_CLOSE "] "
#define LEAF_OPEN  "[val "
#define LEAF_CLOSE "] "
#define TPTR_OPEN  "[tptr "
#define TPTR_CLOSE "] "


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


// Trims trailing whitespace in a leaf8, similar to trunc() but different ;)
// Intended for internal-use ...
void wstrim(pyr_cache *this_pyr, mword *bs){ // wstrim#

    #define WS_CHAR ' '

    mword size8 = array8_size(this_pyr, bs);

    //find last white-space character
    int i = size8-1;
    char *char_bs = (char*)bs;

    while((i >= 0) && (char_bs[i] == ' ')){
        i--;
    }

    //get new mword-size
    mword msize = array8_mword_size(this_pyr, i);

    //calculate, set new alignment-word
    mword alignment_word = array8_enc_align(this_pyr, i);
    ldv(bs,msize-1) = alignment_word;

    //clean-up next-to-last mword
    ldv(bs,msize-2) = rdv(bs,msize-2) & ~alignment_word;

    //trunc the s-field
    sfield(bs) = UNITS_MTO8(msize);

}


// babel-ized sprintf()
//
void bsprintf( pyr_cache *this_pyr, mword *buf, mword *offset, const char *format, ... ){ // bsprintf#

    int printed_length;

    int buf_length = array8_size(this_pyr, buf) 
                            - *offset 
                            + 1; // +1 because vsnprintf() discounts for the null-terminator

    if(buf_length < 0)
        return;

    va_list args;
    va_start(args,format);

    printed_length = vsnprintf((char*)buf+*offset, buf_length, format, args);

    if(printed_length < 0) // encoding error
        return;

    *offset = *offset + printed_length;

    va_end(args);

}


//Returns a string that fully or partially represents the babel-struct
mword *_bs2str(pyr_cache *this_pyr, mword *bs){ // _bs2str#

    mword result_string_length = 
                introspect_calc_buf_length(this_pyr, bs, (1<<10), 65, 23, 33, 68, 140);

    mword *result = _newstr(this_pyr, result_string_length, 0);

    mword offset  = 0;

    _rbs2str(this_pyr, bs, result, &offset);

    _rclean(this_pyr, bs);

    wstrim(this_pyr, result);

    return result;

}


//
//
void _rbs2str(pyr_cache *this_pyr, mword *bs, mword *result, mword *offset){ // _rbs2str#

    int i;

    if( is_traversed_U(bs) ){
        bsprintf(this_pyr, result, offset, " ... ");
        return;
    }

    int num_entries = size(bs);

    if(is_tptr(bs)){ // is_tptr

        if(!is_nil(bs)){ // XXX 140 * NTAG

            bsprintf(this_pyr, result, offset, TPTR_OPEN);

            for(i=0; i<HASH_SIZE; i++){ // XXX 13 * 4 = 52
                bsprintf(this_pyr, result, offset, "0x%08x ", *(mword *)(bs+i));
            }

            mark_traversed_U(bs);

            _rbs2str(this_pyr, (mword *)(bs+HASH_SIZE+1), result, offset);

            bsprintf(this_pyr, result, offset, TPTR_CLOSE);

        }
        else{
            bsprintf(this_pyr, result, offset, " nil ");
        }
    }
    else if(is_ptr(bs)){ // XXX 33 * NIN

        mark_traversed_U(bs);

        bsprintf(this_pyr, result, offset, INTE_OPEN);

        for(i=0; i<num_entries; i++){ // XXX 25
            _rbs2str(this_pyr, (mword *)*(bs+i), result, offset);
        }

        bsprintf(this_pyr, result, offset, INTE_CLOSE);

    }
    else{// if(is_val(bs)){  // XXX 65 * NLF

        mark_traversed_U(bs);

        bsprintf(this_pyr, result, offset, LEAF_OPEN);

        for(i=0; i<num_entries; i++){ // XXX 23 * NVA
            bsprintf(this_pyr, result, offset, "0x%x ", *(mword *)(bs+i));
        }

        bsprintf(this_pyr, result, offset, LEAF_CLOSE);

    }

}


//
//
mword introspect_calc_buf_length(
        pyr_cache *this_pyr,
        mword *bs,
        mword const_padding,
        mword nlf_scaling,
        mword nva_scaling,
        mword nin_scaling,
        mword nptr_scaling,
        mword ntag_scaling){

     return
        ( const_padding
            + _nlf(this_pyr, bs)  * nlf_scaling
            + _nva(this_pyr, bs)  * nva_scaling
            + _nin(this_pyr, bs)  * nin_scaling
            + _nptr(this_pyr, bs) * nptr_scaling
            + _ntag(this_pyr, bs) * ntag_scaling );

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


// Clayton Bauman 2016

