// list.c
//

#include "pyramid.h"
#include "list.h"
#include "bstruct.h"
#include "tptr.h"
#include "array.h"
#include "string.h"
#include "mem.h"


//
//
mword *list_insert(pyr_cache *this_pyr, mword *src_list, mword *dest_list){ // list_insert#

    if(is_nil(dest_list)){
        return src_list;
    }
    
    mword *next_dest_list = pcdr(dest_list);
    mword *end_src_list   = list_find_end(this_pyr, src_list);

    ldp(dest_list,1)      = src_list;
    ldp(end_src_list,1)   = next_dest_list;

    return dest_list;

}


//
//
mword *list_unshift(pyr_cache *this_pyr, mword *list, mword *bs){ // list_unshift#

    mword *endls = list_find_end(this_pyr, list);
    ldp(endls,1) = _cons(this_pyr,  bs, nil );

    return list;

}


//
//
mword *list_shift(pyr_cache *this_pyr, mword *list){ // list_shift#

    if(is_nil(list)) return nil;

    if(list_len(this_pyr,list) < 2) return list;

    mword *endls = list_find_next_to_end(this_pyr, list);

    if(is_nil(endls)) return nil;

    mword *temp = rdp(endls,1);
    ldp(endls,1) = nil;

    return temp;

}


// NOTE: _push does not update list to point to the new head, it
// is the caller's responsibility to maintain this
//
void list_push(pyr_cache *this_pyr, mword *list, mword *bs){ // list_push#

    ldp(bs,1) = list;

}


//
//
mword *list_pop(pyr_cache *this_pyr, mword *list){ // list_pop#

    if(is_nil(list)) return nil;

    mword *temp = pcdr(list);
    ldp(list,1) = nil;

    return temp;

}


//
//
mword *list_find_end(pyr_cache *this_pyr, mword *list){ // list_find_end#

    while(!is_nil(pcdr(list))){
        list = pcdr(list);
    }
    return list;
    
}


//
//
mword *list_find_next_to_end(pyr_cache *this_pyr, mword *list){ // *list_find_next_to_end#

    while(!is_nil(pcdr(pcdr(list)))){
        list = pcdr(list);
    }
    return list;
    
}


//
//
mword list_len(pyr_cache *this_pyr, mword *list){ // list_len#

    mword length = 0;

    while(!is_nil(list)){
        length++;
        list = pcdr(list);
    }

    return length;

}


//
//
mword *list_to_ptr_array(pyr_cache *this_pyr, mword *list){ // list_to_ptr_array#

    mword *head = list;
    mword *arr = mem_new_ptr(this_pyr, list_len(this_pyr, list));
    mword *curr_node;

    int i=0;

    while(!is_nil(list)){
        curr_node = pcar(list);
        if(curr_node == head){
            ldp(arr,i) = arr;
        }
        else{
            ldp(arr,i) = curr_node;
        }        
        i++;
        list = pcdr(list);
    }

    return arr;

}


//
//
mword *list_to_val_array(pyr_cache *this_pyr, mword *list){ // list_to_val_array#

    mword *head = list;
    int total_size=0;
    int list_length=0; // We don't use _len because we're already traversing the list

    while(!is_nil(list)){
        if(!is_val(pcar(list))){
            _fatal("can't cat leaf and inte arrays"); //FIXME: Exception
        }
        total_size += size(pcar(list));
        list_length++;
        list = pcdr(list);        
    }

    if(!list_length){
        return mem_new_val(this_pyr, 1, 0);
    }

    mword *arr = mem_new_val(this_pyr, total_size, 0);

    list = head;

    int i=0;
    int element_size;
    while(!is_nil(list)){

        element_size = size(pcar(list));
        memmove(arr+i, pcar(list), (size_t)UNITS_MTO8(element_size));
        i += element_size;
        list = pcdr(list);

    }

    return arr;

}


// equiv to Lisp's cdrn
//
mword *list_cdri(pyr_cache *this_pyr, mword *list, mword i){ // list_cdri#

    while(i > 0){
        i--;
        list = pcdr(list);
    }

    return list;

}


//
//
mword *list_ith(pyr_cache *this_pyr, mword *list, mword i){ // list_ith#

    list = list_cdri(this_pyr, list, i);

    return pcar(list);

}



//
//
mword *list_reverse(pyr_cache *this_pyr, mword *list, mword *new_cdr){ // list_reverse#

    mword *temp = pcdr(list);

    ldp(list,1) = new_cdr;

    if(is_nil(temp))
        return list;

    return list_reverse(this_pyr, temp, list);

}


//
//
mword *list_split(pyr_cache *this_pyr, mword *list, mword *indices){ // list_split#

    return list_split_r(this_pyr, list,indices,0);

}


//
//
mword *list_split_r(pyr_cache *this_pyr, mword *list, mword *indices, mword count){ // list_split_r#

    mword *orig_list = list;

    if (is_nil(indices)) return _cons(this_pyr,  orig_list, nil );

    if (is_nil(list)) return nil;// 

    mword curr_index = vcar(pcar(indices));

    if (curr_index < count) return _cons(this_pyr,  orig_list, nil );

    if (curr_index == 0) return _cons(this_pyr,  nil, _cons(this_pyr,  orig_list, nil ) );

    indices = pcdr(indices);

    //if (is_nil(list)) return list;
    
    mword *prev_list = list;

    while(!is_nil(list)){

//        if(count++ > 10){
//            _dump(list);
//            die;
//        }

        if(curr_index == count){
            break;
        }

        count++;

        prev_list = list;
        list = pcdr(list);

    }

    if(!is_nil(pcdr(prev_list))){
        ldp(prev_list,1) = nil;
    }

    return _cons(this_pyr,  orig_list, list_split_r(this_pyr, list, indices, count) );

}


// XXX Isn't this just a special case of list_split? XXX
//
mword *list_cut(pyr_cache *this_pyr, mword *list, mword index){ // list_cut#

    mword *temp;

    if(is_nil(list)) return nil;

    if(index == 0) return list;

    if(index == 1){

        temp = pcdr(list);
        ldp(list,1) = nil;

        return temp;

    }

    return list_cut(this_pyr, pcdr(list), index-1 );

}


//
//
mword *list_append(pyr_cache *this_pyr, mword *lists){ // list_append#

    if(is_nil(pcdr(lists)))
        return pcar(lists);
        //return lists;

    return list_append_direct(this_pyr, list_ith(this_pyr, lists,0),list_append(this_pyr, pcdr(lists)));

}


///
//
mword *list_append_direct(pyr_cache *this_pyr, mword *head_list, mword *tail_list){ // list_append_direct#

    mword *endls = list_find_end(this_pyr, head_list);

    ldp(endls,1) = tail_list;

    return head_list;

}


/*****************************************************************************
 *                                                                           *
 *                           DOUBLE-LINKED LIST                              *
 *                                                                           *
 ****************************************************************************/


//
//
mword *dlist_append_direct(pyr_cache *this_pyr, mword *head_list, mword *tail_list){ // dlist_append_direct#

    mword *endls = dlist_find_end(this_pyr, head_list);

    ldp(endls,     1) = tail_list;
    ldp(tail_list, 2) = endls;

    return head_list;

}


// insert into double-list
//
mword *dlist_insert(pyr_cache *this_pyr, mword *src_list, mword *dest_list){ // dlist_insert#

    if(is_nil(dest_list)){
        return src_list;
    }
    
    if(is_nil(src_list)){
        return dest_list;
    }

    mword *next_dest_list = pcdr(dest_list);
    mword *end_src_list   = dlist_find_end(this_pyr, src_list);

    ldp(dest_list,    1) = src_list;
    ldp(end_src_list, 1) = next_dest_list;
    ldp(src_list,     2) = dest_list;

    if(is_nil(next_dest_list)){
        return dest_list;
    }

    ldp(next_dest_list,2) = end_src_list;

    return dest_list;

}


// insert into double-list
//
mword *dlist_insert_prev(pyr_cache *this_pyr, mword *src_list, mword *dest_list){ // dlist_insert_prev#

    if(is_nil(dest_list)){
        return src_list;
    }

    if(is_nil(src_list)){
        return dest_list;
    }

    mword *prev_dest_list = pcpr(dest_list);
    mword *end_src_list   = dlist_find_end(this_pyr, src_list);

    ldp(end_src_list,  1) = dest_list;
    ldp(dest_list,     2) = end_src_list;
    ldp(src_list,      2) = prev_dest_list;

    ldp(prev_dest_list,1) = src_list;

    return dest_list;

}


//
//
mword *dlist_find_end(pyr_cache *this_pyr, mword *list){ // dlist_find_end#

    mword *head = list;
    
    while(!is_nil(pcdr(list)) && head != pcdr(list)){
        list = pcdr(list);
    }

    return list;
    
}


//
//
mword dlist_len(pyr_cache *this_pyr, mword *list){ // dlist_len#

    mword length = 0;
    mword *head = list;

    while(!is_nil(list)){
        length++;
        if(head == pcdr(list)){
            break; }
        list = pcdr(list);
    }

    return length;

}


// This function assumes the dlist is well-formed
//
mword *dlist_reverse(pyr_cache *this_pyr, mword *list, mword *head, mword direction){ // dlist_reverse#

    if(is_nil(list))
        return nil;

    mword *next = rdp(list,1+direction);
    mword *prev = rdp(list,2-direction);

    ldp(list,2-direction) = next;
    ldp(list,1+direction) = prev;

//    mword *next = rdp(list,1);
//    mword *prev = rdp(list,2);
//
//    ldp(list,2) = next;
//    ldp(list,1) = prev;

    if(next == head || is_nil(next))
        return list;

    return dlist_reverse(this_pyr, next, head, direction);

}


//
//
mword *dlist_cut(pyr_cache *this_pyr, mword *list, mword index, mword direction){ // dlist_cut#

    mword *temp;

    if(is_nil(list)) return nil;

    if(index == 0) return list;

    if(direction == CDR_DIRECTION){
        temp = rdp(list,1);
    }
    else{ //if(direction == CPR_DIRECTION)
        temp = rdp(list,2);
    }

    if(index == 1){

        ldp(list,1+direction) = nil;
        ldp(temp,2-direction) = nil;

        return temp;

    }

    return dlist_cut(this_pyr, temp, index-1, direction);

}



//list operators
//--------------
//
//(a) (b c d)   append  --> (a b c d)
//(a b c d) (1) part    --> (a) (b c d)
//(a) (b c d)   cons    --> [(a) (b c d)]
//(b c d) (a)   push    --> (a b c d)
//(a b c d)     pop     --> (b c d) (a)
//(a b c) (d)   unshift --> (a b c d)
//(a b c d)     shift   --> (a b c) (d)


// Clayton Bauman 2017

