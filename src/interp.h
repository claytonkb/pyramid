// interp.h
//

#ifndef INTERP_H
#define INTERP_H

int interp_pyramid(pyr_cache *this_pyr, int argc, char **argv, char **envp);
void interp_core(pyr_cache *this_pyr);

void interp_boilerplate(void);

void interp_init_once(pyr_cache *this_pyr);
void interp_init_zero_hash(void);
void interp_init_nil_mem(void);

mword *interp_init_golden_nil(void);

pyr_cache *interp_reinit(pyr_cache *this_pyr, mword *golden_nil, int argc, char **argv, char **envp, jmp_buf *cat_ex);
void interp_reinitialize_nil(mword *golden_nil);
pyr_cache *interp_init_flags(pyr_cache *this_pyr);
pyr_cache *interp_init_limits(pyr_cache *this_pyr);
pyr_cache *interp_init_privileges(pyr_cache *this_pyr);

void interp_exit(pyr_cache *this_pyr);

void interp_init_tables(pyr_cache *this_pyr);
void interp_init_tags_strings(pyr_cache *this_pyr);
void interp_init_symbols(pyr_cache *this_pyr);

pyr_cache *interp_uninit_fn(pyr_cache *this_pyr);

mword *interp_load_root_bvm(pyr_cache *this_pyr);
mword *interp_load_dev_overrides(pyr_cache *this_pyr);
mword *interp_load_pyr_env(pyr_cache *this_pyr);

mword *interp_init_load_from_file(pyr_cache *this_pyr, char *filename);

#endif //INTERP_H

// Clayton Bauman 2017

