// string.h
// 

#ifndef STRING_H
#define STRING_H

mword *string_c2b(pyr_cache *this_pyr, char *string, mword max_safe_length);
void wstrim(pyr_cache *this_pyr, mword *bs);
void bsprintf( pyr_cache *this_pyr, mword *buf, mword *offset, const char *format, ... );
mword *_radix2cu(pyr_cache *this_pyr, mword *string, mword radix);
mword *string_to_array(pyr_cache *this_pyr, mword *string);

#endif //STRING_H

// Clayton Bauman 2017

