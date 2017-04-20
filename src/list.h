// list.h
// 

#ifndef LIST_H
#define LIST_H

#define _new_cons(x)  (_newin(x, 2))             // _new_cons#
#define _new_dcons(x) (_newin(x, 3))             // _new_dcons#

#define CDR_DIRECTION 0
#define CPR_DIRECTION 1

mword *list_insert(pyr_cache *this_pyr, mword *src_list, mword *dest_list);
mword *list_unshift(pyr_cache *this_pyr, mword *list, mword *bs);
mword *list_shift(pyr_cache *this_pyr, mword *list);
void   list_push(pyr_cache *this_pyr, mword *list, mword *bs);
mword *list_pop(pyr_cache *this_pyr, mword *list);
mword *list_find_end(pyr_cache *this_pyr, mword *list);
mword *list_find_next_to_end(pyr_cache *this_pyr, mword *list);
mword  list_len(pyr_cache *this_pyr, mword *list);
mword *list_to_ptr_array(pyr_cache *this_pyr, mword *list);
mword *list_to_val_array(pyr_cache *this_pyr, mword *list);
mword *list_cdri(pyr_cache *this_pyr, mword *list, mword i);
mword *list_ith(pyr_cache *this_pyr, mword *list, mword i);
mword *list_reverse(pyr_cache *this_pyr, mword *list, mword *new_cdr);
mword *list_split(pyr_cache *this_pyr, mword *list, mword *indices);
mword *list_split_r(pyr_cache *this_pyr, mword *list, mword *indices, mword count);
mword *list_cut(pyr_cache *this_pyr, mword *list, mword index);
mword *list_append(pyr_cache *this_pyr, mword *lists);
mword *list_append_direct(pyr_cache *this_pyr, mword *head_list, mword *tail_list);

mword *dlist_append_direct(pyr_cache *this_pyr, mword *head_list, mword *tail_list);
mword *dlist_insert(pyr_cache *this_pyr, mword *src_list, mword *dest_list);
mword *dlist_insert_prev(pyr_cache *this_pyr, mword *src_list, mword *dest_list);
mword *dlist_find_end(pyr_cache *this_pyr, mword *list);
mword  dlist_len(pyr_cache *this_pyr, mword *list);
mword *dlist_reverse(pyr_cache *this_pyr, mword *list, mword *head, mword direction);
mword *dlist_cut(pyr_cache *this_pyr, mword *list, mword index, mword direction);

#endif //LIST_H

// Clayton Bauman 2013

