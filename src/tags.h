// tags.h
//

#ifndef TAGS_H 
#define TAGS_H 

// noun --> a blob that you can do something to
// verb --> a blob that can do something to other blobs
// pred --> a blob that has a matching predicate-test that reduces to T/F

#define PYR_NUM_NOUN_TAGS 47
#define PYR_NUM_VERB_TAGS 16
#define PYR_NUM_PRED_TAGS 12
#define PYR_NUM_FORM_SYMS 19

#define PYR_XBAR_NUM_FIELDS 4

#define PYR_NOUN_TAG  0
#define PYR_VERB_TAG  1
#define PYR_PRED_TAG  2
#define PYR_FORM_SYM  3
#define PYR_SPEC_FORM 4
#define PYR_EXC_TAG   5

//    X(c_name                    , "/pyramid/tag/<name>"         ,    verb_fn      ,
//          blob_type             , template                      ,    to_verb      ,
//          to_noun               , cacher                        ,    unchacher    ,
//          reserved              , reserved                      ,    reserved     );
#define XBAR_HASH_OFF           0
#define XBAR_STR_OFF            1
#define XBAR_VERB_FN_OFF        2
#define XBAR_BLOB_TYPE_OFF      3
#define XBAR_TEMPLATE_OFF       4
#define XBAR_TO_VERB_OFF        5
#define XBAR_TO_NOUN_OFF        6
#define XBAR_TO_CACHER_OFF      7
#define XBAR_TO_UNCACHER_OFF    8
#define XBAR_RESERVED9_OFF      9
#define XBAR_RESERVED10_OFF     10
#define XBAR_RESERVED11_OFF     11

#define is_noun(x)         (vcar(rdp(x,XBAR_BLOB_TYPE_OFF)) == PYR_NOUN_TAG)
#define is_verb(x)         (vcar(rdp(x,XBAR_BLOB_TYPE_OFF)) == PYR_VERB_TAG)
#define is_pred(x)         (vcar(rdp(x,XBAR_BLOB_TYPE_OFF)) == PYR_PRED_TAG)
#define is_form_sym(x)     (vcar(rdp(x,XBAR_BLOB_TYPE_OFF)) == PYR_FORM_SYM)
#define is_special_form(x) (vcar(rdp(x,XBAR_BLOB_TYPE_OFF)) == PYR_SPEC_FORM)
#define is_exception(x)    (vcar(rdp(x,XBAR_BLOB_TYPE_OFF)) == PYR_EXC_TAG)

typedef enum pyr_tag_type_enum {NOUN, VERB, PRED, SYM} pyr_tag_type; // pyr_tag_type#

#ifdef COMPAT_MODE // Note: This file will not compile in non-COMPAT_MODE :(

#define PYR_NOUN_TAGS                                           \
    X(PYR_TAG_PAGED_ARRAY      , "/pyramid/tag/paged_array",    UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_LIST             , "/pyramid/tag/list",           UNINIT_FN_PTR, \
          PYR_SPEC_FORM           , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_ARRAY8           , "/babel/tag/array8",           UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_ARRAY1           , "/babel/tag/array1",           UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_INTERP_NIL       , "nil",                         UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_NIL              , "/babel/tag/nil",              UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_CODE_ARRAY       , "/pyramid/tag/code_array",     UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_PYR_ENV          , "/pyramid/tag/pyr_env",        UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_OPERAND          , "/pyramid/tag/operand",        UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_OPERATOR         , "/pyramid/tag/operator",       UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_CPTR             , "/pyramid/tag/cptr",           UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_NAN              , "/babel/tag/nan",              UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_REF              , "/babel/tag/ref",              UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_PURE_REF         , "/babel/tag/pure_ref",         UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_HASH_TABLE       , "/babel/tag/hash_table",       UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_HASH_TABLE_ENTRY , "/babel/tag/hash_table_entry", UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_DIRECTORY        , "/babel/tag/directory",        UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_PYR_STRING       , "/babel/tag/babel_string",     UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_CSTRING          , "/babel/tag/cstring",          UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_STRING_ARRAY     , "/babel/tag/string_array",     UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_STRING_LIST      , "/babel/tag/string_list",      UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_NUMERIC          , "/babel/tag/numeric",          UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_BVM              , "/babel/tag/bvm",              UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_SYM_BVM          , "/babel/tag/sym_bvm",          UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_HIBER_BVM        , "/babel/tag/hiber_bvm",        UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_SPARSE_BVM       , "/babel/tag/sparse_bvm",       UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_READY_BVM        , "/babel/tag/ready_bvm",        UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_BVM_CODE         , "/babel/tag/bvm_code",         UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_BVM_STACK        , "/babel/tag/bvm_stack",        UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_BVM_RSTACK       , "/babel/tag/bvm_rstack",       UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_BVM_JUMP_TABLE   , "/babel/tag/bvm_jump_table",   UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_BVM_SYM_TABLE    , "/babel/tag/bvm_sym_table",    UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_BVM_STEPS        , "/babel/tag/bvm_steps",        UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_REF_HASH         , "/babel/tag/ref_hash",         UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_REF_STRING       , "/babel/tag/ref_string",       UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_REF_ARRAY        , "/babel/tag/ref_array",        UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_REF_SYM_LOCAL    , "/babel/tag/ref_sym_local",    UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_REF_SYM_GLOBAL   , "/babel/tag/ref_sym_global",   UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_TRIE             , "/babel/tag/trie",             UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_TRIE_ENTRY       , "/babel/tag/trie_entry",       UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_SENTINEL         , "/babel/tag/sentinel",         UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_MONKEY           , "/babel/tag/monkey",           UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_ROBOT            , "/babel/tag/robot",            UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_BVM_INITD        , "/babel/tag/bvm_initd",        UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_ARGV             , "/babel/sym/argv",             UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_PARENT_BVM       , "/babel/sym/parent_bvm",       UNINIT_FN_PTR, \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_CODE_RESTART_POINT , "/babel/sym/code_restart_point", UNINIT_FN_PTR , \
          PYR_NOUN_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   )

#define PYR_VERB_TAGS                                           \
    X(PYR_TAG_ADD_OP           , "/pyramid/tag/arith_maddi",    UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_APPEND           , "/pyramid/tag/append",         list_append_pyr_op, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_CAT              , "/pyramid/tag/cat",            array_cat_pyr_op, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_SUB_OP           , "/pyramid/tag/sub_op",         UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_EVAL             , "/babel/tag/eval",             UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_LOOP             , "/babel/tag/loop",             UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_TIMES            , "/babel/tag/times",            UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_IFTE             , "/babel/tag/ifte",             UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_EACH             , "/babel/tag/each",             UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_EACHAR           , "/babel/tag/eachar",           UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_WHILE            , "/babel/tag/while",            UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_NEST             , "/babel/tag/nest",             UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_ALT              , "/babel/tag/alt",              UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_SEQ              , "/babel/tag/seq",              UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_LET              , "/babel/tag/let",              UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_COND             , "/babel/tag/cond",             UNINIT_FN_PTR, \
          PYR_VERB_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   )

#define PYR_PRED_TAGS                                           \
    X(PYR_TAG_EXIST            , "/babel/tag/exist",            UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_UNEXIST          , "/babel/tag/unexist",          UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_DEF              , "/babel/tag/def",              UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_UNDEF            , "/babel/tag/undef",            UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_TRUE             , "/babel/tag/true",             UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_FALSE            , "/babel/tag/false",            UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_ACCEPT           , "/babel/tag/accept",           UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_REJECT           , "/babel/tag/reject",           UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_PASS             , "/babel/tag/pass",             UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_FAIL             , "/babel/tag/fail",             UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_UNTYPED          , "/babel/tag/untyped",          UNINIT_FN_PTR, \
          PYR_PRED_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \

#define PYR_EXC_TAGS                                           \
    X(PYR_TAG_MALFORMED_EXPR   , "/pyramid/tag/malformed_expr",     UNINIT_FN_PTR, \
          PYR_EXC_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_UNKNOWN_SYM      , "/pyramid/tag/unknown_sym",     UNINIT_FN_PTR, \
          PYR_EXC_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_KNOWN_SYM       , "/pyramid/tag/known_sym",     UNINIT_FN_PTR, \
          PYR_EXC_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_TAG_PIGS_FLY       , "/pyramid/tag/pigs_fly",     UNINIT_FN_PTR, \
          PYR_EXC_TAG            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   )

#define PYR_FORM_SYMS                                           \
    X(PYR_SYM_IGNORE           , "/pyramid/sym/ignore",         UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_QUOTE            , "/pyramid/sym/quote",          UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_SEXPR            , "/pyramid/sym/sexpr",          UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_BYTES            , "/pyramid/sym/bytes",          UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_LIST             , "/pyramid/sym/list",           UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_CODE             , "/pyramid/sym/code",           UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_HASH             , "/pyramid/sym/hash",           UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_OPER             , "/pyramid/sym/oper",           UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_TPTR             , "/pyramid/sym/tptr",           UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_VAL              , "/pyramid/sym/val",            UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_PTR              , "/pyramid/sym/ptr",            UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_TAG              , "/pyramid/sym/tag",            UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_REF              , "/pyramid/sym/ref",            UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_NIL              , "/pyramid/sym/nil",            UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_SYM              , "/pyramid/sym/sym",            UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_BS               , "/pyramid/sym/bs",             UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_QW               , "/pyramid/sym/qw",             UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_SQUOTE           , "/pyramid/sym/squote",         UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   ) \
    X(PYR_SYM_DQUOTE           , "/pyramid/sym/dquote",         UNINIT_FN_PTR, \
          PYR_FORM_SYM            , UNINIT_PTR                    ,    UNINIT_PTR   , \
          UNINIT_VAL            , UNINIT_FN_PTR                 ,    UNINIT_FN_PTR, \
          UNINIT_PTR            , UNINIT_PTR                    ,    UNINIT_PTR   )

#define PYR_TAGS                                                \
    PYR_NOUN_TAGS                                               \
    PYR_VERB_TAGS                                               \
    PYR_PRED_TAGS                                               \
    PYR_EXC_TAGS                                                \
    PYR_FORM_SYMS

#else
#define PYR_TAGS                                                \
    X(PYR_TAG_CODE_ARRAY       , "/pyramid/tag/code_array", UNINIT_FN_PTR)     \
    X(PYR_TAG_OPERAND          , "/pyramid/tag/operand", UNINIT_FN_PTR)        \
    X(PYR_TAG_OPERATOR         , "/pyramid/tag/operator", UNINIT_FN_PTR)       \
    X(PYR_TAG_ADD_OP           , "/pyramid/tag/add_op", UNINIT_FN_PTR)         \
    X(PYR_TAG_NIL              , "/pyramid/tag/nil", UNINIT_FN_PTR)            \
    X(PYR_TAG_INTERP_NIL       , "nil", UNINIT_FN_PTR)                         \
    X(PYR_TAG_EXIST            , "/pyramid/tag/exist", UNINIT_FN_PTR)          \
    X(PYR_TAG_UNEXIST          , "/pyramid/tag/unexist", UNINIT_FN_PTR)        \
    X(PYR_TAG_DEF              , "/pyramid/tag/def", UNINIT_FN_PTR)            \
    X(PYR_TAG_UNDEF            , "/pyramid/tag/undef", UNINIT_FN_PTR)          \
    X(PYR_TAG_TRUE             , "/pyramid/tag/true", UNINIT_FN_PTR)           \
    X(PYR_TAG_FALSE            , "/pyramid/tag/false", UNINIT_FN_PTR)          \
    X(PYR_TAG_ACCEPT           , "/pyramid/tag/accept", UNINIT_FN_PTR)         \
    X(PYR_TAG_REJECT           , "/pyramid/tag/reject", UNINIT_FN_PTR)         \
    X(PYR_TAG_PASS             , "/pyramid/tag/pass", UNINIT_FN_PTR)           \
    X(PYR_TAG_FAIL             , "/pyramid/tag/fail", UNINIT_FN_PTR)           \
    X(PYR_TAG_NAN              , "/pyramid/tag/nan", UNINIT_FN_PTR)            \
    X(PYR_TAG_REF              , "/pyramid/tag/ref", UNINIT_FN_PTR)            \
    X(PYR_TAG_PURE_REF         , "/pyramid/tag/pure_ref", UNINIT_FN_PTR)       \
    X(PYR_TAG_HASH_TABLE       , "/pyramid/tag/hash_table", UNINIT_FN_PTR)     \
    X(PYR_TAG_HASH_TABLE_ENTRY , "/pyramid/tag/hash_table_entry", UNINIT_FN_PTR)           \
    X(PYR_TAG_DIRECTORY        , "/pyramid/tag/directory", UNINIT_FN_PTR)      \
    X(PYR_TAG_PYR_STRING       , "/pyramid/tag/pyramid_string", UNINIT_FN_PTR) \
    X(PYR_TAG_CSTRING          , "/pyramid/tag/cstring", UNINIT_FN_PTR)        \
    X(PYR_TAG_STRING_ARRAY     , "/pyramid/tag/string_array", UNINIT_FN_PTR)   \
    X(PYR_TAG_STRING_LIST      , "/pyramid/tag/string_list", UNINIT_FN_PTR)    \
    X(PYR_TAG_NUMERIC          , "/pyramid/tag/numeric", UNINIT_FN_PTR)        \
    X(PYR_TAG_ARRAY8           , "/pyramid/tag/array8", UNINIT_FN_PTR)         \
    X(PYR_TAG_ARRAY1           , "/pyramid/tag/array1", UNINIT_FN_PTR)         \
    X(PYR_TAG_BVM              , "/pyramid/tag/pyr", UNINIT_FN_PTR)            \
    X(PYR_TAG_SYM_BVM          , "/pyramid/tag/sym_pyr", UNINIT_FN_PTR)        \
    X(PYR_TAG_HIBER_BVM        , "/pyramid/tag/hiber_pyr", UNINIT_FN_PTR)      \
    X(PYR_TAG_SPARSE_BVM       , "/pyramid/tag/sparse_pyr", UNINIT_FN_PTR)     \
    X(PYR_TAG_READY_BVM        , "/pyramid/tag/ready_pyr", UNINIT_FN_PTR)      \
    X(PYR_TAG_BVM_CODE         , "/pyramid/tag/pyr_code", UNINIT_FN_PTR)       \
    X(PYR_TAG_BVM_STACK        , "/pyramid/tag/pyr_stack", UNINIT_FN_PTR)      \
    X(PYR_TAG_BVM_RSTACK       , "/pyramid/tag/pyr_rstack", UNINIT_FN_PTR)     \
    X(PYR_TAG_BVM_JUMP_TABLE   , "/pyramid/tag/pyr_jump_table", UNINIT_FN_PTR) \
    X(PYR_TAG_BVM_SYM_TABLE    , "/pyramid/tag/pyr_sym_table", UNINIT_FN_PTR)  \
    X(PYR_TAG_BVM_STEPS        , "/pyramid/tag/pyr_steps", UNINIT_FN_PTR)      \
    X(PYR_TAG_EVAL             , "/pyramid/tag/eval", UNINIT_FN_PTR)           \
    X(PYR_TAG_LOOP             , "/pyramid/tag/loop", UNINIT_FN_PTR)           \
    X(PYR_TAG_TIMES            , "/pyramid/tag/times", UNINIT_FN_PTR)          \
    X(PYR_TAG_IFTE             , "/pyramid/tag/ifte", UNINIT_FN_PTR)           \
    X(PYR_TAG_EACH             , "/pyramid/tag/each", UNINIT_FN_PTR)           \
    X(PYR_TAG_EACHAR           , "/pyramid/tag/eachar", UNINIT_FN_PTR)         \
    X(PYR_TAG_WHILE            , "/pyramid/tag/while", UNINIT_FN_PTR)          \
    X(PYR_TAG_NEST             , "/pyramid/tag/nest", UNINIT_FN_PTR)           \
    X(PYR_TAG_ALT              , "/pyramid/tag/alt", UNINIT_FN_PTR)            \
    X(PYR_TAG_SEQ              , "/pyramid/tag/seq", UNINIT_FN_PTR)            \
    X(PYR_TAG_LET              , "/pyramid/tag/let", UNINIT_FN_PTR)            \
    X(PYR_TAG_REF_HASH         , "/pyramid/tag/ref_hash", UNINIT_FN_PTR)       \
    X(PYR_TAG_REF_STRING       , "/pyramid/tag/ref_string", UNINIT_FN_PTR)     \
    X(PYR_TAG_REF_ARRAY        , "/pyramid/tag/ref_array", UNINIT_FN_PTR)      \
    X(PYR_TAG_REF_SYM_LOCAL    , "/pyramid/tag/ref_sym_local", UNINIT_FN_PTR)  \
    X(PYR_TAG_REF_SYM_GLOBAL   , "/pyramid/tag/ref_sym_global", UNINIT_FN_PTR) \
    X(PYR_TAG_COND             , "/pyramid/tag/cond", UNINIT_FN_PTR)           \
    X(PYR_TAG_TRIE             , "/pyramid/tag/trie", UNINIT_FN_PTR)           \
    X(PYR_TAG_TRIE_ENTRY       , "/pyramid/tag/trie_entry", UNINIT_FN_PTR)     \
    X(PYR_TAG_SENTINEL         , "/pyramid/tag/sentinel", UNINIT_FN_PTR)       \
    X(PYR_TAG_MONKEY           , "/pyramid/tag/monkey", UNINIT_FN_PTR)         \
    X(PYR_TAG_ROBOT            , "/pyramid/tag/robot", UNINIT_FN_PTR)          \
    X(PYR_TAG_UNTYPED          , "/pyramid/tag/untyped", UNINIT_FN_PTR)        \
    X(PYR_TAG_BVM_INITD        , "/pyramid/tag/pyr_initd", UNINIT_FN_PTR)      \
    X(PYR_SYM_ARGV             , "/pyramid/sym/argv", UNINIT_FN_PTR)           \
    X(PYR_SYM_PARENT_BVM       , "/pyramid/sym/parent_pyr", UNINIT_FN_PTR)     \
    X(PYR_SYM_CODE_RESTART_POINT , "/pyramid/sym/code_restart_point", UNINIT_FN_PTR)       
#endif //COMPAT_MODE

#endif //TAGS_H

// Clayton Bauman 2017


