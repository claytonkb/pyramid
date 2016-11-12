// interp.h
//

//#include "hash.h"

#ifndef INTERP_H
#define INTERP_H

int interp_pyramid(pyr_cache *this_pyr, int argc, char **argv, char **envp);

pyr_cache *interp_init(pyr_cache *this_pyr, int argc, char **argv, char **envp, jmp_buf *cat_ex);
pyr_cache *interp_init_flags(pyr_cache *this_pyr);
pyr_cache *interp_init_limits(pyr_cache *this_pyr);
pyr_cache *interp_init_privileges(pyr_cache *this_pyr);
pyr_cache *interp_init_globals(pyr_cache *this_pyr);

void interp_exit(pyr_cache *this_pyr);

mword *interp_load_root_bvm(pyr_cache *this_pyr);

mword *interp_init_load_from_file(pyr_cache *this_pyr, char *filename);
FILE *io_open_file(pyr_cache *this_pyr, mword *filename);
mword io_file_size(pyr_cache *this_pyr, FILE *file);
pyr_cache *io_close_file(pyr_cache *this_pyr, FILE *file);

#endif //INTERP_H

// Clayton Bauman 2013


