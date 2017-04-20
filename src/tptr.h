// tptr.h
// 

#ifndef TPTR_H
#define TPTR_H

mword *tptr_new(pyr_cache *this_pyr, const mword *hash, mword *bs);
mword *tptr_detag(pyr_cache *this_pyr, mword *tptr);
mword *tptr_extract_hash(pyr_cache *this_pyr, mword *tptr);
void tptr_update_hash(pyr_cache *this_pyr, mword *tptr, mword *hash);
mword *tptr_extract_ptr(pyr_cache *this_pyr, mword *tptr);
mword *tptr_hard_detag(pyr_cache *this_pyr, mword *tptr);
mword *tptr_find_tag_unsafe(pyr_cache *this_pyr, mword *bs, mword *tag);
mword *tptr_find_tag(pyr_cache *this_pyr, mword *search_bs, mword *tag);

#endif //TPTR_H

// Clayton Bauman 2016

