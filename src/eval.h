// eval.h
// 

#ifndef EVAL_H
#define EVAL_H

blob eval_apply(pyr_cache *this_pyr, blob b);
blob eval_spec_form_handler(pyr_cache *this_pyr, mword *xbar_entry, blob b);

// These short-hashes were generated in COMPAT_MODE... see babel/spec_forms.sp
#ifdef PYRAMID_32_BIT
#define EVAL_SF_IGNORE      0x913641b3           // /pyramid/sym/ignore
#define EVAL_SF_QUOTE       0xb9dbcfcc           // /pyramid/sym/quote
#define EVAL_SF_SEXPR       0xe6ba2b62           // /pyramid/sym/sexpr
#define EVAL_SF_BYTES       0x11f5b786           // /pyramid/sym/bytes
#define EVAL_SF_LIST        0x758e93f3           // /pyramid/tag/list
#define EVAL_SF_CODE        0x651bd4ce           // /pyramid/sym/code
#define EVAL_SF_HASH        0x55aa666e           // /pyramid/sym/hash
#define EVAL_SF_OPER        0xb7278494           // /pyramid/sym/oper
#define EVAL_SF_TPTR        0xf16e80b3           // /pyramid/sym/tptr
#define EVAL_SF_VAL         0xc60f03db           // /pyramid/sym/val
#define EVAL_SF_PTR         0x7b65e7d9           // /pyramid/sym/ptr
#define EVAL_SF_TAG         0x1aacc8c8           // /pyramid/sym/tag
#define EVAL_SF_REF         0x17d720a1           // /pyramid/sym/ref
#define EVAL_SF_NIL         0x1f08d253           // /pyramid/sym/nil
#define EVAL_SF_SYM         0xd7b9aef7           // /pyramid/sym/sym
#define EVAL_SF_BS          0x37efd097           // /pyramid/sym/bs
#define EVAL_SF_QW          0xe7377349           // /pyramid/sym/qw
#define EVAL_SF_SQUOTE      0x4c095a84           // /pyramid/sym/squote
#define EVAL_SF_DQUOTE      0xe0635b47           // /pyramid/sym/dquote
#endif

#ifdef PYRAMID_64_BIT
#define EVAL_SF_IGNORE      0x2fe09b28913641b3   // /pyramid/sym/ignore
#define EVAL_SF_QUOTE       0x848ce4b2b9dbcfcc   // /pyramid/sym/quote
#define EVAL_SF_SEXPR       0xe66c2118e6ba2b62   // /pyramid/sym/sexpr
#define EVAL_SF_BYTES       0x58d2882e11f5b786   // /pyramid/sym/bytes
#define EVAL_SF_LIST        0x68e37732758e93f3   // /pyramid/tag/list
#define EVAL_SF_CODE        0x5c89f24e651bd4ce   // /pyramid/sym/code
#define EVAL_SF_HASH        0xf21fd52655aa666e   // /pyramid/sym/hash
#define EVAL_SF_OPER        0xd19e83c2b7278494   // /pyramid/sym/oper
#define EVAL_SF_TPTR        0xf95eafc0f16e80b3   // /pyramid/sym/tptr
#define EVAL_SF_VAL         0x9a69a408c60f03db   // /pyramid/sym/val
#define EVAL_SF_PTR         0xfed3fabb7b65e7d9   // /pyramid/sym/ptr
#define EVAL_SF_TAG         0x9b940f5e1aacc8c8   // /pyramid/sym/tag
#define EVAL_SF_REF         0x5ee042d717d720a1   // /pyramid/sym/ref
#define EVAL_SF_NIL         0x95bda1ef1f08d253   // /pyramid/sym/nil
#define EVAL_SF_SYM         0x88c362f1d7b9aef7   // /pyramid/sym/sym
#define EVAL_SF_BS          0xfc3633c637efd097   // /pyramid/sym/bs
#define EVAL_SF_QW          0xb61e0636e7377349   // /pyramid/sym/qw
#define EVAL_SF_SQUOTE      0x2a70e6f94c095a84   // /pyramid/sym/squote
#define EVAL_SF_DQUOTE      0xbdc7dc0de0635b47   // /pyramid/sym/dquote
#endif

#endif //EVAL_H

// Clayton Bauman 2017

