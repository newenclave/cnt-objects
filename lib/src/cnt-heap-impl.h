#ifndef CNT_HEAP_IMPL_H
#define CNT_HEAP_IMPL_H

#include "cnt-array-impl.h"

typedef struct CntHeapImpl {
    CntArrayImpl    array_;
} CntHeapImpl;

CntHeapImpl *cnt_heap_impl_new( const CntElementTraits *traits,
                                const CntAllocator *allocator );

void cnt_heap_impl_free( CntHeapImpl *hp );

size_t cnt_heap_impl_size( CntHeapImpl *hp );

int cnt_heap_impl_push  ( CntHeapImpl *hp, const void *element );
void cnt_heap_impl_pop  ( CntHeapImpl *hp );

void *cnt_heap_top( CntHeapImpl *hp );
const void *cnt_heap_ctop( const CntHeapImpl *hp );


#endif // CNT_HEAP_IMPL_H

