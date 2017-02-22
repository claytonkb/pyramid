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

//#define TARGET LINUX 
#define TARGET WINDOWS 

#define DEV_MODE
#define COMPAT_MODE
//#define MEM_DEBUG
#define PROF_MODE
//#define CHK_MODE

//#define INTERP_RESET_TRACE
//#define INTERP_CORE_TRACE
//#define BPDL_TRACE
//#define BPDL2_TRACE
//#define GC_TRACE

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

#if TARGET == WINDOWS
#define WINVER 0x0500
#include <windows.h>
#endif

/*****************************************************************************
 *                                                                           *
 *                         CONSTANTS AND UNITS                               *
 *                                                                           *
 ****************************************************************************/

// exit flags
#define NORMAL_EXIT_CODE    0                                   // NORMAL_EXIT_CODE#
#define DIE_EXIT_CODE       1                                   // DIE_EXIT_CODE#

// jmpbuf flags
#define CAT_EXCEPT          1                                   // CAT_EXCEPT#
#define INTERP_RESET        2                                   // INTERP_RESET#
#define OP_RESTART          4                                   // OP_RESTART#

// dimensions and units-conversion
#define BITS_PER_BYTE 8                                         // BITS_PER_BYTE#

#define MWORD_SIZE sizeof(mword)                                // MWORD_SIZE#
#define MWORD_BIT_SIZE (MWORD_SIZE * BITS_PER_BYTE)             // MWORD_BIT_SIZE#
#define HALF_MWORD_SIZE (MWORD_SIZE/2)                          // HALF_MWORD_SIZE#
#define MWORD_MSB (MWORD_BIT_SIZE-1)                            // MWORD_MSB#
#define MWORD_LSB 0                                             // MWORD_LSB#

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

#define BIT_MASK(mword, upper, lower) (mword & (FMAX >> (MWORD_MSB-upper)) & (FMAX << lower))   // BIT_MASK#
#define BIT_SELECT(mword, upper, lower) (BIT_MASK(mword, upper, lower) >> lower)                // BIT_SELECT#
#define HI_BITS(mword, index) BIT_SELECT(mword, MWORD_MSB, index)                               // HI_BITS#
#define LO_BITS(mword, index) BIT_SELECT(mword, index, 0)                                       // LO_BITS#

#define NBIT_HI_MASK(n) (((n)==0) ? 0 : (FMAX << (MWORD_BIT_SIZE-(n))))      // NBIT_HI_MASK#
#define NBIT_LO_MASK(n) (((n)==0) ? 0 : (FMAX >> (MWORD_BIT_SIZE-(n))))      // NBIT_LO_MASK#
#define BIT_RANGE(hi,lo) ((FMAX >> (MWORD_BIT_SIZE-(hi))) & (FMAX << (lo)))  // BIT_RANGE#

#define MWORD_MUX(A, B, sel) (((A) & sel) | ((B) & (~sel)))
#define BIT_MERGE(A, B, sel) ldv(A,0) = MWORD_MUX(B, rdv(A,0), sel);

#define MASK_1_BYTE 0xff                                        // MASK_1_BYTE#
#define MASK_1_BIT  0x01                                        // MASK_1_BIT#

#define HASH_BIT_SIZE 128                                       // HASH_BIT_SIZE#
#define HASH_BYTE_SIZE (HASH_BIT_SIZE/BITS_PER_BYTE)            // HASH_BYTE_SIZE#
#define HASH_SIZE UNITS_1TOM(HASH_BIT_SIZE)                     // HASH_SIZE#
#define HASH_ALLOC_SIZE (HASH_SIZE+1)                           // HASH_ALLOC_SIZE#

#define TAG_SIZE (HASH_SIZE*MWORD_SIZE)                         // TAG_SIZE#
#define INTERP_TAG_SIZE 1                                       // INTERP_TAG_SIZE#

// NB: Can use INTERP_TAG_SIZE only when certain we are operating on an
// interpreter-generated tag

// +3 -> s-field + s-field + car
#define TPTR_SIZE (HASH_SIZE+3)                                 // TPTR_SIZE#
#define TPTR_ALLOC_SIZE TPTR_SIZE                               // TPTR_ALLOC_SIZE#

#define TPTR_TAG_OFFSET  0                                      // TPTR_TAG_OFFSET#
#define TPTR_PTR_OFFSET  (HASH_SIZE+1)                          // TPTR_PTR_OFFSET#

#define TPTR_SFIELD 0

#define UNINIT_VAL 0xdeadface                                   // Placeholder-value   indicating non-initialization
#define UNINIT_PTR nil                                          // Placeholder-pointer indicating non-initialization

#define CTL_MASK (MWORD_SIZE-1)                                 // CTL_MASK#

/*****************************************************************************
 *                                                                           *
 *                             TYPEDEFS                                      *
 *                                                                           *
 ****************************************************************************/

typedef unsigned mword; // mword#

typedef enum flag_val_enum {CLR, SET, IGN} flag_val; // flag_val#
typedef enum access_size_sel_enum {BIT_ASIZE, BYTE_ASIZE, MWORD_ASIZE} access_size_sel; // access_size_sel#

typedef void hash_fn_ptr(char*, char*, char*, mword);

typedef struct { // alloc_bank#

    mword *base_ptr;
    mword *bound_ptr;
    mword size;

    mword *alloc_ptr;

} alloc_bank; 


typedef struct { // mem_context#

    alloc_bank *primary;
    alloc_bank *secondary;

//    mword  mem_sys_alloc_count;
//    mword  mem_sys_alloc_total;
//
//    mword  GC_count;
//    mword  op_restart_alloc_size;
//    mword *op_restart_alloc_ptr;
//    mword  last_GC_tick_count;

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


typedef struct { // interp_state#

    jmp_buf      *cat_ex;
    jmp_buf      *op_restart;
    int           argc;
    char        **argv;
    char        **envp;
    mword        *interp_argv;
    mem_context  *mem;

    mword        *jump_table; // full jump_table

    mword        *nil;
    mword        *empty_string;

    mword        *cold_null_BVM;
    mword        *warm_null_BVM;

    struct tm    *utc_epoch;
    mword         epoch_ms;
    mword        *srand;

    mword         null_hash[HASH_SIZE];
    mword         thread_counter;
    mword         global_tick_count;

    hash_fn_ptr  *hash_fn;

    interp_limits     *limits;
    interp_privileges *privileges;

#ifdef PROF_MODE
    pyr_profile       *profile;
#endif

} interp_state;


typedef struct { // pyr_cache#

    mword *self;

    mword *code_ptr;

    mword *rstack_ptr;
    mword *dstack_ptr;

    mword *soft_root;
    mword *local_root;
    mword *local_path;
    mword *local_pwd;

    mword thread_id;
    mword bvm_initd;
    mword steps;
    mword advance_type;

    mword dstack_depth;
    mword dstack_diameter;

    interp_flags *flags;

    interp_state *interp;

} pyr_cache;

#define mem_thread_base_page_size 32

typedef struct { // mem_thread_base#

    mword *base_ptr;
    mword *current_page;
    mword current_offset;

    // statistics
    mword sys_alloc_count;
    mword sys_free_count;

} mem_thread_base; 


/*****************************************************************************
 *                                                                           *
 *                              GLOBALS                                      *
 *                                                                           *
 ****************************************************************************/

mword *nil;                                     // nil#
mword *GLOBAL_TAG_ZERO_HASH;                    // GLOBAL_TAG_ZERO_HASH#
mem_thread_base *global_this_thread_mem;        // global_this_thread_mem#

#ifdef DEV_MODE

mword *global_dev_overrides;                    // global_dev_overrides#
mword GLOBAL_BVM_INSTRUMENT_TRIGGER;            // For use with instrument.pl

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

#define pcar(x) ((mword*)rdp(x,0))      // pcar#
#define pcdr(x) ((mword*)rdp(x,1))      // pcdr#

/////// tptr accessors ///////

#define tcar(x) ((mword*)rdp(x,TPTR_PTR_OFFSET))        // tcar#

//#define set_tptr_tag(x,y) (ldv(x,TPTR_TAG_OFFSET) = y)  // set_tptr_tag#

#define tptr_set_tag(dest,src) tagcpy(dest,src)
#define tptr_set_ptr(dest,src) (ldp(dest,TPTR_PTR_OFFSET) = src)  // tptr_set_ptr#

/////// relative offset accessors ///////

//relative offest value
#define rov(x,y) (*(mword*)((char*)x[0]+x[1]+(UNITS_MTO8(y))))  // rov#

//traverse to relative offest for ptr
#define trop(x,y) (x[1] = rov(x,y))                     // trop#

//traverse to relative offest for tptr
#define trot(x) (x[1] = rov(x,TPTR_PTR_OFFSET))         // trot#

//relative offest sfield
#define ros(x) (rov(x,-1))                              // ros#

/*****************************************************************************
 *                                                                           *
 *                            PREDICATES                                     *
 *                                                                           *
 ****************************************************************************/

#define is_nil(x)       ( tageq(x,nil,TAG_SIZE) )                   // is_nil#

#define is_val(x)    ((int)sfield((mword*)x) >  0)                  // is_val#
#define is_ptr(x)    ((int)sfield((mword*)x) <  0)                  // is_ptr#
#define is_tptr(x)   ((int)sfield((mword*)x) == 0)                  // is_tptr#

#define is_traversed_U(x)       (sfield(x) & 0x1)                   // is_traversed_U#
#define is_traversed_V(x)       (sfield(x) & 0x2)                   // is_traversed_V#
#define is_traversed_U_or_V(x)  (sfield(x) & CTL_MASK)              // is_traversed_U_or_V#
#define is_traversed_U_and_V(x) (sfield(x) & 0x3)                   // is_traversed_U_and_V#

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

//#define TOGGLE_FLAG(x) (((x) == IGN) ? (x = IGN) : (((x) == SET) ? (x = CLR) : (x = SET))) // TOGGLE_FLAG#

#define tagcmp(x,y,z) ( (is_tptr(x) || (size(x) >= HASH_SIZE)) ? (memcmp((mword*)x, y, z)) : -1 ) // tagcmp#
#define tageq(x,y,z)  ( tagcmp(x,y,z) == 0 ) // tageq#
#define tagcpy(dest,src) cpy(dest,src,HASH_SIZE)

#define mark_traversed_U(x) (sfield(x) |= 0x1) // mark_traversedU#
#define mark_traversed_V(x) (sfield(x) |= 0x2) // mark_traversedV#

#define MAX(a,b) ((a>=b) ? (a) : (b)) // MAX#
#define MIN(a,b) ((a<=b) ? (a) : (b)) // MIN#

#define size(x)       (abs(sfield(x))/MWORD_SIZE)                       // size#
#define alloc_size(x) (sfield(x) == 0 ? TPTR_SIZE : size(x)+1)          // alloc_size#
#define mem_alloc_size(x) (x == 0 ? TPTR_SIZE : (abs(x)/MWORD_SIZE))    // mem_alloc_size#
#define size_masked(x)       (abs(~CTL_MASK & sfield(x))/MWORD_SIZE)    // size_masked#

#define _mktptr(pyr,key,bs) mem_new_tptr(pyr,HASHC(pyr,key),bs) // _mktptr#

#define mem_new_valz(pyr,size) mem_new_val(pyr, size, 0) // mem_new_valz#
#define mem_sys_free_bs(bs,size) mem_sys_free(bs-1,size) // mem_sys_free_bs#

#define HASHA_FORM(hash_fn,pyr,str,strlen) (hash_fn( pyr, (char*)GLOBAL_TAG_ZERO_HASH, (char*)str, strlen ))

// HASHI# --> Hash a constant string (non-allocating)
// HASHC# --> Hash a constant string (allocating)
// HASH#  --> Hash a Babel string    (allocating)
// HASHM# --> Hash a val-array       (allocating)

#ifdef COMPAT_MODE
#define HASHI(result,str) (pearson16( (char*)result, (char*)GLOBAL_TAG_ZERO_HASH, (char*)str, STRLEN(str) ))
#define HASHC(pyr,str)    HASHA_FORM(pearson16a, pyr, str, STRLEN(str) )
#define HASH(pyr,str)     HASHA_FORM(pearson16a, pyr, str, array8_size(pyr,str) )
#define HASHM(pyr,str)    HASHA_FORM(pearson16a, pyr, str, UNITS_MTO8(array_size(pyr,str)) )
#else
#define HASHI(result,str) (pearson_marsaglia16( (char*)result, (char*)GLOBAL_TAG_ZERO_HASH, (char*)str, STRLEN(str) ))
#define HASHC(pyr,str)    HASHA_FORM(pearson_marsaglia16a, pyr, str, STRLEN(str) )
#define HASH(pyr,str)     HASHA_FORM(pearson_marsaglia16a, pyr, str, array8_size(pyr,str) )
#define HASHM(pyr,str)    HASHA_FORM(pearson_marsaglia16a, pyr, str, UNITS_MTO8(array_size(pyr,str) )
#endif

//#define HASHC(pyr,str) (pearson16a( pyr, (char*)GLOBAL_TAG_ZERO_HASH, (char*)str, STRLEN(str) ))
//#define HASH(pyr,str)  (pearson16a( pyr, (char*)GLOBAL_TAG_ZERO_HASH, (char*)str, array8_size(pyr,str) ))
//#define HASHM(pyr,str) (pearson16a( pyr, (char*)GLOBAL_TAG_ZERO_HASH, (char*)str, array8_size(pyr,str) ))
//
//#define HASHC(pyr,str) (pearson_marsaglia16a( pyr, (char*)GLOBAL_TAG_ZERO_HASH, (char*)str, STRLEN(str) ))
//#define HASH(pyr,str) (pearson_marsaglia16a( pyr, (char*)GLOBAL_TAG_ZERO_HASH, (char*)str, array8_size(pyr,str) ))
//#define HASHM(pyr,str) (pearson_marsaglia16a( pyr, (char*)GLOBAL_TAG_ZERO_HASH, (char*)str, array8_size(pyr,str) ))

// signature is HASHM of unloaded bstruct

/*****************************************************************************
 *                                                                           *
 *                                 TAGS                                      *
 *                                                                           *
 ****************************************************************************/

#define PYR_TAGS                                                             \
    X(PYR_TAG_NIL              , "/pyramid/tag/nil")                         \
    X(PYR_TAG_INTERP_NIL       , "nil")                                      \
    X(PYR_TAG_EXIST            , "/pyramid/tag/exist")                       \
    X(PYR_TAG_UNEXIST          , "/pyramid/tag/unexist")                     \
    X(PYR_TAG_DEF              , "/pyramid/tag/def")                         \
    X(PYR_TAG_UNDEF            , "/pyramid/tag/undef")                       \
    X(PYR_TAG_TRUE             , "/pyramid/tag/true")                        \
    X(PYR_TAG_FALSE            , "/pyramid/tag/false")                       \
    X(PYR_TAG_ACCEPT           , "/pyramid/tag/accept")                      \
    X(PYR_TAG_REJECT           , "/pyramid/tag/reject")                      \
    X(PYR_TAG_PASS             , "/pyramid/tag/pass")                        \
    X(PYR_TAG_FAIL             , "/pyramid/tag/fail")                        \
    X(PYR_TAG_NAN              , "/pyramid/tag/nan")                         \
    X(PYR_TAG_REF              , "/pyramid/tag/ref")                         \
    X(PYR_TAG_PURE_REF         , "/pyramid/tag/pure_ref")                    \
    X(PYR_TAG_HASH_TABLE       , "/pyramid/tag/hash_table")                  \
    X(PYR_TAG_HASH_TABLE_ENTRY , "/pyramid/tag/hash_table_entry")            \
    X(PYR_TAG_DIRECTORY        , "/pyramid/tag/directory")                   \
    X(PYR_TAG_PYR_STRING       , "/pyramid/tag/pyramid_string")              \
    X(PYR_TAG_CSTRING          , "/pyramid/tag/cstring")                     \
    X(PYR_TAG_STRING_ARRAY     , "/pyramid/tag/string_array")                \
    X(PYR_TAG_STRING_LIST      , "/pyramid/tag/string_list")                 \
    X(PYR_TAG_NUMERIC          , "/pyramid/tag/numeric")                     \
    X(PYR_TAG_ARRAY8           , "/pyramid/tag/array8")                      \
    X(PYR_TAG_ARRAY1           , "/pyramid/tag/array1")                      \
    X(PYR_TAG_BVM              , "/pyramid/tag/pyr")                         \
    X(PYR_TAG_SYM_BVM          , "/pyramid/tag/sym_pyr")                     \
    X(PYR_TAG_HIBER_BVM        , "/pyramid/tag/hiber_pyr")                   \
    X(PYR_TAG_SPARSE_BVM       , "/pyramid/tag/sparse_pyr")                  \
    X(PYR_TAG_READY_BVM        , "/pyramid/tag/ready_pyr")                   \
    X(PYR_TAG_BVM_CODE         , "/pyramid/tag/pyr_code")                    \
    X(PYR_TAG_BVM_STACK        , "/pyramid/tag/pyr_stack")                   \
    X(PYR_TAG_BVM_RSTACK       , "/pyramid/tag/pyr_rstack")                  \
    X(PYR_TAG_BVM_JUMP_TABLE   , "/pyramid/tag/pyr_jump_table")              \
    X(PYR_TAG_BVM_SYM_TABLE    , "/pyramid/tag/pyr_sym_table")               \
    X(PYR_TAG_BVM_STEPS        , "/pyramid/tag/pyr_steps")                   \
    X(PYR_TAG_EVAL             , "/pyramid/tag/eval")                        \
    X(PYR_TAG_LOOP             , "/pyramid/tag/loop")                        \
    X(PYR_TAG_TIMES            , "/pyramid/tag/times")                       \
    X(PYR_TAG_IFTE             , "/pyramid/tag/ifte")                        \
    X(PYR_TAG_EACH             , "/pyramid/tag/each")                        \
    X(PYR_TAG_EACHAR           , "/pyramid/tag/eachar")                      \
    X(PYR_TAG_WHILE            , "/pyramid/tag/while")                       \
    X(PYR_TAG_NEST             , "/pyramid/tag/nest")                        \
    X(PYR_TAG_ALT              , "/pyramid/tag/alt")                         \
    X(PYR_TAG_SEQ              , "/pyramid/tag/seq")                         \
    X(PYR_TAG_LET              , "/pyramid/tag/let")                         \
    X(PYR_TAG_REF_HASH         , "/pyramid/tag/ref_hash")                    \
    X(PYR_TAG_REF_STRING       , "/pyramid/tag/ref_string")                  \
    X(PYR_TAG_REF_ARRAY        , "/pyramid/tag/ref_array")                   \
    X(PYR_TAG_REF_SYM_LOCAL    , "/pyramid/tag/ref_sym_local")               \
    X(PYR_TAG_REF_SYM_GLOBAL   , "/pyramid/tag/ref_sym_global")              \
    X(PYR_TAG_COND             , "/pyramid/tag/cond")                        \
    X(PYR_TAG_TRIE             , "/pyramid/tag/trie")                        \
    X(PYR_TAG_TRIE_ENTRY       , "/pyramid/tag/trie_entry")                  \
    X(PYR_TAG_SENTINEL         , "/pyramid/tag/sentinel")                    \
    X(PYR_TAG_MONKEY           , "/pyramid/tag/monkey")                      \
    X(PYR_TAG_ROBOT            , "/pyramid/tag/robot")                       \
    X(PYR_TAG_UNTYPED          , "/pyramid/tag/untyped")                     \
    X(PYR_TAG_BVM_INITD        , "/pyramid/tag/pyr_initd")                   \
    X(PYR_SYM_ARGV             , "/pyramid/sym/argv")                        \
    X(PYR_SYM_PARENT_BVM       , "/pyramid/sym/parent_pyr")                  \
    X(PYR_SYM_CODE_RESTART_POINT , "/pyramid/sym/code_restart_point")       

#define X(a,b) mword *a;
PYR_TAGS
#undef X


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
#define _d(x)           fprintf(stderr, "%s %08x\n", QUOTEME(x), (mword)x); // d#
#define _dd(x)          fprintf(stderr, "%s %d\n",   QUOTEME(x), (mword)x); // dd#
#define _dw(x)          fprintf(stderr, "%s %08x ",  QUOTEME(x), (mword)x); // dw#

#define _die            fprintf(stderr, "Died at %s line %d\n", __FILE__, __LINE__); exit(DIE_EXIT_CODE);  // die#

#define _warn(x)        fprintf(stderr, "WARNING: %s in %s() at %s line %d\n", x, __func__, __FILE__, __LINE__);  // warn#
#define _error(x)       fprintf(stderr, "ERROR: %s in %s() at %s line %d\n", x, __func__, __FILE__, __LINE__); // error#
#define _fatal(x)       fprintf(stderr, "FATAL: %s in %s() at %s line %d\n", x, __func__, __FILE__, __LINE__); _die;  // _fatal#
#define _enhance(x)     fprintf(stderr, "ENHANCEMENT: %s in %s at %s line %d\n", x, __func__, __FILE__, __LINE__); // enhance#

#define _timestamp(x)   (time_ms(void) - x->interp->epoch_ms) // _timestamp#

#define _cat_except(x)    _trace; longjmp(*(x->interp->cat_ex),CAT_EXCEPT); // _cat_except#
#define _interp_reset(x)          longjmp(*(x->interp->cat_ex),INTERP_RESET); // _interp_reset#

/*****************************************************************************
 *                                                                           *
 *                                  DEV                                      *
 *                                                                           *
 ****************************************************************************/

#ifdef DEV_MODE

FILE *dev_log; // dev_log#
int dev_i;     // dev_i#

// #define _mem#
#define _mem(x)                                                     \
    fprintf(stderr, "---- %08x\n", sfield(x));                      \
    for(dev_i=0; dev_i<alloc_size(x)-1; dev_i++){                   \
        if(dev_i>=0){                                               \
            fprintf(stderr, "%04x ", (unsigned)dev_i*MWORD_SIZE);   \
        }                                                           \
        fprintf(stderr, "%08x\n", rdv(x,dev_i));                    \
    }

#endif

#endif //PYRAMID_H

// Clayton Bauman 2016


