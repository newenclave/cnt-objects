#ifndef CNT_DEQUE_IMPL_H
#define CNT_DEQUE_IMPL_H

#include "include/cnt-list.h"
#include "include/cnt-element-traits.h"
#include "include/cnt-allocator.h"

enum cnt_deque_start_point {
     DEQ_START_TOP    = 0
    ,DEQ_START_MIDDLE = 1
    ,DEQ_START_BOTTOM = 2
};

enum cnt_deque_direction {
     DEQ_SIDE_FRONT = 0
    ,DEQ_SIDE_BACK  = 1
};

typedef struct CntDequeUnit {
    CntDLinkedListHead  list_;
    void               *border_[2];
} CntDequeUnit;

typedef struct CntDequeSide {
    CntDequeUnit  *unit_;
    void          *ptr_;
} CntDequeSide;

typedef struct CntDequeImpl {
    const CntAllocator     *allocator_;
    const CntElementTraits *traits_;

    CntDequeSide    sides_[2];
    size_t          count_;

} CntDequeImpl;

CntDequeImpl *cnt_deque_impl_new( const CntElementTraits *traits,
                                  const CntAllocator *allocator );
/**
 * 1 == success
 * 0 == failed
**/
int cnt_deque_impl_init( CntDequeImpl *deq,
                         const CntElementTraits *traits,
                         const CntAllocator *allocator );


void    cnt_deque_impl_free(CntDequeImpl *deq);
/**
 *  Destroy elements
**/
void cnt_deque_impl_deinit( CntDequeImpl *deq );

int     cnt_deque_impl_empty( const CntDequeImpl *deq );
size_t  cnt_deque_impl_size ( const CntDequeImpl *deq );

void   *cnt_deque_impl_front( CntDequeImpl *deq );
void   *cnt_deque_impl_back ( CntDequeImpl *deq );

const void   *cnt_deque_impl_cfront( const CntDequeImpl *deq );
const void   *cnt_deque_impl_cback ( const CntDequeImpl *deq );

void *cnt_deque_impl_create_front( CntDequeImpl *deq );
void *cnt_deque_impl_create_back(  CntDequeImpl *deq );


#endif // CNT_DEQUE_IMPL_H
