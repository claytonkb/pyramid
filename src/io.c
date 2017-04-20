// io.c
//

#include "pyramid.h"
#include "io.h"
#include "mem.h"
#include "array.h"


//
//
FILE *io_open_file(pyr_cache *this_pyr, mword *filename, char *attr){ // io_open_file#

    FILE*  file;

//    char *bfilename = (char*)string_b2c(this_pyr, filename);

    file = fopen((char*)filename, attr);

    // FIXME(_fatal)
    if(file==NULL){ 
        _fatal((char*)filename);
    }

    // XXX register the opened file in global_irt

    return file;

}


//
//
mword io_file_size(pyr_cache *this_pyr, FILE *file){ // io_file_size#

    fseek(file, 0L, SEEK_END);
    mword size = ftell(file);
    rewind(file);

    return size;

}


//
//
pyr_cache *io_close_file(pyr_cache *this_pyr, FILE *file){ // io_close_file#

    fclose(file); // Return value can be checked to see if there were any issues closing the file...

    // XXX un-register the opened file in global_irt

    return this_pyr;

}


//
//
mword *io_slurp8(pyr_cache *this_pyr, char *filename){ // io_slurp8#

    FILE *f = io_open_file(this_pyr, (mword*)filename, "rb");
    mword file_size = io_file_size(this_pyr, f);

    mword *file_buffer = _newstr(this_pyr, file_size, ' ');
    fread((char*)file_buffer, 1, file_size, f);

    io_close_file(this_pyr, f);

    return file_buffer;

}


//
//
mword *io_slurp(pyr_cache *this_pyr, char *filename){ // io_slurp#

    mword *result = io_slurp8(this_pyr, filename);
    array_trunc(this_pyr, result, size(result)-1);

    return result;

}


//
//
void io_spit(pyr_cache *this_pyr, char *filename, mword *fileout, access_size_sel asize, fileout_type ft){ // io_spit#

    FILE *pFile;

    mword filesize;

    if(asize == MWORD_ASIZE){
        filesize = UNITS_MTO8(size(fileout));
    }
    else{ //asize == BYTE_ASIZE
        filesize = array8_size(this_pyr, fileout);
    }

    if(ft == OVERWRITE){
        pFile = io_open_file(this_pyr, (mword*)filename, "wb");
    }
    else{ // ft = APPEND
        pFile = io_open_file(this_pyr, (mword*)filename, "ab");
    }

    fwrite(fileout, 1, filesize, pFile);

    fclose(pFile);

}


// Clayton Bauman 2017

