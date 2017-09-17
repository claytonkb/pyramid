// pearson.h

#ifndef PEARSON_H
#define PEARSON_H

#define PERM_TABLE_WIDTH 256
#define PERM_TABLE_BYTE_SIZE 4096
#define PERM_TABLE_MWORD_SIZE (PERM_TABLE_BYTE_SIZE/MWORD_SIZE)

//extern const char pearson16_byte_perm[4096];
extern const char pearson16_byte_perm[PERM_TABLE_BYTE_SIZE];

mword *pearson_hash8(pyr_cache *this_pyr, mword *key);
mword *pearson_hash(pyr_cache *this_pyr, mword *key);

void pearson16(char *result, char *init, char *key, mword strlen);
void pearson_marsaglia16(char *result, char* init, char *key, mword strlen);

mword *pearson16a(pyr_cache *this_pyr, char *init, char *key, mword strlen);
mword *pearson_marsaglia16a(pyr_cache *this_pyr, char *init, char *key, mword strlen);

mword  pearson_marsaglia_fuzz(mword reset);

//mword *pearson_rand_val(pyr_cache *this_pyr, mword num_mwords);

#endif //PEARSON_H

//Clayton Bauman 2016

