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
    struct CntDequeUnit  *unit_;
    void                 *ptr_;
} CntDequeSide;

typedef struct CntDequeImpl {
    struct CntDequeSide    sides_[2];
    size_t                 element_size_;
    size_t                 count_;

    const CntAllocator     *allocator_;
    const CntElementTraits *traits_;
} CntDequeImpl;

#endif // CNT_DEQUE_IMPL_H
