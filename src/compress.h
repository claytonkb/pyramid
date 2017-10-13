// mtf.h
// 

#ifndef COMPRESS_H
#define COMPRESS_H

#define MTF_UPDATE_TABLE_THRESH 0
#define MTF_BYTE_TABLE_SIZE 256

val8 compress_mtf_encode(pyr_cache *this_pyr, val8 in);
char compress_mtf_encode_byte(char *byte_table, char *index_table, int curr_index, char c);
void compress_mtf_update_tables(char *byte_table, char *index_table, int curr_index, char c);

extern const uint32_t mtf_byte_table[MTF_BYTE_TABLE_SIZE];
extern const uint32_t mtf_index_table[MTF_BYTE_TABLE_SIZE];

#endif //COMPRESS_H

// Clayton Bauman 2017

