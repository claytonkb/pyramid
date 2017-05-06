// pvc.c
//

#include "pyramid.h"
#include "pvc.h"
#include "introspect.h"
#include "xbar.h"
#include "mem.h"

//
//
void pvc_new(pyr_cache *this_pyr){

    _d(nil);

}


// code_block must be de-tagged
//
void pvc_core_interp(pyr_cache *this_pyr, mword *code_block){ // pvc_core_interp#

    mword steps = size(code_block);
    int i;
    mword *this_step;
    mword *temp;

    mword *local_stack = mem_new_ptr(this_pyr, 8);
    mword *local_stack_ptr = local_stack;

    mword *this_operand;

    for(i=0;i<steps;i++){
        this_step = rdp(code_block,i);
        if(itageq(this_step,global_irt->tags->PYR_TAG_OPERAND)){
            this_operand = tcar(this_step);
_d(this_operand);
            temp = _bs2str(this_pyr, this_operand);
            _say((char*)temp);
            *(mword**)local_stack_ptr = this_operand;
            local_stack_ptr++;
        }
        else{
            temp = xbar_tag_to_string(this_pyr, tcar(this_step));
            _say((char*)temp);
            // dispatch function //
            arith_maddi(this_pyr, &local_stack_ptr);
        }
    }

    _mem(local_stack);
    _d(local_stack);
    _d(local_stack_ptr);
    temp = _bs2str(this_pyr, *(mword**)local_stack_ptr);
    _say((char*)temp);

}


//
//
void arith_maddi(pyr_cache *this_pyr, mword **local_stack_ptr){ // arith_maddi#

    mword *result = _val( this_pyr, ldv(*((*local_stack_ptr)-1),0) + ldv(*((*local_stack_ptr)-2),0) );
    *local_stack_ptr -= 2;
    **(mword***)local_stack_ptr = result;

}

// Clayton Bauman 2017

