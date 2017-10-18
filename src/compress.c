// compress.c
// credit: Mark Nelson, Dr. Dobbs Journal 1996

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


// compress_deflate() pseudo-code:
// break into blocks
// for each block:
    // check RLE size
    // if(>80% compression)
    //      set encode-type=0
    //      Apply RLE_encode
    //      Apply MTF_encode
    //      Range encode
    // else
    //      set encode-type=1
    //      Apply BWT_encode
    //      Apply MTF_encode
    //      Range encode
    // cons a pair: [ptr encode-type data]
    // unload
    // return result
//
// NOTE: compress_deflate() checks each level of compression and overall
//      compression to ensure that the result is never larger than the input;
//      if the result would be larger than the input, compress_deflate() just
//      returns the original input back to the caller.
//
// compress_inflate() pseudo-code:
// load
// read encode-type to see how the compressed-data is encoded
// if encode-type=0
//      Range decode
//      Apply MTF_decode
//      Apply RLE_decode
// else if encode-type=1
//      Range decode
//      Apply MTF_decode
//      Apply BWT_decode
// return result
//
// Calling compress_inflate() on uncompressed data has undefined behavior;
// check whether compress_deflate() has actually compressed its input before 
// blindly passing it on to compress_inflate()

//
//
mword *compress_bwt_encode_block(pyr_cache *this_pyr, char *start, int block_size){ // compress_bwt_encode_block#

//    if(block_size > BWT_MAX_BLOCK_SIZE) return (mword*)start;
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

//        start_index |= ((start[0] - '0') << ());


    for(i=0;i<BWT_START_INDEX_WIDTH;i++){
        start_index |= ((start[i] - '0') << (4*i));
//        start_index |= (((start[i] - '0') >> (4*i)) << 4*i);
    }

_d(start_index);

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

    for(i=0; i<block_size; i++){
        c = start[i];
        letter_counts[i] += lex_counts[c];
    }

//_say(start);
//_mem(letter_counts);
//_die;
//_say(start);

    int last_index = start_index; // Only works if we sort with a prefix-character

    for(j=block_size-1; j>=0; j--){
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

//char temp_buf[1000];
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
_trace;
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
_trace;

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

    for(i=c; i>0; i--){ // XXX PERF XXX //
        byte_table[i] = byte_table[i-1];
    }

    byte_table[0] = result;

    return result;

}


// Range-coding can expand the input; detect this condition and just return 
// the original ptr
//
val8 compress_range_encoder(pyr_cache *this_pyr, val8 in){ // compress_range_encoder#

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

    mword in_size = array8_size(this_pyr, in);
    char *cin = (char*)in;

    char *result = (char*)_newstr(this_pyr, in_size*2, ' ');

    int i,j=0;

    unsigned char byte;
    unsigned byte_out;

    mword range_hi = FMAX;
    mword range_lo = 0;
    double range = range_hi-range_lo;

_d(range_hi);
_d(range_lo);

//    double temp1, temp2;
//    mword temp3,temp4;

    double adj_long;
    mword adj_mword;

    mword save_range_lo;

int count=0;

    for(i=0;i<in_size;i++){
        byte=cin[i];
//        for(j=0;j<8;j++){
//_d(range);

//_d(byte);
//temp1 = (range*range_table[byte-1]);
////fprintf(stderr,"%lf\n", floor(temp1));
//
//temp2 = (range*range_table[byte]);
////fprintf(stderr,"%lf\n", floor(temp2));
//
//temp3=(mword)temp1;
//temp4=(mword)temp2;
//fprintf(stderr,"%x\n", temp3);
//fprintf(stderr,"%u\n", temp3);
//fprintf(stderr,"%x\n", temp4);
//fprintf(stderr,"%u\n", temp4);

        if(range_hi < range_lo){_fatal("Argh!!!");}

            save_range_lo = range_lo;

            if(byte){
                adj_long  = (range*range_table[byte-1]);
                adj_mword = (mword)adj_long;
                range_lo = range_lo+adj_mword;
            }

            adj_long  = (range*range_table[byte]);
            adj_mword = (mword)adj_long;
            range_hi = save_range_lo+adj_mword;
//            range_hi = range_lo+((double)range*range_table[byte]);

//_d(range_hi);
//_d(range_lo);

            // XXX check range underflow XXX //
            while((range_hi & 0xff000000) == (range_lo & 0xff000000)){
                count++;
                byte_out = ((range_hi & 0xff000000) >> 24);
//if(count<10){
//_d(byte_out);
//}
                result[j] = byte_out;
                j++;
                range_hi<<=8;
                range_lo<<=8;
                if(!range_hi) range_hi = FMAX;
            }
            range=range_hi-range_lo;
//        }
    }

_d(range_hi);
_d(range_lo);

_d(in_size);
_d(count);

double savings = 1 - ((double)count / (double)in_size);
fprintf(stderr,"%lf\n",savings);

    return (val8)array_shrink(this_pyr, (mword*)result, 0, count-1, BYTE_ASIZE);

}


// RLE-coding can expand the input; detect this condition and just return 
// the original ptr
//
val8 compress_rle_encode(pyr_cache *this_pyr, val8 in){ // compress_rle_encode#

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
        count=0;

        if(c==last){
            j++;
            while(cin[i+1]==last){
                if(count<BYTE_MAX
                   && i<in_size
                   && j<in_size){
                        count++,i++;
                }
                else{
                    break;
                }
            }
            result[j] = count;
        }

        last=cin[i];
    }

    if(i<in_size || j>=i){ // RLE encoding didn't compress the input
        return in;
    }

//_dd(in_size);
//_dd(j);

    return (val8)array_shrink(this_pyr, (mword*)result, 0, j-1, BYTE_ASIZE);

}


//
//
val8 compress_rle_decode(pyr_cache *this_pyr, val8 in){ // compress_rle_decode#

    mword in_size = array8_size(this_pyr, in);
    char *cin = (char*)in;

    int i,j,k;
    int count=1;

    for(i=1; i<in_size; i++){
        if(cin[i]==cin[i-1]){
            i++;
            count += cin[i];
        }
        count++;
    }

    char *result = (char*)_newstr(this_pyr, count, 0xff);

    result[0] = cin[0];

    for(i=1,j=1; i<in_size; i++,j++){  // XXX PERF XXX //
        result[j]=cin[i];
        if(cin[i]==cin[i-1]){
            result[j+1]=cin[i];
            i++;
            j += cin[i];
            if(cin[i]){
                for(k=i;k<=j;k++){
                    result[k]=cin[i-1];
                }
            }
        }
    }

    return (mword*)result;

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


//
//
const double range_table[] =
{
    0.500000, 0.625000, 0.666667, 0.708333, 0.723958, 0.739583, 0.755208, 0.770833, 
    0.777083, 0.783333, 0.789583, 0.795833, 0.802083, 0.808333, 0.814583, 0.820833,
    0.823437, 0.826042, 0.828646, 0.831250, 0.833854, 0.836458, 0.839062, 0.841667,
    0.844271, 0.846875, 0.849479, 0.852083, 0.854687, 0.857292, 0.859896, 0.862500,
    0.863616, 0.864732, 0.865848, 0.866964, 0.868080, 0.869196, 0.870312, 0.871429,
    0.872545, 0.873661, 0.874777, 0.875893, 0.877009, 0.878125, 0.879241, 0.880357,
    0.881473, 0.882589, 0.883705, 0.884821, 0.885937, 0.887054, 0.888170, 0.889286,
    0.890402, 0.891518, 0.892634, 0.893750, 0.894866, 0.895982, 0.897098, 0.898214,
    0.898703, 0.899191, 0.899679, 0.900167, 0.900656, 0.901144, 0.901632, 0.902121,
    0.902609, 0.903097, 0.903585, 0.904074, 0.904562, 0.905050, 0.905539, 0.906027,
    0.906515, 0.907003, 0.907492, 0.907980, 0.908468, 0.908956, 0.909445, 0.909933,
    0.910421, 0.910910, 0.911398, 0.911886, 0.912374, 0.912863, 0.913351, 0.913839,
    0.914328, 0.914816, 0.915304, 0.915792, 0.916281, 0.916769, 0.917257, 0.917746,
    0.918234, 0.918722, 0.919210, 0.919699, 0.920187, 0.920675, 0.921164, 0.921652,
    0.922140, 0.922628, 0.923117, 0.923605, 0.924093, 0.924581, 0.925070, 0.925558,
    0.926046, 0.926535, 0.927023, 0.927511, 0.927999, 0.928488, 0.928976, 0.929464,
    0.929681, 0.929898, 0.930115, 0.930332, 0.930549, 0.930766, 0.930983, 0.931200,
    0.931417, 0.931634, 0.931851, 0.932068, 0.932285, 0.932502, 0.932719, 0.932937,
    0.933154, 0.933371, 0.933588, 0.933805, 0.934022, 0.934239, 0.934456, 0.934673,
    0.934890, 0.935107, 0.935324, 0.935541, 0.935758, 0.935975, 0.936192, 0.936409,
    0.936626, 0.936843, 0.937060, 0.937277, 0.937494, 0.937711, 0.937928, 0.938145,
    0.938362, 0.938579, 0.938796, 0.939013, 0.939230, 0.939447, 0.939664, 0.939881,
    0.940098, 0.940315, 0.940532, 0.940749, 0.940966, 0.941183, 0.941400, 0.941617,
    0.941834, 0.942051, 0.942268, 0.942485, 0.942702, 0.942919, 0.943136, 0.943353,
    0.943570, 0.943787, 0.944004, 0.944221, 0.944438, 0.944655, 0.944872, 0.945089,
    0.945306, 0.945523, 0.945740, 0.945957, 0.946174, 0.946391, 0.946608, 0.946825,
    0.947042, 0.947259, 0.947476, 0.947693, 0.947910, 0.948127, 0.948344, 0.948562,
    0.948779, 0.948996, 0.949213, 0.949430, 0.949647, 0.949864, 0.950081, 0.950298,
    0.950515, 0.950732, 0.950949, 0.951166, 0.951383, 0.951600, 0.951817, 0.952034,
    0.952251, 0.952468, 0.952685, 0.952902, 0.953119, 0.953336, 0.953553, 0.953770,
    0.953987, 0.954204, 0.954421, 0.954638, 0.954855, 0.955072, 0.955289, 0.955506,
    0.955723, 0.955940, 0.956157, 0.956374, 0.956591, 0.956808, 0.957025, 0.957242,
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

