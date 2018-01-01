// flags.h
//

#ifndef FLAGS_H 
#define FLAGS_H 

#define PYR_INTERP_FLAGS                                                    \
    X(MEM_USE_MEM_SYS_ALLOC)                                                \
    X(MEM_USE_DYN)                                                          \
    X(INTERP_BOOT_IN_PROGRESS)                                              \
    X(INTERP_NO_ASYNC)                                                      \
    X(INTERP_NO_EXCEPT)

#define PYR_VM_FLAGS                                                        \
    X(MEM_ALLOC_BLOCKING)                                                   \
    X(MEM_ALLOC_NON_GC)                                                     \
    X(MEM_GC_BLOCKING)                                                      \
    X(MEM_GC_INTERP_BLOCKING)                                               \
    X(MEM_GC_PENDING)                                                       \
    X(MEM_GC_CONVERT_PENDING_TO_OP_RESTART)                                 \
    X(MEM_GC_PNR)                                                           \
    X(MEM_GC_OP_RESTART)                                                    \
    X(MEM_GC_SECONDARY_BANK_ALLOC)                                          \
    X(MEM_GC_ON_EVERY_OP)                                                   \
    X(PYR_INSTR_IN_PROGRESS)                                                \
    X(PYR_INCOHERENT)                                                       \
    X(PYR_SYMBOLS_DEFINED)                                                  \
    X(PYR_CLEAN)                                                            \
    X(PYR_INTERP_OP_TRACE)                                                  \
    X(PYR_INSTRUMENT_TRIGGER)                                               \
    X(PYR_CODE_LIST_EMPTY)                                                  \
    X(PYR_RETURN_TOS_ON_EXIT)                                               \
    X(PYR_CACHE_INVALID)                                                    \
    X(PYR_CACHE_DIRTY)                                                      \
    X(PYR_CACHE_ONLY)                                                       \
    X(PYR_CACHE_BLOCKING)

#endif // FLAGS_H

// Clayton Bauman 2017


