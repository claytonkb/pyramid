// sexpr.h
// 

#ifndef SEXPR_H
#define SEXPR_H

mword *_pre_sexpr(pyr_cache *this_pyr, mword *string, mword *index);
mword *_sexpr(pyr_cache *this_pyr, mword *string, mword *index, mword *sexpr_type);
mword *_unescape(pyr_cache *this_pyr, mword *string, mword escape_char);
mword *sexpr_from_string(pyr_cache *this_pyr, mword *bstring);

#endif //SEXPR_H

// Clayton Bauman 2017

