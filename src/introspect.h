// introspect.h
// 

#ifndef INTROSPECT_H
#define INTROSPECT_H

mword *_bs2str(pyr_cache *this_pyr, mword *bs);
void _rbs2str(pyr_cache *this_pyr, mword *bs, mword *result, mword *offset);

mword introspect_calc_buf_length( pyr_cache *this_pyr, mword *bs, mword const_padding, mword nlf_scaling, mword nva_scaling, mword nin_scaling, mword nptr_scaling, mword ntag_scaling);

mword *_bs2gv(pyr_cache *this_pyr, mword *bs);
void _rbs2gv(pyr_cache *this_pyr, mword *bs, mword *result, mword *offset, int initial);

#endif //INTROSPECT_H

// Clayton Bauman 2016

