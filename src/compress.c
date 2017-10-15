// compress.c
// credit: Mark Nelson, Dr. Dobbs 1996

#include "pyramid.h"
#include "compress.h"
#include "introspect.h"
#include "util.h"
#include "mem.h"
#include "array.h"

char *dev_str;

#define _mem_byte(x) \
    dev_str = (char*)_newstr(this_pyr, 8, 0);               \
    for(dev_i=0; dev_i<256; dev_i++){                       \
        sprintf(dev_str, "%08x", x[dev_i]);                 \
        fprintf(stderr, "%s ", dev_str+6);                  \
        if(!((dev_i+1) % 16)){                              \
            fprintf(stderr,"\n");                           \
        }                                                   \
    }


typedef struct { // mem_thread_base#
    char *v; // base ptr of val8
    int   n; // numerical offset of this ptr
} compress_bwt_index;


#define BWT_BLOCK_SIZE 8
#define BWT_MIN_BLOCK_SIZE 8
#define BWT_MAX_BLOCK_SIZE 4096 // Changes to MAX_BLOCK_SIZE could affect encoding of BWT_START_INDEX_WIDTH
#define BWT_START_INDEX_WIDTH 4

static int   bwt_block_size;
static char *bwt_block_ptr;
static compress_bwt_index *bwt_block_indices;


//
//
mword *compress_bwt_encode_block(pyr_cache *this_pyr, char *start, int block_size){ // compress_bwt_encode_block#

    if(block_size > BWT_MAX_BLOCK_SIZE) return (mword*)start;
    if(block_size < BWT_MIN_BLOCK_SIZE) return (mword*)start;

    bwt_block_size = block_size;
    bwt_block_ptr  = start;

    compress_bwt_index *bwt_indices = 
        (compress_bwt_index*)mem_non_gc_alloc( // XXX WAIVER(mem_non_gc_alloc); free'd below XXX //
                sizeof(compress_bwt_index) * block_size);

    bwt_block_indices = bwt_indices;

//    char *result = (char*)_newstr(this_pyr, block_size, ' ');
    char *result = (char*)_newstr(this_pyr, block_size+BWT_START_INDEX_WIDTH, ' '); // +BWT_START_INDEX_WIDTH to store index of start character

    int i;

    for(i=0; i<block_size; i++){
        bwt_indices[i].v = start;
        bwt_indices[i].n = i;
    }

    qsort( bwt_indices,
           block_size,
           sizeof(compress_bwt_index),
           (int(*)(const void *, const void *))compress_bwt_compare_encode);

    int n;

//    for(i=0; i<MWORD_SIZE; i++){ // the start index is stored in printable characters because why not?
//        result[i] = '0' + ;
//    }
    
    int start_index=0;

    for(i=0;i<block_size;i++){
        if(bwt_indices[i].n == 0) start_index=i;
    }

//    result[0] = '0' + ((start_index & 0x000f)    );
//    result[1] = '0' + ((start_index & 0x00f0)>> 4);
//    result[2] = '0' + ((start_index & 0x0f00)>> 8);
//    result[3] = '0' + ((start_index & 0xf000)>>12);

    for(i=0;i<BWT_START_INDEX_WIDTH;i++){
        result[i] = '0' + ((start_index & (0xf << (4*i))) >> 4*i); // ASCII-coded hexadecimal (use BIT_SELECT?)
    }

    // rewrite block using bwt_indices
    for(i=0; i<block_size; i++){
        n = bwt_indices[i].n;
        if(n == 0){
            result[i+BWT_START_INDEX_WIDTH] = start[block_size-1];
        }
        else{
            result[i+BWT_START_INDEX_WIDTH] = start[n-1];
        }
    }

    mem_non_gc_free(bwt_indices);

    return (mword*)result;

}


// undoes compress_bwt_encode_block()
//
mword *compress_bwt_decode_block(pyr_cache *this_pyr, char *start, int block_size){ // compress_bwt_decode_block#

//0123456789A
//^repetition A
//epetition^r 1
//etition^rep 3
//ion^repetit 7
//ition^repet 5
//n^repetitio 9
//on^repetiti 8
//petition^re 2
//repetition^ 0
//tion^repeti 6
//tition^repe 4
//
//
//nrpttoie^ie S
//0123456789A
//
//nrpttoie^ie
//00001000011   letter_counts
//TABLE 1. Number of Preceding Symbols Matching Symbol in Current Position
//
//^abdefghijklmnopqrstuvwxyz
//00001000300005670809000000   lex_counts
//TABLE 2. Number of Symbols Lexicographically Less Than Current Symbol

// n is the first character, so place it in the last position of result
// the count at 0 (position of n) in TABLE 1 is 0
// the count for n in TABLE 2 is 5
// 0+5 = 5
// S[5] = o, so place o before n in result
//
// the count at 5 in TABLE 1 is 0
// the count for o in TABLE 2 is 6
// 0+6 = 6
// S[6] = i, so place i before o in result
//
// the count at 6 in TABLE 1 is 0
// the count for i in TABLE 2 is 3
// 0+3 = 3
// S[3] = t, so place t before i in result
// ...
//
//If we use a ^ character that sorts before any other character, we are
//guaranteed that the first character of our transform will be the last
//character of the input!

    block_size-=BWT_START_INDEX_WIDTH;

    mword *letter_counts  = mem_new_valz(this_pyr, block_size);
    mword *lex_counts     = mem_new_valz(this_pyr, BWT_COUNT_TABLE_SIZE);
    char *result = (char*)_newstr(this_pyr, block_size, '\0');

    int i,j,start_index=0;
    int c;
    mword temp;
//_say(start);

    for(i=0;i<BWT_START_INDEX_WIDTH;i++){
        start_index |= (((start[i] - '0') >> (4*i)) << 4*i);
    }

//_d(start_index);

    start += BWT_START_INDEX_WIDTH;

//    c = start[0];
//    lex_counts[c] = 1;

    for(i=0;i<block_size;i++){
        c = start[i];
        lex_counts[c]++;
        letter_counts[i] = lex_counts[c]-1;
    }
//_say(start);
//_mem(letter_counts);
//_die;

//_mem(lex_counts);
//_die;

    // letter_counts[i] --> number of times c=start[i] occurs between 0 and i
    // lex_counts[c]    --> number of times c occurs in string

    int running_tot=0;

    for(i=0;i<BWT_COUNT_TABLE_SIZE;i++){
        if(lex_counts[i]){
//_dd(running_tot);
            temp = lex_counts[i];
            lex_counts[i] = running_tot;
            running_tot += temp;
        }        
    }

//_mem(lex_counts);
//_die;

    for(i=0; i<block_size; i++){ // <-- XXX BROKEN XXX
        c = start[i];
        letter_counts[i] += lex_counts[c];
    }

//_say(start);
//_mem(letter_counts);
//_die;
//_say(start);

    int last_index = start_index; // Only works if we sort with a prefix-character

    for( j=block_size-1; j>=0; j--){
//_d(last_index);
        result[j] = start[last_index];
        last_index = letter_counts[last_index];
    }
//_say(result);
//_die;
    return (mword*)result;

}


//
//
int compress_bwt_compare_encode(const void *a, const void *b){ // compress_bwt_compare_encode#

    //   memcmp(a, b, len)
    //   a.n < b.n
    // a |...........*------.......................| cmp_1
    // b |..................................*------|
    //                                      |------| cmp_1_len = (bwt_block_size-b.n)
    //
    // a |...........*......-----------------------| cmp_2
    // b |-----------------------...........*......|
    //   |-----------------------| cmp_2_len = len_a-cmp_1_len
    //
    // a |-----------*.............................| cmp_3
    // b |.......................-----------*......|
    //                          |-----------| cmp_3_len = a.n
    //
    // v+a.n           , v+b.n          , cmp_1_len
    // v+a.n+cmp_1_len , v+0            , cmp_2_len
    // v+0             , v+cmp_2_len    , cmp_3_len
    //
    //   a.n > b.n
    // a |..................................*------| cmp_1
    // b |...........*------.......................|
    //               |------| cmp_1_len = (bwt_block_size-a.n)
    //
    // a |-----------------------...........*......| cmp_2
    // b |...........*......-----------------------|
    //                      |----------------------| cmp_2_len = len_b-cmp_1_len
    //
    // a |.......................-----------*......| cmp_3
    // b |-----------*.............................|
    //   |-----------| cmp_3_len = b.n
    //
    // v+a.n      , v+b.n          , cmp_1_len
    // v+0        , v+b.n+cmp_1_len, cmp_2_len
    // v+cmp_2_len, v+0            , cmp_3_len
    //
    // NOTE: a.n is NEVER equal to b.n

    compress_bwt_index *bwt_a = (compress_bwt_index*)a;
    compress_bwt_index *bwt_b = (compress_bwt_index*)b;

    int ind_a = bwt_a->n;
    int ind_b = bwt_b->n;

//char temp_buf[100];
//
//fprintf(stderr,"a: %s", bwt_block_ptr+ind_a);
//snprintf(temp_buf, ind_a+1, "%s", bwt_block_ptr);
//fprintf(stderr,"%s\n", temp_buf);
//
//fprintf(stderr,"b: %s", bwt_block_ptr+ind_b);
//snprintf(temp_buf, ind_b+1, "%s", bwt_block_ptr);
//fprintf(stderr,"%s\n", temp_buf);

    int len_a = bwt_block_size-ind_a;
    int len_b = bwt_block_size-ind_b;

    int cmp_1_len, cmp_2_len, cmp_3_len;
    char *cmp_a;
    char *cmp_b;

    int cmp_1,cmp_2, cmp_3;

    if(ind_a < ind_b){ // also len_b < len_a
//_trace;
        cmp_1_len = (bwt_block_size - ind_b);
        cmp_a     = (bwt_block_ptr  + ind_a);
        cmp_b     = (bwt_block_ptr  + ind_b);

//fprintf(stderr,"cmp_a %d\n", ind_a);
//fprintf(stderr,"cmp_b %d\n", ind_b);
//_d(cmp_1_len);

        cmp_1 = memcmp(cmp_a, cmp_b, cmp_1_len);
//_d(cmp_1);

        if(!cmp_1){ // If cmp_1 couldn't decide which was greater...

            cmp_2_len =          len_a - cmp_1_len;
            cmp_a     = (bwt_block_ptr + ind_a + cmp_1_len);
            cmp_b     = (bwt_block_ptr + 0);

//fprintf(stderr,"cmp_a %d\n", (cmp_1_len));
//fprintf(stderr,"cmp_b %d\n", 0);
//_d(cmp_2_len);

            cmp_2 = memcmp(cmp_a, cmp_b, cmp_2_len);
//_d(cmp_2);
        
            if(!cmp_2){  // If cmp_2 couldn't decide which was greater...

                cmp_3_len = ind_a;
                cmp_a     = (bwt_block_ptr + 0);
                cmp_b     = (bwt_block_ptr + cmp_2_len);


                cmp_3 = memcmp(cmp_a, cmp_b, cmp_3_len);
//_d(cmp_3);

                return cmp_3;

            }

            return cmp_2;

        }

    }
    else{ // if(bwt_a->n > bwt_b->n) ... len_b > len_a
//_trace;
        cmp_1_len = (bwt_block_size - ind_a);
        cmp_a     = (bwt_block_ptr  + ind_a);
        cmp_b     = (bwt_block_ptr  + ind_b);

//fprintf(stderr,"cmp_a %d\n", ind_a);
//fprintf(stderr,"cmp_b %d\n", ind_b);
//_d(cmp_1_len);

        cmp_1 = memcmp(cmp_a, cmp_b, cmp_1_len);
//_d(cmp_1);

        if(!cmp_1){ // If cmp_1 couldn't decide which was greater...

            cmp_2_len = len_b-cmp_1_len;
            cmp_a     = (bwt_block_ptr + 0);
            cmp_b     = (bwt_block_ptr + ind_b + cmp_1_len);

//fprintf(stderr,"cmp_a %d\n", 0);
//fprintf(stderr,"cmp_b %d\n", cmp_1_len);
//_d(cmp_2_len);

            cmp_2 = memcmp(cmp_a, cmp_b, cmp_2_len);
//_d(cmp_2);

            if(!cmp_2){  // If cmp_2 couldn't decide which was greater...

                cmp_3_len = ind_b;
                cmp_a     = (bwt_block_ptr + cmp_2_len);
                cmp_b     = (bwt_block_ptr + 0);

                cmp_3 = memcmp(cmp_a, cmp_b, cmp_3_len);
//_d(cmp_3);

                return cmp_3;

            }

            return cmp_2;

        }

    }

    return cmp_1;

}


// XXX Performs MTF-encoding in-place (!!) XXX
//
void compress_mtf_encode(pyr_cache *this_pyr, val8 in){ // compress_mtf_encode#

    mword in_size = array8_size(this_pyr, in);
    char *cin = (char*)in;

    mword *byte_table = mem_new_valz(this_pyr, MTF_BYTE_TABLE_SIZE);

    memcpy(byte_table, mtf_byte_table, UNITS_MTO8(MTF_BYTE_TABLE_SIZE));

    int i;

    for(i=0; i<in_size; i++){
        cin[i] = compress_mtf_update_index_table(byte_table, cin[i]);
    }

//_mem(byte_table);

}


// XXX Performs MTF-decoding in-place (!!) XXX
//
void compress_mtf_decode(pyr_cache *this_pyr, val8 in){ // compress_mtf_decode#

    mword in_size = array8_size(this_pyr, in);
    char *cin = (char*)in;

    mword *byte_table = mem_new_valz(this_pyr, MTF_BYTE_TABLE_SIZE);

    memcpy(byte_table, mtf_byte_table, UNITS_MTO8(MTF_BYTE_TABLE_SIZE));

    int i;

    for(i=0; i<in_size; i++){
        cin[i] = compress_mtf_update_byte_table(byte_table, cin[i]);
    }

}


//
//
char compress_mtf_update_index_table(mword *byte_table, char c){ // compress_mtf_update_index_table#

    char result = (char)byte_table[(int)c];

    int i=0, n=0;


    for(i=0; i<MTF_BYTE_TABLE_SIZE; i++){
        if(n==result) break;
        (byte_table[i] < result) ? (byte_table[i]++,n++) : (byte_table[i]);
    }

    byte_table[(int)c] = 0;

    return result;

}


//
//
char compress_mtf_update_byte_table(mword *byte_table, char c){ // compress_mtf_update_byte_table#

    char result = byte_table[(int)c];
    int i;

    for(i=c; i>0; i--){
        byte_table[i] = byte_table[i-1];
    }

    byte_table[0] = result;

    return result;

}


// Range-coding can expand the input; detect this condition and just return 
// the original ptr
//
char *compress_range_encoder(pyr_cache *this_pyr, char *in){ // compress_range_encoder#

// Encoding:
// 0.5 probability assigned to first 25% of MTF table
// 0.5 probability assigned to other 75%
//
// for new byte,
// do 8 times:
//     range = (hi-lo)
//        if range too small, flush a byte, readjust hi/lo and continue[*]
//     if(byte>0x3f)
//          lo = (lo+range/4)
//     else
//          hi = (lo+range/4)
//     byte<<=1
//
// [*] Edge condition occurs when the range is straddling 0x10000000, 0x20000000, 0x40000000 or 0x80000000
//      Just flush and then keep going, but keep track of the straddling byte
//      in case we need to adjust it based on the next byte (think carry-out)
//

    return "";

}


// RLE-coding can expand the input; detect this condition and just return 
// the original ptr
//
char *compress_rle_encode(pyr_cache *this_pyr, val8 in){ // compress_rle_encode#

    mword in_size = array8_size(this_pyr, in);
    char *cin = (char*)in;

    char *result = (char*)_newstr(this_pyr, in_size, ' ');

    int i,j;
    int last=0;
    char c;
    int count;

    for( i=0,         j=0; 
         i<in_size && j<in_size; 
         i++,         j++){
        c = cin[i];
        result[j] = c;
        if(c==last){
            count=0;
            j++;
            while(  count<255 
                    && i<in_size
                    && j<in_size){
                if(cin[i]==last){
                    count++,i++;
                }
                else{
                    break;
                }
            }
            result[j] = count;
        }
        last=c;
    }

    if(i<in_size){ // RLE encoding didn't compress the input
        return (char*)in;
    }

// Need to truncate the result to be a proper array8

    return result;

}


//
//
const mword mtf_byte_table[] = // mtf_byte_table#
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};


// Pre-processing step:
//   Reverse the array

// |...........*.............................|
// |..................................*......|

// Apply BWT
// RLE
// MTF-encode
// Range-encode



// Clayton Bauman 2017

