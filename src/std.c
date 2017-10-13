// std.c
//

#include "pyramid.h"
#include "std.h"
#include "mem.h"
#include "array.h"
#include "tptr.h"
#include "mt19937ar.h"

// Pyramid-level std-library support. Some primitives are perf-critical and
// must be implemented here. Other std-library primitives can be implemented
// in terms of these perf-optimized primitives.

// paged-array
// etc.
//
// relative-offset accessors

// array-of-pairs
// Conversion operators
//    ns>aop          sap>aop
//    aop>ns          aop>sap
//            
//    dir>aop         map>aop
//    aop>dir         aop>map
//            
//    lut>aop         heap>aop
//    aop>lut         aop>heap
//            
//    hist>aop        ptr>aop
//    aop>hist        aop>ptr_key
//                    aop>ptr_val
//
//    enum>aop        val>aop
//    aop>enum        aop>val
//
//    ls>aop          mat>aop  
//    aop>ls          aop>mat


//
//
mword std_time_ms(void){ // std_time_ms#

#ifdef PYRAMID_WINDOWS_BUILD 
    DWORD raw_time = GetTickCount();
    return (mword)raw_time;
#endif

}


//
//
val std_genrand32(pyr_cache *this_pyr, mword num_mwords){ // std_genrand32#

    val result = mem_new_valz(this_pyr, num_mwords);

    int i;

    for(i=0; i<num_mwords; i++){
        result[i] = (mword)genrand_int32();
    }

//    mword tempA;
//    mword tempB;
//
//    for(i=0; i<num_mwords; i++){
//        tempA = (mword)genrand_int32();
//        tempB = (mword)genrand_int32();
//        result[i] = 
//               (pearson16_byte_perm[tempA & 0x000000ff] ^ (tempB & 0x000000ff))
//            || (pearson16_byte_perm[tempA & 0x0000ff00] ^ (tempB & 0x0000ff00))
//            || (pearson16_byte_perm[tempA & 0x00ff0000] ^ (tempB & 0x00ff0000))
//            || (pearson16_byte_perm[tempA & 0xff000000] ^ (tempB & 0xff000000));
//    }

    return result;

}

// std_shift_paged_array
// std_unshift_paged_array

//
//
void std_resize_paged_array(pyr_cache *this_pyr, mword *pa, int new_sfield){ // std_resize_paged_array#

    mword page_size   = vcar(pgsize_pa(pa));
    mword *pages      = pages_pa(pa);
    mword num_pages   = size(pages);

    mword mword_new_sfield = UNITS_8TOM(abs(new_sfield));
    mword new_num_pages = mword_new_sfield / page_size;
    if((mword_new_sfield % page_size) != 0) new_num_pages++;

    mword *new_pages = pages;

    int i;

    if(new_num_pages != num_pages){
        new_pages = mem_new_ptr(this_pyr, new_num_pages);
//        new_num_pages = MIN(num_pages, new_num_pages);
        if(is_val_sfield(new_sfield)){
            for(i=0; i<new_num_pages; i++){
                if(i<num_pages){
                    ldp(new_pages,i) = rdp(pages,i);
                }
                else{
                    ldp(new_pages,i) = mem_new_valz(this_pyr, page_size);
                }
            }
        }
        else{
            for(i=0; i<new_num_pages; i++){
                if(i<num_pages){
                    ldp(new_pages,i) = rdp(pages,i);
                }
                else{
                    ldp(new_pages,i) = mem_new_ptr(this_pyr, page_size);
                }
            }
        }
    }

    ldp(pa,2) = new_pages;
    *(sfield_pa(pa)) = new_sfield;

}


//
//
mword *std_new_paged_array(pyr_cache *this_pyr, mword page_size, int init_sfield){ // std_new_paged_array#

    mword mword_init_sfield = UNITS_8TOM(abs(init_sfield));

    mword num_pages =  mword_init_sfield / page_size;
    if((mword_init_sfield % page_size) != 0) num_pages++;

    if(!num_pages) return nil;

    mword *arrays   = mem_new_ptr(this_pyr, num_pages);
    mword i;

    if(init_sfield > 0){ // val-array
        for(i=0; i<num_pages; i++){
            ldp(arrays,i) = mem_new_valz(this_pyr, page_size);
        }
    }
    else if(init_sfield < 0){ // ptr-array
        for(i=0; i<num_pages; i++){
            ldp(arrays,i) = mem_new_ptr(this_pyr, page_size);
        }
    }
    else{
        _fatal("cannot allocate tptr paged-array");
    }

    mword *pa = mem_new_ptr(this_pyr, 3);

    ldp(pa,0) = _val(this_pyr, (mword)init_sfield);
    ldp(pa,1) = _val(this_pyr, page_size);
    ldp(pa,2) = arrays;

    return pa;

}


//
//
mword *std_read_with_pbp(pyr_cache *this_pyr, mword *bs, mword *pbp){ // std_read_with_pbp#

    int i;
    mword pbp_size = size(pbp);

    for(i=0;i<pbp_size;i++){
        if(is_tptr(bs)){
//            bs = tptr_detag(this_pyr, bs); // XXX check for built-ins?? XXX
            bs = tcar(bs);
        }
        if(is_ptr(bs)){
            bs = rdp(bs,rdv(pbp,i));
        }
        else{ // is_val(bs)
            if(i<pbp_size-1){
                bs = array_slice(this_pyr, bs, rdv(pbp,i), rdv(pbp,i+1));
                break;
            }
            else{
                bs = _val(this_pyr, rdv(bs, rdv(pbp, i)));
                break;
            }
        }
    }

    return bs;

}


// X [a b c] Y th_wmd
// X nil     Y th_wmd  --> overwrites Y into X at offset 0, eqiv. to:
// X [0]     Y th_wmd
// 
// Y    -> X
// leaf -> leaf (range?)
// inte -> inte
//
mword std_write_with_pbp(pyr_cache *this_pyr, mword *bs, mword *pbp, mword *payload){ // std_write_with_pbp#

    int i;

    mword pbp_size = size(pbp);
    mword bs_index = 0;

    for(i=0;i<pbp_size;i++){
        if(is_tptr(bs)){
            bs = tptr_detag(this_pyr, bs);
        }
        if(is_ptr(bs)){
            if(i<pbp_size-1){
                bs = rdp(bs,rdv(pbp,i));
            }
            else{
                bs_index = rdv(pbp,i);
                break;
            }
        }
        else{ // is_val(bs)
            bs_index = rdv(pbp,i);
            break;
        }
    }

    if( is_ptr(bs) ){

        ldp(bs,bs_index) = payload;

    }
    else if( is_val(bs)
            && is_val(payload) ){

        //void    move(mword *dest, mword dest_index, mword *src, mword src_index, mword size);
        array_move(this_pyr, bs, bs_index, payload, 0, size(payload), MWORD_ASIZE);

    }
    else{
        _die;
        return 0;

    }

    return 1;

}




// Clayton Bauman 2017

