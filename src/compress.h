// mtf.h
// 

#ifndef COMPRESS_H
#define COMPRESS_H

#define MTF_UPDATE_TABLE_THRESH 0
#define MTF_BYTE_TABLE_SIZE  256
#define BWT_COUNT_TABLE_SIZE 256

mword *compress_bwt_encode_block(pyr_cache *this_pyr, char *start, int block_size);
mword *compress_bwt_decode_block(pyr_cache *this_pyr, char *start, int block_size);
int compress_bwt_compare_encode(const void *a, const void *b);
int compress_bwt_compare_decode(const void *a, const void *b);

void compress_mtf_encode(pyr_cache *this_pyr, val8 in);
void compress_mtf_decode(pyr_cache *this_pyr, val8 in);

char compress_mtf_update_index_table(mword *byte_table, char c);
char compress_mtf_update_byte_table(mword *byte_table, char c);

char *compress_range_encoder(pyr_cache *this_pyr, char *in);

char *compress_rle_encode(pyr_cache *this_pyr, val8 in);

extern const mword mtf_byte_table[MTF_BYTE_TABLE_SIZE];

#endif //COMPRESS_H

// Clayton Bauman 2017

