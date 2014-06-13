
#include "include/cnt-memblock.h"

#include <memory.h>
#include <string.h>

static void destroy( struct CntObject *obj );
static unsigned int hash( const struct CntObject *obj );

#define cnt_this_object_type_id CNT_OBJ_MEMBLOCK

#ifdef _MSC_VER

static const CntTypeInfo cnt_this_object_type = {
    cnt_this_object_type_id, // .id_
    destroy,                 // .destroy_
    hash                     // .hash_
};

#else

static const CntTypeInfo cnt_this_object_type = {
    .id_        = cnt_this_object_type_id,
    .destroy_   = destroy,
    .hash_      = hash
};

#endif

static void *block_malloc(size_t size)
{
    return malloc(size);
}

static void block_free_ptr(void *ptr)
{
    free(ptr);
}

static void cnt_memblock_destroy( CntMemblock *container )
{
    cnt_memblock_impl_deinit( &container->impl_ );
    block_free_ptr( container );
}

CntMemblock *cnt_memblock_new( )
{
    return cnt_memblock_new_reserved( 0 );
}

CntMemblock *cnt_memblock_new_from( const void *data, size_t length )
{
    CntMemblock *inst = cnt_memblock_new_reserved( length );
    if( inst ) {
        if( !cnt_memblock_impl_assign( &inst->impl_, data, length ) ) {
            cnt_memblock_destroy( inst );
            inst = NULL;
        }
    }
    return inst;
}

CntMemblock *cnt_memblock_new_reserved( size_t reserve_size )
{
    CntMemblock *new_inst = (CntMemblock *)block_malloc( sizeof(*new_inst) );

    if( new_inst ) {

        CNT_OBJECT_INIT( new_inst, &cnt_this_object_type );

        if( !cnt_memblock_impl_init( &new_inst->impl_, reserve_size ) ) {
            block_free_ptr( new_inst );
            new_inst = NULL;
        }

    }
    return new_inst;
}

size_t cnt_memblock_size( const CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_size( &mb->impl_ );
}

size_t cnt_memblock_capacity( const CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_capacity( &mb->impl_ );
}

size_t cnt_memblock_available( const CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_available( &mb->impl_ );
}

void  *cnt_memblock_begin( CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_begin( &mb->impl_ );
}

void  *cnt_memblock_end( CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_end( &mb->impl_ );
}

void  *cnt_memblock_at( CntMemblock *mb, size_t position )
{
    assert( mb != NULL );
    return cnt_memblock_impl_at( &mb->impl_, position );
}

const void  *cnt_memblock_const_begin(const CntMemblock *mb)
{
    assert( mb != NULL );
    return cnt_memblock_impl_const_begin( &mb->impl_ );
}

const void  *cnt_memblock_const_end(const CntMemblock *mb)
{
    assert( mb != NULL );
    return cnt_memblock_impl_const_end( &mb->impl_ );
}

const void  *cnt_memblock_const_at(const CntMemblock *mb, size_t position)
{
    assert( mb != NULL );
    return cnt_memblock_impl_const_at( &mb->impl_, position);
}

int cnt_memblock_reserve ( CntMemblock *mb, size_t new_size )
{
    assert( mb != NULL );
    return cnt_memblock_impl_reserve ( &mb->impl_, new_size );
}

int cnt_memblock_resize(CntMemblock *mb, size_t new_size)
{
    assert( mb != NULL );
    return cnt_memblock_impl_resize( &mb->impl_, new_size);
}

int cnt_memblock_push_back( CntMemblock *mb, char c)
{
    assert( mb != NULL );
    return cnt_memblock_impl_push_back( &mb->impl_, c );
}

void cnt_memblock_clear(CntMemblock *mb)
{
    assert( mb != NULL );
    cnt_memblock_impl_clear( &mb->impl_ );
}

void cnt_memblock_swap ( CntMemblock *lmb, CntMemblock *rmb )
{
    assert( lmb != NULL );
    assert( rmb != NULL );
    cnt_memblock_impl_swap ( &lmb->impl_, &rmb->impl_ );
}

void cnt_memblock_zero( CntMemblock *mb )
{
    assert( mb != NULL );
    cnt_memblock_impl_zero( &mb->impl_ );
}

void *cnt_memblock_create_back( CntMemblock *mb, size_t count )
{
    assert( mb != NULL );
    return cnt_memblock_impl_create_back( &mb->impl_, count );
}

void *cnt_memblock_create_front( CntMemblock *mb, size_t count )
{
    return cnt_memblock_create_insert( mb, 0, count );
}

void *cnt_memblock_create_insert( CntMemblock *mb,
                                     size_t position, size_t count )
{
    assert( mb != NULL );
    return cnt_memblock_impl_create_insert( &mb->impl_, position, count );
}

void *cnt_memblock_delete( CntMemblock *mb, size_t position, size_t count )
{
    assert( mb != NULL );
    return cnt_memblock_impl_delete( &mb->impl_, position, count );
}

void cnt_memblock_reduce( CntMemblock *mb, size_t count )
{
    assert( mb != NULL );
    cnt_memblock_impl_reduce( &mb->impl_, count );
}

int cnt_memblock_extend( CntMemblock *mb, size_t count )
{
    assert( mb != NULL );
    return cnt_memblock_impl_resize( &mb->impl_,
                                cnt_memblock_impl_size( &mb->impl_ ) + count );
}

int cnt_memblock_append( CntMemblock *mb, const void *data, size_t len )
{
    assert( mb != NULL );
    return cnt_memblock_impl_append( &mb->impl_, data, len );
}


/// ========================== DESTROY AND HASH ========================== ///
static void destroy( struct CntObject *obj )
{
    CntMemblock *container;

    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    container = CNT_OBJECT_CONTAINER( obj, CntMemblock );\

    cnt_memblock_destroy( container );
}

static unsigned int hash( const struct CntObject *obj )
{
    CntMemblock *container;
    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    container = CNT_OBJECT_CONTAINER( obj, CntMemblock );

    return tdb_hash( cnt_memblock_begin( container ),
                     cnt_memblock_size( container ) );
}

