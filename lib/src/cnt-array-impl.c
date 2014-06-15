#include "cnt-array-impl.h"

#define ARR_ELEMENT_SIZE( arr ) ((arr)->traits_->element_size)

#define ARR_ELEMENTS_SIZE( element_size, count ) ((element_size) * (count))

#define MBPIMPL( arr ) (&(arr)->mblock_)

#define ARR_ELEMENT_SHIFT( ptr, element_size, count )    \
    (((char *)ptr) + ((element_size) * (count)))

#define ARR_ELEMENT_NEXT( ptr, element_size )    \
    (((char *)ptr) + (element_size))

#define ARR_ELEMENT_PREV( ptr, element_size )    \
    (((char *)ptr) - (element_size))

static CntArrayImpl *create_arr( const CntElementTraits *traits,
                                 const CntAllocator *allocator,
                                 size_t reserve )
{
    CntArrayImpl *inst =
            (CntArrayImpl *)CNT_CALL_ALLOCATE( allocator, sizeof(*inst) );

    if( inst ) {

        inst->traits_ = traits;

        int res = cnt_memblock_impl_init(
                            &inst->mblock_,
                            ARR_ELEMENTS_SIZE( traits->element_size, reserve ),
                            allocator );
        if( !res ) {
            CNT_CALL_DEALLOCATE( allocator, inst );
            inst = 0;
        }
    }

    return inst;
}

CntArrayImpl *cnt_array_impl_new( const CntElementTraits *traits,
                                  const CntAllocator *allocator )
{
    assert( traits != NULL );
    assert( traits->copy != NULL );
    assert( traits->destroy != NULL );

    assert( allocator != NULL );
    assert( allocator->allocate != NULL );
    assert( allocator->deallocate != NULL );

    return create_arr( traits, allocator, 0 );
}

void cnt_array_impl_free( CntArrayImpl *arr )
{
    void *begin;
    size_t i;
    size_t count;

    assert( arr != NULL );
    assert( arr->traits_ != NULL );
    assert( arr->traits_->destroy != NULL );

    count = cnt_array_impl_size( arr );
    begin = cnt_memblock_impl_begin( MBPIMPL( arr ) );

    for( i=0; i<count; ++i ) {
        CNT_ELEMENT_DESTROY( arr->traits_, begin );
        begin = ARR_ELEMENT_NEXT( begin, ARR_ELEMENT_SIZE( arr ) );
    }

    cnt_memblock_impl_deinit( MBPIMPL( arr ) );
}

size_t cnt_array_impl_size( CntArrayImpl *arr )
{
    assert( arr != NULL );
    return ARR_ELEMENT_SIZE( arr ) * cnt_memblock_impl_size( MBPIMPL(arr) );
}

