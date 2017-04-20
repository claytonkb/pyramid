// string.c
//

#include "pyramid.h"
#include "string.h"
#include "array.h"
#include "mem.h"
#include "utf8.h"


//
//
mword *string_c2b(pyr_cache *this_pyr, char *string, mword max_safe_length){ // string_c2b#

    mword char_length;

    //strlen is unsafe, use memchr instead:
    void *null_term = memchr(string, 0, (size_t)max_safe_length);

    if(null_term == NULL){
        char_length = max_safe_length;
    }
    else{
        char_length = (mword)((char*)null_term - string);
    }

    mword *result = _newstr(this_pyr, char_length, ' ');

    memcpy(result, string, char_length); // XXX WAIVER(memcpy) XXX //

    return result;

}



// Trims trailing whitespace in a leaf8, similar to trunc() but different ;)
// Intended for internal-use ...
void wstrim(pyr_cache *this_pyr, mword *bs){ // wstrim#

    #define WS_CHAR ' '

    mword size8 = array8_size(this_pyr, bs);

    //find last white-space character
    int i = size8-1;
    char *char_bs = (char*)bs;

    while((i >= 0) && (char_bs[i] == ' ')){
        i--;
    }

    //get new mword-size
    mword msize = array8_mword_size(this_pyr, i);

    //calculate, set new alignment-word
    mword alignment_word = array8_enc_align(this_pyr, i);
    ldv(bs,msize-1) = alignment_word;

    //clean-up next-to-last mword
    ldv(bs,msize-2) = rdv(bs,msize-2) & ~alignment_word;

    //trunc the s-field
    sfield(bs) = UNITS_MTO8(msize);

}


// babel-ized sprintf()
//
void bsprintf( pyr_cache *this_pyr, mword *buf, mword *offset, const char *format, ... ){ // bsprintf#

    int printed_length;

    int buf_length = array8_size(this_pyr, buf) 
                            - *offset 
                            + 1; // +1 because vsnprintf() discounts for the null-terminator

    if(buf_length < 0)
        return;

    va_list args;
    va_start(args,format);

    printed_length = vsnprintf((char*)buf+*offset, buf_length, format, args);

    if(printed_length < 0) // encoding error
        return;

    *offset = *offset + printed_length;

    va_end(args);

}


//
//
mword *_radix2cu(pyr_cache *this_pyr, mword *string, mword radix){ // _radix2cu#

    unsigned long *result = 
        (unsigned long *)
            mem_new_val(
                    this_pyr, 
                    (sizeof(unsigned long) / sizeof(mword)),
                    0);

    *result = strtoul((char*)string,NULL,radix);

    return (mword*)result;

}


//
//
mword *string_to_array(pyr_cache *this_pyr, mword *string){ // string_to_array#

    mword length8 = array8_size(this_pyr, string);
    mword u8_length = (mword)u8_strlen((char *)string, length8);

    mword *result = mem_new_val(this_pyr, u8_length+1, 0);

    mword length = u8_toucs((uint32_t *)result, u8_length+1, (char *)string, length8);

    array_trunc(this_pyr, result, length); //FIXME: Change to array_resize

    return result;

}


// Clayton Bauman 2017

