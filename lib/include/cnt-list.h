#ifndef CNT_LIST_H
#define CNT_LIST_H

#include <stdlib.h>
#include "cnt-common.h"

/// ================ Linked list ================ ///

typedef struct CntListHead {
    struct CntListHead *flink;
} CntListHead;

static CntListHead *linked_list_last( CntListHead *lst )
{
    while( lst->flink ) lst = lst->flink;
    return lst;
}

#define CNT_LIST_INSERT( element, new_ptr )       \
    ((new_ptr)->flink = (element)->flink,         \
     (element)->flink = (new_ptr))

#define CNT_LIST_INSERT_LIST( src, oth  )         \
    (linked_list_last(oth)->flink = (src)->flink, \
     (src)->flink                 = (oth))

#define CNT_LIST_INSERT_BY_FIELD( element, new_ptr, field_name )        \
    CNT_LIST_INSERT( &(element)->(field_name), &(new_ptr)->(field_name) )

#define CNT_LIST_INSERT_LIST_BY_FIELD( src, oth, field_name )           \
    CNT_LIST_INSERT_LIST( &(src)->(field_name), &(oth)->(field_name) )


/// ================ Doubly linked list ================ ///

typedef struct CntDLinkedListHead {
    struct CntDLinkedListHead *links_[2];
} CntDLinkedListHead;

enum CntDLinkedListDirection  {
     LINK_DIRECT_BACKWARD  = 0
    ,LINK_DIRECT_FORWARD   = 1
};

#define CNT_DLINKED_LIST_INSERT( src, new_l, forward )                         \
    ((new_l)->links_[forward!=0] = (src)->links_[forward!=0],                  \
     (src)  ->links_[forward!=0] = (new_l),                                    \
     (new_l)->links_[forward==0] = (src))

#define CNT_DLINKED_LIST_INSERT_BY_FIELD( src, new_l, list_field, forward )    \
    CNT_DLINKED_LIST_INSERT(&(src)->list_field, &(new_l)->list_field, forward)

#define CNT_DLINKED_LIST_REMOVE( src )                                         \
    ((src)->links_[0] ?                                                        \
     (src)->links_[0]->links_[1] = (src)->links_[1] : (NULL),                  \
     (src)->links_[1] ?                                                        \
     (src)->links_[1]->links_[0] = (src)->links_[0] : (NULL))


#define CNT_DLINKED_LIST_REMOVE_BY_FIELD( src, list_field )                    \
    CNT_DLINKED_LIST_REMOVE(&(src)->list_field)

#define CNT_DLINKED_LIST_STEP( l, forward ) ((l)->links_[forward!=0])

#define CNT_DLINKED_LIST_NEXT( l ) \
    CNT_DLINKED_LIST_STEP( l, LINK_DIRECT_FORWARD )

#define CNT_DLINKED_LIST_PREV( l ) \
    CNT_DLINKED_LIST_STEP( l, LINK_DIRECT_BACKWARD )

#endif // CNTLIST_H
