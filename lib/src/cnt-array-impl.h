#ifndef CNT_ARRAY_IMPL_H
#define CNT_ARRAY_IMPL_H

#include "cnt-memblock-impl.h"
#include "include/cnt-element-traits.h"

typedef struct CntArrayImpl {
    CntMemblockImpl         mblock_;
    const CntElementTraits *traits_;
} CntArrayImpl;

CntArrayImpl *cnt_array_impl_new( const CntElementTraits *traits,
                                  const CntAllocator *allocator );

void cnt_array_impl_free( CntArrayImpl *arr );

size_t cnt_array_impl_size( CntArrayImpl *arr );

#endif // CNTARRAYIMPL_H

