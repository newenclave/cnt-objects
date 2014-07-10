#include <assert.h>

#include "cnt-deque-impl.h"


CntDequeImpl *cnt_deque_impl_new( const CntElementTraits *traits,
                                  const CntAllocator *allocator )
{
    CntDequeImpl *inst;

    assert( traits != NULL );
    assert( allocator != NULL );

    inst = (CntDequeImpl *)allocator->allocate( sizeof(*inst) );

    if( inst ) {
        inst->traits_    = traits;
        inst->allocator_ = allocator;
    }

    return inst;
}

