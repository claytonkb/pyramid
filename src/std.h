// std.h
// 

#ifndef STD_H
#define STD_H

mword *frobnicate(void);
mword *std_new_paged_array(pyr_cache *this_pyr, mword page_size, int init_sfield);
void std_resize_paged_array(pyr_cache *this_pyr, mword *pa, int new_sfield);

#endif //STD_H

// Clayton Bauman 2017
