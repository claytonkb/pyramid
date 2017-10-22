// mem.c
//

#include "pyramid.h"
#include "mem.h"
#include "array.h"



/*****************************************************************************
 *                                                                           *
 *                    NON-GC MEMORY (global_irt->sys_mem)                    *
 *                                                                           *
 ****************************************************************************/


// XXX WAIVER REQUIRED mem_sys_alloc XXX
// NB: size is in units of bytes
// DESC: malloc() wrapper (check ret ptr + track stats)
//
void *mem_sys_alloc(int size){ // mem_sys_alloc#

    void *alloc_attempt = malloc(size); // XXX WAIVER(malloc) XXX

    if(alloc_attempt == NULL){ // malloc failed
        _fatal("malloc failed");
    }

    global_irt->sys_mem->alloc_count++; // FIXME Wrap this in PROF_MODE guards

    return alloc_attempt;

}


// NB: Use only with mem created by mem_sys_alloc
// DESC: free() wrapper (track stats)
//
void mem_sys_free(void *p){ // mem_sys_free#

    free(p); // XXX WAIVER(free) XXX

    global_irt->sys_mem->free_count++; // FIXME Wrap this in PROF_MODE guards

}


//
//
void mem_non_gc_new(void){ // mem_non_gc_new#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    global_irt->sys_mem = malloc(sizeof(mem_thread_base)); // XXX WAIVER(malloc) XXX //
    global_irt->sys_mem = global_irt->sys_mem;

    global_irt->sys_mem->alloc_count = 0;
    global_irt->sys_mem->free_count = 0;

    // XXX We can use mem_sys_alloc() now
    global_irt->sys_mem->base_ptr = mem_sys_alloc(UNITS_MTO8(mem_thread_base_page_size));
    global_irt->sys_mem->current_page = global_irt->sys_mem->base_ptr;
    global_irt->sys_mem->current_offset = 0;

    int i;
    for(i=0; i<mem_thread_base_page_size; i++){
        ((mword**)(global_irt->sys_mem->base_ptr))[i] = UNINIT_PTR;
    }

}


// XXX DEPRECATE XXX TURN THIS INTO A MACRO XXX
//
void *mem_non_gc_alloc2(mword alloc_sfield){ // mem_non_gc_alloc2#

    mword alloc_request_size = mem_alloc_size(alloc_sfield)+1;

    mword *result;

    result = (void*)mem_sys_alloc(UNITS_MTO8(alloc_request_size)); // XXX WAIVER(mem_sys_alloc) XXX //

    mem_non_gc_insert(result);

    return result;

}


// Allocates non-GC memory using sfield
// XXX This is intended for bootstrap and memory-debug use ONLY XXX
//mword *mem_non_gc_alloc(mword alloc_sfield){ // mem_non_gc_alloc#
void *mem_non_gc_alloc(int size){ // mem_non_gc_alloc#

    mword *result;

    result = (void*)mem_sys_alloc(size); // XXX WAIVER(mem_sys_alloc) XXX //

    mem_non_gc_insert(result);

    return result;

}


// Returns 1 if insert succeeded, 0 otherwise
// 
mword mem_non_gc_insert(void *non_gc_ptr){ // mem_non_gc_free#

    mword *temp;
    int i;

    if(global_irt->sys_mem->current_offset
            == (mem_thread_base_page_size-2)){ // Leave a space for the chaining pointer

        ((mword**)(global_irt->sys_mem->current_page))[mem_thread_base_page_size-2] 
            = non_gc_ptr;

        temp = mem_sys_alloc(UNITS_MTO8(mem_thread_base_page_size));
        for(i=0; i<mem_thread_base_page_size; i++){
            ((mword**)temp)[i] = UNINIT_PTR;
        }

        ((mword**)(global_irt->sys_mem->current_page))[mem_thread_base_page_size-1] 
            = temp;

        global_irt->sys_mem->current_page   = temp;
        global_irt->sys_mem->current_offset = 0;

    }
    else{
        global_irt->sys_mem->current_page[global_irt->sys_mem->current_offset]
            = (mword)non_gc_ptr;
    }

    global_irt->sys_mem->current_offset++;

    return 1;

}


// Returns 1 if freed, 0 otherwise
// 
mword mem_non_gc_free(void *non_gc_ptr){ // mem_non_gc_free#

    return 0;

}


//
// 
void mem_non_gc_reset(void){ // mem_non_gc_reset#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    mem_non_gc_reset_pages(global_irt->sys_mem->base_ptr);

    global_irt->sys_mem->current_page = global_irt->sys_mem->base_ptr;
    global_irt->sys_mem->current_offset = 0;

    int i;
    for(i=0; i<mem_thread_base_page_size; i++){
        ((mword**)(global_irt->sys_mem->base_ptr))[i] = UNINIT_PTR;
    }

}


// FIXME why isn't this calling mem_non_gc_free()? FIXME
//
void mem_non_gc_reset_pages(mword *page_ptr){ // mem_non_gc_reset_pages#

    int i;

    if(((mword**)page_ptr)[mem_thread_base_page_size-1] 
            != UNINIT_PTR){
        mem_non_gc_reset_pages(((mword**)page_ptr)[mem_thread_base_page_size-1]);
    }

    for(i=0; i<mem_thread_base_page_size; i++){
        if(((mword**)page_ptr)[i] != UNINIT_PTR){
            mem_sys_free((mword*)page_ptr[i]);
        }
    }


}


//
//
void mem_non_gc_teardown(void){ // mem_non_gc_teardown#

    mem_non_gc_reset();

    mem_sys_free(global_irt->sys_mem->base_ptr);

#ifdef PROF_MODE
    _dd(global_irt->sys_mem->alloc_count);
    _dd(global_irt->sys_mem->free_count);
#endif

    free(global_irt->sys_mem);

}


/*****************************************************************************
 *                                                                           *
 *                       GC MEMORY (global_irt->gc_mem)                      *
 *                                                                           *
 ****************************************************************************/

//
//
void mem_new(pyr_cache *this_pyr, mword init_mem_size){ // mem_new#

#if(defined INTERP_RESET_TRACE)
_reset_trace;
#elif(defined GC_TRACE)
_trace;
#endif

    mem_context *m = mem_non_gc_alloc(sizeof(mem_context));

    m->primary   = mem_non_gc_alloc(sizeof(alloc_bank));
    m->secondary = mem_non_gc_alloc(sizeof(alloc_bank));

    mem_bank_alloc(m->primary,   init_mem_size);
    mem_bank_alloc(m->secondary, init_mem_size);

    global_irt->gc_mem = m;

}


//
//
void mem_destroy(mem_context *m){ // mem_destroy#

    mem_bank_free(m->primary);
    mem_bank_free(m->secondary);

}


//
//
inline void mem_bank_alloc(alloc_bank *a, mword bank_size){ // mem_bank_alloc#

    a->base_ptr    = mem_sys_alloc(bank_size); // XXX WAIVER(mem_sys_alloc) XXX //
    a->size        = bank_size;
    a->bound_ptr   = (mword*)((char*)a->base_ptr   + (a->size-MWORD_SIZE));
    a->alloc_ptr   = a->bound_ptr;

    mem_reset_bank(a);

}


//
//
inline void mem_bank_free(alloc_bank *a){ // mem_bank_free#

    mem_sys_free(a->base_ptr);

    a->base_ptr    = nil;
    a->size        = 0;
    a->bound_ptr   = nil;
    a->alloc_ptr   = nil;

}


//
//
mword *mem_alloc(pyr_cache *this_pyr, mword alloc_sfield){ // mem_alloc#

    mword *return_ptr;

    // mem_alloc is non-reentrant... this is enforced with the MEM_ALLOC_BLOCKING flag
    if(global_irt->flags->MEM_ALLOC_BLOCKING == SET){
        _fatal("global_irt->flags->MEM_ALLOC_BLOCKING == SET");
    }

    if(global_irt->flags->MEM_ALLOC_NON_GC == SET){
        mword *result = mem_non_gc_alloc(UNITS_MTO8(mem_alloc_size(alloc_sfield)+1));
        result++;
        sfield(result) = alloc_sfield;
        return result;
    }

    mword alloc_request_size = mem_alloc_size(alloc_sfield)+1; // +1 is for s-field

#if 0
    _d(global_irt->gc_mem->primary->base_ptr);
    _d(global_irt->gc_mem->primary->bound_ptr);
    _d(global_irt->gc_mem->primary->alloc_ptr);
    _dd(global_irt->gc_mem->primary->size);

    _d(global_irt->gc_mem->secondary->base_ptr);
    _d(global_irt->gc_mem->secondary->bound_ptr);
    _d(global_irt->gc_mem->secondary->alloc_ptr);
    _dd(global_irt->gc_mem->secondary->size);
#endif

    alloc_bank *b = global_irt->gc_mem->primary;

//_d(b->base_ptr);
//_d(b->size);
//_d(b->bound_ptr);
//_d(b->alloc_ptr);
//
//_die;
//
//    global_irt->flags->MEM_ALLOC_BLOCKING = FLAG_SET;

//mword mem_usage = b->bound_ptr-b->alloc_ptr;
//_d(mem_usage);

    if((b->alloc_ptr - alloc_request_size) <= b->base_ptr){
        _fatal("memory bank empty");
    }

    b->alloc_ptr -= alloc_request_size;
    return_ptr = b->alloc_ptr+1;
    sfield(return_ptr) = alloc_sfield;

//    global_irt->flags->MEM_ALLOC_BLOCKING = FLAG_CLR;

    return return_ptr;

}


/*****************************************************************************
 *                                                                           *
 *                             ALLOC UTILITIES                               *
 *                                                                           *
 ****************************************************************************/

//
//
inline void *mem_new_val(pyr_cache *this_pyr, mword size, mword init){ // mem_new_val#

    mword local_size = UNITS_MTO8(size);

    void *ptr = (void*)mem_alloc(this_pyr, UNITS_MTO8(size));

    memset((char*)ptr,init,local_size);

    return ptr;

}

// mem_new_valz (see pyramid.h)

//
//
inline void *mem_new_ptr(pyr_cache *this_pyr, mword size){ // mem_new_ptr#

    void *ptr;

    if(!size){

        return nil;

    }
    else{

        ptr = (void*)mem_alloc(this_pyr, -1*UNITS_MTO8(size));

        int i;
        for(i = 0; i<size; i++){
            ldp(ptr,i) = (void*)nil;
        }

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

//    if(!set_char)
//        set_char = ' ';

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


// Make a double-linked list cons cell (three entries instead of two)
//
mword *_dcons(pyr_cache *this_pyr, mword *car, mword *cdr, mword *cpr){ // _dcons#

    mword **dcons_cell = (mword**)mem_new_cons(this_pyr);
    ldp(dcons_cell,0) = car;
    ldp(dcons_cell,1) = cdr;
    ldp(dcons_cell,2) = cpr;

    return (mword*)dcons_cell;

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


// make aop ==> make "array-of-pairs"
// array_size = number-of-arguments / 2
//
void *_mk_aop(pyr_cache *this_pyr, mword array_size, ...){ // _mk_aop#

    void *ptr = (void*)mem_new_ptr(this_pyr, array_size);

    va_list vl;
    va_start(vl,array_size);

    int i;
    mword *car;
    mword *cdr;

    for(i=0;i<array_size;i++){
        car = va_arg(vl,mword*);
        cdr = va_arg(vl,mword*);
        ldp(ptr,i) = _cons(this_pyr, car, cdr);
    }

    va_end(vl);

    return ptr;

}


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
//    mword *new_cons = mem_new_ptr(this_pyr, 2);
    mword *new_cons = _cons(this_pyr, nil, nil);
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


// creates a new circular, doubly-linked list of given size
// note that the list is created in REVERSE order
//
mword *_mkdls(pyr_cache *this_pyr, mword list_size, ...){ // _mkdls#

    va_list vl;
    va_start(vl,list_size);

    int i;

    mword *last_dcons  = nil;
    mword *this_dcons  = va_arg(vl,mword*);
           last_dcons  = (void*)_dcons(this_pyr, this_dcons, last_dcons, nil);
    mword *first_dcons = last_dcons;

    for(i=1;i<list_size;i++){
        this_dcons = va_arg(vl,mword*);
        this_dcons = (void*)_dcons(this_pyr, this_dcons, last_dcons, nil);
        ldp(last_dcons,2) = this_dcons;
        last_dcons = this_dcons;
    }

    va_end(vl);

    // close the ends to form a circle
    ldp(last_dcons,2)  = first_dcons;
    ldp(first_dcons,1) = last_dcons;

    return last_dcons;

}


// Clayton Bauman 2017

