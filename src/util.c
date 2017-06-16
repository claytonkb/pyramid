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

        switch(cmd_code){
            case 0:
                util_bare_metal_menu();
                break;
            case 1:
//                util_strtok_dec(cmd_code_str, cmd_code);
//                util_strtok_dec(cmd_code_str, cmd_code2);
//
//                       ACC = _mkval(this_pyr, 4, 0xf0f0f0f0, 0x0f0f0f0f, 0xf0f0f0f0, 0x0f0f0f0f);
//                mword *src = _mkval(this_pyr, 4, 0x77777777, 0x99999999, 0xbbbbbbbb, 0xdddddddd);
//
//                array1_move(this_pyr, 
//                        ACC,
//                        cmd_code, 
//                        src,
//                        cmd_code2);

//                temp = _mkls(this_pyr, 3, _val(this_pyr, 0xdeadbabe), _val(this_pyr, 0xbeefface), _val(this_pyr, 0xabadcafe));
//                ACC = bstruct_cp(this_pyr, temp);

//                ACC = sexpr_from_string(this_pyr, C2B("[val 1 2 3]\n"));

//                ACC = _mk_aop(this_pyr, 2,
//                        _val(this_pyr, 0xdeadbabe),
//                        _val(this_pyr, 0xbeefface),
//                        _val(this_pyr, 0xcafefade),
//                        _val(this_pyr, 0xabaddeed));

//                ACC = global_irt->tags->PYR_TAG_TRIE_ENTRY;
//                ACC = global_irt->tags->PYR_TAG_TRIE;

//                ACC = global_irt->fs;
//                trie_insert(this_pyr, global_irt->fs, HASH8(this_pyr, "foo"), nil, _val(this_pyr, 0x01234567));
//                trie_insert(this_pyr, global_irt->fs, HASH8(this_pyr, "bar"), nil, _val(this_pyr, 0x89abcdef));
//                trie_insert(this_pyr, global_irt->fs, HASH8(this_pyr, "baz"), nil, _val(this_pyr, 0x00112233));
//                trie_insert(this_pyr, global_irt->fs, HASH8(this_pyr, "aaa"), nil, _val(this_pyr, 0x44556677));
//                trie_insert(this_pyr, global_irt->fs, HASH8(this_pyr, "aab"), nil, _val(this_pyr, 0x8899aabb));
//                trie_insert(this_pyr, global_irt->fs, HASH8(this_pyr, "aac"), nil, _val(this_pyr, 0xccddeeff));
//                trie_insert(this_pyr, global_irt->fs, HASH8(this_pyr, "aad"), nil, _val(this_pyr, 0x01012323));
//                trie_insert(this_pyr, global_irt->fs, HASH8(this_pyr, "aae"), nil, _val(this_pyr, 0x45456767));

//                map_insert(global_irt->fs, "foo", _val(this_pyr, 0x01234567));
//                map_insert(global_irt->fs, "bar", _val(this_pyr, 0x89abcdef));
//                map_insert(global_irt->fs, "baz", _val(this_pyr, 0x00112233));
//                map_insert(global_irt->fs, "aaa", _val(this_pyr, 0x44556677));
//                map_insert(global_irt->fs, "aab", _val(this_pyr, 0x8899aabb));
//                map_insert(global_irt->fs, "aac", _val(this_pyr, 0xccddeeff));
//                map_insert(global_irt->fs, "aad", _val(this_pyr, 0x01012323));
//                map_insert(global_irt->fs, "aae", _val(this_pyr, 0x45456767));

//                trie_remove(this_pyr, global_irt->fs, HASH8(this_pyr, "aac"), nil);

//                trie_remove(this_pyr, global_irt->fs, HASH8(this_pyr, "aad"), nil);

//                trie_remove(this_pyr, global_irt->fs, HASH8(this_pyr, "foo"), nil);
//                trie_remove(this_pyr, global_irt->fs, nil,                    string_c2b(this_pyr, "foo", 100));
//                ACC = _val(this_pyr, 
//                        trie_exists(this_pyr, global_irt->fs, HASH8(this_pyr, "bap"), nil));

//#define trie_exists(pyr, trie, key, secondary_key) (!is_nil(trie_lookup_hash(pyr, trie, key, secondary_key))) // trie_exists#

//   result0 = _load(this_bvm, oi0.data, size(oi0.data)); 

//                temp = io_slurp8(this_pyr, "aop.bbl");
//                temp = bstruct_load(this_pyr, temp, size(temp));
//
//                array_sort(this_pyr, temp, LEX_BYTE);

//                ACC = temp;
//
//                ACC = _val(this_pyr,
//                        array_search(this_pyr, temp, C2B("iguana"), LEX_BYTE));

//mword array_search(pyr_cache *this_pyr, mword *array, mword *target, sort_type st){ // array_search#

//                array_build_max_heap(ACC);

//                temp  = _mkval(this_pyr, 4, 3, 1, 0, 0);
//                temp2 = _mkval(this_pyr, 7, 2, 1, 0, 0, 0, 0, 0);
////                temp2 = _val(this_pyr, 3);
//_mem(temp);
//_say("-------");
//_mem(temp2);
//_say("-------");
//                ACC = _val(this_pyr, array_cmp_num(temp, temp2));

//                temp = io_slurp8(this_pyr, "presidents.bbl");
//                temp = bstruct_load(this_pyr, temp, size(temp));
//
////                ACC = bstruct_cp(this_pyr, temp);
//                ACC = list_to_ptr_array(this_pyr, trie_entries(this_pyr, temp));
//                array_sort(this_pyr, ACC, UNSIGNED);

//                ACC = global_irt->symbols->SEXPR_IGNORE_SYMBOL;

//                ACC = _val(this_pyr,
//                        array_ncmp(this_pyr, C2B("foobaz"), 3, C2B("bar"), 2, BYTE_ASIZE));

//                tempv = _mu(this_pyr, global_irt->env);
//                _prn("_mu: ");
//                _dd(tempv);
//
//                tempv = _nar(this_pyr, global_irt->env);
//                _prn("_nar: ");
//                _dd(tempv);
//
//                tempv = _nin(this_pyr, global_irt->env);
//                _prn("_nin: ");
//                _dd(tempv);
//
//                tempv = _nlf(this_pyr, global_irt->env);
//                _prn("_nlf: ");
//                _dd(tempv);
//
//                tempv = _ntag(this_pyr, global_irt->env);
//                _prn("_ntag: ");
//                _dd(tempv);
//
//                tempv = _nva(this_pyr, global_irt->env);
//                _prn("_nva: ");
//                _dd(tempv);
//
//                tempv = _nptr(this_pyr, global_irt->env);
//                _prn("_nptr: ");
//                _dd(tempv);

//                ACC = bstruct_to_array(this_pyr, global_irt->env);

//                ACC = global_irt->tags->PYR_TAG_CPTR;
//                temp = mem_new_tptr(this_pyr, global_irt->tags->PYR_TAG_CPTR, nil);
//                ACC = _val(this_pyr, is_cptr(temp));

//                ACC = _mk_aop(this_pyr, 2,
//                        mem_new_tptr(this_pyr, global_irt->tags->PYR_TAG_CPTR, _val(this_pyr, 0xdeadbeef)),
//                        _val(this_pyr, 0xdeadbabe),
//                        mem_new_tptr(this_pyr, global_irt->tags->PYR_TAG_OPERAND, nil),
//                        _val(this_pyr, 0xbeefface),
//                        _val(this_pyr, 0xcafefade),
//                        _val(this_pyr, 0xabaddeed));

//                ACC = global_irt->tags_strings;

//                ACC = _val(this_pyr,
//                        array_search(this_pyr, global_irt->tags_strings, global_irt->tags->PYR_TAG_CPTR, LEX_MWORD));

//                tempv = array_search(this_pyr, global_irt->tags_strings, global_irt->tags->PYR_TAG_CPTR, LEX_MWORD);
//                ACC = pcar( rdp(global_irt->tags_strings, tempv) );
//
//                _mem(ACC);
//                _say("------------------------------------");
//                _mem(global_irt->tags->PYR_TAG_CPTR);

//                ACC = xbar_tag_to_string(this_pyr, global_irt->tags->PYR_TAG_CPTR);

//                ACC = global_irt->env;

//                ACC = this_pyr->self;

//                num_tags = size(global_irt->tags);
//
//                for(i=0; i<num_tags; i++){
////                    tempv = array_search_rewrite(this_pyr, global_irt->tags_strings, rdp(global_irt->tags,i), LEX_MWORD);
//                    tempv = array_search(this_pyr, global_irt->tags_strings, rdp(global_irt->tags,i), LEX_MWORD);
////                    _mem(rdp(global_irt->tags,i));
////                    _say("---------------------");
//                    if(tempv!=-1){
////                        _mem(pcar(rdp(global_irt->tags_strings,tempv)));
//                        found_count++;
//                    }
////                    _say("/////////////////////");
//                }
//
//                _dd(num_tags);
//                _dd(found_count);

//                fprintf(stderr, "%d\n", sizeof(unsigned));
//                fprintf(stderr, "%d\n", sizeof(double));
//
//                double foo = 12345.6789;
//                fprintf(stderr, "%lf\n", foo);

//                ACC = global_irt->xbar;

//                ACC = io_slurp8(this_pyr, "code_block.bbl");
//                ACC = bstruct_load(this_pyr, ACC, size(ACC));
//                _say("code_block.bbl loaded");
//
//                pvc_core_interp(this_pyr, tcar(ACC));
//                _say("done");

//                ACC = io_slurp8(this_pyr, "tree.bbl");
//                ACC = bstruct_load(this_pyr, ACC, size(ACC));
//                _say("tree.bbl loaded");

//                ACC = arrays_pa(ACC);

//                tempv = rdv_pa(ACC,9);
//                ACC = _val(this_pyr, tempv);

//                ACC = std_new_paged_array(this_pyr, 4, -1*UNITS_MTO8(9));
//
//                for(tempv=0; tempv<9; tempv++){
//                    ldp_pa(ACC,tempv) = _val(this_pyr, (tempv * tempv));
//                }
//
//                std_resize_paged_array(this_pyr, ACC, -1*UNITS_MTO8(13));

//_dd(pgsize_pa(ACC));
//_dd(sfield_pa(ACC));

//tempv = rrdp(ACC,MWORD_SIZE,0);
//_d(rrdp(ACC,tempv,0));

//                ACC = io_slurp(this_pyr, "tag_val.bbl");
////                ACC = bstruct_load(this_pyr, ACC, size(ACC));
//                _prn("tag_val.bbl loaded\n");
//
//_d(ACC);
//_mem(ACC);
//                temp = rtcar(ACC,MWORD_SIZE);
//                ACC = _val(this_pyr, *(rel_to_abs(ACC,temp)));
//
////                ACC = bstruct_cp(this_pyr, ACC);
////                ACC = global_dev_ptr;

// NEXT: Test std_*_pbp using tree.bbl; integrate with relative-offset
// operators
//
//                temp = pcdr(ACC);
//                ACC = pcar(ACC);
//
//                ACC = std_read_with_pbp(this_pyr, ACC, temp);

//                ACC = io_slurp(this_pyr, "pvc_code.bbl");
//                ACC = bstruct_load(this_pyr, ACC, size(ACC));
//                _say("pvc_code.bbl loaded");
//
//                _say("done");

                ACC = xbar_search(this_pyr, global_irt->tags->PYR_TAG_ADD_OP);

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
            "16    .....    show pyramid constants and geometries");

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

