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

static void array_elements_del( CntArrayImpl *arr,
                                void *begin, size_t count,
                                void (* freecall)(void *, size_t) )
{
    if( freecall ) {

        size_t i;

        for( i=0; i<count; ++i ) {
            freecall( begin, ARR_ELEMENT_SIZE( arr ) );
            begin = ARR_ELEMENT_NEXT( begin, ARR_ELEMENT_SIZE( arr ) );
        }
    }
}

static void reduce_array( CntArrayImpl *arr, size_t count,
                          void (* destroy)( void *, size_t ) )
{
    size_t old_count = ARR_ELEMENTS_COUNT( arr );

    void *tail_ptr = ARR_ELEMENT_SHIFT( MBPTR(arr),
                                        ARR_ELEMENT_SIZE(arr),
                                        old_count - count );

    array_elements_del( arr, tail_ptr, count, destroy );

    MBUSED( arr ) = ARR_ELEMENTS_SIZE( ARR_ELEMENT_SIZE(arr),
                                       old_count - count );
}

static void copy_elements( void *dst, const void *src,
                           const size_t count,  const size_t element_size,
                           void *(* copy)( void *, const void *, size_t ))
{
    if( copy ) {

        size_t i;

        for( i = 0; i < count; ++i ) {
            copy( dst, src, element_size );
            dst = ARR_ELEMENT_NEXT( dst, element_size );
            src = ARR_ELEMENT_NEXT( src, element_size );
        }
    } else {
        arr_memcopy( dst, src, count * element_size );
    }
}

static void *init_elements( void *src, size_t count, size_t element_size,
                            int (* init)( void *, size_t, size_t ) )
{
    if( src && init ) {
        init( src, count, element_size );
    }
    return src;
}

static int extend_array( CntArrayImpl *arr, size_t count,
                         int (* init)( void *, size_t, size_t ))
{
    int res = 0;

    if( init ) {

        void *tail = cnt_memblock_impl_create_back( MBPIMPL(arr),
                                 ARR_ELEMENTS_SIZE( ARR_ELEMENT_SIZE( arr ),
                                                    count ) );
        if( tail ) {
            init( tail, count, ARR_ELEMENT_SIZE( arr ) );
            res = 1;
        }

    } else {
        res = cnt_memblock_impl_extend( MBPIMPL( arr ),
              ARR_ELEMENTS_SIZE( arr->traits_->element_size, count ));
    }
    return res;
}

CntArrayImpl *cnt_array_impl_new_reserved( const CntElementTraits *traits,
                                           const CntAllocator *allocator ,
                                           size_t count )
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
    int res;
    size_t old_size;

    assert( arr != NULL );

    old_size = ARR_ELEMENTS_COUNT( arr );

    if( old_size < count ) {
        res = extend_array( arr, count - old_size, arr->traits_->init );
    }
    return res;
}

int cnt_array_impl_resize( CntArrayImpl *arr, size_t count )
{
    int res = 1;
    size_t old_count;

    assert( arr != NULL );

    old_count = ARR_ELEMENTS_COUNT( arr );

    if( old_count < count ) {
        res = cnt_array_impl_reserve( arr, count );
        if( res ) {
            MBUSED( arr ) = ARR_ELEMENTS_SIZE( ARR_ELEMENT_SIZE(arr), count );
        }
    } else if( old_count > count ) {
        reduce_array( arr, old_count - count, arr->traits_->destroy  );
    }

    return res;
}

int cnt_array_impl_extend( CntArrayImpl *arr, size_t count )
{
    assert( arr != NULL );
    return extend_array( arr, count, arr->traits_->init );
}

int cnt_array_impl_push_back ( CntArrayImpl *arr, const void *element )
{
    cnt_array_impl_append ( arr, element, 1 );
}

int cnt_array_impl_append ( CntArrayImpl *arr, const void *src, size_t count )
{
    int     res;
    size_t  old_size;

    assert( arr != NULL );
    assert( arr->traits_ != NULL );

    old_size = ARR_ELEMENTS_COUNT( arr );

    res = extend_array( arr, count, arr->traits_->init );

    if( res ) {

        void *dst = ARR_ELEMENT_SHIFT( MBPTR( arr ),
                                      ARR_ELEMENT_SIZE( arr ), old_size );

        copy_elements( dst, src, count, ARR_ELEMENT_SIZE( arr ),
                       arr->traits_->copy );

    }
    return res;
}

void *cnt_array_impl_create_back( CntArrayImpl *arr, size_t count )
{
    return init_elements(
                cnt_memblock_impl_create_back (
                    MBPIMPL(arr),
                    ARR_ELEMENT_SIZE( arr ) * count ),
                count,
                ARR_ELEMENT_SIZE( arr ),
                arr->traits_->init );
}

void *cnt_array_impl_create_front( CntArrayImpl *arr, size_t count )
{
    return init_elements(
                cnt_memblock_impl_create_front (
                    MBPIMPL(arr),
                    ARR_ELEMENT_SIZE( arr ) * count ),
                count,
                ARR_ELEMENT_SIZE( arr ),
                arr->traits_->init );
}

void *cnt_array_impl_create_insert( CntArrayImpl *arr,
                                    size_t posision, size_t count )
{
    return init_elements(
                cnt_memblock_impl_create_insert (
                    MBPIMPL(arr),
                    ARR_ELEMENT_SIZE( arr ) * posision,
                    ARR_ELEMENT_SIZE( arr ) * count ),
                count,
                ARR_ELEMENT_SIZE( arr ),
                arr->traits_->init );
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

size_t cnt_array_impl_foreach( CntArrayImpl *arr,
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

size_t cnt_array_impl_cforeach( const CntArrayImpl *arr,
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

    array_elements_del( arr,
                        MBPTR( arr ), ARR_ELEMENTS_COUNT( arr ),
                        arr->traits_->destroy );

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


