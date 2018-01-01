// mem.h
// 

#ifndef MEM_H
#define MEM_H

#define TOP_OF_ALLOC_BANK(x) ((char*)(x->base_ptr)+(x->size-1)) // TOP_OF_ALLOC_BANK#

#define MEM_GC_STAT_BANK_MIN_SIZE (1<<12)
#define MEM_GC_STAT_BANK_MAX_SIZE (1<<27) // XXX ARBITRARY! XXX

#define MEM_ABS_MAX_ALLOC (((mword)-1)>>1)

#define mem_reset_bank(x) (x->alloc_ptr = x->bound_ptr)

#define mem_bank_in_use(x)    ((mword)x->bound_ptr - (mword)x->alloc_ptr) // mem_bank_in_use#
#define mem_bank_available(x) ((mword)x->alloc_ptr - (mword)x->base_ptr) // mem_bank_available#

#define mem_new_cons(x) (mem_new_ptr(x, 2)) // mem_new_cons#

void *mem_sys_alloc(int size);
//void  mem_sys_free(void *p, int size);
void mem_sys_free(void *p);

void mem_bank_alloc(alloc_bank *a, mword bank_size);
void mem_bank_free(alloc_bank *a);

void mem_new(pyr_cache *this_pyr, mword init_mem_size);
void mem_destroy(mem_context *m);

//void mem_reset_bank(alloc_bank *b);

void *mem_non_gc_alloc2(mword alloc_sfield);
void *mem_non_gc_alloc(int size);
mword mem_non_gc_free(void *non_gc_ptr);
void mem_non_gc_teardown(void);
void mem_non_gc_reset(void);
void mem_non_gc_new(void);
mword mem_non_gc_insert(void *non_gc_ptr);

void mem_non_gc_reset_pages(mword *page_ptr);

mword *mem_alloc(pyr_cache *this_pyr, mword sfield);
//mword *mem_alloc2(pyr_cache *this_pyr, mword alloc_sfield, access_size_sel access_size);

void *mem_new_val(pyr_cache *this_pyr, mword size, mword init);
void *mem_new_ptr(pyr_cache *this_pyr, mword size);
void *mem_new_tptr(pyr_cache *this_pyr, const mword *hash, mword *bs);
void *mem_new_cptr(pyr_cache *this_pyr, const mword *hash, mword *cptr);

void *_val(pyr_cache *this_pyr, mword value);
void *_ptr(pyr_cache *this_pyr, mword *unsafe_ptr);
//void *_tptr(pyr_cache *this_pyr, const mword *hash, mword *bs);

mword *_newstr(pyr_cache *this_pyr, mword size8, char set_char);
mword *_newbits(pyr_cache *this_pyr, mword size1);

mword *_cons(pyr_cache *this_pyr, mword *car, mword *cdr);
mword *_dcons(pyr_cache *this_pyr, mword *car, mword *cdr, mword *cpr);

void *_mkval(pyr_cache *this_pyr, mword array_size, ...);
void *_mkptr(pyr_cache *this_pyr, mword array_size, ...);
void *_mk_aop(pyr_cache *this_pyr, mword array_size, ...);

mword *_mkls(pyr_cache *this_pyr, mword list_size, ...);
mword *_mkdls(pyr_cache *this_pyr, mword list_size, ...);

// mem_alloc                -> vanilla memory-allocation (mword resolution)
// mem_alloc_align_masked   -> vanilla memory-allocation (byte/bit resolution)
// mem_alloc_c              -> vanilla memory-allocation of bstruct-wrapped C object
// mem_dyn_alloc            -> dynamic memory-allocation (mword/byte/bit resolution)
// mem_rstack_alloc         -> rstack memory-allocation (mword/byte/bit resolution)

// mem_bstruct_free         -> free(bs-1) wrapper (track stats)
// mem_dyn_free             -> undoes mem_dyn_alloc
// mem_rstack_free          -> undoes mem_rstack_alloc

// mem_newval               -> wrapper allocates new val-array
// mem_newptr               -> wrapper allocates new ptr-array
// mem_newcons              -> wrapper allocates new ptr-array (size=2)
// mem_newtptr              -> wrapper allocates new tptr
// mem_newstr               -> wrapper allocates new str

#endif //MEM_H

// Clayton Bauman 2016

