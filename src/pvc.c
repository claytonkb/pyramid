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
//    mword *local_stack_ptr = local_stack;

    this_pyr->cpu->code_block = code_block;
    this_pyr->cpu->rstack_base_ptr = local_stack;
    this_pyr->cpu->rstack_ptr = local_stack;

//    mword *this_operand;
    mword *this_op;

    pyramid_op operator;
    pyr_cache *discard;

    for(i=0;i<steps;i++){
        this_step = rdp(code_block,i);
        this_op = tcar(this_step);
        if(itageq(this_step,global_irt->tags->PYR_TAG_OPERAND)){
//            this_operand = tcar(this_step);
_d(this_op);
            temp = _bs2str(this_pyr, this_op);
            _say((char*)temp);
//            *(mword**)local_stack_ptr = this_operand;
            *(mword**)this_pyr->cpu->rstack_ptr = this_op;
//            local_stack_ptr++;
            this_pyr->cpu->rstack_ptr++;
        }
        else{
//            arith_maddi2(this_pyr);
            temp = xbar_tag_to_fn(this_pyr, this_op);

            if(is_nil_fast(temp)){
                _say("------------------------ code_block ----------------------->");
                temp = _bs2str(this_pyr, code_block);
                _say((char*)temp);
                _say("------------------------ step ----------------------------->");
                _d(i);
                _say("------------------------ operator ------------------------->");
                _mem(this_op);
                _fatal("Unrecognized operator");
            }

            temp = xbar_tag_to_string(this_pyr, this_op);
            _say((char*)temp);
            // dispatch function //

            operator = (pyramid_op)xbar_tag_to_fn(this_pyr, tcar(this_step));
            discard = operator(this_pyr);
            discard = discard; // suppress compiler warning; gets optimized away

        }
    }

    _mem(local_stack);
    _d(local_stack);
//    _d(local_stack_ptr);
    temp = _bs2str(this_pyr, *(mword**)this_pyr->cpu->rstack_ptr);
    _say((char*)temp);

}


//bvm_cache *interp_op_exec(bvm_cache *this_bvm, mword opcode){ // interp_op_exec#
//
//    bvm_cache *discard;
//    babel_op op_ptr;
//
//    op_ptr = (babel_op)this_bvm->interp->jump_table[ opcode % NUM_INTERP_OPCODES ];
//
//    discard = op_ptr(this_bvm);
//    discard = discard; // suppress compiler warning, gets optimized away
//
//    return this_bvm;
//
//}


//
//
pyr_cache *arith_maddi(pyr_cache *this_pyr){ // arith_maddi#

    this_pyr->cpu->rstack_ptr -= 2; // FIXME: BOUNDS CHECK RSTACK_PTR!
    mword *local_stack_ptr = this_pyr->cpu->rstack_ptr;

    mword *result = _val( this_pyr, vcar(ldv(local_stack_ptr,0)) + vcar(ldv(local_stack_ptr,1)) );

    ldp(this_pyr->cpu->rstack_ptr,0) = result;

    return this_pyr;

}


////
////
//void arith_maddi(pyr_cache *this_pyr, mword **local_stack_ptr){ // arith_maddi#
//
//    mword *result = _val( this_pyr, ldv(*((*local_stack_ptr)-1),0) + ldv(*((*local_stack_ptr)-2),0) );
//    *local_stack_ptr -= 2;
//    **(mword***)local_stack_ptr = result;
//
//}


//// code_block must be de-tagged
////
//void pvc_core_interp(pyr_cache *this_pyr, mword *code_block){ // pvc_core_interp#
//
//    mword steps = size(code_block);
//    int i;
//    mword *this_step;
//    mword *temp;
//
//    mword *local_stack = mem_new_ptr(this_pyr, 8);
//    mword *local_stack_ptr = local_stack;
//
//
//
//    mword *this_operand;
//
//    for(i=0;i<steps;i++){
//        this_step = rdp(code_block,i);
//        if(itageq(this_step,global_irt->tags->PYR_TAG_OPERAND)){
//            this_operand = tcar(this_step);
//_d(this_operand);
//            temp = _bs2str(this_pyr, this_operand);
//            _say((char*)temp);
//            *(mword**)local_stack_ptr = this_operand;
//            local_stack_ptr++;
//        }
//        else{
//            temp = xbar_tag_to_string(this_pyr, tcar(this_step));
//            _say((char*)temp);
//            // dispatch function //
//            arith_maddi(this_pyr, &local_stack_ptr);
//        }
//    }
//
//    _mem(local_stack);
//    _d(local_stack);
//    _d(local_stack_ptr);
//    temp = _bs2str(this_pyr, *(mword**)local_stack_ptr);
//    _say((char*)temp);
//
//}
//
//
////
////
//void arith_maddi(pyr_cache *this_pyr, mword **local_stack_ptr){ // arith_maddi#
//
//    mword *result = _val( this_pyr, ldv(*((*local_stack_ptr)-1),0) + ldv(*((*local_stack_ptr)-2),0) );
//    *local_stack_ptr -= 2;
//    **(mword***)local_stack_ptr = result;
//
//}

// Clayton Bauman 2017

