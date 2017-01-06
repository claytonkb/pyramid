// array.h
// 

#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SORT_TYPE_LEAF     0
#define ARRAY_SORT_TYPE_NON_LEAF 1

#define array8_aligned(x) ((x) % BITS_PER_BYTE == 0)

mword *array_th(pyr_cache *this_pyr, mword *bs, mword entry);

mword array8_read(mword *array, mword offset);
void array8_write(mword *array, mword offset, mword value);
mword *array8_th(pyr_cache *this_pyr, mword *val_array, mword entry8);

mword array8_size(pyr_cache *this_pyr, mword *string);
mword array8_dec_align(pyr_cache *this_pyr, mword alignment_word);
mword array8_enc_align(pyr_cache *this_pyr, mword size8);
mword array8_mword_size(pyr_cache *this_pyr, mword size8);

mword array1_read(mword *array, mword offset);
void array1_write(mword *array, mword offset, mword value);
mword *array1_th(pyr_cache *this_pyr, mword *val_array, mword entry1);

mword array1_size(pyr_cache *this_pyr, mword *string);
mword array1_dec_align(pyr_cache *this_pyr, mword alignment_word);
mword array1_enc_align(pyr_cache *this_pyr, mword size1);
mword array1_mword_size(pyr_cache *this_pyr, mword size1);

mword *array_mwords_to_bytes(pyr_cache *this_pyr, mword *array);
mword *array_bytes_to_mwords(pyr_cache *this_pyr, mword *array8);
mword *array_bits_to_mwords(pyr_cache *this_pyr, mword *array1);
mword *array_mwords_to_bits(pyr_cache *this_pyr, mword *array);

mword *array8_cat(pyr_cache *this_pyr, mword *left, mword *right);
mword *array_cat(pyr_cache *this_pyr, mword *left, mword *right);
mword *array1_cat(pyr_cache *this_pyr, mword *left, mword *right);

int array_cmp_lex(mword *left, mword *right);
int array8_cmp_lex(pyr_cache *this_pyr, mword *left, mword *right);
int array_cmp_alpha(pyr_cache *this_pyr, mword *left, mword *right, access_size_sel access_size);

void array_move(pyr_cache *this_pyr, mword *dest, mword dest_index, mword *src, mword src_index, mword size_arg, access_size_sel access_size);
void array1_move(pyr_cache *this_pyr, mword *dest, mword dest_begin, mword *src, mword size_arg);

void array1_move_single(pyr_cache *this_pyr, mword *dest, mword dest_mod, mword *src, mword size_arg);
void array1_move_double(pyr_cache *this_pyr, mword *dest, mword dest_mod, mword *src, mword size_arg);
void array1_move_n(pyr_cache *this_pyr, mword *dest, mword dest_mod, mword *src, mword size_arg);

mword *array1_slice(pyr_cache *this_pyr, mword *array, mword start, mword end);

#define _arcmp8(x,y,z) array8_cmp_lex(x, y, z)
#define  _arcmp(x,y,z)  array_cmp_lex(x, y, z)

#define _areq8(x,y,z) (array8_cmp_lex(x, y, z) == 0)
#define  _areq(x,y,z)  (array_cmp_lex(x, y, z) == 0)

//mword  _alignment_word8(pyr_cache *this_pyr, mword size8);
////void   _trunc(pyr_cache *this_pyr, mword *operand, mword new_size);
//mword *_slice(pyr_cache *this_pyr, mword *array, mword start, mword end);
//mword  _cxr1(pyr_cache *this_pyr, mword *val, mword bit);
//mword *_th(pyr_cache *this_pyr, mword *bs, mword entry);
//mword *_arcat(pyr_cache *this_pyr, mword *left, mword *right);
//mword  _array8_size(pyr_cache *this_pyr, mword size8);
//mword *_arcat8(pyr_cache *this_pyr, mword *left, mword *right);
//mword *_lf2by(pyr_cache *this_pyr, mword *leaf_arr);
//mword *_by2lf(pyr_cache *this_pyr, mword *array8);
//mword *_lf2bi(pyr_cache *this_pyr, mword *array);
//mword *_bi2lf(pyr_cache *this_pyr, mword *array8);
//mword  _arlen1(pyr_cache *this_pyr, mword *string);
//mword  _dec_alignment_word1(pyr_cache *this_pyr, mword alignment_word);
//mword  _alignment_word1(pyr_cache *this_pyr, mword size1);
//void   _wrcxr1(pyr_cache *this_pyr, mword *arr, mword off, mword val);
//mword  _array1_size(pyr_cache *this_pyr, mword size1);
//void   _msort(pyr_cache *this_pyr, mword *array);
//void   _rmsort(pyr_cache *this_pyr, mword left, mword right, mword *array);
//void   _merge(pyr_cache *this_pyr, mword *array, mword left_start, mword left_end, mword right_start, mword right_end);
////void    move(mword *dest, mword dest_index, mword *src, mword src_index, mword size);
//void    move(pyr_cache *this_pyr, mword *dest, mword dest_index, mword *src, mword src_index, mword size, mword access_size);
//int    _arcmp(mword *left, mword *right);
//int _arcmp8(pyr_cache *this_pyr, mword *left, mword *right);
//mword *_ar2bytes(pyr_cache *this_pyr, mword *array);
//mword *_th8(pyr_cache *this_pyr, mword *bs, mword entry);
//mword *array_find_val(pyr_cache *this_pyr, mword *bs, mword *val);
//mword *array_find_ptr(pyr_cache *this_pyr, mword *bs, mword *ptr);
//mword *_slice8(pyr_cache *this_pyr, mword *array, mword start, mword end);
//
//void array_ptr_sort(pyr_cache *this_pyr, mword *array, mword *comparator);
//void array_rptr_sort(pyr_cache *this_pyr, mword left, mword right, mword *array, mword *comparator, mword sort_type);
//void array_ptr_sort_merge(pyr_cache *this_pyr, mword *array, mword left_start, mword left_end, mword right_start, mword right_end, mword *comparator, mword sort_type);
//
//int array_cmp_alpha(pyr_cache *this_pyr, mword *left, mword *right, mword lex_alpha_select, mword access_size);
//
//pyr_cache *th_r8d(pyr_cache *this_pyr);
//pyr_cache *th_rmd(pyr_cache *this_pyr);
//pyr_cache *th_wmp(pyr_cache *this_pyr);
//pyr_cache *move_md(pyr_cache *this_pyr);
//pyr_cache *move_8d(pyr_cache *this_pyr);
//pyr_cache *arlen_md(pyr_cache *this_pyr);
//pyr_cache *cat_md(pyr_cache *this_pyr);
//pyr_cache *slice_md(pyr_cache *this_pyr);
//pyr_cache *slice_8d(pyr_cache *this_pyr);
//pyr_cache *arcmp_md(pyr_cache *this_pyr);
//pyr_cache *arcmp_8d(pyr_cache *this_pyr);
//
//pyr_cache *sortval_d(pyr_cache *this_pyr);
//pyr_cache *merge_sort_d(pyr_cache *this_pyr);
//
//pyr_cache *cat_8d(pyr_cache *this_pyr);
//pyr_cache *arlen_8d(pyr_cache *this_pyr);
//
//pyr_cache *strcmp_8d(pyr_cache *this_pyr);
//pyr_cache *strcmp_md(pyr_cache *this_pyr);

#endif //ARRAY_H

// Clayton Bauman 2016

