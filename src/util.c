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

    mword *ACC = init_ptr;
    mword  tempv=0;
    char  *tempc = NULL;
    mword *temp=nil;
//    mword *temp2=nil;
    tempv+=0; // silence compiler warning

    _notify("entering bare metal prompt");

//    mword *temp = mem_new_val(this_pyr,2,0);
//    ldp(temp,0) = this_pyr->self;
//    ldv(temp,1) = MWORD_SIZE;

//    mword num_tags;
//    mword found_count=0;

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

//                ACC = io_slurp(this_pyr, "pvc_code.bbl");
//                ACC = bstruct_load(this_pyr, ACC, size(ACC));
//                _say("pvc_code.bbl loaded");
//
//                ACC = (blob)eval_apply(this_pyr, ACC);
//                _say("done");

                ACC = _mkval(this_pyr, 20,
                        64, 87, 27, 64, 89, 98, 28,  9,  0, 73, 
                        89, 76, 74, 82, 14, 16, 77, 83, 71, 63);
                temp = _bs2str(this_pyr, ACC);
                _say((char*)temp);

                array_sort(this_pyr, ACC, UNSIGNED);

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
                ACC = this_pyr->self;
                _say("ACC <== this_pyr->self");
                break;
            case 11:
                ACC = nil;
                _say("ACC <== nil");
                break;
            case 12:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ continue; }
                ACC = (mword*)strtoul((char*)cmd_code_str,NULL,16);
                _say("ACC <== p");
                break;
            case 13:
                ACC = init_ptr;
                _say("ACC <== init_ptr");
                break;
            case 14:
                ACC = global_dev_overrides;
                _say("ACC <== global_dev_overrides");
                break;
             case 15:
                tempc = cmd_code_str + strlen(cmd_code_str) + 1;
                ACC = io_slurp(this_pyr, tempc);
                ACC = bstruct_load(this_pyr, ACC, size(ACC));
                _prn((char*)tempc);
                _prn(" loaded\n");
                break;
             case 16:
                util_show_geometries();
                break;
             case 17:
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


//
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
            "9 S   .....    ACC <== sexpr(S)\n"
            "10    .....    ACC <== this_pyr->self\n"
            "11    .....    ACC <== nil\n"
            "12 p  .....    ACC <== p\n"
            "13    .....    ACC <== init_ptr\n"
            "14    .....    ACC <== global_dev_overrides\n"
            "15 f  .....    ACC <== loaded .bbl file f\n"
            "16    .....    show pyramid constants and geometries\n"
            "17    .....    xbar_tag_to_string(ACC)");

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

