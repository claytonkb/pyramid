// introspect.c
//

#include "pyramid.h"
#include "introspect.h"
#include "mem.h"
#include "array.h"
#include "bstruct.h"
#include "string.h"

#define INTE_OPEN  "[ptr "
#define INTE_CLOSE "] "
#define LEAF_OPEN  "[val "
#define LEAF_CLOSE "] "
#define TPTR_OPEN  "[tptr "
#define TPTR_CLOSE "] "


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


//Returns a string containing the Graphviz text file
//
mword *_bs2gv(pyr_cache *this_pyr, mword *bs){ // _bs2gv#

    mword result_string_length = 
                introspect_calc_buf_length(this_pyr, bs, (1<<10), 65, 23, 33, 68, 140);

    mword *result = _newstr(this_pyr, result_string_length, 0);

    mword offset  = 0;

    bsprintf(this_pyr, result, &offset, "digraph babel {\nnode [shape=record];\n");
    bsprintf(this_pyr, result, &offset, "graph [rankdir = \"LR\"];\n");

    _rbs2gv(this_pyr, bs, result, &offset, 1);

    _rclean(this_pyr, bs);

    bsprintf(this_pyr, result, &offset, "}\n");

    wstrim(this_pyr, result);

    return result;

}


//
//
void _rbs2gv(pyr_cache *this_pyr, mword *bs, mword *result, mword *offset, int initial){ // _rbs2gv#

    int i;
    int clipped=0;

    if( is_traversed_U(bs) ){
        return;
    }

    int num_entries = size(bs);

    if(is_tptr(bs)){ // is_tptr

        if(!is_nil(bs)){ // XXX 140 * NTAG

            bsprintf(this_pyr, result, offset, "s%08x [shape=record,label=\"", (mword)bs); // XXX 31

            for(i=0; i<HASH_SIZE; i++){ // XXX 13 * 4 = 52

                bsprintf(this_pyr, result, offset, "<f%d> %x", i, *(mword *)(bs+i)); // XXX 12

                if(i<(HASH_SIZE-1)){
                    bsprintf(this_pyr, result, offset, "|"); // XXX 1
                }

            }

            bsprintf(this_pyr, result, offset, "\"];\n"); // XXX 4

            bsprintf(this_pyr, result, offset,
                        "\"s%08x\":f0 -> \"s%08x\":f0 [arrowhead=\"none\"];\n", // XXX 53
                        (mword)bs, 
                        (mword)(bs+HASH_SIZE+1));

            mark_traversed_U(bs);

            _rbs2gv(this_pyr, (mword *)(bs+HASH_SIZE+1), result, offset, 0);

        }
    }
    else if(is_ptr(bs)){ // XXX 33 * NIN

        mark_traversed_U(bs);

        bsprintf(this_pyr, result, offset, "\"s%08x\" [shape=record,label=\"", (mword)bs); // XXX 33

        for(i=0; i<num_entries; i++){ // XXX 25

            bsprintf(this_pyr, result, offset, "<f%d> %d", i, i); // XXX 24

            if(i<(num_entries-1)){
                bsprintf(this_pyr, result, offset, "|"); // XXX 1
            }
        }

        if(initial){ // XXX constant-padding
            bsprintf(this_pyr, result, offset, "\",style=\"dashed\"];\n");
        }
        else{
            bsprintf(this_pyr, result, offset, "\"];\n");
        }

        for(i=0; i<num_entries; i++){

            if(is_nil(pcar(bs+i))){
                continue;
            }

            bsprintf(this_pyr, result, offset, "\"s%08x\":f%d -> \"s%08x\":f0;\n", (mword)bs, i, *(mword *)(bs+i)); // XXX 43
            _rbs2gv(this_pyr, (mword *)*(bs+i), result, offset, 0);

        }

        // XXX (25 + 43) * NPT

    }
    else{// if(is_val(bs)){  // XXX 65 * NLF

        if(num_entries > 8){
            num_entries=8;
            clipped=1;
        }
        else{
            clipped=0;
        }

        bsprintf(this_pyr, result, offset, "s%08x [style=bold,shape=record,label=\"", (mword)bs); // XXX 43

        for(i=0; i<num_entries; i++){ // XXX 23 * NVA

            bsprintf(this_pyr, result, offset, "<f%d> %x", i, *(mword *)(bs+i));              // XXX 22

            if(i<(num_entries-1)){
                bsprintf(this_pyr, result, offset, "|");              // XXX 1
            }

        }
        if(clipped){

            bsprintf(this_pyr, result, offset, "|");              // XXX 1
            bsprintf(this_pyr, result, offset, "<f%d> ...", i);   // XXX <=17

        }

        bsprintf(this_pyr, result, offset, "\"];\n");             // XXX 4

    }

    mark_traversed_U(bs);

}



// Clayton Bauman 2017

