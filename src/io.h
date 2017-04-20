// io.h
// 

#ifndef IO_H
#define IO_H

//FILE *io_open_file(pyr_cache *this_pyr, mword *filename);
FILE *io_open_file(pyr_cache *this_pyr, mword *filename, char *attr);
mword io_file_size(pyr_cache *this_pyr, FILE *file);
pyr_cache *io_close_file(pyr_cache *this_pyr, FILE *file);
mword *io_slurp8(pyr_cache *this_pyr, char *filename);
mword *io_slurp(pyr_cache *this_pyr, char *filename);
void io_spit(pyr_cache *this_pyr, char *filename, mword *fileout, access_size_sel asize, fileout_type ft);

#endif //IO_H

// Clayton Bauman 2017

