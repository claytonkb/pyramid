// std.c
//

#include "pyramid.h"
#include "std.h"
#include "mem.h"
#include "array.h"
#include "tptr.h"

// Pyramid-level std-library support. Some primitives are perf-critical and
// must be implemented here. Other std-library primitives can be implemented
// in terms of these perf-optimized primitives.

// array-of-pairs
// paged-array
// etc.
//
// relative-offset accessors
// 


mword *frobnicate(void){

    return nil;

}

//-----------------------------------------------------------------------------
//
//"save stack"
//    stack -> AoP
//
//"load stack"
//    AoP -> stack
//
//NOTE: AoP needs to be able to convert to list array and back
//
//-----------------------------------------------------------------------------
//
//* Babel -> Pyramid conversion
//
//    perl -p -i.bak -e 's/\bBYTE_SIZE\b/UNITS_MTO8/' *
//
//    %s///g
// 
//* Program Input/Conversion
//
//    (1) Parse input: Raw text -> s-expressions
//        <text> sexpr
//    (2) Rewrite terms: s-expressions -> s-expressions
//        <sexpr> <rewrite rules> rewrite
//    (3) Convert to bstruct and apply macros
//        <sexpr> <macro-table> bpdl
//    (4) Brickify and give to core_brick_dispatch
//
//* ptr-array as special form
//
//    If we have a ptr-array whose first element is a tag (or hash-val), we can
//    treat this ptr-array as a "special form", not unlike the Lisp concept as
//    applied to S-expressions. "Brick" is a synonym for this idea of a
//    ptr-array as a special form.
//
//* Bricks (formally-tagged bstructs)
//
//    A brick is a bstruct that is tagged with a tag specifying its formal
//    structure.
//
//        [ptr [tag <brick-tag> nil] ...]
//
//    The exact format of the bstruct depends on the brick-tag. The
//    interp_core_dispatch function always knows what to do with a brick that is
//    defined. For example, suppose we define a brick-tag for the 'over'
//    meta-operation:
//
//        [ptr [tag "/pyramid/tag/over" nil] code operand0 operand1 ... operandn]
//
//    A brick that calls a meta-operation is called an *active brick*.
//
//    For data-type bricks (passive bricks), the interp_core_dispatch will
//    return the function pointers for accessing (reading/writing) the
//    respective brick.
//
//* Tensors
//
//    Pyramid will have built-in support for tensors. (See Nial language)
//    tensor8, tensor1 support (see below, vector/tensor topics)
//
//        [ptr [tag "/pyramid/tag/tensor/ptr"  nil] [val dim0 dim1 ... dimn] [ptr ptr0 ptr1 ... ptrn ] ]
//        [ptr [tag "/pyramid/tag/tensor/val"  nil] [val dim0 dim1 ... dimn] [val val0 val1 ... valn ] ]
//        [ptr [tag "/pyramid/tag/tensor/val8" nil] [val dim0 dim1 ... dimn] [val byte0 byte1 ... byten ... align_word ] ]
//
//* Courses
//
//    A course is a ptr-array that is an aggregrate of bricks (it is also a brick).
//
//        [ptr [tag "/pyramid/tag/course" nil] [tag <brick-tag> nil] brick0 brick1 ... brickn]
//
//    When interp_core_dispatch encounters a course, it recursively calls itself
//    for each brick in that course.
//
//* Pyramids
//
//    A pyramid is a course or any composition of courses. A pyramid can be ragged.
//
//* Variadic pyramids
//
//    Vector/tensor data type cannot be resized (without a new allocation)
//    Variadics allow "growable/shrinkable" vector/tensors
//    For example, instead of {256 newval}, we use {256 newptr} and then 
//        set each element of this array to point to a 1 element val-array,
//        as each element comes available.
//    Variadics are either packed or sparse; a packed-variadic is expected
//        to be filled "left-to-right"; a sparse-variadic can be arbitrarily
//        indexed
//    Variadics can be nested according to any regular dimensional geometry
//        but the leaves are always a value-array, that is, a standard 
//        vector/tensor
//
//* Ncubes
//
//    An ncube is a regular pyramid, i.e. each level in the pyramid must have
//    the same dimension/size (but different levels can have different sizes)
//
//        [ptr [tag "/pyramid/tag/ncube" nil] [val dim0 dim1 ... dimn] elem0 elem1 ... elemn ]
//
//    Unlike a tensor, an ncube cannot be indexed "all-at-once". Indexing occurs
//    via successive lookups.
//
//    A "flat" ncube, or 1-dimensional ncube, has nil in the dimensions entry:
//
//        [ptr [tag "/pyramid/tag/elem/ncube" nil] nil elem0 elem1 ... elemn ]
//
//    Indexing of ncubes is via a val-array, where the 0th element of the
//    val-array indexes along the first dimension, the 1th element along the second
//    dimension, and so on.
//
//        [ptr [tag "/pyramid/tag/ncube" nil] [val 2] elem0  elem1  elem2  elem3  ... elemn ]
//                                                    ^[0 0] ^[0 1] ^[1 0] ^[1 1] ... ^[n/2 1]
//
//    Note that [val 1] is not a valid dimension.
//
//* Ntrees
//
//    An ntree is an ncube where every level is of the same size. It is an n-ary
//    tree.
//
//        [ptr [tag "/pyramid/tag/ntree" nil] [val dim] elem0 elem1 ... elemn ]
//
//    Indexing occurs via successive lookups.
//
//* Vines
//
//    A vine is a flattened n-ary tree. If you have a tree where every node n has
//    exactly k children, then you can place the children of node n at positions
//    k*n+m in the array, where m is between 0 and k-1. A vine can be ptr, array
//    or array8.
//
//        [ptr [tag "/pyramid/tag/vine/ptr"  nil] [val dim] [ptr ptr0 ptr1 ... ptrn ] ]
//        [ptr [tag "/pyramid/tag/vine/val"  nil] [val dim] [val val0 val1 ... valn ] ]
//        [ptr [tag "/pyramid/tag/vine/val8" nil] [val dim] [val byte0 byte1 ... byten ... align_word ] ]
//
//    A vine has dimension 2 or greater. If dimenson is set to 1, the vine is just 
//    a flat-array (synonym for a flat-array).
//
//    Indexing occurs "all-at-once".
//
//* Lists (already implemented, see list.*)
//
//* Array-of-Pairs
//
//    This is a type of ncube:
//
//        [ptr [tag "/pyramid/tag/ncube" nil] [val 2] pair0 pair1 ... pairn ]
//
//    AoP is used by many kinds of meta-operations, including sort, hist,
//    ls2map, map2ls, and so on.
//
//* Gates
//
//    A gate is a singleton object - all access to the bstruct behind the
//    gate must go through the gate; no direct references allowed. Use 
//    gate_read() and gate_write() to perform accesses to a gated bstruct.
//
//    Gates are tracked in a special list. Accesses must go through this list
//    (every time, no stored pointers). Cptrs are bounded to the gc_mem arrays
//    and, thus, cannot point into a gated memory area.
//
//Courses, pyramids, ncubes and ntrees must be tree-like in respect
//to one another (though not to their payloads) - that is, one level of a
//pyramid (for example) must not point to another level, and so on.
//
//* Bales
//
//    A bale is a packed data structure stored in a value-array. Bales can be 
//    stuffed or unstuffed (similar to Perl's pack/unpack). Array8 and array1
//    should not be confused with bales.
//
//* Hash-table, Namespace & Directory
//
//   See babel/arch_notes.txt 
//
//* Vector/Tensor Topics
//
//    - binary heaps
//        http://www.cs.ucf.edu/~dmarino/ucf/cop3502/sampleprogs/heap.c
//        http://courses.csail.mit.edu/6.006/fall10/handouts/recitation10-8.pdf
//        http://www.personal.kent.edu/~rmuhamma/Algorithms/MyAlgorithms/Sorting/heapSort.htm
//
//    - Byte-Pair Encoding
//        This may be a good candidate if it can be implemented with reliably 
//            fast compression time.
//        http://www.csse.monash.edu.au/cluster/RJK/Compress/problem.html
//        Paper: "Byte pair encoding: a text compression scheme that accelerates pattern matching"
//
//    - String-matching
//
//        string_ascii_char_match (fast scan for a character)
//        string_ascii_char_match_all (fast-scan for all chars, e.g. "/", " ", "\n", etc.)
//        string_ascii_str_match (first match only; returns offset)
//        string_ascii_str_match_all (returns list of offsets)
//        array_match      --> general-purpose matching, i.e. UTF8 strings; also rooted sub-graphs
//        array_match_all
//
//        http://igm.univ-mlv.fr/~lecroq/string/index.html
//
//        Current favorite: Boyer-Moore Horspool
//            Very fast + simple + good worst-case
//
//    - Vector/tensor processing
//
//        Why vectors/tensors:
//            Very fast in CPU/GPU
//            Eliminate pointer-manipulation - O(1) lookup for every access
//            Uses:
//                Multi-media (images, audio, video, etc.)
//                Multi-precision arithmetic/averaging
//                Histograms/probabilities/entropies
//                Parallelism
//                    http://research.cs.washington.edu/zpl/comicbook/zcbp1.html
//            Non-uses:
//                Ragged/irregular-shaped data
//                Dynamically-resizable data (unless chunked/blocked)
//                Very small data objects in isolation, such as characters or bits
//
//        Implement after support for large pages + GC-max-alloc-page-size
//        https://gcc.gnu.org/onlinedocs/gcc-4.2.4/gcc/X86-Built_002din-Functions.html
//        https://www.youtube.com/watch?v=6Q8_kYbTqhY&list=PLLcTTp6EQ_egylIerYEjCBbEVhnPzSdXP
//        https://software.intel.com/sites/landingpage/IntrinsicsGuide/#
//
//        Key interface between vector and normal modes: scatter and gather
//
//            rows --> matrix
//            [a a a] [b b b] [c c c] [d d d] --> [a a a b b b c c c d d d]
//            matrix --> rows
//            [a a a b b b c c c d d d] --> [a a a] [b b b] [c c c] [d d d]
//
//            matrix(interleaved) --> rows
//            [a b c d a b c d a b c d] --> [a a a] [b b b] [c c c] [d d d]
//            rows --> matrix(interleaved) 
//            [a a a] [b b b] [c c c] [d d d] --> [a b c d a b c d a b c d]
//
//            rows(interleaved) --> matrix
//            [a b c d] [a b c d] [a b c d] --> [a a a b b b c c c d d d]
//            matrix --> rows(interleaved) 
//            [a b c d] [a b c d] [a b c d] --> [a a a b b b c c c d d d]
//
//            Transpose orientation as interleaving:
//
//            1  2  3      1  4  7  10
//            4  5  6  ==> 2  5  8  11
//            7  8  9      3  6  9  12
//            10 11 12 
//
//            4,3   col-size,row-size
//            [a a a b b b c c c d d d]    [1 2 3 4 5 6 7 8 9 10 11 12]
//                                          ^ ^ ^
//            3,4
//            [a a a a b b b b c c c c]
//
//            3,4 T
//            [a b c d a b c d a b c d]
//
//            4,3 T
//            [a b c a b c a b c a b c]    [1 2 3 4 5 6 7 8 9 10 11 12]
//                                          ^     ^     ^     ^
//            2,4,3
//            [a a a b b b c c c d d d e e e f f f g g g h h h]
//                multi-dim transpose? (permutation/rotation of dim vector)

//                             +-------------+
//           gc_mem         +->| page0       |
//        +-----------+     |  +-------------+
//        |           |-----+  
//        +-----------+        +-------------+
//        |           |------->| page1       |
//        +-----------+        +-------------+
//        |           |-----+
//        +-----------+     |  +-------------+
//        |           |--+  +->| page2       |
//        +-----------+  |     +-------------+
//              .        |
//              .        |     +-------------+
//              .        +---->| page3       |
//                             +-------------+
//        +-----------+              .
//        |           |--+           .
//        +-----------+  |           .
//                       |
//                       |     +-------------+
//                       +---->| pageK       |
//                             +-------------+
//
// K x page_size -> Ncube


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
            bs = tptr_detag(this_pyr, bs);
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

