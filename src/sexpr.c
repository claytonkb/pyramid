// sexpr.c
//

#include "pyramid.h"
#include "sexpr.h"
#include "string.h"
#include "array.h"
#include "list.h"
#include "bstruct.h"
#include "mem.h"
#include "tptr.h"

#define append_sexpr(x)                                     \
    if(is_nil(curr_sexpr)){                                 \
        curr_sexpr = _cons(this_pyr, x, nil);               \
    }                                                       \
    else{                                                   \
        list_unshift(this_pyr,                              \
            curr_sexpr,                                     \
            x);                                             \
    }


#define capture_token                                       \
    capture_length = j-i;                                   \
    if(capture_length){                                     \
        captured_token = array_slice(this_pyr, string, i, j); \
        append_sexpr(captured_token);                       \
    }                                                       \
    else{                                                   \
        parse_error;                                        \
    }                                                       \


#define capture_dquote                                      \
    capture_length = j-i;                                   \
    if(capture_length){                                     \
        captured_token = _unescape( this_pyr, array_slice(this_pyr, string, i, j), '\\');    \
        append_sexpr(captured_token);                       \
    }                                                       \
    else{                                                   \
        parse_error;                                        \
    }                                                       \


#define capture_squote                                      \
    capture_length = j-i;                                   \
    if(capture_length){                                     \
        captured_token = _unescape( this_pyr, array_slice(this_pyr, string, i, j), '/'); \
        append_sexpr(captured_token);                       \
    }                                                       \
    else{                                                   \
        parse_error;                                        \
    }                                                       \


#define  adv(x) j++;  goto x;     //advance
#define  ret(x) j--;  goto x;     //retard
#define  esc(x) j+=2; goto x;     //escape
#define advi(x) i=j; j++; goto x; //advance, set i
#define parse_error return nil;

#define chkdone    if(!(j<length)){goto done;}


//
//
mword *_pre_sexpr(pyr_cache *this_pyr, mword *string, mword *index){ // _pre_sexpr#

    mword j;
    j=*index;

    mword length = size(string);

    if(!length){ goto done; }

//    mword SEXPR_LIST_SYMBOL[] = { 0x10, 0x6c, 0x69, 0x73, 0x74 };
//    mword SEXPR_CODE_SYMBOL[] = { 0x10, 0x63, 0x6f, 0x64, 0x65 };

//// file.c
//char* myArray[] = { "str1", "str2", ... "str100" };
//const size_t sizeof_myArray = sizeof myArray;

null_context:
//_trace;
    chkdone;
    switch(string[j]){
        case ' ' :
        case 0x0a:
        case 0x0d: adv(null_context);
        case '{' : 
                *index = j+1;
// XXX XXX Was copying entire _sexpr... probably unnecessary XXX XXX
//                return bstruct_cp( this_pyr, _sexpr(this_pyr, string, index, SEXPR_CODE_SYMBOL) );
                return _sexpr(this_pyr, string, index, bstruct_cp(this_pyr, global_irt->symbols->SEXPR_CODE_SYMBOL));
        case '(' : 
                *index = j+1;
                return _sexpr(this_pyr, string, index, bstruct_cp(this_pyr, global_irt->symbols->SEXPR_LIST_SYMBOL) );
//                return bstruct_cp( this_pyr, _sexpr(this_pyr, string, index, SEXPR_LIST_SYMBOL) );
                //return _sexpr(this_pyr, string, index, SEXPR_LIST_SYMBOL);
        case '[' : 
                *index = j+1;
                return _sexpr(this_pyr, string, index, nil);
//                return bstruct_cp( this_pyr, _sexpr(this_pyr, string, index, nil) );
        case '-' : adv(comment_required);
        default  : parse_error;
    }


comment_required:
//_trace;
    chkdone;
    switch(string[j]){
        case '-': adv(comment);
        default : parse_error;
    }


comment:
//_trace;
    chkdone;
    switch(string[j]){
        case 0x0d: 
        case 0x0a: adv(null_context);
        default  : adv(comment);
    }    

done:
    return nil;

}


//
//
mword *_sexpr(pyr_cache *this_pyr, mword *string, mword *index, mword *sexpr_type){ // _sexpr#

    mword i,j;
    i=j=*index;

    mword length = size(string);

    if(!length){ goto done; }

    mword *curr_sexpr     = nil;
    mword *captured_token = nil;
    mword capture_length  = 0;

    if(!is_nil(sexpr_type)){
        append_sexpr(sexpr_type);
    }

list_context:
//_trace;
    chkdone;
    switch(string[j]){
        case ' ' :
        case 0x0a:
        case 0x0d: adv(list_context);
        case '{' : 
                *index = ++j;
                append_sexpr(_sexpr(this_pyr, string, index, bstruct_cp(this_pyr, global_irt->symbols->SEXPR_CODE_SYMBOL)));
//                append_sexpr(bstruct_cp( this_pyr, _sexpr(this_pyr, string, index, SEXPR_CODE_SYMBOL)));
                j = *index;
                goto list_context;
        case '(' : 
                *index = ++j;
                append_sexpr(_sexpr(this_pyr, string, index, bstruct_cp(this_pyr, global_irt->symbols->SEXPR_LIST_SYMBOL)));
//                append_sexpr(bstruct_cp( this_pyr, _sexpr(this_pyr, string, index, SEXPR_LIST_SYMBOL)));
                j = *index;
                goto list_context;
        case '[' :
                // add a state to check for nil braces []
                *index = ++j;
                append_sexpr(_sexpr(this_pyr, string, index, nil));
                j = *index;
                goto list_context;
        case '-' : adv(comment_or_token);
        case '}' : 
        case ')' : 
        case ']' : adv(done);
        default  : goto token;
    }


token:
//_trace;
    switch(string[j]){
        case '"' : advi(dquote);
        case '\'': advi(squote);
        default  : advi(non_quote);
    }


dquote:
//_trace;
    chkdone;
    switch(string[j]){
        case '"' : j++;
                   capture_dquote;
                   goto list_context;
        case '\\': esc(dquote);
        default  : adv(dquote);
    }


squote:        
//_trace;
    chkdone;
    switch(string[j]){
        case '\'': j++;
                   capture_squote;
                   goto list_context;
        case '/' : esc(squote);
        default  : adv(squote);
    }


non_quote:
//_trace;
    chkdone;
    switch(string[j]){
        case ' ' :
        case 0x0a:
        case 0x0d:
        case '{' :
        case '}' :
        case '(' :
        case ')' :
        case '[' :
        case ']' :
            capture_token;
            goto list_context;
        case '\\': esc(non_quote);
        default  : adv(non_quote);
    }


comment_or_token:
//_trace;
    chkdone;
    switch(string[j]){
        case '-': adv(comment);
        default : ret(token);
    }


comment:
//_trace;
    chkdone;
    switch(string[j]){
        case 0x0d: 
        case 0x0a: adv(list_context);
        default  : adv(comment);
    }


done:
//_trace;
    *index = j;

    if((list_len(this_pyr, curr_sexpr) == 1) && (array_cmp_lex(this_pyr, pcar(curr_sexpr), global_irt->symbols->SEXPR_LIST_SYMBOL, MWORD_ASIZE) == 0)){
        curr_sexpr = _ptr(this_pyr, bstruct_cp(this_pyr, global_irt->symbols->SEXPR_NIL_SYMBOL));
    }

    return curr_sexpr;

}


// input string: array-8 string ... INCLUDING THE QUOTES
// returns: standard Babel-string
//
mword *_unescape(pyr_cache *this_pyr, mword *string, mword escape_char){ // _unescape#

    int i,j;
    mword *temp_string;
    mword *final_string;
    mword string_length = size(string);

    if(!string_length){ return mem_new_val(this_pyr, 1, 0); } //return the empty string

    temp_string = mem_new_val(this_pyr, string_length, 0);

    for(i=0,j=0;i<string_length;i++,j++){

        if(rdv(string,i) == escape_char){
            mword character = rdv(string,i+1);
            if(character == 'n'){
                rdv(temp_string,j) = 0x0a;
                i++;
            }
            else if(character == 'r'){
                rdv(temp_string,j) = 0x0d;
                i++;
            }
            else if(character == 't'){
                rdv(temp_string,j) = 0x09;
                i++;
            }
            else if(character == '0'){

                #define ASCII_CODE_CHAR_LENGTH 2
                mword *ascii_code = mem_new_val(this_pyr, ASCII_CODE_CHAR_LENGTH, 0);

                ldv(ascii_code,0) = rdv(string,i+2);
                ldv(ascii_code,1) = rdv(string,i+3);

                mword *ascii_value = _radix2cu(this_pyr, array_to_string(this_pyr, ascii_code), 16);

                ldv(temp_string,j) = rdv(ascii_value,0);

                i+=3;

            }
            else if(character == 'u'){

                #define UNICODE_CHAR_LENGTH 4
                mword *unicode = mem_new_val(this_pyr, UNICODE_CHAR_LENGTH, 0);

                ldv(unicode,0) = rdv(string,i+2);
                ldv(unicode,1) = rdv(string,i+3);
                ldv(unicode,2) = rdv(string,i+4);
                ldv(unicode,3) = rdv(string,i+5);

                mword *unicode_value = _radix2cu(this_pyr, array_to_string(this_pyr, unicode), 16);

                ldv(temp_string,j) = rdv(unicode_value,0);

                i+=5;

            }
            else{
                ldv(temp_string,j) = character;
                i++;
            }

        }
        else{ //Not an escape sequence

            ldv(temp_string,j) = rdv(string,i);

        }

    }

    if(j != string_length){
        final_string = array_slice(this_pyr, temp_string, 0, j);
    }
    else{
        final_string = temp_string;
    }

    return final_string;

}


//    mword *bstring = _str2ar(this_pyr, oi0.data);
//    result0 = sexpr_op2(this_pyr, bstring);
//
//
mword *sexpr_from_string(pyr_cache *this_pyr, mword *bstring){ // sexpr_from_string#

    mword index=0;
    return _pre_sexpr(this_pyr, string_to_array(this_pyr, bstring), &index);

}


// Clayton Bauman 2017

