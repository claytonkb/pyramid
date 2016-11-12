// util.c
//

#include "pyramid.h"
#include "util.h"
#include "mem.h"
#include "array.h"
#include "introspect.h"
#include "pearson.h"

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

//
//
void util_bare_metal_prompt(pyr_cache *this_pyr, mword *init_ptr){ // util_bare_metal_prompt#

    char *cmd_code_str;
    int cmd_code;
//    int cmd_code2;

    char buffer[256];
    int i;

    mword *ACC = init_ptr;
    mword  tempv=0;
    mword *temp=nil;

//    mword *temp = mem_new_val(this_pyr,2,0);
//    ldp(temp,0) = this_pyr->self;
//    ldv(temp,1) = MWORD_SIZE;

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
                _say("dev one-off");

//                ACC = _val(this_pyr, 0xdeadbee6);
//                ACC = _th1(this_pyr, ACC, 3);
//                _mem(ACC);
//
//                ACC = _val(this_pyr, 0xdeadbee6);
//                ACC = _th1(this_pyr, ACC, 2);
//                _mem(ACC);
//
//                ACC = _val(this_pyr, 0xdeadbee6);
//                ACC = _th1(this_pyr, ACC, 1);
//                _mem(ACC);
//
//                ACC = _val(this_pyr, 0xdeadbee6);
//                ACC = _th1(this_pyr, ACC, 0);
//                _mem(ACC);

//                _d( is_nil(nil) );

//                mword start = 45;
//                mword end = 60;
//
//                mword bit_length = end-start;
//
//                mword src_start_mword = UNITS_1TOM(start);
//                mword src_end_mword   = UNITS_1TOM(end);
//
//                mword mword_length = src_start_mword-src_end_mword;
//
//                mword src_start_modulus  = MODULO_MTO1(start);
//                mword src_end_modulus    = MODULO_MTO1(end);
//
//                mword src_start_split    = MWORD_BIT_SIZE - src_start_modulus;
//                mword src_end_split      = MWORD_BIT_SIZE - src_end_modulus;
//
//                _dd(bit_length);
//                _dd(src_start_mword);
//                _dd(src_end_mword);
//                _dd(mword_length);
//                _dd(src_start_modulus);
//                _dd(src_end_modulus);
//                _dd(src_start_split);
//                _dd(src_end_split);

//                cmd_code_str = strtok(NULL, " ");
//                if(cmd_code_str == NULL){
//                    _say("Two arguments required");
//                    continue; 
//                }
//                cmd_code = atoi(cmd_code_str);

//                util_strtok_dec(cmd_code_str, cmd_code);
//                util_strtok_dec(cmd_code_str, cmd_code2);
//
//                ACC = _mkval(this_pyr, 3, 0xdeadbeef, 0xfacefeed, 0xabaddeed);
//                ACC = array1_slice(this_pyr, ACC, cmd_code, cmd_code2);

//                    ACC = _mkls(this_pyr, 3, _val(this_pyr, 0xdeadbabe), _val(this_pyr, 0xbeefface), _val(this_pyr, 0xabadcafe)); 

//                ACC = _mkval(this_pyr, 1, LO_BITS(0xdeadbeef, 11));

//                temp = _mkval(this_pyr, 3, 0x11111111, 0x22222222, 0x33333333);
//                ACC  = _mkval(this_pyr, 3, 0x44444444, 0x55555555, 0x66666666);
//
//                ldv(ACC,1) = rdv(temp,0);

//                temp = _mkls(this_pyr, 3, _val(this_pyr, 0xdeadbabe), _val(this_pyr, 0xbeefface), _val(this_pyr, 0xabadcafe)); 

//                ACC = array_th(this_pyr, temp, 1);

//                ACC = pearson16a(this_pyr, (char*)GLOBAL_TAG_ZERO_HASH, "foo", STRLEN("foo")); 

//                ACC = _mktptr(this_pyr, "foo", nil);

                temp = _mkval(this_pyr, 3, 0x11223344, 0x55667788, 0x99aabbcc);
                ACC = _val(this_pyr, array8_read(temp, tempv++));

//                ACC = _mkval(this_pyr, 3, 0x11223344, 0x55667788, 0x99aabbcc);
//                array8_write(ACC, tempv++, 0xdd);

                // implement/test array1 functions...

                break;
            case 2:
                _say("exiting bare metal prompt");
                return;
            case 3:
                _say("command code inactive");
                cmd_code_str = strtok(NULL, " ");
//                _d(cmd_code_str);
                if(cmd_code_str == NULL){ continue; }
                cmd_code = atoi(cmd_code_str);
                _d(cmd_code);
                break;
            case 4:
                _interp_reset(this_pyr);
                break;
            case 5:
                _say("command code inactive");
                break;
            case 6:
                _say("command code inactive");
                break;
            case 7:
                _say("command code inactive");
                break;
            case 8:
                _say("command code inactive");
                break;
            case 9:
                _d(ACC);
                break;
            case 10:
                _d(sfield(ACC));
                break;
            case 11:
                _mem(ACC);
                break;
            case 12:
                temp = _bs2str(this_pyr, ACC);
                _say((char*)temp);
                break;
            case 13:
                ACC = init_ptr;
                _say("ACC <== init_ptr");
                break;
            case 14:
                ACC = this_pyr->self;
                _say("ACC <== this_pyr->self");
                break;
            case 15:
                ACC = nil;
                _say("ACC <== nil");
                break;
            case 16:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ continue; }
                ACC = (mword*)strtoul((char*)cmd_code_str,NULL,16);
                _say("ACC <== p");
                break;
            case 17:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ continue; }
                tempv = strtoul((char*)cmd_code_str,NULL,10);
                ACC = rdp(ACC,tempv);
                _say("ACC <== rdp(ACC,n)");
                break;
            case 18:
                ACC = pcar(ACC);
                _say("ACC <== pcar(ACC)");
                break;
            case 19:
                ACC = pcdr(ACC);
                _say("ACC <== pcdr(ACC)");
                break;
            case 20:
                ACC = tcar(ACC);
                _say("ACC <== tcar(ACC)");
                break;
            case 21:
                temp = mem_new_val(this_pyr,2,0);
                ldp(temp,0) = ACC;
                ldv(temp,1) = MWORD_SIZE;
                ACC = temp;
                _say("ACC <== rel_offset_ptr(ACC)");
                break;
            case 22:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ continue; }
                tempv = strtoul((char*)cmd_code_str,NULL,10);
                _d(rov(ACC,tempv));
                break;
            case 23:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ continue; }
                tempv = strtoul((char*)cmd_code_str,NULL,10);
                trop(ACC,tempv);
                _say("trop(ACC,n)");
                break;
            case 24:
                trot(ACC);
                _say("trot(ACC)");
                break;
            case 25:
                _d(ros(ACC));
                break;
            case 26:
                tempv = ACC[0] + ACC[1];
                _mem((mword*)tempv);
                break;
            case 27:
                ldv(ACC,1) = MWORD_SIZE;
                _say("reset rel_offset_ptr(ACC)");
                break;
            case 28:
                cmd_code_str = strtok(NULL, " ");
                if(cmd_code_str == NULL){ continue; }
                tempv = strtoul((char*)cmd_code_str,NULL,16);
                _d(rov(ACC,UNITS_8TOM(tempv)));
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

    _say("0     -> list command codes");
    _say("1     -> dev one-off");
    _say("2     -> exit bare metal");
    _say("3 n   -> _dev(n)");
    _say("4     -> hard reset");
    _say("5 f v -> set flag f to value v");
    _say("6     -> hard run GC");
    _say("7     -> _mem( prev-allocation ) (iterates)");
    _say("8     -> reset command code 7");
    _say("9     -> _d(ACC)");
    _say("10    -> _d(sfield(ACC))");
    _say("11    -> _mem(ACC)");
    _say("12    -> _bs2str(ACC)");
    _say("13    -> ACC <== init_ptr");
    _say("14    -> ACC <== this_pyr->self");
    _say("15    -> ACC <== nil");
    _say("16 p  -> ACC <== p");
    _say("17 n  -> ACC <== rdp(ACC,n)");
    _say("18    -> ACC <== pcar(ACC)");
    _say("19    -> ACC <== pcdr(ACC)");
    _say("20    -> ACC <== tcar(ACC)");
    _say("21    -> ACC <== rel_offset_ptr(ACC)");
    _say("22 n  -> _d(rov(ACC,n))");
    _say("23 n  -> trop(ACC,n)");
    _say("24    -> trot(ACC)");
    _say("25    -> _d(ros(ACC))");
    _say("26    -> _mem(ro(ACC))");
    _say("27    -> reset rel_offset_ptr(ACC)");
    _say("28 h  -> _d(rov(ACC,hex_offset))");

}

#endif

// Clayton Bauman 2016

