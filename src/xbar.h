// xbar.h
// 

#ifndef XBAR_H
#define XBAR_H

void xbar_new(pyr_cache *this_pyr);
mword *xbar_search(pyr_cache *this_pyr, mword *tag);
mword *xbar_search_string(pyr_cache *this_pyr, mword *bstr);
mword xbar_is_builtin(pyr_cache *this_pyr, mword *tag);
mword *xbar_tag_to_string(pyr_cache *this_pyr, mword *tag);
mword *xbar_tag_to_fn(pyr_cache *this_pyr, mword *tag);

#endif //XBAR_H

// Clayton Bauman 2017

