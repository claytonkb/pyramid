// bstruct.h
// 

#ifndef BSTRUCT_H
#define BSTRUCT_H

typedef mword (*bstruct_op_fn_ptr)(pyr_cache *this_pyr, mword *bs, void *v);

void _rclean(pyr_cache *this_pyr, mword *bs);
void _recurse(pyr_cache *this_pyr, mword *bs, bstruct_op_fn_ptr bfn, void *v);
mword _fn_recurse(pyr_cache *this_pyr, mword *bs, bstruct_op_fn_ptr bfn, void *v);

mword _mu(pyr_cache *this_pyr, mword *bs);
mword _rmu(pyr_cache *this_pyr, mword *bs, void *v);
mword _nlf(pyr_cache *this_pyr, mword *bs);
mword _rnlf(pyr_cache *this_pyr, mword *bs, void *v);
mword _nin(pyr_cache *this_pyr, mword *bs);
mword _rnin(pyr_cache *this_pyr, mword *bs, void *v);
mword _ntag(pyr_cache *this_pyr, mword *bs);
mword _rntag(pyr_cache *this_pyr, mword *bs, void *v);
mword _nva(pyr_cache *this_pyr, mword *bs);
mword _rnva(pyr_cache *this_pyr, mword *bs, void *v);
mword _nptr(pyr_cache *this_pyr, mword *bs);
mword _rnptr(pyr_cache *this_pyr, mword *bs, void *v);

//mword get_offset_from_ptr(pyr_cache *this_pyr, mword *span_array, mword *ptr);
void set_offset_for_ptr(pyr_cache *this_pyr, mword *span_array, mword *ptr, mword *offset_array, mword this_offset);

mword *bstruct_to_array(pyr_cache *this_pyr, mword *bs);
void bstruct_to_array_r(pyr_cache *this_pyr, mword *bs, mword *arr_list, mword *offset);

mword *bstruct_cp(pyr_cache *this_pyr, mword *bs);

mword *bstruct_load(pyr_cache *this_pyr, mword *bs, mword size);
void bstruct_rload(pyr_cache *this_pyr, mword *tree, mword offset);


mword *bstruct_unload(pyr_cache *this_pyr, mword *bs);
mword bstruct_unload_r(
        pyr_cache   *this_pyr, 
        mword       *bs, 
        mword       *dest, 
        mword       *dest_offset, 
        mword       *span_array, 
        mword       *offset_array);

mword *bstruct_find(pyr_cache *this_pyr, mword *target_bs, mword *find_bs);


#endif //BSTRUCT_H

// Clayton Bauman 2017

