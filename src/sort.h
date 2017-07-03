// sort.h
// 

#ifndef SORT_H
#define SORT_H

#define SORT_ARRAY_TYPE_VAL 0
#define SORT_ARRAY_TYPE_PTR 1

void sort(pyr_cache *this_pyr, mword *array, sort_type st);
void sort_r(pyr_cache *this_pyr, mword left, mword right, mword *array, sort_type st);
void sort_aop(pyr_cache *this_pyr, mword *array, mword key_index, sort_type st);
void sort_aop_unsigned(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index);
void sort_aop_signed(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index);
void sort_aop_alpha_mword(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index);
void sort_aop_alpha_byte(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index);
void sort_aop_lex_mword(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index);
void sort_aop_lex_byte(pyr_cache *this_pyr, mword left, mword right, mword *array, mword key_index);
void sort_insertion(pyr_cache *this_pyr, mword *begin, mword *end, mword array_type, sort_type st, mword key_index);

#endif //SORT_H

// Clayton Bauman 2017

