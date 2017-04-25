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

    interp_init_once(this_pyr);

    mword *golden_nil = interp_init_golden_nil();

    // Interpreter reset or catastrophic-exception
    jmp_buf cat_ex;

    int val;
    val = setjmp(cat_ex);

    if(val==CAT_EXCEPT){
        _say("CATASTROPHIC EXCEPTION: pyramid");
        _die;
    }
    else if(val==INTERP_RESET){
//        interp_exit(this_pyr);
        mem_destroy(this_pyr->interp->mem);
        _say("INTERP_RESET: pyramid");
    }

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


//
//
void interp_boilerplate(void){

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

    global_irt = malloc(sizeof(interp_runtime)); // XXX WAIVER(malloc) XXX //
    global_irt->tags = malloc(sizeof(interp_tags));  // XXX WAIVER(malloc) XXX //

    interp_init_zero_hash();
    interp_init_nil_mem();
    mem_non_gc_new();

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

    ////////////////////////////
    // init this_pyr->interp  //
    ////////////////////////////

    global_irt->symbols         = mem_non_gc_alloc(sizeof(interp_symbols));  // XXX WAIVER(mem_sys_alloc) XXX //
    global_irt->strings         = mem_non_gc_alloc(sizeof(interp_strings));  // XXX WAIVER(mem_sys_alloc) XXX //

    this_pyr->interp            = mem_non_gc_alloc(sizeof(interp_state)); // XXX WAIVER(mem_sys_alloc) XXX //
//    this_pyr->interp->tags      = mem_non_gc_alloc(sizeof(interp_tags));  // XXX WAIVER(mem_sys_alloc) XXX //
//    this_pyr->interp->symbols   = mem_non_gc_alloc(sizeof(interp_symbols));  // XXX WAIVER(mem_sys_alloc) XXX //
    this_pyr->interp->cat_ex    = cat_ex;

    this_pyr->interp->op_restart        = (jmp_buf*)UNINIT_VAL;
    this_pyr->interp->thread_counter    = 0;
    this_pyr->interp->global_tick_count = 0;

    this_pyr->interp->argc      = argc;
    this_pyr->interp->argv      = argv;
    this_pyr->interp->envp      = envp;

#ifdef COMPAT_MODE
    this_pyr->interp->hash_fn   = pearson16;
#else
    this_pyr->interp->hash_fn   = pearson_marsaglia16;
#endif

    ////////////////////////////////////
    // init this_pyr->interp->flags   //
    ////////////////////////////////////

    interp_init_flags(this_pyr);

    ////////////////////////////
    // init this_pyr          //
    ////////////////////////////

    this_pyr->self              = UNINIT_PTR;
    this_pyr->code_ptr          = UNINIT_PTR;
    this_pyr->rstack_ptr        = UNINIT_PTR;
    this_pyr->dstack_ptr        = UNINIT_PTR;
    this_pyr->soft_root         = UNINIT_PTR;
    this_pyr->local_root        = UNINIT_PTR;
    this_pyr->local_path        = UNINIT_PTR;
    this_pyr->local_pwd         = UNINIT_PTR;

    this_pyr->thread_id         = UNINIT_VAL;
    this_pyr->steps             = UNINIT_VAL;
    this_pyr->advance_type      = UNINIT_VAL;
    this_pyr->bvm_initd         = UNINIT_VAL;

    this_pyr->dstack_depth      = UNINIT_VAL;
    this_pyr->dstack_diameter   = UNINIT_VAL;

    ////////////////////////////////
    // init this_pyr->interp->mem //
    ////////////////////////////////

    mem_new(this_pyr, MEM_GC_STAT_BANK_MAX_SIZE);

    this_pyr->flags->MEM_ALLOC_BLOCKING = CLR; // It is now safe to use mem_alloc()

    this_pyr->flags->MEM_ALLOC_NON_GC = SET; // Continue using mem_non_gc_alloc until boot is complete (ensures proper teardown)

    interp_init_limits(this_pyr);

    interp_init_privileges(this_pyr);

#ifdef PROF_MODE
    this_pyr->interp->profile = mem_non_gc_alloc(sizeof(pyr_profile));
#endif

//#define X(a, b) a = HASHC(this_pyr, b);
//PYR_TAGS    
//#undef X

//#define X(a, b) this_pyr->interp->tags->a = HASHC(this_pyr, b);

#define X(a, b) global_irt->tags->a = HASHC(this_pyr, b);
PYR_TAGS    
#undef X

#define X(a, b) global_irt->strings->a = string_c2b(this_pyr, b, 1000); 
PYR_TAGS    
#undef X

    interp_init_symbols(this_pyr);

    this_pyr->self = interp_load_root_bvm(this_pyr);

    global_dev_overrides = interp_load_dev_overrides(this_pyr);
    global_dev_overrides++;

    global_irt->fs = trie_new(this_pyr);

    this_pyr->flags->INTERP_BOOT_IN_PROGRESS = CLR;

#ifdef INTERP_RESET_TRACE
_msg("INTERP_RESET_TRACE: COMPLETE");
#endif

    return this_pyr;

}


//
//
void interp_reinitialize_nil(mword *golden_nil){ // interp_reinitialize_nil#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    // Init nil from golden_nil to protect against accidental nil-overwrite
    memcpy(nil-1,golden_nil-1,UNITS_MTO8(TPTR_SIZE));
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

    this_pyr->flags = f;

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

    this_pyr->interp->limits = l;

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

    this_pyr->interp->privileges = p;

    return this_pyr;

}


// must be LAST function called
//
void interp_exit(pyr_cache *this_pyr){ // interp_exit#

    // Complete mem teardown
    mem_destroy(this_pyr->interp->mem);

    mem_non_gc_teardown();

    free(nil-1);
//    free(global_irt->tags->PYR_TAG_ZERO_HASH);
    free(global_irt->tags);
    free(global_irt);

#ifdef DEV_MODE
    _say("PYRAMID: exiting normally");
#endif

}


//
//
void interp_init_symbols(pyr_cache *this_pyr){ // interp_init_symbols#

    // XXX Unless MC_ALLOC_NON_GC is set, this could break XXX
    global_irt->symbols->SEXPR_IGNORE_SYMBOL   = _mkval( this_pyr, 6, 0x69, 0x67, 0x6e, 0x6f, 0x72, 0x65 );
    global_irt->symbols->SEXPR_QUOTE_SYMBOL    = _mkval( this_pyr, 5, 0x71, 0x75, 0x6f, 0x74, 0x65 );
    global_irt->symbols->SEXPR_SEXPR_SYMBOL    = _mkval( this_pyr, 5, 0x73, 0x65, 0x78, 0x70, 0x72 );
    global_irt->symbols->SEXPR_BYTES_SYMBOL    = _mkval( this_pyr, 5, 0x62, 0x79, 0x74, 0x65, 0x73 );
    global_irt->symbols->SEXPR_LIST_SYMBOL     = _mkval( this_pyr, 4, 0x6c, 0x69, 0x73, 0x74 );
    global_irt->symbols->SEXPR_CODE_SYMBOL     = _mkval( this_pyr, 4, 0x63, 0x6f, 0x64, 0x65 );
    global_irt->symbols->SEXPR_HASH_SYMBOL     = _mkval( this_pyr, 4, 0x68, 0x61, 0x73, 0x68 );
    global_irt->symbols->SEXPR_OPER_SYMBOL     = _mkval( this_pyr, 4, 0x6f, 0x70, 0x65, 0x72 );
    global_irt->symbols->SEXPR_TPTR_SYMBOL     = _mkval( this_pyr, 4, 0x74, 0x70, 0x74, 0x72 );
    global_irt->symbols->SEXPR_VAL_SYMBOL      = _mkval( this_pyr, 3, 0x76, 0x61, 0x6c );
    global_irt->symbols->SEXPR_PTR_SYMBOL      = _mkval( this_pyr, 3, 0x70, 0x74, 0x72 );
    global_irt->symbols->SEXPR_TAG_SYMBOL      = _mkval( this_pyr, 3, 0x74, 0x61, 0x67 );
    global_irt->symbols->SEXPR_REF_SYMBOL      = _mkval( this_pyr, 3, 0x72, 0x65, 0x66 );
    global_irt->symbols->SEXPR_NIL_SYMBOL      = _mkval( this_pyr, 3, 0x6e, 0x69, 0x6c );
    global_irt->symbols->SEXPR_SYM_SYMBOL      = _mkval( this_pyr, 3, 0x73, 0x79, 0x6d );
    global_irt->symbols->SEXPR_BS_SYMBOL       = _mkval( this_pyr, 2, 0x62, 0x73 );
    global_irt->symbols->SEXPR_QW_SYMBOL       = _mkval( this_pyr, 2, 0x71, 0x77 );
    global_irt->symbols->SEXPR_SQUOTE_SYMBOL   = _mkval( this_pyr, 1, 0x27 );
    global_irt->symbols->SEXPR_DQUOTE_SYMBOL   = _mkval( this_pyr, 1, 0x22 );

}


//
//
mword *interp_load_root_bvm(pyr_cache *this_pyr){ // interp_load_root_bvm#

//    return interp_init_load_from_file(this_pyr, "rt/root.bbl");
    return io_slurp8(this_pyr, "rt/root.bbl");

}


//
//
mword *interp_load_dev_overrides(pyr_cache *this_pyr){ // interp_load_dev_overrides#

//    return interp_init_load_from_file(this_pyr, "dev/overrides.bbl");
    return io_slurp8(this_pyr, "dev/overrides.bbl");

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


// Clayton Bauman 2017

