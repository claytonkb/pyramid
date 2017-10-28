// math.h
// 

#ifndef MATH_H
#define MATH_H 

val math_add(pyr_cache *this_pyr, val a, val b);
val math_neg(pyr_cache *this_pyr, val a);
val math_sub(pyr_cache *this_pyr, val a, val b);
val math_lattice_mul(pyr_cache *this_pyr, val a, val b);
hword math_add_hword(pyr_cache *this_pyr, hword *a, mword size_a, hword *b, mword size_b);
void math_add_special(pyr_cache *this_pyr, mword *in, mword *out);

#endif //MATH_H

// Clayton Bauman 2017

