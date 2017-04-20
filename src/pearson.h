// pearson.h

#ifndef PEARSON_H
#define PEARSON_H

#define PERM_TABLE_WIDTH 256

extern const char pearson16_byte_perm[4096];

mword *pearson_hash8(pyr_cache *this_pyr, mword *key);
mword *pearson_hash(pyr_cache *this_pyr, mword *key);

void pearson16(char *result, char *init, char *key, mword strlen);
void pearson_marsaglia16(char *result, char* init, char *key, mword strlen);

mword *pearson16a(pyr_cache *this_pyr, char *init, char *key, mword strlen);
mword *pearson_marsaglia16a(pyr_cache *this_pyr, char *init, char *key, mword strlen);

mword  pearson_marsaglia_fuzz(mword reset);

#endif //PEARSON_H

//Clayton Bauman 2016

