#ifndef CNT_HEAP_IMPL_H
#define CNT_HEAP_IMPL_H

#include "cnt-array-impl.h"

typedef struct CntHeapImpl {
    CntArrayImpl    array_;
} CntHeapImpl;

CntHeapImpl *cnt_heap_impl_new( const CntElementTraits *traits,
                                const CntAllocator *allocator );

void cnt_heap_impl_free( CntHeapImpl *hp );


/**
 *  Destroy elements
**/
void cnt_heap_impl_deinit( CntHeapImpl *hp );

/**
 * 1 == success
 * 0 == failed
**/
int cnt_heap_impl_init( CntHeapImpl *hp,
                        const CntElementTraits *traits,
                        const CntAllocator *allocator );

int cnt_heap_impl_assign( CntHeapImpl *hp, const void *elements, size_t count );

int cnt_heap_impl_assign_array( CntHeapImpl *hp, const CntArrayImpl *arr );

size_t cnt_heap_impl_size( CntHeapImpl *hp );

void cnt_heap_impl_swap( CntHeapImpl *lhp, CntHeapImpl *rhp );

int cnt_heap_impl_push  ( CntHeapImpl *hp, const void *element );
void cnt_heap_impl_pop  ( CntHeapImpl *hp );

void *cnt_heap_impl_top       ( CntHeapImpl *hp );
const void *cnt_heap_impl_ctop( const CntHeapImpl *hp );


#endif // CNT_HEAP_IMPL_H

