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
//      These are native operations, don't use extern library in order to
//      avoid the cost of conversion and because native implementation has
//      superior performance.
//
// Multiply/Divide
//      Multiply is conditionally native or extern library, depending on size
//      of input
//
//      Divide is extern library
//          Note: Divide is Euclidean division (whole quotient + remainder)
//           by default. To get an ordinary division (fractional, base-2),
//           must convert to an inexact number.
//
// Base/string conversion
//      Probably native; copy libtom approach where suitable
//
// Exactness
//      Use Scheme-like construct to store numbers in exact form. An exact
//      number can be converted to inexact form to any desired level of 
//      precision.
//


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

    for(i=0;i<size_a;i++){ // XXX PERF: Get the conditional out of the loop, use two separate loops

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


// High-level mul pseudocode:
// if size_a=1 & size_b=1
// perform uint64_t multiplication and return the result
//
// if size_a*size_b < PYR_LATTICE_MUL_THRESH (64 or so? Run some timing loops to test)
//      perform lattice multiplication
// else if size_a*size_b < PYR_OPTIMIZED_MUL_THRESH (64k or so? Run some timing loops to test)
//      perform optimized multiplication
// else if size_a*size_b < PYR_MAX_MUL_THRESH (test to find a good max thresh)
//      convert a & b to libtom format
//      call mp_mul() in libtommath
//      convert back to Babel format
//      return result
// else
//      give an exception
//
// Also: threading, parallelism


//
//
val math_lattice_mul(pyr_cache *this_pyr, val a, val b){ // math_lattice_mul#

// Lattice-multiplication algorithm; requires O(n^2) time but only O(2n)
// space. There are more efficient methods but they are more difficult to
// implement and verify. Lattice multiplication is far more efficient than
// classroom methods like shift and add and it is fairly quick on modern
// computers for inputs up to several kilobytes in size. For performance
// reasons, this function is hard-limited with an assert() to operand sizes
// whose product is <= 2^28 mwords (256k mwords). For larger operands, a
// more efficient multiplication algorithm should be utilized. 
//
// This function performs no operand-optimization. Leading zeros in the operands
// should be truncated (to reduce pointless multiplications) and power-of-2
// checks should be performed in case the multiplication can be performed by
// a fast left-shift.
//
// Multiplying two random n-bit numbers on a desktop Westmere CPU (1 thread)
// compiled to 32-bit target:
// 
// bits             time (ms)
// ---------------------------------------------
// 32768            32  
// 65536            47
// 131072           172
// 262144           687
// 524288           2793
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
//
// For large arguments, use divide & conquer: C:/ckbauman/pyramid/arch/articles/algorithms/chap2.pdf
//
// Suppose a and b are both n-bit operands
// Divide each operand in two halves: a_L, a_R, b_L, b_R
//
// a*b = (2^(n/2)*a_L + a_R)(2^(n/2)*b_L + b_R) = 2^n * a_L * b_L + 2^(n/2)*(a_L*b_R + a_R*b_L) + a_R*b_R
//
// Calculate a_L*b_L, a_R*b_R, and (a_L+a_R)*(b_L+b_R)
//
// Since a_L*b_R + a_R*b_L = (a_L+a_R)*(b_L+b_R) - a_L*b_L - a_R*a_R
//
// ... we can reconstruct a*b from the three calculated products
//
// Note: these products are recursive, so we can call another instance
// of math_mul_div_conquer() on each of these products until we reach a base-case
// Each time, n is being divided by 2, leading to fewer multiplications.


    val swap;

    if(size(b) > size(a)){ // ensure that size_a >= size_b
        swap = a;
        a = b;
        b = swap;
    }

    mword size_a = size(a);
    mword size_b = size(b);

    assert((size_a * size_b) <= (1<<28)); // For larger numbers, use a more efficient multiplication algorithm

    mword size_result = size_a+size_b;

    dmword *result    = (dmword*)mem_new_valz(this_pyr, 2*size_result);
    dmword *hi_digits = (dmword*)mem_new_valz(this_pyr, 2*size_a);
    dmword *lo_digits = (dmword*)mem_new_valz(this_pyr, 2*size_a);

    int i,j;

    dmword product;

    for(i=size_b-1; i>=0; i--){

        product = (dmword)a[0] * b[i]; // detect b[i] = 0 ... set lo_digits[0]=0 and then go to next iteration

        hi_digits[0] = lo_digits[0] + DMWORD_HI(product);
        lo_digits[0] = DMWORD_LO(product);

        for(j=1; j<size_a; j++){ // generate row product

            product = (dmword)a[j] * b[i];

            hi_digits[j] = lo_digits[j]   + DMWORD_HI(product);
            lo_digits[j] = hi_digits[j-1] + DMWORD_LO(product);

        }

        // digest row products into result
        result[size_a+i] = hi_digits[size_a-1];
    }

//_mem(hi_digits);
//_mem(lo_digits);

    // finish digesting column products into result
    for(i=0; i<size_a; i++){
        result[i] = lo_digits[i];
    }

//_mem(result);

    mword *final_result = mem_new_valz(this_pyr, size_result);

    math_add_special(this_pyr, (mword*)result, final_result);

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
//_mem(final_result);
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
void math_add_special(pyr_cache *this_pyr, mword *in, mword *out){ // math_add_special#

    mword size_out = size(out);
    int i;

    mword last = 0;
    mword carry = 0;

    for(i=0;i<size_out;i++){ // indirections in this loop not optimized

        out[i] = in[i*2] + last + carry;

        if(out[i] < in[i*2]
                ||
           out[i] < last){
            carry = 1;
        }
        else{
            carry = 0;
        }

        last = in[i*2+1]; // gets ignored on final loop

    }

}


// Clayton Bauman 2017

