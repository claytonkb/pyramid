// interp.c
//

#include "pyramid.h"
#include "interp.h"
#include "mem.h"
#include "pearson.h"
#include "util.h"
#include "io.h"
#include "trie.h"
#include "string.h"
#include "bstruct.h"
#include "array.h"
#include "introspect.h"
#include "list.h"
#include "sort.h"
#include "std.h"
#include "mt19937ar.h"

// PYR_TAGS includes:
#include "pvc.h"


// invocation
// |    init_once
// |    |   reinit
// |    |   |   global_irt ready
// |    |   |   |   pyr_vm ready
// |    |   |   |   |   pvc ready
// |    |   |   |   |   |   init_once_per_install (build templates, std JIT)
// |    |   |   |   |   |   |   interp_core
// |    |   |   |   |   |   |   |   pvc_core
// |    |   |   |   |   |   |   |   |   code_exec
// |    |   |   |   |   |   |   |   |   |
// v    v   v   v   v   v   v   v   v   v
// |----------------------------------------> t


/*****************************************************************************
 *                                                                           *
 *                         INTERPRETER EXECUTION                             *
 *                                                                           *
 ****************************************************************************/


//
//
int interp_pyramid(pyr_cache *this_pyr, int argc, char **argv, char **envp){ // interp_pyramid#

    // XXX //SECURITY// **DO NOT REMOVE** //SECURITY// XXX //
    interp_boilerplate();

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    clock_t wall_clock_time;
    wall_clock_time = clock();

    /////////////////////////////
    //       ONE-TIME INIT     //
    /////////////////////////////
    interp_init_once(this_pyr);

    mword *golden_nil = interp_init_golden_nil();

    /////////////////////////////
    //    INTERP RESET POINT   //
    /////////////////////////////
    jmp_buf cat_ex;

    int val;
    val = setjmp(cat_ex);

    if(val==CAT_EXCEPT){
        _say("CATASTROPHIC EXCEPTION: pyramid");
        _die;
    }
    else if(val==INTERP_RESET){
        mem_destroy(global_irt->gc_mem);
        _say("INTERP_RESET: pyramid");
    }

    /////////////////////////////
    //     RE-ENTRANT INIT     //
    /////////////////////////////
    interp_reinit(this_pyr, golden_nil, argc, argv, envp, &cat_ex);

#ifdef DEV_MODE
    util_bare_metal_prompt(this_pyr, nil);
#endif

//    _pre_interp(this_pyr);
//
//    _babel_root(this_pyr, this_pyr->self);
//
//    _post_interp(this_pyr);

    wall_clock_time = (clock() - wall_clock_time) / CLOCKS_PER_SEC;

    mem_sys_free(golden_nil-1);

    interp_exit(this_pyr);

    return wall_clock_time;

}


// Main entry-point to interpreter execution environment
//
void interp_core(pyr_cache *this_pyr){ // interp_core#

    _die;

}


/*****************************************************************************
 *                                                                           *
 *                      INTERPRETER INITIALIZATION                           *
 *                                                                           *
 ****************************************************************************/


//
//
void interp_boilerplate(void){ // interp_boilerplate#

#ifdef MEM_DEBUG
_prn(" >>MEM_DEBUG<< ");
#endif

#ifdef DEV_MODE
_prn(" >>DEV_MODE<< ");
#ifdef PYRAMID_64_BIT
_prn(" >>64-BIT<< ");
#endif
#ifdef PYRAMID_32_BIT
_prn(" >>32-BIT<< ");
#endif
#endif

#ifdef PROF_MODE
_prn(" >>PROF_MODE<< ");
#endif

#ifdef COMPAT_MODE
_prn(" >>COMPAT_MODE<< ");
#endif

#ifdef CHK_MODE
_prn(" >>CHK_MODE<< ");
#endif

_prn("\n");

}


//
//
void interp_init_once(pyr_cache *this_pyr){ // interp_init_once#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    UNINIT_FN_PTR = interp_uninit_fn;

    global_irt = malloc(sizeof(interp_runtime));    // XXX WAIVER(malloc) XXX //
    mword *temp = malloc(sizeof(interp_tags)+MWORD_SIZE); // XXX WAIVER(malloc) XXX //
    ldv(temp,0) = sizeof(interp_tags);
    temp++;
    global_irt->tags = (interp_tags*)temp;

    interp_init_zero_hash();
    interp_init_nil_mem();
    mem_non_gc_new();

//    interp_init_epoch(this_pyr);

}


//
//
void interp_init_zero_hash(void){ // interp_init_zero_hash#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    global_irt->tags->PYR_TAG_ZERO_HASH = malloc(UNITS_MTO8(HASH_ALLOC_SIZE)); // XXX WAIVER(malloc) XXX // Cannot use mem_sys_alloc before mem_non_gc_new
    ldv(global_irt->tags->PYR_TAG_ZERO_HASH ,0) = UNITS_MTO8(HASH_SIZE);
    global_irt->tags->PYR_TAG_ZERO_HASH++;
    memset((char*)global_irt->tags->PYR_TAG_ZERO_HASH, 0, UNITS_MTO8(HASH_SIZE));

}


//
//
void interp_init_nil_mem(void){ // interp_init_nil_mem#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    nil = malloc(UNITS_MTO8(TPTR_SIZE)); // XXX WAIVER(malloc) XXX // Cannot use mem_sys_alloc before mem_non_gc_new
    nil++;

}


//
//
mword *interp_init_golden_nil(void){ // interp_init_golden_nil#

    mword *golden_nil = mem_sys_alloc(UNITS_MTO8(TPTR_SIZE));
    golden_nil++;

    sfield(golden_nil) = 0;
    sfield(golden_nil+HASH_SIZE+1) = (-1*MWORD_SIZE);

#ifdef COMPAT_MODE
    HASHI(golden_nil, "nil");
#else
    HASHI(golden_nil, "/pyramid/tag/nil");
#endif

    tptr_set_ptr(golden_nil, golden_nil);

    return golden_nil;

}


// Initializes the root virtual machine and interpreter-only state...
//
pyr_cache *interp_reinit(pyr_cache *this_pyr, mword *golden_nil, int argc, char **argv, char **envp, jmp_buf *cat_ex){ // interp_init#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    mem_non_gc_reset();

    interp_reinitialize_nil(golden_nil);

    //////////////////////
    // init global_irt  //
    //////////////////////
    global_irt->symbols = mem_non_gc_alloc(sizeof(interp_symbols));  // XXX WAIVER(mem_sys_alloc) XXX //
    global_irt->strings = mem_non_gc_alloc(sizeof(interp_strings));  // XXX WAIVER(mem_sys_alloc) XXX //
    global_irt->fns     = mem_non_gc_alloc(sizeof(interp_fns));  // XXX WAIVER(mem_sys_alloc) XXX //

    global_irt->cat_ex  = cat_ex;

    global_irt->op_restart        = (jmp_buf*)UNINIT_VAL;
    global_irt->thread_counter    = 0;
    global_irt->global_tick_count = 0;

    global_irt->argc      = argc;
    global_irt->argv      = argv;
    global_irt->envp      = envp;

//#ifdef COMPAT_MODE
    global_irt->hash_fn   = pearson16;
//#else
//    global_irt->hash_fn   = pearson_marsaglia16;
//#endif

    //////////////////////////////
    // init global_irt->flags   //
    //////////////////////////////
    interp_init_flags(this_pyr);

    ////////////////////////////
    // init this_pyr          //
    ////////////////////////////
    this_pyr->self = UNINIT_PTR;

    //////////////////////////
    // init global_irt->mem //
    //////////////////////////
    mem_new(this_pyr, MEM_GC_STAT_BANK_MAX_SIZE);

    global_irt->flags->MEM_ALLOC_BLOCKING = CLR; // It is now safe to use mem_alloc()
    global_irt->flags->MEM_ALLOC_NON_GC   = CLR; // ... so stop using mem_non_gc_alloc()

    interp_init_limits(this_pyr);
    interp_init_privileges(this_pyr);

#ifdef PROF_MODE
    global_irt->profile = mem_non_gc_alloc(sizeof(pyr_profile));
#endif

#define X(a,b,c,d,e,f,g,h,i,j,k,l) global_irt->tags->a = HASHC(this_pyr, b);
PYR_TAGS
#undef X

#define X(a,b,c,d,e,f,g,h,i,j,k,l) global_irt->strings->a = string_c2b(this_pyr, b, 1000); 
PYR_TAGS
#undef X

#define X(a,b,c,d,e,f,g,h,i,j,k,l) global_irt->fns->a = c;
PYR_TAGS
#undef X

    interp_init_xbar(this_pyr);

    interp_init_tables(this_pyr);

    interp_init_symbols(this_pyr);

    interp_init_epoch(this_pyr);

    interp_init_srand(this_pyr);

    this_pyr->self = interp_load_root_bvm(this_pyr);
    this_pyr->cpu  = mem_non_gc_alloc(sizeof(pyr_cpu));

    global_dev_overrides = interp_load_dev_overrides(this_pyr);
    global_dev_overrides++;

    global_irt->env = interp_load_pyr_env(this_pyr);

    global_irt->flags->INTERP_BOOT_IN_PROGRESS = CLR;

#ifdef INTERP_RESET_TRACE
_msg("INTERP_RESET_TRACE: COMPLETE");
#endif

    return this_pyr;

}


//
//
pyr_cache *interp_init_epoch(pyr_cache *this_pyr){ // interp_init_epoch#

#ifdef BABEL_RESET_TRACE
_trace;
#endif

    time_t rawtime;
    struct tm *utc_epoch;

    time( &rawtime );
    utc_epoch = gmtime( &rawtime );

    //    tm_sec     int    seconds after the minute    0-61*
    //    tm_min     int    minutes after the hour      0-59
    //    tm_hour    int    hours since midnight        0-23
    //    tm_mday    int    day of the month            1-31
    //    tm_mon     int    months since January        0-11
    //    tm_year    int    years since 1900
    //    tm_wday    int    days since Sunday           0-6
    //    tm_yday    int    days since January 1        0-365

//    struct tm *pyr_utc_epoch = mem_sys_alloc(sizeof(struct tm)); //&this_pyr->interp->utc_epoch;
    struct tm *pyr_utc_epoch = (struct tm *)_newstr(this_pyr, sizeof(struct tm), 0);

    pyr_utc_epoch->tm_sec  = utc_epoch->tm_sec;
    pyr_utc_epoch->tm_min  = utc_epoch->tm_min;
    pyr_utc_epoch->tm_hour = utc_epoch->tm_hour;
    pyr_utc_epoch->tm_mday = utc_epoch->tm_mday;
    pyr_utc_epoch->tm_mon  = utc_epoch->tm_mon;
    pyr_utc_epoch->tm_year = utc_epoch->tm_year;
    pyr_utc_epoch->tm_wday = utc_epoch->tm_wday;
    pyr_utc_epoch->tm_yday = utc_epoch->tm_yday;

    global_irt->utc_epoch = pyr_utc_epoch;

    global_irt->epoch_ms = _val(this_pyr, std_time_ms());

    return this_pyr;

}


// must be called AFTER interp_new_epoch
//
void interp_init_srand(pyr_cache *this_pyr){ // interp_init_srand#

#ifdef INTERP_RESET_TRACE
_trace;
#endif

    #define SRAND_STRING_SIZE 32

    char srand_string[SRAND_STRING_SIZE];

    sprintf(srand_string, 
            "%d%d%d%d%d%d", 
            global_irt->utc_epoch->tm_sec,
            global_irt->utc_epoch->tm_min,
            global_irt->utc_epoch->tm_hour,
            global_irt->utc_epoch->tm_mday,
            global_irt->utc_epoch->tm_mon,
            global_irt->utc_epoch->tm_year);

    mword *pyr_srand_string = string_c2b(this_pyr, srand_string, SRAND_STRING_SIZE);

    mword *time_hash = HASH(this_pyr, pyr_srand_string);

//    init_by_array( (unsigned long *)time_hash, HASH_SIZE*(sizeof(mword)/sizeof(unsigned long)));
    init_by_array( (unsigned long *)time_hash, UNITS_MTO8(HASH_SIZE)/sizeof(unsigned long) );

    global_irt->srand = time_hash;

}



//
//
void interp_init_tables(pyr_cache *this_pyr){ // interp_init_tables#

    int i=0;

#ifdef INTERP_RESET_TRACE
_prn("PYR_NUM_VERB_TAGS is ");
fprintf(stderr,"%d\n",PYR_NUM_VERB_TAGS);
_prn("PYR_NUM_NOUN_TAGS is ");
fprintf(stderr,"%d\n",PYR_NUM_NOUN_TAGS);
#endif

    global_irt->verb_table = mem_new_ptr(this_pyr, PYR_NUM_VERB_TAGS);
    global_irt->noun_table = mem_new_ptr(this_pyr, PYR_NUM_NOUN_TAGS);

////////////////////////////////////////////////////////////////////////////
#define X(a,b,c,d,e,f,g,h,j,k,l,m) \
    ldp(global_irt->verb_table,i++) = _cons(this_pyr, global_irt->tags->a, nil);
    PYR_VERB_TAGS    
#undef X

    for(;i<PYR_NUM_VERB_TAGS;i++){
        ldp(global_irt->verb_table,i) = _cons(this_pyr, nil, nil);
    }

    sort(this_pyr, global_irt->verb_table, LEX_MWORD);

    i=0;

////////////////////////////////////////////////////////////////////////////
#define X(a,b,c,d,e,f,g,h,j,k,l,m) \
    ldp(global_irt->noun_table,i++) = _cons(this_pyr, global_irt->tags->a, nil);
    PYR_NOUN_TAGS    
#undef X

    for(;i<PYR_NUM_NOUN_TAGS;i++){
        ldp(global_irt->noun_table,i) = _cons(this_pyr, nil, nil);
    }

    sort(this_pyr, global_irt->noun_table, LEX_MWORD);

}


//
//
void interp_init_xbar(pyr_cache *this_pyr){ // interp_init_xbar#

    int i=0;
    mword *temp;
//    mword *cdr;

#ifdef INTERP_RESET_TRACE
_prn("PYR_NUM_TAGS is ");
_dd(PYR_NUM_TAGS);
#endif

    ////////////////////////
    // GLOBAL_IRT -> XBAR //
    ////////////////////////

    global_irt->xbar = mem_new_ptr(this_pyr, PYR_NUM_TAGS);

    ldp(global_irt->xbar,i) = mem_new_ptr(this_pyr, PYR_XBAR_NUM_FIELDS);
    i++;


/////////////////////////////////////////////////////////////////////////////
// NOTE: temp[2] is intentionally stored as a _val not as a _ptr in order
// to keep the xbar safe for dumping/introspection.
/////////////////////////////////////////////////////////////////////////////
#define X(a,b,c,d,e,f,g,h,j,k,l,m) \
    temp = mem_new_ptr(this_pyr, PYR_XBAR_NUM_FIELDS); \
    ldp(temp,0) = HASHC(this_pyr, b); \
    ldp(temp,1) = string_c2b(this_pyr, b, 1000); \
    ldp(temp,2) = _val(this_pyr,(mword)c); \
    ldp(temp,3) = _val(this_pyr,d); \
    ldp(global_irt->xbar,i) = temp; \
    i++;
    PYR_TAGS    
#undef X

    sort(this_pyr, global_irt->xbar, LEX_MWORD);

//_dump(global_irt->xbar);
//_die;

}


//
//
void interp_reinitialize_nil(mword *golden_nil){ // interp_reinitialize_nil#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    // Init nil from golden_nil to protect against accidental nil-overwrite
//    memcpy(nil-1,golden_nil-1,UNITS_MTO8(TPTR_SIZE));
    cpy(nil-1,golden_nil-1,TPTR_SIZE);
    tptr_set_ptr(nil, nil);

}


//
//
pyr_cache *interp_init_flags(pyr_cache *this_pyr){ // interp_init_flags#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    interp_flags *f = mem_non_gc_alloc(sizeof(interp_flags));

    f->MEM_ALLOC_BLOCKING                   = SET; // Cleared after mem_new in interp_init
    f->MEM_ALLOC_NON_GC                     = CLR;

    f->MEM_GC_BLOCKING                      = CLR;
    f->MEM_GC_INTERP_BLOCKING               = CLR;

    f->MEM_GC_PENDING                       = CLR;
    f->MEM_GC_CONVERT_PENDING_TO_OP_RESTART = CLR;
    f->MEM_GC_PNR                           = CLR;
    f->MEM_GC_OP_RESTART                    = CLR;
    f->MEM_GC_SECONDARY_BANK_ALLOC          = CLR;
    f->MEM_GC_ON_EVERY_OP                   = CLR;

    f->MEM_USE_MALLOC                       = CLR;
    f->MEM_USE_DYN                          = CLR;

    f->INTERP_BOOT_IN_PROGRESS              = SET; // Cleared at end of interp_init 
    f->INTERP_NO_ASYNC                      = CLR;
    f->INTERP_NO_EXCEPT                     = CLR;

    f->PYR_INSTR_IN_PROGRESS                = CLR;
    f->PYR_INCOHERENT                       = CLR;
    f->PYR_SYMBOLS_DEFINED                  = CLR;
    f->PYR_CLEAN                            = CLR;
    f->PYR_INTERP_OP_TRACE                  = CLR;
    f->PYR_INSTRUMENT_TRIGGER               = CLR;
    f->PYR_CODE_LIST_EMPTY                  = CLR;
    f->PYR_RETURN_TOS_ON_EXIT               = CLR;

    f->PYR_CACHE_INVALID                    = CLR;
    f->PYR_CACHE_DIRTY                      = CLR;
    f->PYR_CACHE_ONLY                       = CLR;
    f->PYR_CACHE_BLOCKING                   = CLR;

    global_irt->flags = f;

    return this_pyr;

}


//
//
pyr_cache *interp_init_limits(pyr_cache *this_pyr){ // init_interp_limits#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    interp_limits *l = mem_non_gc_alloc(sizeof(interp_limits)); // XXX WAIVER(mem_sys_alloc) XXX //

    l->max_num_sys_threads      = 1;        // XXX unimplemented
    l->max_num_steps            = -1;       // XXX: Implement command-line init of this value
    l->max_bytes_mem_alloc      = (1<<30);  // Arbitrary 1GB limit
    l->max_num_file_descriptors = 0;        // XXX unimplemented
    l->max_num_file_access      = 0;        // XXX unimplemented
    l->max_run_time_ms          = 0;        // XXX unimplemented

    global_irt->limits = l;

    return this_pyr;

}


//
//
pyr_cache *interp_init_privileges(pyr_cache *this_pyr){ // init_interp_privileges#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    interp_privileges *p = mem_non_gc_alloc(sizeof(interp_privileges)); // XXX WAIVER(mem_sys_alloc) XXX //

    p->FILE_READ_ALLOWED  = SET;
    p->FILE_WRITE_ALLOWED = SET;
    p->SYS_EXEC_ALLOWED   = SET;
    p->BVM_EXEC_ALLOWED   = SET;
    p->BVM_FORK_ALLOWED   = SET;

    global_irt->privileges = p;

    return this_pyr;

}


// must be LAST function called
//
void interp_exit(pyr_cache *this_pyr){ // interp_exit#

    // Complete mem teardown

    mem_destroy(global_irt->gc_mem);

    mem_non_gc_teardown();

    free(nil-1);                                    // XXX WAIVER(free) XXX
    free(global_irt->tags->PYR_TAG_ZERO_HASH-1);    // XXX WAIVER(free) XXX
    free(global_irt->tags);                         // XXX WAIVER(free) XXX
    free(global_irt);                               // XXX WAIVER(free) XXX

#ifdef DEV_MODE
    _say("PYRAMID: exiting normally");
#endif

}


//
//
void interp_init_symbols(pyr_cache *this_pyr){ // interp_init_symbols#

    // XXX Unless MC_ALLOC_NON_GC is set, this could break XXX
    // FIXME: Use quoted char's FIXME
    global_irt->symbols->SEXPR_IGNORE_SYMBOL   = _mkval( this_pyr, 6, 'i', 'g', 'n', 'o', 'r', 'e' );
    global_irt->symbols->SEXPR_QUOTE_SYMBOL    = _mkval( this_pyr, 5, 'q', 'u', 'o', 't', 'e' );
    global_irt->symbols->SEXPR_SEXPR_SYMBOL    = _mkval( this_pyr, 5, 's', 'e', 'x', 'p', 'r' );
    global_irt->symbols->SEXPR_BYTES_SYMBOL    = _mkval( this_pyr, 5, 'b', 'y', 't', 'e', 's' );
    global_irt->symbols->SEXPR_LIST_SYMBOL     = _mkval( this_pyr, 4, 'l', 'i', 's', 't' );
    global_irt->symbols->SEXPR_CODE_SYMBOL     = _mkval( this_pyr, 4, 'c', 'o', 'd', 'e' );
    global_irt->symbols->SEXPR_HASH_SYMBOL     = _mkval( this_pyr, 4, 'h', 'a', 's', 'h' );
    global_irt->symbols->SEXPR_OPER_SYMBOL     = _mkval( this_pyr, 4, 'o', 'p', 'e', 'r' );
    global_irt->symbols->SEXPR_TPTR_SYMBOL     = _mkval( this_pyr, 4, 't', 'p', 't', 'r' );
    global_irt->symbols->SEXPR_VAL_SYMBOL      = _mkval( this_pyr, 3, 'v', 'a', 'l' );
    global_irt->symbols->SEXPR_PTR_SYMBOL      = _mkval( this_pyr, 3, 'p', 't', 'r' );
    global_irt->symbols->SEXPR_TAG_SYMBOL      = _mkval( this_pyr, 3, 't', 'a', 'g' );
    global_irt->symbols->SEXPR_REF_SYMBOL      = _mkval( this_pyr, 3, 'r', 'e', 'f' );
    global_irt->symbols->SEXPR_NIL_SYMBOL      = _mkval( this_pyr, 3, 'n', 'i', 'l' );
    global_irt->symbols->SEXPR_SYM_SYMBOL      = _mkval( this_pyr, 3, 's', 'y', 'm' );
    global_irt->symbols->SEXPR_BS_SYMBOL       = _mkval( this_pyr, 2, 'b', 's' );
    global_irt->symbols->SEXPR_QW_SYMBOL       = _mkval( this_pyr, 2, 'q', 'w' );
    global_irt->symbols->SEXPR_SQUOTE_SYMBOL   = _mkval( this_pyr, 1, '\'' );
    global_irt->symbols->SEXPR_DQUOTE_SYMBOL   = _mkval( this_pyr, 1, '"' );

}


//
//
mword *interp_load_root_bvm(pyr_cache *this_pyr){ // interp_load_root_bvm#

//    return interp_init_load_from_file(this_pyr, "rt/root.bbl");

    mword *temp = io_slurp8(this_pyr, "rt/root.bbl");
    return bstruct_load(this_pyr, temp, size(temp));

//    return io_slurp8(this_pyr, "rt/root.bbl");

}


//
//
mword *interp_load_dev_overrides(pyr_cache *this_pyr){ // interp_load_dev_overrides#

    return io_slurp8(this_pyr, "dev/overrides.bbl");

}


//
//
mword *interp_load_pyr_env(pyr_cache *this_pyr){ // interp_load_pyr_env#

    mword *temp = io_slurp8(this_pyr, "env/pyr_env.bbl");
    return bstruct_load(this_pyr, temp, size(temp));

}


// XXX WAIVER REQUIRED interp_init_load_from_file XXX
//
mword *interp_init_load_from_file(pyr_cache *this_pyr, char *filename){ // interp_init_load_from_file#

#ifdef BABEL_RESET_TRACE
_trace;
#endif

    FILE *f = io_open_file(this_pyr, (mword*)filename, "rb");
    mword file_size = io_file_size(this_pyr, f);

    mword *file_buffer = _newstr(this_pyr, file_size, ' ');
//    mword *file_buffer = mem_alloc(this_pyr, file_size);
//    mword *file_buffer = mem_sys_alloc(file_size);

    fread((char*)file_buffer, 1, file_size, f);

    io_close_file(this_pyr, f);

    return file_buffer;

}


//
//
blob interp_uninit_fn(pyr_cache *this_pyr, blob b){ // interp_uninit_fn#
    _fatal("An unintialized function was invoked");
}


// Clayton Bauman 2017

