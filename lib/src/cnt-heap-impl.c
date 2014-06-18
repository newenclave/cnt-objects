#include "cnt-heap-impl.h"

#define ARRPTR( hp )        (&(hp)->array_)
#define ARRALOCATOR( hp )   (ARRPTR( hp )->mblock_.allocator_)
#define ARRTRAITS( hp )     (ARRPTR( hp )->traits_)

CntHeapImpl *cnt_heap_impl_new( const CntElementTraits *traits,
                                const CntAllocator *allocator )
{
    CntHeapImpl *inst;
    assert( traits != NULL );

    assert( allocator != NULL );
    assert( allocator->allocate != NULL );
    assert( allocator->deallocate != NULL );

    inst = (CntHeapImpl *)allocator->allocate( sizeof(*inst) );
    if(inst) {
        cnt_array_impl_init( ARRPTR(inst), traits, allocator );
    }

    return inst;
}

void cnt_heap_impl_free( CntHeapImpl *hp )
{
    assert( hp != NULL );

    void (*dealloc)(void *) = ARRALOCATOR(hp)->deallocate;

    cnt_array_impl_deinit( ARRPTR(hp) );

    dealloc( hp );
}


