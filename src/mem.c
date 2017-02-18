// mem.c
//

#include "pyramid.h"
#include "mem.h"
#include "array.h"


// mem_new
//
void mem_new(pyr_cache *this_pyr, mword init_mem_size){ // mem_new#

#if(defined INTERP_RESET_TRACE)
_reset_trace;
#elif(defined GC_TRACE)
_trace;
#endif

    mem_context *m = mem_sys_alloc(sizeof(mem_context));    // XXX WAIVER(mem_sys_alloc) XXX //

    m->primary   = mem_sys_alloc(sizeof(alloc_bank));       // XXX WAIVER(mem_sys_alloc) XXX //
    m->secondary = mem_sys_alloc(sizeof(alloc_bank));       // XXX WAIVER(mem_sys_alloc) XXX //

    mem_bank_alloc(m->primary,   init_mem_size);
    mem_bank_alloc(m->secondary, init_mem_size);

    this_pyr->interp->mem = m;

}


//
//
void mem_destroy(mem_context *m){ // mem_destroy#

    mem_bank_free(m->primary);
    mem_bank_free(m->secondary);

    mem_sys_free(m->primary,   sizeof(alloc_bank));
    mem_sys_free(m->secondary, sizeof(alloc_bank));

    mem_sys_free(m, sizeof(mem_context));

}


//
//
inline void mem_bank_alloc(alloc_bank *a, mword bank_size){ // mem_bank_alloc#

    a->base_ptr    = mem_sys_alloc(bank_size); // XXX WAIVER(mem_sys_alloc) XXX //
    a->size        = bank_size;
    a->bound_ptr   = (mword*)((char*)a->base_ptr   + (a->size-1));

    mem_reset_bank(a);

}


//
//
inline void mem_bank_free(alloc_bank *a){ // mem_bank_free#

    mem_sys_free(a->base_ptr, a->size);

    a->base_ptr    = nil;
    a->size        = 0;
    a->bound_ptr   = nil;

}


// XXX WAIVER REQUIRED mem_sys_alloc XXX
// NB: size is in units of bytes
// DESC: malloc() wrapper (check ret ptr, track stats)
//
void *mem_sys_alloc(int size){ // mem_sys_alloc#

    void *alloc_attempt = malloc(size); // XXX WAIVER(malloc) XXX

    if(alloc_attempt == NULL){ // malloc failed
        _fatal("malloc failed");
    }

    global_mem_sys_alloc_count++;
    global_mem_sys_alloc_total+=size;

    return alloc_attempt;

}


// NB: Use only with mem created by mem_sys_alloc
// DESC: free() wrapper (track stats)
//
void mem_sys_free(void *p, int size){ // mem_sys_free#

//_d(p);

    free(p); // XXX WAIVER(free) XXX
    global_mem_sys_free_count++;
    global_mem_sys_free_total+=size;

}


#define alloc_size_check(x)                                                 \
    mword alloc_request_size = mem_alloc_size(x)+1;                         \
                                                                            \
    if(alloc_request_size > MEM_ABS_MAX_ALLOC){                             \
        _fatal("alloc failed: alloc_request_size > MEM_ABS_MAX_ALLOC");     \
    }


// Allocates non-GC memory using sfield
// XXX This is intended for bootstrap and memory-debug use ONLY XXX
inline mword *mem_non_gc_alloc(mword alloc_sfield){ // mem_non_gc_alloc#

    alloc_size_check(alloc_sfield);

    mword *result;

    result = (void*)mem_sys_alloc(UNITS_MTO8(alloc_request_size)); // XXX WAIVER(mem_sys_alloc) XXX //
    sfield(result) = alloc_sfield;

    return result;

}


//// Allocates non-GC memory using sfield
//// XXX This is intended for bootstrap and memory-debug use ONLY XXX
//inline mword *mem_non_gc_alloc(mword alloc_sfield){ // mem_non_gc_alloc#
//
//    mword *result;
//
////    lci(global_boot_mem_allocs,global_mem_sys_alloc_count) = alloc_attempt;
//
//    result = (void*)mem_sys_alloc(UNITS_MTO8(alloc_request_size)); // XXX WAIVER(mem_sys_alloc) XXX //
//    sfield(result) = alloc_sfield;
//
//    return result;
//
//}
//

//
//
mword *mem_alloc(pyr_cache *this_pyr, mword alloc_sfield){ // *mem_alloc#

    // mem_alloc is non-reentrant... this is enforced with the MEM_ALLOC_BLOCKING flag
    if(this_pyr->flags->MEM_ALLOC_BLOCKING == SET){
        _fatal("this_pyr->flags->MEM_ALLOC_BLOCKING == SET");
    }

    if(this_pyr->flags->MEM_USE_MALLOC == SET)
        return mem_non_gc_alloc(alloc_sfield);

//    if(this_pyr->flags->MEM_USE_DYN == SET)
//        return mem_user_dyn_alloc(alloc_sfield);

    alloc_size_check(alloc_sfield);

    alloc_bank *b = this_pyr->interp->mem->primary;

    if((b->alloc_ptr - alloc_request_size) < b->base_ptr){
        _fatal("out of memory");
    }

    b->alloc_ptr -= alloc_request_size;

    mword *result = b->alloc_ptr+1;
    sfield(result) = alloc_sfield;

    this_pyr->flags->MEM_ALLOC_BLOCKING = CLR;

    return result;

//#ifdef MEM_DEBUG
//#endif
//
//    this_pyr->flags->MC_ALLOC_BLOCKING = FLAG_SET;
//
//    alloc_bank *b = this_pyr->interp->mem->primary;
//
//    if((b->alloc_ptr - alloc_request_size) <= b->base_ptr){ // swap banks and set MC_GC_PENDING
//
//        if(this_pyr->flags->MC_GC_PENDING == FLAG_CLR){
//
//#ifdef GC_TRACE
//_msg("MC_GC_PENDING");
//#endif
//
//            mem_swap_banks(this_pyr);
//            b = this_pyr->interp->mem->primary; // update local copy
//            this_pyr->flags->MC_GC_PENDING = FLAG_SET;
//
//        }
//        else{
//
//#ifdef GC_TRACE
//_msg("MC_GC_OP_RESTART");
//#endif
//
//            if(this_pyr->flags->MC_GC_INTERP_BLOCKING == FLAG_SET){
//                _fatal("this_pyr->flags->MC_GC_INTERP_BLOCKING == FLAG_SET");
//            }
//
//            if(this_pyr->flags->INTERP_BOOT_IN_PROGRESS == FLAG_SET){
//                _fatal("MC_GC_OP_RESTART while INTERP_BOOT_IN_PROGRESS");
//            }
//
//            this_pyr->flags->MC_GC_BLOCKING = FLAG_CLR;
//            this_pyr->flags->MC_ALLOC_BLOCKING = FLAG_CLR;
//            this_pyr->interp->mem->op_restart_alloc_size = alloc_request_size;
//            _op_restart(this_pyr);
//
//        }
//
//    }
//
//#if 0
//    mword mbu = mem_bank_in_use(b);
//
//    if( (mbu+alloc_request_size) >= b->size ){
//
//        if(this_pyr->flags->MC_GC_PENDING == FLAG_CLR){
//
//#ifdef GC_TRACE
//_msg("MC_GC_PENDING");
//#endif
//            mem_swap_banks(this_pyr);
//            b = this_pyr->interp->mem->primary; // update local copy
//            this_pyr->flags->MC_GC_PENDING = FLAG_SET;
//
//        }
//        else{
//
//#ifdef GC_TRACE
//_msg("MC_GC_OP_RESTART");
//#endif
//
//            if(this_pyr->flags->MC_GC_INTERP_BLOCKING == FLAG_SET){
//                _fatal("this_pyr->flags->MC_GC_INTERP_BLOCKING == FLAG_SET");
//            }
//
//            if(this_pyr->flags->INTERP_BOOT_IN_PROGRESS == FLAG_SET){
//                _msg("MC_GC_OP_RESTART while INTERP_BOOT_IN_PROGRESS");
//                _cat_except(this_pyr);
//            }
//
//            if(this_pyr->flags->MC_GC_BLOCKING == FLAG_SET){
//                this_pyr->flags->MC_GC_BLOCKING = FLAG_CLR;
//            }
//
//            this_pyr->flags->MC_ALLOC_BLOCKING = FLAG_CLR;
//            this_pyr->interp->mem->op_restart_alloc_size = alloc_request_size;
//            _op_restart(this_pyr);
//
//        }
//
//    }
//#endif
//
//    b->alloc_ptr -= alloc_request_size;
//
//#if 0
//if(b->alloc_ptr <= b->base_ptr){
//    _d((mword)b->alloc_ptr);
//    _d((mword)b->base_ptr);
//    _die;
//}
//#endif
//
//    return_ptr = b->alloc_ptr+1;
//
//    sfield(return_ptr) = alloc_sfield;
//
//    this_pyr->flags->MC_ALLOC_BLOCKING = FLAG_CLR;
//    this_pyr->flags->MC_GC_OP_RESTART  = FLAG_CLR;
//
////mword bounds_check = mem_bounds_check(this_pyr, return_ptr);
////_d(bounds_check);
//
//    return return_ptr;

}


//
//
inline void *mem_new_val(pyr_cache *this_pyr, mword size, mword init){ // mem_new_val#

    mword local_size = UNITS_MTO8(size);

    void *ptr = (void*)mem_alloc(this_pyr, local_size);

    memset((char*)ptr,init,local_size);

    return ptr;

}

// mem_new_valz (see pyramid.h)

//
//
inline void *mem_new_ptr(pyr_cache *this_pyr, mword size){ // mem_new_ptr#

    void *ptr = (void*)mem_alloc(this_pyr, -1*UNITS_MTO8(size));

    int i;
    for(i = 0; i<size; i++){
        ldp(ptr,i) = (void*)nil;
    }

    return ptr;

}


//
//
inline void *mem_new_tptr(pyr_cache *this_pyr, const mword *hash, mword *bs){

    mword *ptr = mem_alloc(this_pyr, TPTR_SFIELD);

    tagcpy(ptr, hash);

    // ptr s-field
    ldv(ptr,HASH_SIZE) = (NEG_ONE * MWORD_SIZE);

    tptr_set_ptr(ptr, bs);

    return ptr;

}


//
//
inline void *mem_new_cptr(pyr_cache *this_pyr, const mword *hash, mword *cptr){ // mem_new_cptr#

    return mem_new_tptr(this_pyr, hash, _val(this_pyr, (mword)cptr));

}


// Accepts a data value and returns a val-array of size 1 containing that 
// data value
//
inline void *_val(pyr_cache *this_pyr, mword value){ // _val#

    void *ptr = mem_new_val(this_pyr, 1, 0);
    ldv(ptr,0) = value;

    return ptr;

}


// Accepts a single unsafe pointer and returns a safe ptr-array of size
// 1 containing the unsafe pointer
//
inline void *_ptr(pyr_cache *this_pyr, mword *unsafe_ptr){ // _ptr#

    void *ptr = mem_new_ptr(this_pyr, 1);
    ldp(ptr,0) = unsafe_ptr;

    return ptr;

}


// _tptr (see pyramid.h)


// Intended for internal-use... 
//
mword *_newstr(pyr_cache *this_pyr, mword size8, char set_char){ // _newstr#

    if(!set_char)
        set_char = ' ';

    mword arlength = array8_mword_size(this_pyr, size8);

    mword *result = mem_new_val(this_pyr, arlength, set_char);

    mword alignment_word = array8_enc_align(this_pyr, size8);

    ldv(result, arlength-1) = alignment_word;
    ldv(result, arlength-2) = (rdv(result, arlength-2)) & ~alignment_word;

    return result;

}


// Intended for internal-use... 
//
mword *_newbits(pyr_cache *this_pyr, mword size1){ // _newbits#

    mword arlength = array1_mword_size(this_pyr, size1);

    mword *result = mem_new_val(this_pyr, arlength, 0);

    mword alignment_word = array1_enc_align(this_pyr, size1);

    ldv(result, arlength-1) = alignment_word;
    ldv(result, arlength-2) = (rdv(result, arlength-2)) & ~alignment_word;

    return result;

}


//
//
inline mword *_cons(pyr_cache *this_pyr, mword *car, mword *cdr){ // _cons#

    mword **cons_cell = (mword**)mem_new_cons(this_pyr);
    ldp(cons_cell,0) = car;
    ldp(cons_cell,1) = cdr;

    return (mword*)cons_cell;

}


//
//
void *_mkval(pyr_cache *this_pyr, mword array_size, ...){ // _mkval#

    void *val = (void*)mem_new_val(this_pyr, array_size, 0);

    va_list vl;
    va_start(vl,array_size);

    int i;

    for(i=0;i<array_size;i++){
        ldv(val,i) = va_arg(vl,mword);
    }

    va_end(vl);

    return val;

}


//
//
void *_mkptr(pyr_cache *this_pyr, mword array_size, ...){ // _mkptr#

    void *ptr = (void*)mem_new_ptr(this_pyr, array_size);

    va_list vl;
    va_start(vl,array_size);

    int i;

    for(i=0;i<array_size;i++){
        ldp(ptr,i) = va_arg(vl,mword*);
    }

    va_end(vl);

    return ptr;

}


//// creates a new list of given size
//// note that the list is created in REVERSE order:
//// _newls(this_pyr, 3, _val(this_pyr,3), _val(this_pyr,2), _val(this_pyr,1)) --> (1 2 3)
////
//mword *_mkls(pyr_cache *this_pyr, mword list_size, ...){ // _mkls#
//
//    va_list vl;
//    va_start(vl,list_size);
//
//    int i;
//
//    mword *last_cons = nil;
//
//    for(i=0;i<list_size;i++){
//        last_cons = (void*)_cons(this_pyr, va_arg(vl,mword*), last_cons);
//    }
//
//    va_end(vl);
//
//    return last_cons;
//
//}


// creates a new list of given size
// note that the list is created in REVERSE order:
// _newls(this_pyr, 3, _val(this_pyr,3), _val(this_pyr,2), _val(this_pyr,1)) --> (1 2 3)
//
// [ptr [val 0x1 ] [ptr [val 0x2 ] [ptr [val 0x3 ]  nil ] ] ]
//
mword *_mkls(pyr_cache *this_pyr, mword list_size, ...){ // _mkls#

    va_list vl;
    va_start(vl,list_size);

    int i;

    mword *last_cons = nil;
    mword *new_cons = mem_new_ptr(this_pyr, 2);
    mword *head = new_cons;

    ldp(new_cons,0) = va_arg(vl,mword*);

    for(i=1;i<list_size;i++){

        last_cons = new_cons;
        new_cons = mem_new_ptr(this_pyr, 2);
        ldp(new_cons,0) = va_arg(vl,mword*);
        ldp(last_cons,1) = new_cons;

    }

    va_end(vl);

    return head;

}


// Clayton Bauman 2016

