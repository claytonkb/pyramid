// trie.h
// 

#ifndef TRIE_H
#define TRIE_H

#define trie_new_entry(pyr,payload,secondary_key,key) _mkls(pyr, 3, payload, secondary_key, key)  // trie_new_entry#

#define trie_entry_get_key(pyr,entry)           list_ith(pyr, entry, 0) // trie_entry_get_key#
#define trie_entry_get_secondary_key(pyr,entry) list_ith(pyr, entry, 1) // trie_entry_get_secondary_key#
#define trie_entry_get_payload(pyr,entry)       list_ith(pyr, entry, 2) // trie_entry_get_payload#
#define trie_entry_get_payload2(pyr,entry)       list_cdri(pyr, entry, 2) // trie_entry_get_payload2#

#define trie_entry_set_payload(pyr,entry,payload)     (ldp(trie_entry_get_payload(pyr,entry), 0)=(payload)) // trie_entry_set_payload#
#define trie_entry_set_payload2(pyr,entry,payload)    (ldp(trie_entry_get_payload2(pyr,entry),0)=(payload)) // trie_entry_set_payload2#

#define trie_exists(pyr, trie, key, secondary_key) (!is_nil(trie_lookup_hash(pyr, trie, key, secondary_key))) // trie_exists#

#ifdef DEV_MODE
#define map_insert(x, y, z) trie_insert(this_pyr, x, HASH8(this_pyr, y), C2B(y), z); // XXX DEV ONLY map_insert#
#endif

mword *trie_new(pyr_cache *this_pyr);

mword *trie_new_cell(pyr_cache *this_pyr, mword *key, mword *secondary_key, mword *payload);
mword *trie_new_cell2(pyr_cache *this_pyr, mword *key, mword *secondary_key, mword *payload);

void trie_insert(pyr_cache *this_pyr, mword *trie, mword *key, mword *secondary_key, mword *payload);
//void rtrie_insert(pyr_cache *this_pyr, mword *trie, mword *key, mword *entry, mword level);

//void trie_insert2(pyr_cache *this_pyr, mword *trie, mword *key, mword *secondary_key, mword *payload);
void rtrie_insert(pyr_cache *this_pyr, mword *trie, mword *key, mword *secondary_key, mword *payload, mword level);

mword *trie_lookup_hash(pyr_cache *this_pyr, mword *trie, mword *hash_key, mword *string_key);
mword *trie_lookup_binary(pyr_cache *this_pyr, mword *trie, mword *binary_key);
mword *rtrie_lookup(pyr_cache *this_pyr, mword *trie, mword *key, mword level);

mword trie_remove(pyr_cache *this_pyr, mword *trie, mword *key, mword *secondary_key);
mword rtrie_remove(pyr_cache *this_pyr, mword *trie, mword *key, mword level);

mword *rtrie_entries(pyr_cache *this_pyr, mword *trie, mword level);
mword *trie_entries(pyr_cache *this_pyr, mword *trie);

#endif //TRIE_H

// Clayton Bauman 2017

