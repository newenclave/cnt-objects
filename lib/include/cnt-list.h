#ifndef CNT_LIST_H
#define CNT_LIST_H

#include "cnt-common.h"

typedef struct CntListHead {
    struct CntListHead *flink;
} CntListHead;

static CntListHead *linked_list_last( CntListHead *lst )
{
    while( lst->flink ) lst = lst->flink;
    return lst;
}

#define CNT_LIST_INSERT( element, new_ptr )       \
    ((new_ptr)->flink =  (element)->flink,        \
     (element)->flink =  (new_ptr))

#define CNT_LIST_INSERT_LIST( src, oth  )         \
    (linked_list_last(oth)->flink = (src)->flink, \
     (src)->flink =                 (oth))

#define CNT_LIST_INSERT_BY_FIELD( element, new_ptr, field_name )        \
    CNT_LIST_INSERT( &(element)->(field_name), &(new_ptr)->(field_name) )

#define LINKED_LIST_INSERT_LIST_BY_FIELD( src, oth, field_name )        \
    CNT_LIST_INSERT_LIST( &(src)->(field_name), &(oth)->(field_name) )


#endif // CNTLIST_H
