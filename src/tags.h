// tags.h
//

#ifndef TAGS_H 
#define TAGS_H 

#ifdef COMPAT_MODE
#define PYR_TAGS                                                           \
    X(PYR_TAG_CODE_ARRAY       , "/pyramid/tag/code_array")                \
    X(PYR_TAG_OPERAND          , "/pyramid/tag/operand")                   \
    X(PYR_TAG_OPERATOR         , "/pyramid/tag/operator")                  \
    X(PYR_TAG_ADD_OP           , "/pyramid/tag/add_op")                    \
    X(PYR_TAG_NIL              , "/babel/tag/nil")                         \
    X(PYR_TAG_INTERP_NIL       , "nil")                                    \
    X(PYR_TAG_EXIST            , "/babel/tag/exist")                       \
    X(PYR_TAG_UNEXIST          , "/babel/tag/unexist")                     \
    X(PYR_TAG_DEF              , "/babel/tag/def")                         \
    X(PYR_TAG_UNDEF            , "/babel/tag/undef")                       \
    X(PYR_TAG_TRUE             , "/babel/tag/true")                        \
    X(PYR_TAG_FALSE            , "/babel/tag/false")                       \
    X(PYR_TAG_ACCEPT           , "/babel/tag/accept")                      \
    X(PYR_TAG_REJECT           , "/babel/tag/reject")                      \
    X(PYR_TAG_PASS             , "/babel/tag/pass")                        \
    X(PYR_TAG_FAIL             , "/babel/tag/fail")                        \
    X(PYR_TAG_NAN              , "/babel/tag/nan")                         \
    X(PYR_TAG_REF              , "/babel/tag/ref")                         \
    X(PYR_TAG_PURE_REF         , "/babel/tag/pure_ref")                    \
    X(PYR_TAG_HASH_TABLE       , "/babel/tag/hash_table")                  \
    X(PYR_TAG_HASH_TABLE_ENTRY , "/babel/tag/hash_table_entry")            \
    X(PYR_TAG_DIRECTORY        , "/babel/tag/directory")                   \
    X(PYR_TAG_PYR_STRING       , "/babel/tag/babel_string")                \
    X(PYR_TAG_CSTRING          , "/babel/tag/cstring")                     \
    X(PYR_TAG_STRING_ARRAY     , "/babel/tag/string_array")                \
    X(PYR_TAG_STRING_LIST      , "/babel/tag/string_list")                 \
    X(PYR_TAG_NUMERIC          , "/babel/tag/numeric")                     \
    X(PYR_TAG_ARRAY8           , "/babel/tag/array8")                      \
    X(PYR_TAG_ARRAY1           , "/babel/tag/array1")                      \
    X(PYR_TAG_BVM              , "/babel/tag/bvm")                         \
    X(PYR_TAG_SYM_BVM          , "/babel/tag/sym_bvm")                     \
    X(PYR_TAG_HIBER_BVM        , "/babel/tag/hiber_bvm")                   \
    X(PYR_TAG_SPARSE_BVM       , "/babel/tag/sparse_bvm")                  \
    X(PYR_TAG_READY_BVM        , "/babel/tag/ready_bvm")                   \
    X(PYR_TAG_BVM_CODE         , "/babel/tag/bvm_code")                    \
    X(PYR_TAG_BVM_STACK        , "/babel/tag/bvm_stack")                   \
    X(PYR_TAG_BVM_RSTACK       , "/babel/tag/bvm_rstack")                  \
    X(PYR_TAG_BVM_JUMP_TABLE   , "/babel/tag/bvm_jump_table")              \
    X(PYR_TAG_BVM_SYM_TABLE    , "/babel/tag/bvm_sym_table")               \
    X(PYR_TAG_BVM_STEPS        , "/babel/tag/bvm_steps")                   \
    X(PYR_TAG_EVAL             , "/babel/tag/eval")                        \
    X(PYR_TAG_LOOP             , "/babel/tag/loop")                        \
    X(PYR_TAG_TIMES            , "/babel/tag/times")                       \
    X(PYR_TAG_IFTE             , "/babel/tag/ifte")                        \
    X(PYR_TAG_EACH             , "/babel/tag/each")                        \
    X(PYR_TAG_EACHAR           , "/babel/tag/eachar")                      \
    X(PYR_TAG_WHILE            , "/babel/tag/while")                       \
    X(PYR_TAG_NEST             , "/babel/tag/nest")                        \
    X(PYR_TAG_ALT              , "/babel/tag/alt")                         \
    X(PYR_TAG_SEQ              , "/babel/tag/seq")                         \
    X(PYR_TAG_LET              , "/babel/tag/let")                         \
    X(PYR_TAG_REF_HASH         , "/babel/tag/ref_hash")                    \
    X(PYR_TAG_REF_STRING       , "/babel/tag/ref_string")                  \
    X(PYR_TAG_REF_ARRAY        , "/babel/tag/ref_array")                   \
    X(PYR_TAG_REF_SYM_LOCAL    , "/babel/tag/ref_sym_local")               \
    X(PYR_TAG_REF_SYM_GLOBAL   , "/babel/tag/ref_sym_global")              \
    X(PYR_TAG_COND             , "/babel/tag/cond")                        \
    X(PYR_TAG_TRIE             , "/babel/tag/trie")                        \
    X(PYR_TAG_TRIE_ENTRY       , "/babel/tag/trie_entry")                  \
    X(PYR_TAG_SENTINEL         , "/babel/tag/sentinel")                    \
    X(PYR_TAG_MONKEY           , "/babel/tag/monkey")                      \
    X(PYR_TAG_ROBOT            , "/babel/tag/robot")                       \
    X(PYR_TAG_UNTYPED          , "/babel/tag/untyped")                     \
    X(PYR_TAG_BVM_INITD        , "/babel/tag/bvm_initd")                   \
    X(PYR_SYM_ARGV             , "/babel/sym/argv")                        \
    X(PYR_SYM_PARENT_BVM       , "/babel/sym/parent_bvm")                  \
    X(PYR_SYM_CODE_RESTART_POINT , "/babel/sym/code_restart_point")       
#else
#define PYR_TAGS                                                             \
    X(PYR_TAG_CODE_ARRAY       , "/pyramid/tag/code_array")                \
    X(PYR_TAG_OPERAND          , "/pyramid/tag/operand")                   \
    X(PYR_TAG_OPERATOR         , "/pyramid/tag/operator")                  \
    X(PYR_TAG_ADD_OP           , "/pyramid/tag/add_op")                    \
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
#endif //COMPAT_MODE

#endif //TAGS_H

// Clayton Bauman 2017


