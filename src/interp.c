// interp.c
//

#include "pyramid.h"
#include "interp.h"
#include "mem.h"
#include "pearson.h"
#include "util.h"


//
//
int interp_pyramid(pyr_cache *this_pyr, int argc, char **argv, char **envp){ // interp_pyramid#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    clock_t wall_clock_time;
    wall_clock_time = clock();

    interp_init_globals(this_pyr);

    // Initialize golden_nil
//    GLOBAL_TAG_ZERO_HASH = mem_sys_alloc(UNITS_MTO8(HASH_SIZE));
//    memset((char*)GLOBAL_TAG_ZERO_HASH, 0, UNITS_MTO8(HASH_SIZE));

    GLOBAL_TAG_ZERO_HASH = mem_sys_alloc(UNITS_MTO8(HASH_ALLOC_SIZE));
    ldv(GLOBAL_TAG_ZERO_HASH,0) = UNITS_MTO8(HASH_SIZE);
    GLOBAL_TAG_ZERO_HASH++;
    memset((char*)GLOBAL_TAG_ZERO_HASH, 0, UNITS_MTO8(HASH_SIZE));

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

    nil = mem_sys_alloc(UNITS_MTO8(TPTR_SIZE));
    nil++;

    // Interpreter reset or catastrophic-exception
    jmp_buf cat_ex;

    int val;
    val = setjmp(cat_ex);

    if(val==CAT_EXCEPT){
        _say("CATASTROPHIC EXCEPTION: pyramid");
        _die;
    }
    else if(val==INTERP_RESET){
        interp_exit(this_pyr);
        _say("INTERP_RESET: pyramid");
    }

    // Init nil from golden_nil to protect against accidental nil-overwrite
    memcpy(nil-1,golden_nil-1,UNITS_MTO8(TPTR_SIZE));
    tptr_set_ptr(nil, nil);

    interp_init(this_pyr, argc, argv, envp, &cat_ex);

    util_bare_metal_prompt(this_pyr, nil);

//    _pre_interp(this_pyr);
//
//    _babel_root(this_pyr, this_pyr->self);
//
//    _post_interp(this_pyr);

    wall_clock_time = (clock() - wall_clock_time) / CLOCKS_PER_SEC;

//    mem_sys_free(golden_nil-1, UNITS_MTO8(TPTR_SIZE));
//    mem_sys_free(nil-1, UNITS_MTO8(TPTR_SIZE));
//    mem_sys_free(GLOBAL_TAG_ZERO_HASH, UNITS_MTO8(HASH_SIZE));

    mem_sys_free_bs(golden_nil, UNITS_MTO8(TPTR_SIZE));
    mem_sys_free_bs(nil, UNITS_MTO8(TPTR_SIZE));
    mem_sys_free_bs(GLOBAL_TAG_ZERO_HASH, UNITS_MTO8(HASH_ALLOC_SIZE));
//    mem_sys_free(GLOBAL_TAG_ZERO_HASH, UNITS_MTO8(HASH_SIZE));

    interp_exit(this_pyr);

    return wall_clock_time;

}


// must be LAST function called before exit
//
void interp_exit(pyr_cache *this_pyr){ // interp_exit#

    // Complete mem teardown

    mem_destroy(this_pyr->interp->mem);

    mem_sys_free(this_pyr->interp->limits,sizeof(interp_limits));
    mem_sys_free(this_pyr->interp->privileges,sizeof(interp_privileges));

#ifdef PROF_MODE
    mem_sys_free(this_pyr->interp->profile,sizeof(pyr_profile));
#endif

    mem_sys_free(this_pyr->interp,sizeof(interp_state));
    mem_sys_free(this_pyr->flags,sizeof(interp_flags));

#define X(a, b) mem_sys_free_bs(a, UNITS_MTO8(HASH_ALLOC_SIZE));
PYR_TAGS    
#undef X

}


//
//
pyr_cache *interp_init_globals(pyr_cache *this_pyr){ // interp_init_globals#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    global_mem_sys_alloc_count = 0;
    global_mem_sys_alloc_total = 0;
    global_mem_sys_free_count  = 0;
    global_mem_sys_free_total  = 0;

    return this_pyr;

}


// Initializes the root virtual machine and interpreter-only state...
//
pyr_cache *interp_init(pyr_cache *this_pyr, int argc, char **argv, char **envp, jmp_buf *cat_ex){ // interp_init#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

// XXX //SECURITY// **DO NOT REMOVE** FOLLOWING LINES //SECURITY// XXX //
#ifdef MEM_DEBUG
_prn(" >>MEM_DEBUG<< ");
#endif

#ifdef DEV_MODE
_prn(" >>DEV_MODE<< ");
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

// XXX //SECURITY// **DO NOT REMOVE** ABOVE LINES     //SECURITY// XXX //

//#ifdef DEV_MODE
//    init_dev_overrides(this_pyr);
//#endif

    ////////////////////////////
    // init this_pyr->interp  //
    ////////////////////////////

    this_pyr->interp            = mem_sys_alloc(sizeof(interp_state)); // XXX WAIVER(mem_sys_alloc) XXX //
    this_pyr->interp->cat_ex    = cat_ex;

    interp_init_flags(this_pyr);

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

    interp_init_limits(this_pyr);
    interp_init_privileges(this_pyr);

#ifdef PROF_MODE
    this_pyr->interp->profile = mem_sys_alloc(sizeof(pyr_profile));       // XXX WAIVER(mem_sys_alloc) XXX //
#endif

this_pyr->flags->MEM_USE_MALLOC = SET;

#define X(a, b) a = HASHC(this_pyr, b);
PYR_TAGS    
#undef X

this_pyr->flags->MEM_USE_MALLOC = CLR;

    this_pyr->self = interp_load_root_bvm(this_pyr);

//    init_interp_consts(this_pyr);
//    init_jump_table(this_pyr);
//    init_new_epoch(this_pyr);
//    init_new_srand(this_pyr);
////    interp_new_null_hash(this_pyr);
////    interp_new_stdin_capture(this_pyr);
//
////    bpdl_new(this_pyr);
//    bpdl_init_opcode_table(this_pyr); // XXX MOVE to bpdl_new()
//    bpdl_init_macro_table(this_pyr);  // XXX MOVE to bpdl_new()
//
//
//    init_load_root_bvm(this_pyr);
//
//    init_capture_argv(this_pyr);
//
//#ifdef PROF_MODE
//    init_bvm_profile(this_pyr);
//#endif
//
//    this_pyr->flags->INTERP_BOOT_IN_PROGRESS = FLAG_CLR;
//
//#if( defined BABEL_RESET_TRACE && defined MEM_DEBUG )
//    _d(mem_sys_alloc_count);
//    _d(mem_sys_alloc_total);
//#endif

    this_pyr->flags->INTERP_BOOT_IN_PROGRESS = CLR;

#ifdef INTERP_RESET_TRACE
_msg("INTERP_RESET_TRACE: COMPLETE");
#endif

    return this_pyr;

}


//
//
pyr_cache *interp_init_flags(pyr_cache *this_pyr){ // interp_init_flags#

#ifdef INTERP_RESET_TRACE
_reset_trace;
#endif

    interp_flags *f = mem_sys_alloc(sizeof(interp_flags)); // XXX WAIVER(mem_sys_alloc) XXX //

    f->MEM_ALLOC_BLOCKING                   = SET; // Cleared after mem_new in interp_init
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

    interp_limits *l = mem_sys_alloc(sizeof(interp_limits)); // XXX WAIVER(mem_sys_alloc) XXX //

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

    interp_privileges *p = mem_sys_alloc(sizeof(interp_privileges)); // XXX WAIVER(mem_sys_alloc) XXX //

    p->FILE_READ_ALLOWED  = SET;
    p->FILE_WRITE_ALLOWED = SET;
    p->SYS_EXEC_ALLOWED   = SET;
    p->BVM_EXEC_ALLOWED   = SET;
    p->BVM_FORK_ALLOWED   = SET;

    this_pyr->interp->privileges = p;

    return this_pyr;

}


//
//
mword *interp_load_root_bvm(pyr_cache *this_pyr){

    return interp_init_load_from_file(this_pyr, "rt/root.bbl");

}


// XXX WAIVER REQUIRED interp_init_load_from_file XXX
//
mword *interp_init_load_from_file(pyr_cache *this_pyr, char *filename){ // interp_init_load_from_file#

#ifdef BABEL_RESET_TRACE
_trace;
#endif

    FILE *f = io_open_file(this_pyr, (mword*)filename);
    mword file_size = io_file_size(this_pyr, f);

    mword *file_buffer = mem_alloc(this_pyr, file_size);

    fread((char*)file_buffer, 1, file_size, f);

    io_close_file(this_pyr, f);

//    file_buffer++;

    return file_buffer;

}


//
//
FILE *io_open_file(pyr_cache *this_pyr, mword *filename){ // io_open_file#

    FILE*  file;

//    char *bfilename = (char*)string_b2c(this_pyr, filename);

    file = fopen( (char*)filename, "rb" );

    // FIXME: set a flag instead? cat_ex is overkill...
    if(file==NULL){ 
        _fatal((char*)filename);
    }

    return file;

}


//
//
mword io_file_size(pyr_cache *this_pyr, FILE *file){ // io_file_size#

    fseek(file, 0L, SEEK_END);
    mword size = ftell(file);
    rewind(file);

    return size;

}


//
//
pyr_cache *io_close_file(pyr_cache *this_pyr, FILE *file){ // io_close_file#

    fclose(file); // Return value can be checked to see if there were any issues closing the file...

    return this_pyr;

}


////
////
//pyr_cache *init_load_root_bvm(pyr_cache *this_pyr){ // init_load_root_bvm#
//
//#ifdef BABEL_RESET_TRACE
//_trace;
//#endif
//
//    mword *load_bbl = (void*)mem_alloc(this_pyr, UNITS_MTO8(BBL_SIZE)); // BBL_SIZE is sfield/4
//    memcpy(load_bbl, bbl, (size_t)UNITS_MTO8(BBL_SIZE)); // XXX WAIVER(memcpy) XXX //
//    this_pyr->self = _load(this_pyr, (mword*)load_bbl, BBL_SIZE);
//
//this_pyr->flags->MC_USE_MALLOC = FLAG_SET; // XXX WAIVER(MC_USE_MALLOC); temporary until large_alloc_list XXX //
//
//    mword *load_null_bvm = (void*)mem_alloc(this_pyr, UNITS_MTO8(NULL_BVM_SIZE)); // NULL_BVM_SIZE is sfield/4
//    memcpy(load_null_bvm, null_bvm, (size_t)UNITS_MTO8(NULL_BVM_SIZE)); // XXX WAIVER(memcpy) XXX //
//    this_pyr->interp->cold_null_BVM = _load(this_pyr, (mword*)load_null_bvm, NULL_BVM_SIZE);
//
//this_pyr->flags->MC_USE_MALLOC = FLAG_CLR; // XXX WAIVER(MC_USE_MALLOC); temporary until large_alloc_list XXX //
//
//    return this_pyr;
//
//}


////
////
//pyr_cache *init_jump_table(pyr_cache *this_pyr){ // init_jump_table#
//
//#ifdef BABEL_RESET_TRACE
//_trace;
//#endif
//
////    this_pyr->interp->jump_table = 
////        _newlfcp(this_pyr, (mword*)opcode_array, NUM_INTERP_OPCODES);
//
//    this_pyr->interp->jump_table = mem_sys_alloc(UNITS_MTO8(NUM_INTERP_OPCODES));
//
//    int i;
//    for(i=0;i<NUM_INTERP_OPCODES;i++){
//        this_pyr->interp->jump_table[i] = (mword)opcode_array[i];
//    }
//
//    return this_pyr;
//
//}
//
//
////
////
//pyr_cache *init_new_epoch(pyr_cache *this_pyr){ // init_new_epoch#
//
//#ifdef BABEL_RESET_TRACE
//_trace;
//#endif
//
//    time_t rawtime;
//    struct tm *utc_epoch;
//
//    time( &rawtime );
//    utc_epoch = gmtime( &rawtime );
//
//    //    tm_sec     int    seconds after the minute    0-61*
//    //    tm_min     int    minutes after the hour      0-59
//    //    tm_hour    int    hours since midnight        0-23
//    //    tm_mday    int    day of the month            1-31
//    //    tm_mon     int    months since January        0-11
//    //    tm_year    int    years since 1900
//    //    tm_wday    int    days since Sunday           0-6
//    //    tm_yday    int    days since January 1        0-365
//
//    struct tm *bvm_utc_epoch = mem_sys_alloc(sizeof(struct tm)); //&this_pyr->interp->utc_epoch;
//
//    bvm_utc_epoch->tm_sec  = utc_epoch->tm_sec;
//    bvm_utc_epoch->tm_min  = utc_epoch->tm_min;
//    bvm_utc_epoch->tm_hour = utc_epoch->tm_hour;
//    bvm_utc_epoch->tm_mday = utc_epoch->tm_mday;
//    bvm_utc_epoch->tm_mon  = utc_epoch->tm_mon;
//    bvm_utc_epoch->tm_year = utc_epoch->tm_year;
//    bvm_utc_epoch->tm_wday = utc_epoch->tm_wday;
//    bvm_utc_epoch->tm_yday = utc_epoch->tm_yday;
//
//    this_pyr->interp->utc_epoch = bvm_utc_epoch;
//
//    this_pyr->interp->epoch_ms = time_ms();
//
//    return this_pyr;
//
//}
//
//
//// must be called AFTER init_new_epoch
////
//void init_new_srand(pyr_cache *this_pyr){ // init_new_srand#
//
//#ifdef BABEL_RESET_TRACE
//_trace;
//#endif
//
//    #define SRAND_STRING_SIZE 32
//
//    char srand_string[SRAND_STRING_SIZE];
//
//    sprintf(srand_string, 
//            "%d%d%d%d%d%d", 
//            this_pyr->interp->utc_epoch->tm_sec,
//            this_pyr->interp->utc_epoch->tm_min,
//            this_pyr->interp->utc_epoch->tm_hour,
//            this_pyr->interp->utc_epoch->tm_mday,
//            this_pyr->interp->utc_epoch->tm_mon,
//            this_pyr->interp->utc_epoch->tm_year);
//
//    mword *babel_srand_string = string_c2b(this_pyr, srand_string, SRAND_STRING_SIZE);
//
//    mword *time_hash = _hash8(this_pyr, babel_srand_string);
//
////    mword *time_hash = _pearson16(
////                            this_pyr,  
////                            hash_init, 
////                            babel_srand_string,
////                            _arlen8(this_pyr, babel_srand_string) );
//
//    init_by_array( (unsigned long *)time_hash, HASH_SIZE*(sizeof(mword)/sizeof(unsigned long)));
//
//    this_pyr->interp->srand = time_hash;
//
//    time_hash = _hash(this_pyr, time_hash);
//
//    xoroshiro128_s[0] = time_hash[0];
//    xoroshiro128_s[1] = time_hash[1];
//
////    return time_hash;
//
//}
//
//
////
////
//pyr_cache *init_nil(pyr_cache *this_pyr){ // init_nil#
//
//#ifdef BABEL_RESET_TRACE
//_trace;
//#endif
//
//    mword *nil_hash = _sys_cp( this_pyr, HASH8(this_pyr, "nil") );
//
//    nil = mem_sys_alloc(UNITS_MTO8(TPTR_SIZE));
//    nil++;
//
//    s(nil) = 0;
//
//    memcpy(nil, nil_hash, UNITS_MTO8(HASH_SIZE)); // XXX WAIVER(memcpy) XXX
//
//    lcl(nil,HASH_SIZE)   = -1 * UNITS_MTO8(1);
//    lci(nil,HASH_SIZE+1) = nil;
//
//    this_pyr->interp->nil = nil;
//
//    return this_pyr;
//
//}
//
//
////
////
//void init_interp_consts(pyr_cache *this_pyr){ // init_interp_consts#
//
//#ifdef BABEL_RESET_TRACE
//_trace;
//#endif
//
//    this_pyr->interp->empty_string = _newlfi(this_pyr, 1, 0);
//    this_pyr->interp->alt_tag      = _sys_cp(this_pyr, BABEL_TAG_ALT);
//    this_pyr->interp->seq_tag      = _sys_cp(this_pyr, BABEL_TAG_SEQ);
//    this_pyr->interp->fail_tag     = _sys_cp(this_pyr, BABEL_TAG_FAIL);
//    this_pyr->interp->pass_tag     = _sys_cp(this_pyr, BABEL_TAG_PASS);
//    this_pyr->interp->accept_tag   = _sys_cp(this_pyr, BABEL_TAG_ACCEPT);
//    this_pyr->interp->reject_tag   = _sys_cp(this_pyr, BABEL_TAG_REJECT);
//    this_pyr->interp->true_tag     = _sys_cp(this_pyr, BABEL_TAG_TRUE);
//    this_pyr->interp->false_tag    = _sys_cp(this_pyr, BABEL_TAG_FALSE);
//    this_pyr->interp->exist_tag    = _sys_cp(this_pyr, BABEL_TAG_EXIST);
//    this_pyr->interp->unexist_tag  = _sys_cp(this_pyr, BABEL_TAG_UNEXIST);
//
//}
//
//
////
////
//pyr_cache *init_capture_argv(pyr_cache *this_pyr){ // init_capture_argv#
//
//#ifdef BABEL_RESET_TRACE
//_trace;
//#endif
//
//    // 1. babel
//    // 2. switches
//    // 3. - OR .bbl file name
//
//    char **argv = this_pyr->interp->argv;
//    int    argc = this_pyr->interp->argc;
//
//    mword *temp_argv = nil;
//
//    //initialize argv
//    //XXX This will change when we add CLI processing:
//    #define NUM_BABEL_INTERP_ARGS 1 
//
//    if(argc > NUM_BABEL_INTERP_ARGS){
//
//        temp_argv = _cons(
//                        this_pyr, 
//                        string_c2b(this_pyr, argv[NUM_BABEL_INTERP_ARGS], 100), nil);
//
//        int i;
//        for( i = NUM_BABEL_INTERP_ARGS+1; i < argc; i++ ){
//
//            temp_argv = _unshift(this_pyr, temp_argv, string_c2b(this_pyr, argv[i], 100));
//
//        }
//
//    }
//
//    this_pyr->interp->interp_argv = temp_argv;
//
//    trie_insert(
//            this_pyr, 
//            tptr_detag(this_pyr, 
//                this_pyr->self), 
//            BABEL_SYM_ARGV, 
//            nil, 
//            temp_argv);
//
//    return this_pyr;
//
//}
//
//
////
////
//#ifdef PROF_MODE
//void init_bvm_profile(pyr_cache *this_pyr){
//
//    bvm_profile *p = this_pyr->interp->profile;
//
//    p->BVM_PROFILE_ENABLE   = FLAG_CLR;
//    p->operator_ms          = 0;
//    p->GC_ms                = 0;
//    p->cache_flush_count    = 0;
//    p->cache_update_count   = 0;
//    p->mem_alloc_count      = 0;
//    p->stack_pops           = 0;
//    p->stack_pushes         = 0;
//    p->GC_overshoot         = 0;
//    p->interp_boot_time     = 0;
//
//}
//#endif
//
//
////
////
//void init_dev_overrides(pyr_cache *this_pyr){ // init_dev_override#
//
//#ifdef BABEL_RESET_TRACE
//_trace;
//#endif
//
//    FILE *f = io_open_file(this_pyr, (mword*)"init_dev_overrides.bbl");
//    mword file_size = io_file_size(this_pyr, f);
//
//    global_dev_overrides = mem_sys_alloc(file_size);
//
//    fread( (char*)global_dev_overrides, 1, file_size, f );
//
//    io_close_file(this_pyr, f);
//
//    global_dev_overrides++;
//
//}
//


// Clayton Bauman 2016

