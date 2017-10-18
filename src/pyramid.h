// pyramid.h
//
// Every .c file in the Pyramid project should include this file

#ifndef PYRAMID_H
#define PYRAMID_H

/*****************************************************************************
 *                                                                           *
 *                         PREPROCESSOR SWITCHES                             *
 *                                                                           *
 ****************************************************************************/

#define PYRAMID_WINDOWS_BUILD

#define DEV_MODE
#define COMPAT_MODE
//#define MEM_DEBUG
//#define PROF_MODE
//#define CHK_MODE

//#define INTERP_RESET_TRACE
//#define INTERP_CORE_TRACE
//#define BPDL_TRACE
//#define BPDL2_TRACE
//#define GC_TRACE
//#define PYR_INTROSPECT_SHOW_NIL

/*****************************************************************************
 *                                                                           *
 *                              INCLUDES                                     *
 *                                                                           *
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#ifdef PYRAMID_WINDOWS_BUILD
#define WINVER 0x0500
#include <windows.h>
#endif

#include "tags.h"
#include "symbols.h"
#include "flags.h"

/*****************************************************************************
 *                                                                           *
 *                         CONSTANTS AND UNITS                               *
 *                                                                           *
 ****************************************************************************/

#if UINTPTR_MAX == 0xffffffff
#define PYRAMID_32_BIT
#elif UINTPTR_MAX == 0xffffffffffffffff
#define PYRAMID_64_BIT
#endif

// exit flags
#define NORMAL_EXIT_CODE    0                                   // NORMAL_EXIT_CODE#
#define DIE_EXIT_CODE       1                                   // DIE_EXIT_CODE#

// jmpbuf flags
#define CAT_EXCEPT          1                                   // CAT_EXCEPT#
#define INTERP_RESET        2                                   // INTERP_RESET#
#define OP_RESTART          4                                   // OP_RESTART#

// dimensions and units-conversion
#define BITS_PER_BYTE 8                                         // BITS_PER_BYTE#
#define BYTE_MAX 255                                            // BYTE_MAX#

#define MWORD_SIZE sizeof(mword)                                // MWORD_SIZE#
#define MWORD_BIT_SIZE (MWORD_SIZE * BITS_PER_BYTE)             // MWORD_BIT_SIZE#
#define HALF_MWORD_BIT_SIZE (MWORD_BIT_SIZE/2)                  // HALF_MWORD_SIZE#
#define MWORD_MSB (MWORD_BIT_SIZE-1)                            // MWORD_MSB#
#define MSB_MASK ((mword)1<<MWORD_MSB)                          // MSB_MASK#
#define MWORD_LSB (mword)0                                      // MWORD_LSB#

#define NEG_ONE ((mword)-1)                                     // NEG_ONE#
#define FMAX    NEG_ONE                                         // FMAX#

#define UNITS_MTO8(x) ((x)*MWORD_SIZE)                          // UNITS_MTO8#
#define UNITS_8TOM(x) ((x)/MWORD_SIZE)                          // UNITS_8TOM#

#define UNITS_8TO1(x) ((x)*BITS_PER_BYTE)                       // UNITS_8TO1#
#define UNITS_1TO8(x) ((x)/BITS_PER_BYTE)                       // UNITS_1TO8#

#define UNITS_MTO1(x) ((x)*MWORD_BIT_SIZE)                      // UNITS_MTO1#
#define UNITS_1TOM(x) ((x)/MWORD_BIT_SIZE)                      // UNITS_1TOM#

#define MODULO_MTO1(x) ((x)%MWORD_BIT_SIZE)                     // MODULO_MTO1#
#define MODULO_MTO8(x) ((x)%MWORD_SIZE)                         // MODULO_MTO8#
#define MODULO_8TO1(x) ((x)%BITS_PER_BYTE)                      // MODULO_8TO1#

#define COMPLEMENT_MTO1(x) (MWORD_BIT_SIZE-(x))                 // COMPLEMENT_MTO1#

#define BIT_RANGE(hi,lo) ((FMAX >> (MWORD_MSB-(hi))) & (FMAX << (lo))) // BIT_RANGE#
#define BIT_MASK(mword, hi, lo) (mword & BIT_RANGE(hi, lo))            // BIT_MASK#
#define BIT_SELECT(mword, hi, lo) (BIT_MASK(mword, hi, lo) >> lo)      // BIT_SELECT#
#define HI_BITS(mword, index) BIT_SELECT(mword, MWORD_MSB, index)      // HI_BITS#
#define LO_BITS(mword, index) BIT_SELECT(mword, index, 0)              // LO_BITS#

#define NBIT_HI_MASK(n) (((n)==0) ? 0 : (FMAX << (MWORD_BIT_SIZE-(n)))) // NBIT_HI_MASK#
#define NBIT_LO_MASK(n) (((n)==0) ? 0 : (FMAX >> (MWORD_BIT_SIZE-(n)))) // NBIT_LO_MASK#

#define MWORD_MUX(A, B, sel) (((A) & sel) | ((B) & (~sel)))             // MWORD_MUX#
#define BIT_MERGE(A, B, sel) ldv(A,0) = MWORD_MUX(B, rdv(A,0), sel);    // BIT_MERGE#

#define MWORD_SHIFT(A, n) ((n>0) ? (A << (n)) : (A >> (abs(n))))
#define MWORD_ROTATE(A, n) ((A << n) | (A >> (MWORD_MSB-n)))

#define MASK_1_BYTE (mword)0xff                                        // MASK_1_BYTE#
#define MASK_1_BIT  (mword)0x01                                        // MASK_1_BIT#

#define HASH_BIT_SIZE (mword)128                                       // HASH_BIT_SIZE#
#define HASH_BYTE_SIZE (mword)(HASH_BIT_SIZE/BITS_PER_BYTE)            // HASH_BYTE_SIZE#
#define HASH_SIZE (mword)UNITS_1TOM(HASH_BIT_SIZE)                     // HASH_SIZE#
#define HASH_ALLOC_SIZE (HASH_SIZE+1)                           // HASH_ALLOC_SIZE#

#define TAG_SIZE (HASH_SIZE*MWORD_SIZE)                         // TAG_SIZE#
#define INTERP_TAG_SIZE (mword)1                                       // INTERP_TAG_SIZE#

// NB: Can use INTERP_TAG_SIZE only when certain we are operating on an
// interpreter-generated tag

// +3 -> s-field + s-field + car
#define TPTR_SIZE (HASH_SIZE+3)                                 // TPTR_SIZE#
#define TPTR_ALLOC_SIZE TPTR_SIZE                               // TPTR_ALLOC_SIZE#

#define TPTR_TAG_OFFSET  (mword)0                                      // TPTR_TAG_OFFSET#
#define TPTR_PTR_OFFSET  (HASH_SIZE+1)                          // TPTR_PTR_OFFSET#

#define TPTR_SFIELD (mword)0 // TPTR_SFIELD#

#define CTL_MASK (MWORD_SIZE-1)                                 // CTL_MASK#

// Placeholders indicating non-initialization
#define UNINIT_VAL_32 0xdeadbeef // UNINIT_VAL_32#
#define UNINIT_PTR_32 nil // UNINIT_PTR_32#

#define UNINIT_VAL_64 0xabadfaceabaddeed // UNINIT_VAL_64#
#define UNINIT_PTR_64 nil // UNINIT_PTR_64#

#define EMPTY_CSTRING ""

/*****************************************************************************
 *                                                                           *
 *                             TYPEDEFS                                      *
 *                                                                           *
 ****************************************************************************/

// mword#
#ifdef PYRAMID_32_BIT
typedef uint32_t mword;
#define UNINIT_VAL UNINIT_VAL_32
#define UNINIT_PTR UNINIT_PTR_32
#endif

#ifdef PYRAMID_64_BIT
typedef uint64_t mword;
#define UNINIT_VAL UNINIT_VAL_64
#define UNINIT_PTR UNINIT_PTR_64
#endif

typedef mword* bstruct; // bstruct#

typedef mword* blob; // blob#

// std types:
typedef blob ptr;  // ptr-array ptr#
typedef blob val;  // val-array val#
typedef blob val8; // byte val-array val8#
typedef blob val1; // byte val-array val1#
typedef blob tptr; // tagged-pointer tptr#
typedef blob cptr; // C-style (unsafe) pointer cptr#
typedef blob ls;   // linked-list ls#
typedef blob dls;  // doubly linked-list dls#
typedef blob pyr;  // nested ptr-array pyr#
typedef blob mat;  // ls-based matrix mat#
typedef blob aop;  // array-of-pairs aop#
typedef blob sap;  // sorted aop sap#
typedef blob tens; // tensor tens#
typedef blob bstr; // Babel-string (mword) bstr#
typedef blob hash; // HASH_SIZE hash value hash#
typedef blob bsig; // bstruct-signature bsig#
typedef blob map;  // hash-table map#
typedef blob hist; // map-based histogram hist#
typedef blob hista; // aop-based histogram hista#
typedef blob lut;  // map-based lookup-table lut#
typedef blob luta; // aop-based lookup-table luta#
typedef blob tmap; // tag-only map tmap#
typedef blob ns;   // namespace ns#
typedef blob dir;  // directory dir#

typedef blob u8str; // utf-8 encoded byte-string u8str#
typedef blob sexpr; // ls-based s-expression sexpr#
typedef blob aexpr; // array-based s-expression aexpr#
typedef blob enuma; // aop-based enumeration enuma#

typedef blob tree;  // binary tree data-structre tree#
typedef blob ntree; // aop-based n-ary tree data-structure ntree#
typedef blob graph; // undirected graph data-structure graph#
typedef blob digraph; // directed graph data-structure digraph#
typedef blob hygraph; // hyper-graph data-structure hygraph#
typedef blob adjmat;  // adjacency-matrix adjmat#

// eval-related blobs
typedef blob sym;  // general-purpose symbol sym#
typedef blob fn;   // applicable function fn#
typedef blob args; // argument list args#
typedef blob qu;   // quoted expression qu#
typedef blob qq;   // quasi-quoted expression qq#
typedef blob mac;  // macro form mac#

typedef blob thunk;  // delayed-eval thunk#
    // list of thunks:
    //      - loadable
    //      - unloadable
    //      - evalable
    //      - stdlib (load)
    //      ...

// 32-bit versus 64-bit string-formats
#ifdef PYRAMID_WINDOWS_BUILD
#ifdef PYRAMID_32_BIT
#define dpr "%I32d"
#define xpr "%I32x"
#define x0pr "%08I32x"
#endif
#ifdef PYRAMID_64_BIT
#define dpr "%I64d"
#define xpr "%I64x"
#define x0pr "%016I64x"
#endif
#else // !PYRAMID_WINDOWS_BUILD
#ifdef PYRAMID_32_BIT
#define dpr "%d"
#define xpr "%x"
#define x0pr "%08x"
#endif
#ifdef PYRAMID_64_BIT
#define dpr "%ld"
#define xpr "%lx"
#define x0pr "%016lx"
#endif
#endif

// enums
typedef enum flag_val_enum {CLR, SET, IGN} flag_val; // flag_val#
typedef enum access_size_sel_enum {BIT_ASIZE, BYTE_ASIZE, MWORD_ASIZE} access_size_sel; // access_size_sel#
typedef enum sort_type_enum {UNSIGNED, SIGNED, ALPHA_MWORD, ALPHA_BYTE, LEX_MWORD, LEX_BYTE, VAL, CUSTOM} sort_type; // sort_type#
typedef enum fileout_type_enum {OVERWRITE, APPEND} fileout_type; // fileout_type_enum#

typedef void hash_fn_ptr(char*, char*, char*, mword); // hash_fn_ptr#

// base-memory page size (in mwords)
#define mem_thread_base_page_size 32

typedef struct { // mem_thread_base#

    mword *base_ptr;
    mword *current_page;
    mword current_offset;

    // statistics
    mword alloc_count;
    mword free_count;

} mem_thread_base; 

        /////////////////////////////////////////////////////////
        // OLD OLD OLD OLD OLD OLD OLD OLD OLD OLD OLD OLD OLD //
        // ||| ||| ||| ||| ||| ||| ||| ||| ||| ||| ||| ||| ||| //
        // vvv vvv vvv vvv vvv vvv vvv vvv vvv vvv vvv vvv vvv //
        /////////////////////////////////////////////////////////

typedef struct { // alloc_bank#

    mword *base_ptr;
    mword *bound_ptr;
    mword size;

    mword *alloc_ptr;

} alloc_bank; 


typedef struct { // mem_context#

    alloc_bank *primary;
    alloc_bank *secondary;

    mword alloc_count;
    mword free_count; // XXX This doesn't really make sense...

} mem_context;


typedef struct { // interp_limits#

    mword max_num_sys_threads;      // Checked when forking
    mword max_num_steps;            // Checked against global_tick_count on operator boundary
    mword max_bytes_mem_alloc;      // Checked during increment_alloc()
    mword max_num_file_descriptors; // Checked whenever opening I/O
    mword max_num_file_access;      // Checked whenever calling I/O function
    mword max_run_time_ms;          // Checked against epoch on each operator boundary

} interp_limits;


typedef struct { // interp_privileges#

    flag_val FILE_READ_ALLOWED;  // CLR disables file descriptor read
    flag_val FILE_WRITE_ALLOWED; // CLR disables file descriptor write
    flag_val SYS_EXEC_ALLOWED;   // CLR disables system() calls
    flag_val BVM_EXEC_ALLOWED;   // CLR disables babel operator
    flag_val BVM_FORK_ALLOWED;   // CLR disables fork operator

} interp_privileges;


typedef struct { // interp_flags#

    flag_val MEM_ALLOC_BLOCKING;
    flag_val MEM_ALLOC_NON_GC;

    flag_val MEM_GC_BLOCKING;
    flag_val MEM_GC_INTERP_BLOCKING;

    flag_val MEM_GC_PENDING;
    flag_val MEM_GC_CONVERT_PENDING_TO_OP_RESTART;
    flag_val MEM_GC_PNR;
    flag_val MEM_GC_OP_RESTART;
    flag_val MEM_GC_SECONDARY_BANK_ALLOC;
    flag_val MEM_GC_ON_EVERY_OP; // XXX debug flag XXX

    flag_val MEM_USE_MALLOC;
    flag_val MEM_USE_DYN;

    flag_val INTERP_BOOT_IN_PROGRESS;
    flag_val INTERP_NO_ASYNC;
    flag_val INTERP_NO_EXCEPT;

    flag_val PYR_INSTR_IN_PROGRESS;
    flag_val PYR_INCOHERENT;
    flag_val PYR_SYMBOLS_DEFINED;
    flag_val PYR_CLEAN;
    flag_val PYR_INTERP_OP_TRACE;
    flag_val PYR_INSTRUMENT_TRIGGER;
    flag_val PYR_CODE_LIST_EMPTY;
    flag_val PYR_RETURN_TOS_ON_EXIT;

    flag_val PYR_CACHE_INVALID;
    flag_val PYR_CACHE_DIRTY;
    flag_val PYR_CACHE_ONLY;
    flag_val PYR_CACHE_BLOCKING;

} interp_flags;


#ifdef PROF_MODE
typedef struct {

    mword PYR_PROFILE_ENABLE;   // enable/disable profiling
    mword operator_ms;          // milliseconds spent executing operators

    mword GC_ms;                // milliseconds spent in GC
    mword GC_overshoot;         // amount of memory allocated after signalling MC_GC_PENDING

    mword cache_flush_count;    // number of cache flushes
    mword cache_update_count;   // number of cache updates

    mword mem_alloc_count;      // number of mem_alloc calls
    mword mem_alloc_total;
    mword mem_alloc_largest;

    mword stack_pops;           // number of dstack pops
    mword stack_pushes;         // number of dstack pushes

    mword interp_boot_time;     // time from interp_init to user-code (use dev-code to signal finish)

} pyr_profile;
#endif

typedef struct { // interp_builtins#
    mword *PYR_TAG_ZERO_HASH;
#define X(a,b,c,d,e,f,g,h,i,j,k,l) mword *a;
PYR_TAGS
#undef X
} interp_builtins;


typedef struct { // interp_tags#
    mword *PYR_TAG_ZERO_HASH;
#define X(a,b,c,d,e,f,g,h,i,j,k,l) mword *a;
PYR_TAGS
#undef X
} interp_tags;


typedef struct { // interp_strings#
#define X(a,b,c,d,e,f,g,h,i,j,k,l) mword *a;
PYR_TAGS
#undef X
} interp_strings;


typedef struct { // interp_symbols#
#define X(a,b) mword *a;
PYR_SYMBOLS
#undef X
} interp_symbols;


typedef struct { // pvc_xbar#

    interp_tags             *tags;
    interp_strings          *strings;
//    interp_fns              *fns;
//    interp_xlats            *xlats;

    mword                   *xbar_table;

} pvc_xbar;


/////////////////////////
// PYRAMID VIRTUAL CPU //
/////////////////////////
typedef struct { // pyr_cpu#

    mword *code_block;
    mword *rstack_base_ptr;
    mword *rstack_ptr;
    mword step;

    mword *noun_table;
    mword *verb_table;
    mword *std_code_table;
    mword *user_code_table;
    mword *reg_file;
    mword *rstack;
    mword *cpu_env;
    void  *jit_tcc;
    void  *parent_env;

} pyr_cpu;


///////////////////////////////////
// PYRAMID VIRTUAL MACHINE CACHE //
///////////////////////////////////
typedef struct { // pyr_cache#

    mword           *self;

    pyr_cpu         *cpu;
//    mem_context     *mem;

    mword           *pyr_cpu_array;
//    interp_flags    *flags;
//    interp_runtime  *interp;

} pyr_cache;

typedef pyr_cache *(*pyramid_op)(pyr_cache *); // pyramid_op#
typedef blob (*pyr_op)(pyr_cache*, blob); // pyr_op#

typedef struct { // interp_fns#
#define X(a,b,c,d,e,f,g,h,i,j,k,l)  pyr_op a;
PYR_TAGS
#undef X
} interp_fns;


///////////////////////////////////
// PYRAMID INTERPRETER RUNTIME   //
///////////////////////////////////
typedef struct { // interp_runtime

#ifdef PROF_MODE
    pyr_profile *profile;
#endif

    mem_thread_base         *sys_mem;
    mem_context             *gc_mem;

    interp_tags             *tags;
    interp_strings          *strings;
    interp_fns              *fns;
//    interp_templates        *templates;
//    interp_cachers          *cachers;
//    interp_uncachers        *uncachers;

//    mword                   *tags_strings;
//    mword                   *tags_fns;
    mword                   *xbar;          // sorted by tag

    interp_symbols          *symbols;
    interp_flags            *flags;         // XXX not to be confused with pyr_vm_flags
    interp_limits           *limits;
    interp_privileges       *privileges;

    mword                   *noun_table;
    mword                   *verb_table;

    jmp_buf                 *cat_ex;
    jmp_buf                 *op_restart;
    int                      argc;
    char                   **argv;
    mword                   *interp_argv;
    char                   **envp;

    mword                   *nil;            // nil#
    mword                   *empty_string;

    struct tm               *utc_epoch;
    mword                   *epoch_ms;
    mword                   *srand;
    mword                   *dispatch_table;

    mword                    thread_id;
    mword                    thread_counter;
    mword                    global_tick_count;

    hash_fn_ptr             *hash_fn;

    mword                   *env;

} interp_runtime;


/*****************************************************************************
 *                                                                           *
 *                              GLOBALS                                      *
 *                                                                           *
 ****************************************************************************/

interp_runtime *global_irt;                     // global_irt#
#define nil global_irt->nil
pyr_op UNINIT_FN_PTR;

// dev-mode globals ... 
// XXX code must be able to build with !defined(DEV_MODE) !!! XXX
#ifdef DEV_MODE

mword *global_dev_overrides;                    // global_dev_overrides#
mword GLOBAL_BVM_INSTRUMENT_TRIGGER;            // For use with instrument.pl
mword *global_dev_ptr;                          // general-purpose global pointer

mword *global_dev_abnormal_ctr;                 // used by _abnormal

// dev-mode flags/commandline-args
int   global_dev_bare_metal;                    // selects entrance to bare-metal prompt on startup
char *global_dev_seed;                          // allow specifying seed from command-line (gets hashed)
char *global_dev_srand;                         // allow specifying srand from command-line (does not get hashed)

#define FLAG_IGN    ((mword)-1) // FLAG_IGN#
#define FLAG_SET    1 // FLAG_SET#
#define FLAG_CLR    0 // FLAG_CLR#
#define TOGGLE_FLAG(x) (((x) == FLAG_SET) ? (x = FLAG_CLR) : (x = FLAG_SET)) // XXX: DO NOT USE ON FLAG_IGN

#endif

/*****************************************************************************
 *                                                                           *
 *                            ACCESSORS                                      *
 *                                                                           *
 ****************************************************************************/

/////// sfield accessor ///////
#define sfield(x) (*((mword*)x-1))      // sfield#

/////// pointer-deref accessors ///////

//RHS dereference for ptr array
#define rdp(x,y) (*((mword**)x+y))      // rdp#

//RHS dereference for val array
#define rdv(x,y) (*((mword*)x+y))       // rdv#

//LHS dereference for ptr array
#define ldp(x,y) (*((mword**)x+y))      // ldp#

//LHS dereference for val array
#define ldv(x,y) (*((mword*)x+y))       // ldv#

/////// Lisp-style accessors ///////
#define vcar(x) ((mword)rdv(x,0))       // vcar#
#define vcdr(x) ((mword)rdv(x,1))       // vcdr#
#define vcpr(x) ((mword)rdv(x,2))       // vcpr#

#define pcar(x) ((mword*)rdp(x,0))      // pcar#
#define pcdr(x) ((mword*)rdp(x,1))      // pcdr#
#define pcpr(x) ((mword*)rdp(x,2))      // pcpr#

//list-safe car/cdr (not tptr safe):
#define lcar(x)     (is_nil(x) ? nil : pcar(x)) // lcar#
#define lcdr(x)     (is_nil(x) ? nil : pcdr(x)) // lcdr#

/////// tptr accessors ///////
#define tcar(x) ((mword*)rdp(x,TPTR_PTR_OFFSET))                    // tcar#
#define tptr_set_tag(dest,src) tagcpy(dest,src)                     // tptr_set_tag#
#define tptr_set_ptr(dest,src) (ldp(dest,TPTR_PTR_OFFSET) = src)    // tptr_set_ptr#

/////// generic bstruct accessors ///////
#define rbs(x,y,z) (sfield(x)>0) ? ((z)=rdv(x,y)) : ((sfield(x)<0) ? ((z)=rdp(x,y)) : ((z)=read_thunk(this_pyr,x,y)))
#define wbs(x,y,z) (sfield(x)>0) ? (ldv(x,y)=(z)) : ((sfield(x)<0) ? (ldp(x,y)=(z)) : (write_thunk(this_pyr,x,y,z)))

/////// relative offset accessors ///////
#define rel_to_abs(base,rel) (mword*)(base+UNITS_8TOM((mword)rel))
#define abs_to_rel(base,abs) (mword*)(base-UNITS_8TOM((mword)abs))

#define rrdp(base,x,y)  (*((mword*)(base+(UNITS_8TOM(x)))+y))
#define rldp(base,x,y)  (*((mword*)(base+(UNITS_8TOM(x)))+y))

#define rrdv(base,x,y)  (*((mword*)(base+(UNITS_8TOM(x)))+y))
#define rldv(base,x,y)  (*((mword*)(base+(UNITS_8TOM(x)))+y))

#define rtcar(base,x)  ((mword*)rrdp(base,x,TPTR_PTR_OFFSET))

////relative offest value
//#define rov(x,y) (*(mword*)((char*)x[0]+x[1]+(UNITS_MTO8(y))))  // rov#
//
////traverse to relative offest for ptr
//#define trop(x,y) (x[1] = rov(x,y))                     // trop#
//
////traverse to relative offest for tptr
//#define trot(x) (x[1] = rov(x,TPTR_PTR_OFFSET))         // trot#
//
////relative offest sfield
//#define ros(x) (rov(x,-1))                              // ros#


/////// paged-array accessors ///////
#define sfield_pa(pa)     (rdp(pa,0))
#define pgsize_pa(pa)     (rdp(pa,1))
#define pages_pa(pa)      (rdp(pa,2))
#define rdv_pa(pa,offset) (rdv(rdp(pages_pa(pa),(offset/(*pgsize_pa(pa)))),(offset%(*pgsize_pa(pa)))))
#define rdp_pa(pa,offset) (rdp(rdp(pages_pa(pa),(offset/(*pgsize_pa(pa)))),(offset%(*pgsize_pa(pa)))))
#define ldv_pa(pa,offset) (ldv(rdp(pages_pa(pa),(offset/(*pgsize_pa(pa)))),(offset%(*pgsize_pa(pa)))))
#define ldp_pa(pa,offset) (ldp(rdp(pages_pa(pa),(offset/(*pgsize_pa(pa)))),(offset%(*pgsize_pa(pa)))))

/////// aop accessors ///////
#define  key_aopv(x,y) vcar(pcar(rdp(x,y)))
#define data_aopv(x,y) vcar(pcar(rdp(x,y)))

#define  key_aop(x,y) pcar(rdp(x,y))
#define data_aop(x,y) pcar(rdp(x,y))

/*****************************************************************************
 *                                                                           *
 *                            PREDICATES                                     *
 *                                                                           *
 ****************************************************************************/

#define is_nil(x)       ( tageq(x,nil,TAG_SIZE) )                   // is_nil#
#define is_nil_fast(x)  ( itageq(x,nil) )                           // is_nil_fast#

#define is_val(x)    ((int)sfield((mword*)x) >  0)                  // is_val#
#define is_ptr(x)    ((int)sfield((mword*)x) <  0)                  // is_ptr#
#define is_tptr(x)   ((int)sfield((mword*)x) == 0)                  // is_tptr#
#define is_cptr(x)   tageq((x), global_irt->tags->PYR_TAG_CPTR, TAG_SIZE) // is_cptr#

#define is_val_sfield(x)    (((int)x) > 0) // is_val_sfield#
//#define is_val(x)    is_val_sfield(sfield((mword*)x))

#define is_tptr_spec(x)   (!is_val(x) && !is_ptr && !is_nil(x))     // is_tptr_spec#

#define is_conslike(x) (is_ptr(x) && size(x) == 2)                  // is_conslike#

#define is_traversed_U(x)       (!((sfield(x) & 0x1)==0))           // is_traversed_U#
#define is_traversed_V(x)       (!((sfield(x) & 0x2)==0))           // is_traversed_V#
#define is_traversed_U_or_V(x)  (!((sfield(x) & CTL_MASK)==0))      // is_traversed_U_or_V#
#define is_traversed_U_and_V(x) (!((sfield(x) & 0x3)==0))           // is_traversed_U_and_V#

#define is_val_masked(x)     ((~CTL_MASK & (int)sfield((mword*)x)) >  0) // is_val_masked#
#define is_ptr_masked(x)     ((~CTL_MASK & (int)sfield((mword*)x)) <  0) // is_ptr_masked#
#define is_tptr_masked(x)    ((~CTL_MASK & (int)sfield((mword*)x)) == 0) // is_tptr_masked#

#define is_false_tptr(tptr)                                 \
           (tageq(tptr, PYR_TAG_FALSE     , TAG_SIZE)       \
        ||  tageq(tptr, PYR_TAG_REJECT    , TAG_SIZE)       \
        ||  tageq(tptr, PYR_TAG_UNEXIST   , TAG_SIZE)       \
        ||  tageq(tptr, PYR_TAG_INTERP_NIL, TAG_SIZE))

//X(PYR_TAG_NIL              
//X(PYR_TAG_INTERP_NIL       
//X(PYR_TAG_EXIST            
//X(PYR_TAG_UNEXIST          
//X(PYR_TAG_DEF              
//X(PYR_TAG_UNDEF            
//X(PYR_TAG_TRUE             
//X(PYR_TAG_FALSE            
//X(PYR_TAG_ACCEPT           
//X(PYR_TAG_REJECT           

/*****************************************************************************
 *                                                                           *
 *                              PRIMITIVES                                   *
 *                                                                           *
 ****************************************************************************/

// size is in units of MWORDS
#define cpy(dest,src,size) memcpy(dest,src,UNITS_MTO8(size)) // cpy#

#define val_gt(left,right) (array_cmp_num(left, right) >  0)
#define val_ge(left,right) (array_cmp_num(left, right) >= 0)
#define val_lt(left,right) (array_cmp_num(left, right) <  0)
#define val_le(left,right) (array_cmp_num(left, right) <= 0)
#define val_eq(left,right) (array_cmp_num(left, right) == 0)
#define val_ne(left,right) (array_cmp_num(left, right) != 0)

#define ptr2val(x) sfield(x) = abs(sfield(x))

// FIXME sed -i to choose_max(), choose_min()
#define MAX(a,b) ((a>=b) ? (a) : (b)) // MAX#
#define MIN(a,b) ((a<=b) ? (a) : (b)) // MIN#

#define floor_clamp(x,y) ((x) < (y)) ? (y) : (x); // floor_clamp#
#define ceil_clamp(x,y)  ((x) > (y)) ? (y) : (x); // ceil_clamp#

// returns true if x>=y and x<=z
#define in_bounds(x,y,z) (!(((x) < (y)) || ((x) > (z)))) // in_bounds#

//#define TOGGLE_FLAG(x) (((x) == IGN) ? (x = IGN) : (((x) == SET) ? (x = CLR) : (x = SET))) // TOGGLE_FLAG#

#define tagcmp(x,y,z) ( (is_tptr(x) || (size(x) >= HASH_SIZE)) ? (memcmp((mword*)x, y, z)) : -1 ) // tagcmp#
#define tageq(x,y,z)  ( tagcmp(x,y,z) == 0 ) // tageq#
#define itageq(x,y)   ( tagcmp(x,y,INTERP_TAG_SIZE) == 0 ) // itageq#
#define tagcpy(dest,src) cpy(dest,src,HASH_SIZE) // tagcpy#

#define mark_traversed_U(x) (sfield(x) |= 0x1) // mark_traversedU#
#define mark_traversed_V(x) (sfield(x) |= 0x2) // mark_traversedV#

#define size(x)             (abs(sfield(x))/MWORD_SIZE)                 // size#
#define size_special(x)     (sfield(x) == 0 ? HASH_SIZE : size(x))      // size_special#
#define alloc_size(x)       (sfield(x) == 0 ? TPTR_SIZE : size(x)+1)    // alloc_size#
#define mem_alloc_size(x)   (x == 0 ? TPTR_SIZE : (abs(x)/MWORD_SIZE))  // mem_alloc_size#
#define size_masked(x)      (abs(~CTL_MASK & sfield(x))/MWORD_SIZE)     // size_masked#

#define _mktptr(pyr,key,bs) mem_new_tptr(pyr,HASHC(pyr,key),bs)         // _mktptr#

#define mem_new_valz(pyr,size) mem_new_val(pyr, size, 0) // mem_new_valz#
#define mem_sys_free_bs(bs,size) mem_sys_free(bs-1,size) // mem_sys_free_bs#

#define HASHA_FORM(hash_fn,pyr,str,strlen) (hash_fn( pyr, (char*)global_irt->tags->PYR_TAG_ZERO_HASH, (char*)str, strlen ))

// HASHI# --> Hash a constant string (non-allocating)
// HASHC# --> Hash a constant string (allocating)
// HASH#  --> Hash a Babel string    (allocating)
// HASHM# --> Hash a val-array       (allocating)

#ifdef COMPAT_MODE
#define HASHI(result,str) (pearson16( (char*)result, (char*)global_irt->tags->PYR_TAG_ZERO_HASH, (char*)str, STRLEN(str) ))
#define HASHC(pyr,str)    HASHA_FORM(pearson16a, pyr, str, STRLEN(str) )
#define HASH(pyr,str)     HASHA_FORM(pearson16a, pyr, str, array8_size(pyr,str) )
#define HASHM(pyr,str)    HASHA_FORM(pearson16a, pyr, str, UNITS_MTO8(array_size(pyr,str)) )
#else
#define HASHI(result,str) (pearson_marsaglia16( (char*)result, (char*)global_irt->tags->PYR_TAG_ZERO_HASH, (char*)str, STRLEN(str) ))
#define HASHC(pyr,str)    HASHA_FORM(pearson_marsaglia16a, pyr, str, STRLEN(str) )
#define HASH(pyr,str)     HASHA_FORM(pearson_marsaglia16a, pyr, str, array8_size(pyr,str) )
#define HASHM(pyr,str)    HASHA_FORM(pearson_marsaglia16a, pyr, str, UNITS_MTO8(array_size(pyr,str) )
#endif

#define HASH8(pyr,str) (pearson_hash8(pyr,string_c2b(this_pyr, str, STRLEN(str)))) // HASH8#

// signature is HASHM of unloaded bstruct

#define C2B(str) (string_c2b(this_pyr, str, STRLEN(str))) // C2B#

/*****************************************************************************
 *                                                                           *
 *                                 TAGS                                      *
 *                                                                           *
 ****************************************************************************/

#define PYR_NUM_TAGS (sizeof(interp_tags)/MWORD_SIZE)

/*****************************************************************************
 *                                                                           *
 *                            MISC UTILITIES                                 *
 *                                                                           *
 ****************************************************************************/

#define STRLEN(s) (sizeof(s)-1) // STRLEN#

#define _prn(x)         fprintf(stderr, "%s", x); // _prn#
#define _say(x)         fprintf(stderr, "%s\n", x);   // _say#
#define _notify(x)      fprintf(stderr, "PYRAMID: %s\n", x);   // _say#
#define _msg(x)         fprintf(stderr, "PYRAMID: %s in %s(), %s line %d\n", x, __func__, __FILE__, __LINE__);   // _msg#
#define _trace          fprintf(stderr, "PYRAMID: %s() in %s line %d\n", __func__, __FILE__, __LINE__);   // _trace#
#define _reset_trace    fprintf(stderr, "INTERP_RESET_TRACE: %s() in %s line %d\n", __func__, __FILE__, __LINE__);   // _trace#

#define QUOTEME(x)      #x
//#define _d(x)           fprintf(stderr, "%s %08x\n", QUOTEME(x), (mword)x);
#define _d(x)           fprintf(stderr, "%s " x0pr "\n", QUOTEME(x), (mword)x); // d#
//#define _dd(x)          fprintf(stderr, "%s %d\n",   QUOTEME(x), (mword)x);
#define _dd(x)          fprintf(stderr, "%s " dpr "\n",   QUOTEME(x), (mword)x); // dd#
#define _dw(x)          fprintf(stderr, "%s %08x ",  QUOTEME(x), (mword)x); // dw#

#define _die            fprintf(stderr, "Died at %s line %d\n", __FILE__, __LINE__); exit(DIE_EXIT_CODE);  // die#

#define _warn(x)        fprintf(stderr, "WARNING: %s in %s() at %s line %d\n", x, __func__, __FILE__, __LINE__);  // warn#
#define _error(x)       fprintf(stderr, "ERROR: %s in %s() at %s line %d\n", x, __func__, __FILE__, __LINE__); // error#
#define _fatal(x)       fprintf(stderr, "FATAL: %s in %s() at %s line %d\n", x, __func__, __FILE__, __LINE__); _die;  // _fatal#
#define _pigs_fly       _fatal("Pigs CAN fly...") // _pigs_fly#
#define _give_up        _fatal("gave up... don't know how to do this") // _give_up#
#define _enhance(x)     fprintf(stderr, "ENHANCEMENT: %s in %s at %s line %d\n", x, __func__, __FILE__, __LINE__); // enhance#
#define _debug(x)       util_bare_metal_prompt(this_pyr, x);

#ifdef DEV_MODE
#define _abnormal       global_dev_abnormal_ctr++;
#else
#define _abnormal       ;
#endif

#define _timestamp(x)   (time_ms(void) - x->interp->epoch_ms) // _timestamp#

#define _break(x)       util_bare_metal_prompt(this_pyr, x);

//#define _cat_except(x)    _trace; longjmp(*(x->interp->cat_ex),CAT_EXCEPT); // _cat_except#
//#define _interp_reset(x)          longjmp(*(x->interp->cat_ex),INTERP_RESET); // _interp_reset#
#define _cat_except(x)    _trace; longjmp(*(global_irt->cat_ex),CAT_EXCEPT); // _cat_except#
#define _interp_reset          longjmp(*(global_irt->cat_ex),INTERP_RESET); // _interp_reset#


/*****************************************************************************
 *                                                                           *
 *                                  PERF                                     *
 *                                                                           *
 ****************************************************************************/

// Increasing this parameter increases the amount of time spent in 
// array_linear_search() when calling array_search()
#define ARRAY_PERF_LINEAR_HOLDOFF 3 // ARRAY_PERF_LINEAR_HOLDOFF#


/*****************************************************************************
 *                                                                           *
 *                                  DEV                                      *
 *                                                                           *
 ****************************************************************************/

#ifdef DEV_MODE

FILE *dev_log; // dev_log#
int dev_i;     // dev_i#

#define _dump(x)                                                    \
    io_spit(this_pyr, "test.dot",                                   \
        _bs2gv(this_pyr,                                            \
            (x)),                                                   \
            BYTE_ASIZE,                                             \
            OVERWRITE);

// #define _mem#
#define _mem32(x)                                                     \
    fprintf(stderr, "-------- " x0pr "\n", sfield(x));                \
    for(dev_i=0; dev_i<alloc_size(x)-1; dev_i++){                     \
        if(dev_i>=0){                                                 \
            fprintf(stderr, x0pr " ", (unsigned)dev_i*MWORD_SIZE);    \
        }                                                             \
        fprintf(stderr, x0pr "\n", rdv(x,dev_i));                     \
    }

#define _mem64(x)                                                     \
    fprintf(stderr, "---------------- " x0pr "\n", sfield(x));    \
    for(dev_i=0; dev_i<alloc_size(x)-1; dev_i++){                     \
        if(dev_i>=0){                                                 \
            fprintf(stderr, x0pr " ", (unsigned)dev_i*MWORD_SIZE);    \
        }                                                             \
        fprintf(stderr, x0pr "\n", rdv(x,dev_i));                     \
    }

#ifdef PYRAMID_32_BIT
#define _mem _mem32
#endif

#ifdef PYRAMID_64_BIT
#define _mem _mem64
#endif

#endif


//#define _mem(x)
//    fprintf(stderr, "---- %08x\n", sfield(x));
//    for(dev_i=0; dev_i<alloc_size(x)-1; dev_i++){ 
//        if(dev_i>=0){
//            fprintf(stderr, "%04x ", (unsigned)dev_i*MWORD_SIZE); 
//        }            
//        fprintf(stderr, "%08x\n", rdv(x,dev_i));    
//    }



#endif //PYRAMID_H

// Clayton Bauman 2016


