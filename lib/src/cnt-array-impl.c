#include "cnt-array-impl.h"

#include <assert.h>

#define ARR_ELEMENT_SIZE( arr ) ((arr)->traits_->element_size)

#define ARR_ELEMENTS_SIZE( element_size, count ) ((element_size) * (count))

#define MBPIMPL( arr ) (&(arr)->mblock_)

#define MBUSED( arr )       ((arr)->mblock_.used_)
#define MBPTR( arr )        ((arr)->mblock_.data_.ptr_)
#define MBCAPACITY( arr )   ((arr)->mblock_.capacity_)

#define ARR_ELEMENTS_COUNT(arr) (MBUSED(arr) / ARR_ELEMENT_SIZE(arr))

#define ARR_ELEMENT_SHIFT( ptr, element_size, count )    \
    (((char *)ptr) + ((element_size) * (count)))

#define ARR_ELEMENT_CSHIFT( ptr, element_size, count )    \
    (((const char *)ptr) + ((element_size) * (count)))

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

CntArrayImpl *cnt_array_impl_new_reserved(const CntElementTraits *traits,
                                          const CntAllocator *allocator ,
                                          size_t count)
{
    assert( traits != NULL );
    assert( traits->copy != NULL );
    assert( traits->destroy != NULL );

    assert( allocator != NULL );
    assert( allocator->allocate != NULL );
    assert( allocator->deallocate != NULL );

    return create_arr( traits, allocator, count );
}

CntArrayImpl *cnt_array_impl_new( const CntElementTraits *traits,
                                  const CntAllocator *allocator )
{
    return cnt_array_impl_new_reserved( traits, allocator, 0 );
}

void cnt_array_impl_free( CntArrayImpl *arr )
{
    void  *begin;
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
    CNT_CALL_DEALLOCATE( MBPIMPL( arr )->allocator_, arr );
}

size_t cnt_array_impl_size( CntArrayImpl *arr )
{
    assert( arr != NULL );
    return  ARR_ELEMENTS_COUNT( arr );
}

void  *cnt_array_impl_begin( CntArrayImpl *arr )
{
    assert( arr );
    return MBPTR( arr );
}

void  *cnt_array_impl_end( CntArrayImpl *arr )
{
    assert( arr );
    return ARR_ELEMENT_SHIFT( MBPTR( arr ),
                              ARR_ELEMENT_SIZE( arr ), MBUSED( arr ) );
}

void  *cnt_array_impl_at( CntArrayImpl *arr, size_t index )
{
    assert( arr );
    assert( ARR_ELEMENTS_COUNT( arr ) > index );

    return ARR_ELEMENT_SHIFT( MBPTR( arr ),
                              ARR_ELEMENT_SIZE( arr ), index );
}

const void *cnt_array_impl_cbegin( const CntArrayImpl *arr )
{
    assert( arr );
    return MBPTR( arr );
}

const void *cnt_array_impl_cend( const CntArrayImpl *arr )
{
    assert( arr );
    return ARR_ELEMENT_CSHIFT( MBPTR( arr ),
                               ARR_ELEMENT_SIZE( arr ), MBUSED( arr ) );
}

const void *cnt_array_impl_cat( const CntArrayImpl *arr, size_t index)
{
    assert( arr );
    assert( ARR_ELEMENTS_COUNT( arr ) > index );

    return ARR_ELEMENT_CSHIFT( MBPTR( arr ),
                               ARR_ELEMENT_SIZE( arr ), index );

}


