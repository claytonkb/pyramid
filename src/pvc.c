// pvc.c
//

#include "pyramid.h"
#include "pvc.h"
#include "introspect.h"
#include "xbar.h"

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

    for(i=0;i<steps;i++){
        this_step = rdp(code_block,i);
        if(itageq(this_step,global_irt->tags->PYR_TAG_OPERAND)){
            temp = _bs2str(this_pyr, tcar(this_step));
            _say((char*)temp);
        }
        else{
            temp = xbar_tag_to_string(this_pyr, tcar(this_step));
            _say((char*)temp);
        }
    }

}


// Clayton Bauman 2017

