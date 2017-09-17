// std.h
// 

#ifndef STD_H
#define STD_H

mword  std_time_ms(void);
val    std_genrand32(pyr_cache *this_pyr, mword num_mwords);
mword *std_new_paged_array(pyr_cache *this_pyr, mword page_size, int init_sfield);
void   std_resize_paged_array(pyr_cache *this_pyr, mword *pa, int new_sfield);
mword *std_read_with_pbp(pyr_cache *this_pyr, mword *bs, mword *pbp);
mword  std_write_with_pbp(pyr_cache *this_pyr, mword *bs, mword *pbp, mword *payload);

#endif //STD_H

// Clayton Bauman 2017

