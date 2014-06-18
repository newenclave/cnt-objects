#ifndef CNT_HEAP_IMPL_H
#define CNT_HEAP_IMPL_H

#include "cnt-array-impl.h"

typedef struct CntHeapImpl {
    CntArrayImpl    array_;
} CntHeapImpl;

CntHeapImpl *cnt_heap_impl_new( const CntElementTraits *traits,
                                const CntAllocator *allocator );

void cnt_heap_impl_free( CntHeapImpl *hp );


#endif // CNT_HEAP_IMPL_H

