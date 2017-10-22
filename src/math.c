// math.c
//

#include "pyramid.h"
#include "math.h"
#include "introspect.h"
#include "util.h"
#include "mem.h"
#include "array.h"
#include "bstruct.h"

// Add/subtract
// Multiply/Divide
// Remainder



//
//
val math_add(pyr_cache *this_pyr, val a, val b){ // math_add#

    assert(is_val(a));
    assert(is_val(b));

    val swap;

    if(size(b) > size(a)){ // ensure that size_a >= size_b
        swap = a;
        a = b;
        b = swap;
    }

    mword size_a = size(a);
    mword size_b = size(b);

    mword *result = mem_new_valz(this_pyr, size_a+1); // NB: +1 for carry-out, if any

    int i;
    mword sum;
    mword carry=0;

    for(i=0;i<size_a;i++){

        if(i < size_b){

            sum = a[i] + b[i] + carry;

            if((sum < a[i]) || (sum < b[i]))
                carry = 1;
            else
                carry = 0;

        }
        else{

            sum = a[i] + carry;

            if(sum < a[i])
                carry = 1;
            else
                carry = 0;

        }

        result[i] = sum;

    }

    if(carry)
        result[i] = carry;
    else
        array_shrink(this_pyr, result, 0, size_a-1, MWORD_ASIZE);

    return (val)result;

}


// XXX modifies a in-place XXX
//
val math_neg(pyr_cache *this_pyr, val a){ // math_neg#

    assert(is_val(a));

    mword size_a = size(a);

    int i;

    for(i=0;i<size_a;i++){
        a[i] = ~a[i];
    }

    return math_add(this_pyr, a, _val(this_pyr,1));

}


// XXX modifies b in-place XXX
//
val math_sub(pyr_cache *this_pyr, val a, val b){ // math_sub#

    mword *neg_b = math_neg(this_pyr, b);

    mword size_neg_b = size(neg_b);

    val result = math_add(this_pyr, a, neg_b);

    if(size(result) > size_neg_b){ // there was a carry-out, discard it
        array_shrink(this_pyr, result, 0, size_neg_b-1, MWORD_ASIZE);
    }

    return result;

}



//
//
val math_mul(pyr_cache *this_pyr, val a, val b){ // math_mul#

// Lattice-multiplication algorithm; requires O(n^2) time but only O(2n)
// space. There are more efficient methods but they are more difficult to
// implement and verify. Lattice multiplication is far more efficient than
// classroom methods like shift and add and it is fairly quick on modern
// computers for inputs up to a few kilobytes in size.
//
// Multiplying two random n-bit numbers on a desktop Westmere CPU (1 thread):
// 
// bits             time (ms)
// ---------------------------------------------
// 2048             31
// 4096             62
// 8192             171
// 16384            577 
// 32768            2325
// 65536            8954
//
//                                      <----- j
//     MSB
//     2   3   9   5   8   2   3   3
//   +---+---+---+---+---+---+---+---+-
//   |1 /|1 /|4 /|2 /|4 /|1 /|1 /|1 /|            i
//MSB| / | / | / | / | / | / | / | / | 5 MSB      |
// 01|/ 0|/ 5|/ 5|/ 5|/ 0|/ 0|/ 5|/ 5|            |
//   +---+---+---+---+---+---+---+---+-           |
//   |1 /|2 /|7 /|4 /|6 /|1 /|2 /|2 /|            v
//   | / | / | / | / | / | / | / | / | 8
// 02|/ 6|/ 4|/ 2|/ 0|/ 4|/ 6|/ 4|/ 4|
//   +---+---+---+---+---+---+---+---+-
//   |0 /|0 /|2 /|1 /|2 /|0 /|0 /|0 /|
//   | / | / | / | / | / | / | / | / | 3
// 17|/ 6|/ 9|/ 7|/ 5|/ 4|/ 6|/ 9|/ 9|
//   +---+---+---+---+---+---+---+---+-
//   |0 /|0 /|0 /|0 /|0 /|0 /|0 /|0 /|
//   | / | / | / | / | / | / | / | / | 0
// 24|/ 0|/ 0|/ 0|/ 0|/ 0|/ 0|/ 0|/ 0|
//   +---+---+---+---+---+---+---+---+-
//     26  15  13  18  17  13  09  00
//
//
// 01
// 002
// 0017
// 00024
// 000026
// 0000015
// 00000013
// 000000018
// 0000000017
// 00000000013
// 000000000009
// 0000000000000
// -------------
//  139676498390
//
// We want a to be the "wide" operand and b the narrow operand so that we
// utilize more space and perform fewer iterations in the outer loop.
// We still have to make the same number of arithmetic operations either way, 
// but the cache utilization is superior in this arrangement.

    val swap;

    if(size(b) > size(a)){ // ensure that size_a >= size_b
        swap = a;
        a = b;
        b = swap;
    }

    mword size_a = size(a);
    mword size_b = size(b);

    mword hsize_a = hsize(a); // since hword is half the size of mword
    mword hsize_b = hsize(b);

    mword hsize_result = hsize_a+hsize_b; // +1 in case of overflow of most-significant hword

    ptr result    = mem_new_ptr(this_pyr, hsize_result);
    ptr hi_digits = mem_new_ptr(this_pyr, hsize_a);
    ptr lo_digits = mem_new_ptr(this_pyr, hsize_a);

    hword *ha = (hword*)a;
    hword *hb = (hword*)b;

    int i,j;

    for(i=0; i<hsize_result; i++)
        ldp(result,i) = _val(this_pyr, 0);

    // set up partial sums in row product (digit) arrays; initialize to zero
    for(i=0; i<hsize_a; i++){
        ldp(hi_digits,i) = _val(this_pyr, 0);
        ldp(lo_digits,i) = _val(this_pyr, 0);
    }

    mword product;

    for(i=hsize_b-1; i>=0; i--){

        product = ha[0] * hb[i];

        ldv(ldp(hi_digits,0),0) = rdv(rdp(lo_digits,0),0) + HWORD_HI(product);
        ldv(ldp(lo_digits,0),0) = HWORD_LO(product);

        for(j=1; j<hsize_a; j++){ // generate row product

            product = ha[j] * hb[i];

            ldv(ldp(hi_digits,j),0) = rdv(rdp(lo_digits,j),0)   + HWORD_HI(product);
            ldv(ldp(lo_digits,j),0) = rdv(rdp(hi_digits,j-1),0) + HWORD_LO(product);

        }

        // digest row products into result; bstruct_cp() is OK here
        ldv(ldp(result,(hsize_a+i)),0) = rdv(ldp(hi_digits,hsize_a-1),0);
    }

    // finish digesting column products into result
    for(i=0; i<hsize_a; i++){
        ldv(ldp(result,i),0) = rdv(ldp(lo_digits,i),0);
    }

    mword *final_result = mem_new_valz(this_pyr, size_a+size_b);
    hword *hfinal_result = (hword*)final_result;

    hword *curr_product;

    // combine partial sums into final_result
    for(i=0; i<hsize_result; i++){
        curr_product = (hword*)ldp(result,i);
        math_add_hword(this_pyr, (hfinal_result+i), hsize_result-i, curr_product, hsize(curr_product));
    }

    // truncate leading zeros, if any
    i=size_a+size_b;
    while(i>0){
        if(final_result[i-1] != 0)
            break;
        else
            i--;
    }

    if(i<size_a+size_b)
        return array_shrink(this_pyr, final_result, 0, i-1, MWORD_ASIZE);

    return final_result;

}


// a = a + b
// a,b hword aligned, sizes denominated in hwords
//
hword math_add_hword(pyr_cache *this_pyr, hword *a, mword size_a, hword *b, mword size_b){ // math_add_special#

    int i;
    hword sum;
    hword carry=0;

    for(i=0;i<size_a;i++){

        if(i < size_b){

            sum = a[i] + b[i] + carry;

            if((sum < a[i]) || (sum < b[i]))
                carry = 1;
            else
                carry = 0;

        }
        else{

            if(carry==0) break;

            sum = a[i] + carry;

            if(sum < a[i])
                carry = 1;
            else
                carry = 0;

        }

        a[i] = sum;

    }

    if(carry)
        return 1;

    return 0;

}


// provides streamlined handling of running sums for math_mul()
//
mword *math_add_special(pyr_cache *this_pyr, mword *a, mword *b){ // math_add_special#

    return nil;

}


// Clayton Bauman 2017

