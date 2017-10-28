// util.c
//

#include "pyramid.h"
#include "util.h"
#include "mem.h"
#include "array.h"
#include "introspect.h"
#include "pearson.h"
#include "bstruct.h"
#include "sexpr.h"
#include "string.h"
#include "trie.h"
#include "tptr.h"
#include "io.h"
#include "bstruct.h"
#include "list.h"
#include "interp.h"
#include "xbar.h"
#include "pvc.h"
#include "std.h"
#include "eval.h"
#include "sort.h"
#include "mt19937ar.h"
#include "aexpr.h"
#include "compress.h"
#include "math.h"

/*****************************************************************************
 *                                                                           *
 *                              MISC UTILS                                   *
 *                                                                           *
 ****************************************************************************/



/*****************************************************************************
 *                                                                           *
 *                               DEV UTILS                                   *
 *                                                                           *
 ****************************************************************************/
#ifdef DEV_MODE

void util_rsvd(void){
    _msg("reserved");
}

#define util_strtok_rest(x)                     \
    x = strtok(NULL, " ");                      \
    if(x == NULL){                              \
        _say("Not enough arguments");           \
        continue;                               \
    }

#define util_strtok_dec(x,y)                    \
    util_strtok_rest(x)                         \
    y = atoi(x);

#define util_strtok_hex(x,y)                    \
    util_strtok_rest(x)                         \
    y = (mword*)strtoul((char*)x,NULL,16);

//                util_strtok_dec(cmd_code_str, cmd_code);
//                util_strtok_dec(cmd_code_str, cmd_code2);

//
//
void util_bare_metal_prompt(pyr_cache *this_pyr, mword *init_ptr){ // util_bare_metal_prompt#

    char *cmd_code_str;
    int cmd_code =0;
//    int cmd_code2=0;

    char buffer[256];
    int i;

//char c;
    mword *ACC = init_ptr;
    mword  tempv=0;
    mword  tempw=0;
    char  *tempc = NULL;
    mword *temp=nil;
    mword *temp2=nil;
    tempv+=0; // silence compiler warning
    tempw+=0; // silence compiler warning

    _notify("entering bare metal prompt");

//uint64_t u;
//mword v = 0xffffffff;
//mword w = 0xffffffff;
//u = (uint64_t)v*w;
//mword *x = (mword*)&u;
//_d(x[0]);
//_d(x[1]);
//_die;

//    mword *temp = mem_new_val(this_pyr,2,0);
//    ldp(temp,0) = this_pyr->self;
//    ldv(temp,1) = MWORD_SIZE;

//    mword num_tags;
//    mword found_count=0;

//mword xbar_avg = 0x27E6714;


//int clz_table[256] = {
//8, 
//7, 
//6, 6, 
//5, 5, 5, 5, 
//4, 4, 4, 4, 4, 4, 4, 4,
//3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
//2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
//2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
//1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
//
//double x;
//double y;
//int f;
//double running=0;
//
//for(i=0;i<256;i++){
//    f = (8-clz_table[i]+1);
//    x = 1<<f;
//    y = 1/(f*x);
//    running += y;
//    fprintf(stderr, "%lf, ", running);
//}
//_die;
mp_int number;
int result;

    while(1){

        _prn("% ");

        util_get_line(buffer, stdin);

        cmd_code_str = strtok(buffer, " ");
        if(cmd_code_str == NULL) continue;
        cmd_code = atoi(cmd_code_str);

//mword *A;
//mword *B;

        switch(cmd_code){
            case 0:
                util_bare_metal_menu();
                break;
            case 1:

// NEXT: Test std_*_pbp using tree.bbl; integrate with relative-offset
// operators
//
//                temp = pcdr(ACC);
//                ACC = pcar(ACC);
//
//                ACC = std_read_with_pbp(this_pyr, ACC, temp);

//                A = _mkls(this_pyr, 3, 
//                            _val(this_pyr, 1), 
//                            _val(this_pyr, 2), 
//                            _val(this_pyr, 3));
//
//                B = _mkls(this_pyr, 3, 
//                            _val(this_pyr, 4),
//                            _val(this_pyr, 5), 
//                            _val(this_pyr, 6));
//
//                temp = _cons(this_pyr, A, B);
//
//                ACC = list_append_pyr_op(this_pyr, temp);

//                temp = _mkptr(this_pyr, 3, 
//                        _val(this_pyr,0x11111111),
//                        _val(this_pyr,0x22222222),
//                        _val(this_pyr,0x33333333));
//
//                ACC = _mkptr(this_pyr, 3, 
//                        _val(this_pyr,0x44444444),
//                        _val(this_pyr,0x55555555),
//                        _val(this_pyr,0x66666666));
//
//                ACC = (mword*)array_cat_pyr_op(this_pyr, (blob)_cons(this_pyr, temp,ACC));

//                ACC = _mkval(this_pyr, 20,
//                        64, 87, 27, 64, 89, 98, 28,  9,  0, 73, 
//                        89, 76, 74, 82, 14, 16, 77, 83, 71, 63);
//                temp = _bs2str(this_pyr, ACC);
//                _say((char*)temp);
//
////                sort(this_pyr, ACC, UNSIGNED);
//
//                sort_insertion(this_pyr, ACC, (mword*)(ACC+size(ACC)), SORT_ARRAY_TYPE_VAL, UNSIGNED, 0);
//                temp = _bs2str(this_pyr, ACC);
//                _say((char*)temp);
//
//                tempv = array_search(this_pyr, ACC, _val(this_pyr, 64), UNSIGNED);
//_dd(tempv);

//                ACC = _val(this_pyr, _mu(this_pyr, global_irt->xbar));

//                ACC = global_irt->srand;

//                ACC = std_genrand32(this_pyr, 13);

//                ACC = tptr_uniq(this_pyr, nil);

//                temp = pearson_hash8(this_pyr, C2B("/foo"));
//
//                ACC = pearson_hash8p(this_pyr, (char*)temp, C2B("/foo"));

//                ACC = io_slurp(this_pyr, "pvc_code.bbl");
//                ACC = bstruct_load(this_pyr, ACC, size(ACC));
//                _say("pvc_code.bbl loaded");
//
//                ACC = (blob)eval_apply(this_pyr, ACC);
//                _say("done");

//                ACC = C2B("");

//                ACC = global_irt->srand;

                // TODO:
                // 1. sexpr-to-aexpr format (with tags)
                // 2. command-line execution of opcodes (translate to hash
                //      look up in op-table, etc.) Allow accumulator to be used
                //      to implement nesting.

//                ACC = bstruct_cp(this_pyr, global_irt->xbar);
//                sort(this_pyr, ACC, UNSIGNED);
//
//                for(i=1;i<size(ACC);i++){
//                    tempv = ldv(ldp((mword*)ACC[i],0),3);
////                    tempw = abs(i-(tempv/xbar_avg));
//                    tempw = i-(tempv/xbar_avg);
//                    _dd(tempw);
//                }

//                tempv = ldv(ldp((mword*)ACC[1],           0),3);
//                tempw = ldv(ldp((mword*)ACC[size(ACC)-1], 0),3);
//
//                _d(tempv);
//                _d(tempw);
//                _d(tempw-tempv);
//                _dd(size(ACC));

//        ldv(dest,0) = MWORD_MUX(    
//                        BIT_MASK(rdv(src,0),(src_begin+size_arg-1),src_begin),
//                        rdv(dest,0), 
//                        BIT_RANGE((dest_begin+size_arg-1),dest_begin));

//                tempv = (BIT_MASK(0xdeadbeef,27,16));
//                tempw = (BIT_RANGE(19,8));
//
//                _d(tempv);
//                _d(tempw);

//                temp = _mkval(this_pyr, 3, 0xdeadbeef, 0xfacefeed, 0xabaddeed);
//                ACC  = _mkval(this_pyr, 3, 0xcccccccc, 0xaaaaaaaa, 0);
//                array1_move(this_pyr, ACC, 4, temp, 28, 40);

//                temp = _mkval(this_pyr, 5, 0x88888888, 0x99999999, 0xaaaaaaaa, 0xbbbbbbbb, 0xcccccccc);
//                ACC  = _mkval(this_pyr, 5, 0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555);
//                array1_move(this_pyr, ACC, 4, temp, 28, 64);

                // 00000000 00000000 00000000 00000000
                // ff000000 ffffffff 00ffffff 00000000

//                temp = _mkval(this_pyr, 8, 0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777, 0x88888888);
//                ACC  = _mkval(this_pyr, 8, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000);
//global_dev_ptr = ACC;
//                array1_move(this_pyr, ACC, 48, temp, 60, 12);

//                for(i=4; i<64; i+=4){
//
//                    tempv = (1 + (genrand_int32() % 32)) * 4;
//
//                    temp = _mkval(this_pyr, 8, 0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777, 0x88888888);
//                    ACC  = _mkval(this_pyr, 8, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000);
//                    array1_move(this_pyr, ACC, 64, temp, 4, tempv);
//
//                    _dd(i);
//                    _dd(2*i);
//                    _dd(tempv);
//
//                    temp = _bs2str(this_pyr, ACC);
//                    _say((char*)temp);
//                    _say("");
//
//                }

//                ACC = bstruct_cp(this_pyr, global_irt->xbar);
//                sort(this_pyr, ACC, UNSIGNED);

//                ACC = io_slurp(this_pyr, "pvc_code.bbl");
//                ACC = bstruct_load(this_pyr, ACC, size(ACC));
//                _say("pvc_code.bbl loaded");
//
//                ACC = (blob)eval_apply(this_pyr, ACC);
//                _say("done");

//                _say("^-^ hi...");

//                ACC = C2B("banana brains");
//                compress_mtf_encode(this_pyr, ACC);

//                ACC = _mkval(this_pyr, 5, 0x16e6262, 0x3230101, 0x56b0372, 0x73, 0xffffff00);
//                compress_mtf_decode(this_pyr, ACC);

//                ACC = C2B("In the beginning, God created the heavens and the earth.");
//                compress_mtf_encode(this_pyr, ACC);
//                compress_mtf_decode(this_pyr, ACC);
//
//_say((char*)ACC);




//                ACC = io_slurp8(this_pyr, "getty.txt");

//                ACC = C2B("invention contention");
//                ACC = C2B("contention invention nation rotation caution");
//                ACC = C2B("noituac noitator noitan noitnevni noitnetnoc");

//                ACC = C2B("repetition");
//                temp = compress_bwt_encode_block(this_pyr, (char*)ACC, array8_size(this_pyr, ACC));
//ACC = compress_bwt_decode_block(this_pyr, (char*)temp, array8_size(this_pyr, temp));
//_say((char*)ACC);
//_die;

                //compress_mtf_encode(this_pyr, temp);
//_say((char*)temp);

//ACC = (mword*)compress_rle_encode(this_pyr, temp);

//compress_mtf_encode(this_pyr, temp);
//compress_mtf_decode(this_pyr, ACC);
//temp = _bs2str(this_pyr, ACC);
//_say((char*)temp);
//_die;
//ACC = compress_range_encoder(this_pyr, temp);

//ACC = _bs2str(this_pyr, temp);

    //_say((char*)temp);

//temp = compress_rle_decode(this_pyr, ACC);
//ACC = temp;

//_d(temp);
//ACC = temp;
//compress_mtf_decode(this_pyr, temp);

//                ACC = _mkval(this_pyr, 5, 0x16e6262, 0x3230101, 0x56b0372, 0x73, 0x00);
//                ACC = array_shrink(this_pyr, ACC, 0, 14, BYTE_ASIZE);

//                ACC = io_slurp8(this_pyr, "getty.txt");


//i = memcmp( "\x070\x070", "\x080\x080", 2 );
//_d(i);
//_die;

//                ACC = _mkval(this_pyr, 65,
//                    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
//                    0x10111213, 0x14151617, 0x18191a1b, 0x1c1d1e1f,
//                    0x20212223, 0x24252627, 0x28292a2b, 0x2c2d2e2f,
//                    0x30313233, 0x34353637, 0x38393a3b, 0x3c3d3e3f,
//                    0x40414243, 0x44454647, 0x48494a4b, 0x4c4d4e4f,
//                    0x50515253, 0x54555657, 0x58595a5b, 0x5c5d5e5f,
//                    0x60616263, 0x64656667, 0x68696a6b, 0x6c6d6e6f,
//                    0x70717273, 0x74757677, 0x78797a7b, 0x7c7d7e80,
//                    0x80818283, 0x84858687, 0x88898a8b, 0x8c8d8e8f,
//                    0x90919293, 0x94959697, 0x98999a9b, 0x9c9d9e9f,
//                    0xa0a1a2a3, 0xa4a5a6a7, 0xa8a9aaab, 0xacadaeaf,
//                    0xb0b1b2b3, 0xb4b5b6b7, 0xb8b9babb, 0xbcbdbebf,
//                    0xc0c1c2c3, 0xc4c5c6c7, 0xc8c9cacb, 0xcccdcecf,
//                    0xd0d1d2d3, 0xd4d5d6d7, 0xd8d9dadb, 0xdcdddedf,
//                    0xe0e1e2e3, 0xe4e5e6e7, 0xe8e9eaeb, 0xecedeeef,
//                    0xf0f1f2f3, 0xf4f5f6f7, 0xf8f9fafb, 0xfcfdfeff, 0);
//
////                ldv(ACC,3)=0;
//
//temp = _bs2str(this_pyr, ACC);
//_say((char*)temp);
//assert(0);
//ACC = temp;
//ACC = compress_bwt_decode_block(this_pyr, (unsigned char*)temp, array8_size(this_pyr, temp));
//ACC = temp;
//_say((char*)ACC);
//                temp = compress_rle_encode(this_pyr, temp);

//ACC = temp;
//_say((char*)ACC);
//                ACC = compress_bwt_decode_block(this_pyr, (unsigned char*)temp, array8_size(this_pyr, temp));
//_say((char*)ACC);

//                // TODO: Get range-decoder working; add parameter for range-decoder distribution XXX //
//                ACC = io_slurp8(this_pyr, "doi.txt");
//                temp = compress_bwt_encode_block(this_pyr, (unsigned char*)ACC, array8_size(this_pyr, ACC));
//                compress_mtf_encode(this_pyr, temp);
//                ACC = compress_range_encoder(this_pyr, temp);

//                ACC = _mkval(this_pyr, 2, 0xf8f9fafb, 0xfcfdfeff);

//                ACC = math_add(this_pyr, _val(this_pyr, 0xffffffff), _val(this_pyr, 0x1));
//                ACC = math_add(this_pyr, 
//                        _mkval(this_pyr, 2, 0x55555555, 0xffffffff), 
//                        _mkval(this_pyr, 2, 0xaaaaaaaa, 0x1));
////                        _val(this_pyr, 0x1));
 
//                ACC = _mkval(this_pyr, 2, 0xf8f9fafb, 0xfcfdfeff);
//                ACC = _mkval(this_pyr, 2, 0x7060504, 0x3020100);
//                ACC = math_add(this_pyr, ACC, _val(this_pyr, 1));
//                ACC = math_neg(this_pyr, ACC);

//                ACC = math_sub(this_pyr, 
//                        _mkval(this_pyr, 2, 0xffffffff, 3),
//                        _mkval(this_pyr, 2, 0xffffffff, 12));


temp = std_genrand32(this_pyr, 16384);
//////ACC = _bs2str(this_pyr, temp);
//////_say((char*)ACC);
////
temp2 = std_genrand32(this_pyr, 16384);
//////ACC = _bs2str(this_pyr, temp2);
//////_say((char*)ACC);
//
tempv = std_time_ms();
//
////for(i=0;i<1000;i++){
    ACC = math_lattice_mul(this_pyr,
        temp,
        temp2);
////}
//
////                ACC = math_mul(this_pyr,
////                        mem_new_val(this_pyr, 4, 0xff),
////                        mem_new_val(this_pyr, 4, 0xff));
////
tempw = std_time_ms();
_dd(tempw-tempv);


if ((result = mp_init(&number)) != MP_OKAY) {
_say(mp_error_to_string(result));
_die;
}
else{
    _say("blast off!! ^-^");
}


                break;
            case 2:
                _notify("exiting bare metal prompt");
                return;
            case 3:
                _interp_reset;
                break;
            case 4:
                _d(ACC);
                break;
            case 5:
                _d(sfield(ACC));
                break;
            case 6:
                _mem(ACC);
                break;
            case 7:
                temp = _bs2str(this_pyr, ACC);
                _say((char*)temp);
                break;
            case 8:
                temp = _bs2gv(this_pyr, ACC);
                io_spit(this_pyr, "test.dot", temp, BYTE_ASIZE, OVERWRITE);
                _say("_bs2gv(ACC) ==> test.dot");
                break;
            case 9:
                tempc = cmd_code_str + strlen(cmd_code_str) + 1;
                _say((char*)tempc);
                ACC = sexpr_from_string(this_pyr, string_c2b(this_pyr, tempc, 300));
                break;
            case 10:
                tempc = cmd_code_str + strlen(cmd_code_str) + 1;
                _say((char*)tempc);
                temp = sexpr_from_string(this_pyr, string_c2b(this_pyr, tempc, 300));
                ACC = aexpr_from_sexpr(this_pyr, temp);
                break;
            case 11:
                ACC = this_pyr->self;
                _say("ACC <== this_pyr->self");
                break;
            case 12:
                ACC = nil;
                _say("ACC <== nil");
                break;
            case 13:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ continue; }
                ACC = (mword*)strtoul((char*)cmd_code_str,NULL,16);
                _say("ACC <== p");
                break;
            case 14:
                ACC = init_ptr;
                _say("ACC <== init_ptr");
                break;
            case 15:
                ACC = global_dev_overrides;
                _say("ACC <== global_dev_overrides");
                break;
             case 16:
                tempc = cmd_code_str + strlen(cmd_code_str) + 1;
                ACC = io_slurp(this_pyr, tempc);
                ACC = bstruct_load(this_pyr, ACC, size(ACC));
                _prn((char*)tempc);
                _prn(" loaded\n");
                break;
             case 17:
                util_show_geometries();
                break;
             case 18:
                temp = xbar_tag_to_string(this_pyr, ACC);
                _say((char*)temp);
                break;
             default:
                _say("unrecognized command code");
                util_bare_metal_menu();
                break;
        }

        for(i=0;i<256;i++){ buffer[i]=0; } // clear out the buffer

    }

}


// XXX Add buffer-size for safety XXX
//
void util_get_line(char *buffer, FILE *stream){ // util_get_line#

    int c, i=0;

    while(1){ //FIXME unsafe, wrong
        c = fgetc(stream);
        if(c == EOF || c == '\n'){
            break;
        }
        buffer[i] = c;
        i++;
    }

    buffer[i] = '\0';

}


//
//
void util_bare_metal_menu(void){

    _say(   "0     .....    list command codes\n"
            "1     .....    dev one-off\n"
            "2     .....    exit bare metal\n"
            "3     .....    hard reset\n"
            "4     .....    _d(ACC)\n"
            "5     .....    _d(sfield(ACC))\n"
            "6     .....    _mem(ACC)\n"
            "7     .....    _bs2str(ACC)\n"
            "8     .....    _bs2gv(ACC)\n"
            "9  S  .....    ACC <== sexpr(S)\n"
            "10 S  .....    ACC <== aexpr(S)\n"
            "11    .....    ACC <== this_pyr->self\n"
            "12    .....    ACC <== nil\n"
            "13 p  .....    ACC <== p\n"
            "14    .....    ACC <== init_ptr\n"
            "15    .....    ACC <== global_dev_overrides\n"
            "16 f  .....    ACC <== loaded .bbl file f\n"
            "17    .....    show pyramid constants and geometries\n"
            "18    .....    xbar_tag_to_string(ACC)");

}


//
//
void util_show_geometries(void){ // util_show_geometries#

    interp_boilerplate();

    fprintf(stderr, "MWORD_SIZE " dpr "\n", MWORD_SIZE);
    fprintf(stderr, "MWORD_BIT_SIZE " dpr "\n", MWORD_BIT_SIZE);
    fprintf(stderr, "HALF_MWORD_BIT_SIZE " dpr "\n", HALF_MWORD_BIT_SIZE);
    fprintf(stderr, "MWORD_MSB " dpr "\n", MWORD_MSB);
    fprintf(stderr, "MWORD_LSB " dpr "\n", MWORD_LSB);
    fprintf(stderr, "MSB_MASK " xpr "\n", MSB_MASK);
    fprintf(stderr, "NEG_ONE " dpr "\n", NEG_ONE);
    fprintf(stderr, "FMAX " xpr "\n", FMAX);
    fprintf(stderr, "MASK_1_BYTE " xpr "\n", MASK_1_BYTE);
    fprintf(stderr, "MASK_1_BIT " xpr "\n", MASK_1_BIT);
    fprintf(stderr, "HASH_BIT_SIZE " dpr "\n", HASH_BIT_SIZE);
    fprintf(stderr, "HASH_BYTE_SIZE " dpr "\n", HASH_BYTE_SIZE);
    fprintf(stderr, "HASH_SIZE " dpr "\n", HASH_SIZE);
    fprintf(stderr, "HASH_ALLOC_SIZE " dpr "\n", HASH_ALLOC_SIZE);
    fprintf(stderr, "TAG_SIZE " dpr "\n", TAG_SIZE);
    fprintf(stderr, "INTERP_TAG_SIZE " dpr "\n", INTERP_TAG_SIZE);
    fprintf(stderr, "TPTR_SIZE " dpr "\n", TPTR_SIZE);
    fprintf(stderr, "TPTR_ALLOC_SIZE " dpr "\n", TPTR_ALLOC_SIZE);
    fprintf(stderr, "TPTR_TAG_OFFSET " dpr "\n", TPTR_TAG_OFFSET);
    fprintf(stderr, "TPTR_PTR_OFFSET " dpr "\n", TPTR_PTR_OFFSET);
    fprintf(stderr, "TPTR_SFIELD " dpr "\n", TPTR_SFIELD);

    fprintf(stderr, "UNINIT_VAL " xpr "\n", (mword)UNINIT_VAL);
    fprintf(stderr, "UNINIT_PTR " xpr "\n", (mword)UNINIT_PTR);

    fprintf(stderr, "CTL_MASK " dpr "\n", CTL_MASK);

}

#endif

// Clayton Bauman 2017

