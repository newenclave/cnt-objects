#include "cnt-array-impl.h"

#include <assert.h>
#include <string.h>

#define ARR_ELEMENT_SIZE( arr ) ((arr)->traits_->element_size)

#define ARR_ELEMENTS_SIZE( element_size, count ) ((element_size) * (count))

#define MBPIMPL( arr ) (&(arr)->mblock_)

#define MBUSED( arr )       ((arr)->mblock_.used_)
#define MBPTR( arr )        ((arr)->mblock_.data_.ptr_)
#define MBCAPACITY( arr )   ((arr)->mblock_.capacity_)

#define ARR_ELEMENTS_COUNT(arr) (MBUSED(arr) / ARR_ELEMENT_SIZE(arr))
#define ARR_ELEMENTS_CAPACITY(arr) (MBCAPACITY(arr) / ARR_ELEMENT_SIZE(arr))
#define ARR_ELEMENTS_AVAILABLE( arr )  \
        (ARR_ELEMENTS_CAPACITY(arr) - ARR_ELEMENTS_COUNT(arr))

#define ARR_ELEMENT_SHIFT( ptr, element_size, count )    \
    (((char *)ptr) + ((element_size) * (count)))

#define ARR_ELEMENT_CSHIFT( ptr, element_size, count )    \
    (((const char *)ptr) + ((element_size) * (count)))

#define ARR_ELEMENT_NEXT( ptr, element_size )    \
    (((char *)ptr) + (element_size))

#define ARR_ELEMENT_PREV( ptr, element_size )    \
    (((char *)ptr) - (element_size))

static void *arr_memcopy( void *dst, const void *src, size_t size )
{
    return memcpy( dst, src, size );
}

static CntArrayImpl *create_arr( const CntElementTraits *traits,
                                 const CntAllocator *allocator,
                                 size_t reserve )
{
    CntArrayImpl *inst =
            (CntArrayImpl *)CNT_CALL_ALLOCATE( allocator, sizeof(*inst) );

    if( inst ) {

        int res;
        inst->traits_ = traits;

        res = cnt_memblock_impl_init(
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

static size_t array_elements_del( CntArrayImpl *arr, void (* freecall)(void *) )
{
    if( freecall ) {

        void  *begin = cnt_memblock_impl_begin( MBPIMPL( arr ) );
        size_t count = cnt_array_impl_size( arr );

        size_t i;

        for( i=0; i<count; ++i ) {
            freecall( begin );
            begin = ARR_ELEMENT_NEXT( begin, ARR_ELEMENT_SIZE( arr ) );
        }
    }
}

static int extend_array_size( CntArrayImpl *arr, size_t count,
                              void *(* copy)( void *, const void *, size_t ))
{
    int res = 0;
    if( copy ) {

        const size_t old_size = ARR_ELEMENTS_COUNT( arr );

        CntArrayImpl *tmp_arr = create_arr( arr->traits_,
                                            MBPIMPL(arr)->allocator_,
                                            old_size + count );
        if( tmp_arr ) {

            size_t  i;
            void   *begin;
            void   *old_begin;

            MBUSED( tmp_arr ) = ARR_ELEMENTS_SIZE( ARR_ELEMENT_SIZE( arr ),
                                                   old_size );

            begin     = MBPTR( tmp_arr );
            old_begin = MBPTR( arr );

            for( i = 0; i < old_size; ++i ) {

                copy( begin, old_begin, ARR_ELEMENT_SIZE( arr ) );

                begin     = ARR_ELEMENT_NEXT( begin,
                                              ARR_ELEMENT_SIZE( arr ) );

                old_begin = ARR_ELEMENT_NEXT( old_begin,
                                              ARR_ELEMENT_SIZE( arr ) );
            }

            cnt_array_impl_swap( arr, tmp_arr );
            cnt_array_impl_free( tmp_arr );

            res = 1;
        }

    } else {
        res = cnt_memblock_impl_extend( MBPIMPL( arr ),
              ARR_ELEMENTS_SIZE( arr->traits_->element_size, count ));
    }
    return res;
}

CntArrayImpl *cnt_array_impl_new_reserved(const CntElementTraits *traits,
                                          const CntAllocator *allocator ,
                                          size_t count)
{
    assert( traits != NULL );

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

int cnt_array_impl_reserve( CntArrayImpl *arr, size_t count )
{
    size_t old_size;
    assert( arr != NULL );

    old_size = ARR_ELEMENTS_COUNT( arr );
    if( old_size < count ) {

    } else {

    }
    return 0;
}

void cnt_array_impl_swap( CntArrayImpl *larr, CntArrayImpl *rarr )
{
    const CntElementTraits *tmp_trait;

    assert( larr != NULL );
    assert( rarr != NULL );

    /// swap trait
    tmp_trait     = larr->traits_;
    larr->traits_ = rarr->traits_;
    rarr->traits_ = tmp_trait;

    /// swap blocks
    cnt_memblock_impl_swap( MBPIMPL( larr ), MBPIMPL( rarr ) );
}

size_t cnt_array_foreach( CntArrayImpl *arr,
                          void (*call)(void *, void *), void *data )
{
    void  *begin;
    size_t i;
    size_t count;

    assert( arr != NULL );
    assert( arr->traits_ != NULL );

    count = cnt_array_impl_size( arr );
    begin = cnt_memblock_impl_begin( MBPIMPL( arr ) );

    for( i=0; i<count; ++i ) {
        call( begin, data );
        begin = ARR_ELEMENT_NEXT( begin, ARR_ELEMENT_SIZE( arr ) );
    }
    return i;
}

size_t cnt_array_cforeach( const CntArrayImpl *arr,
                           void (*c_call)(const void *, void *), void *data )
{
    const void  *begin;
    size_t i;
    size_t count;

    assert( arr != NULL );
    assert( arr->traits_ != NULL );

    count = cnt_array_impl_size( arr );
    begin = cnt_memblock_impl_cbegin( MBPIMPL( arr ) );

    for( i=0; i<count; ++i ) {
        c_call( begin, data );
        begin = ARR_ELEMENT_NEXT( begin, ARR_ELEMENT_SIZE( arr ) );
    }
    return i;
}

void cnt_array_impl_free( CntArrayImpl *arr )
{
    assert( arr != NULL );
    assert( arr->traits_ != NULL );

    array_elements_del( arr, arr->traits_->destroy );

    cnt_memblock_impl_deinit( MBPIMPL( arr ) );
    CNT_CALL_DEALLOCATE( MBPIMPL( arr )->allocator_, arr );
}

size_t cnt_array_impl_size( const CntArrayImpl *arr )
{
    assert( arr != NULL );
    return  ARR_ELEMENTS_COUNT( arr );
}

size_t cnt_array_impl_capacity( const CntArrayImpl *arr )
{
    assert( arr != NULL );
    return ARR_ELEMENTS_CAPACITY(arr);
}

size_t cnt_array_impl_available( const CntArrayImpl *arr )
{
    assert( arr != NULL );
    return ARR_ELEMENTS_AVAILABLE( arr );
}

const CntElementTraits *cnt_array_impl_element_traits(const CntArrayImpl *arr)
{
    assert( arr != NULL );
    return arr->traits_;
}

int cnt_array_impl_push_back( CntArrayImpl *arr, void *element )
{
    return 0;
}

int cnt_array_impl_append( CntArrayImpl *arr,
                           void *elements, size_t count )
{
    return 0;
}


//// ptrs
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


