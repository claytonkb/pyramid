// introspect.h
// 

#ifndef INTROSPECT_H
#define INTROSPECT_H

typedef mword (*bstruct_op_fn_ptr)(pyr_cache *this_pyr, mword *bs, void *v);

void _rclean(pyr_cache *this_pyr, mword *bs);
void wstrim(pyr_cache *this_pyr, mword *bs);

mword *_bs2str(pyr_cache *this_pyr, mword *bs);
void _rbs2str(pyr_cache *this_pyr, mword *bs, mword *result, mword *offset);

mword introspect_calc_buf_length( pyr_cache *this_pyr, mword *bs, mword const_padding, mword nlf_scaling, mword nva_scaling, mword nin_scaling, mword nptr_scaling, mword ntag_scaling);

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

#endif //INTROSPECT_H

// Clayton Bauman 2016

