#ifndef CNT_DEQUE_IMPL_H
#define CNT_DEQUE_IMPL_H

#include "include/cnt-list.h"
#include "include/cnt-element-traits.h"
#include "include/cnt-allocator.h"

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
 *  Destroy elements
**/
void cnt_deque_impl_deinit( CntDequeImpl *deq );

/**
 * 1 == success
 * 0 == failed
**/
int cnt_deque_impl_init( CntDequeImpl *deq,
                         const CntElementTraits *traits,
                         const CntAllocator *allocator );


#endif // CNT_DEQUE_IMPL_H
