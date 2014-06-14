
#include "include/cnt-memblock.h"
#include "cnt-memblock-impl.h"

#include <memory.h>
#include <string.h>

static void destroy( struct CntObject *obj );
static unsigned int hash( const struct CntObject *obj );

#define cnt_this_object_type_id CNT_OBJ_MEMBLOCK

#define MBPIMPL( mb ) (mb)->impl_

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

static void cnt_memblock_destroy( CntMemblock *container )
{
    cnt_memblock_impl_free( container->impl_ );
    CNT_CALL_DEALLOCATE( container->base_.allocator_, container );
}

CntMemblock *cnt_memblock_new( void )
{
    return cnt_memblock_new_reserved( 0 );
}

CntMemblock *cnt_memblock_new_from( const void *data, size_t length )
{
    return cnt_memblock_new_from_al( data, length, &cnt_default_allocator );
}

CntMemblock *cnt_memblock_new_reserved( size_t reserve_size )
{
    return cnt_memblock_new_reserved_al( reserve_size, &cnt_default_allocator );
}

CntMemblock *cnt_memblock_new_al( const CntAllocator *allocator )
{
    return cnt_memblock_new_reserved_al( 0, allocator );
}

CntMemblock *cnt_memblock_new_from_al( const void *data, size_t length,
                                       const CntAllocator *allocator )
{
    CntMemblock *inst = cnt_memblock_new_reserved_al( length, allocator );
    if( inst ) {
        inst->impl_ = cnt_memblock_impl_new_from( data, length, allocator );
        if( !inst->impl_ ) {
            cnt_memblock_destroy( inst );
            inst = NULL;
        }
    }
    return inst;

}

CntMemblock *cnt_memblock_new_reserved_al( size_t reserve_size,
                                           const CntAllocator *allocator )
{
    CntMemblock *new_inst;

    assert( allocator != NULL );
    assert( allocator->allocate != NULL );
    assert( allocator->deallocate != NULL );
    assert( allocator->reallocate != NULL );

    new_inst = (CntMemblock *)CNT_CALL_ALLOCATE( allocator, sizeof(*new_inst) );

    if( new_inst ) {

        CNT_OBJECT_INIT( new_inst, &cnt_this_object_type, allocator );

        new_inst->impl_ =
                cnt_memblock_impl_new_reserved( reserve_size, allocator );

        if( NULL == new_inst->impl_ ) {
            CNT_CALL_DEALLOCATE( allocator, new_inst );
            new_inst = NULL;
        }

    }
    return new_inst;
}


size_t cnt_memblock_size( const CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_size( MBPIMPL( mb ) );
}

size_t cnt_memblock_capacity( const CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_capacity( MBPIMPL( mb ) );
}

size_t cnt_memblock_available( const CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_available( MBPIMPL( mb ) );
}

void  *cnt_memblock_begin( CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_begin( MBPIMPL( mb ) );
}

void  *cnt_memblock_end( CntMemblock *mb )
{
    assert( mb != NULL );
    return cnt_memblock_impl_end( MBPIMPL( mb ) );
}

void  *cnt_memblock_at( CntMemblock *mb, size_t position )
{
    assert( mb != NULL );
    return cnt_memblock_impl_at( MBPIMPL( mb ), position );
}

const void  *cnt_memblock_const_begin(const CntMemblock *mb)
{
    assert( mb != NULL );
    return cnt_memblock_impl_const_begin( MBPIMPL( mb ) );
}

const void  *cnt_memblock_const_end(const CntMemblock *mb)
{
    assert( mb != NULL );
    return cnt_memblock_impl_const_end( MBPIMPL( mb ) );
}

const void  *cnt_memblock_const_at(const CntMemblock *mb, size_t position)
{
    assert( mb != NULL );
    return cnt_memblock_impl_const_at( MBPIMPL( mb ), position);
}

int cnt_memblock_reserve ( CntMemblock *mb, size_t new_size )
{
    assert( mb != NULL );
    return cnt_memblock_impl_reserve ( MBPIMPL( mb ), new_size );
}

int cnt_memblock_resize(CntMemblock *mb, size_t new_size)
{
    assert( mb != NULL );
    return cnt_memblock_impl_resize( MBPIMPL( mb ), new_size);
}

int cnt_memblock_push_back( CntMemblock *mb, char c)
{
    assert( mb != NULL );
    return cnt_memblock_impl_push_back( MBPIMPL( mb ), c );
}

void cnt_memblock_clear( CntMemblock *mb )
{
    assert( mb != NULL );
    cnt_memblock_impl_clear( MBPIMPL( mb ) );
}

void cnt_memblock_swap ( CntMemblock *lmb, CntMemblock *rmb )
{
    CntMemblockImpl *tmp_impl;

    assert( lmb != NULL );
    assert( rmb != NULL );

    tmp_impl   = lmb->impl_;
    lmb->impl_ = rmb->impl_;
    rmb->impl_ = tmp_impl;

    //cnt_memblock_impl_swap ( MBIMPL_PTR( lmb ), MBIMPL_PTR( rmb ) );
}

void cnt_memblock_zero( CntMemblock *mb )
{
    assert( mb != NULL );
    cnt_memblock_impl_zero( MBPIMPL( mb ) );
}

void *cnt_memblock_create_back( CntMemblock *mb, size_t count )
{
    assert( mb != NULL );
    return cnt_memblock_impl_create_back( MBPIMPL( mb ), count );
}

void *cnt_memblock_create_front( CntMemblock *mb, size_t count )
{
    return cnt_memblock_create_insert( mb, 0, count );
}

void *cnt_memblock_create_insert( CntMemblock *mb,
                                     size_t position, size_t count )
{
    assert( mb != NULL );
    return cnt_memblock_impl_create_insert( MBPIMPL( mb ), position, count );
}

void *cnt_memblock_delete( CntMemblock *mb, size_t position, size_t count )
{
    assert( mb != NULL );
    return cnt_memblock_impl_delete( MBPIMPL( mb ), position, count );
}

void cnt_memblock_reduce( CntMemblock *mb, size_t count )
{
    assert( mb != NULL );
    cnt_memblock_impl_reduce( MBPIMPL( mb ), count );
}

int cnt_memblock_extend( CntMemblock *mb, size_t count )
{
    assert( mb != NULL );
    return cnt_memblock_impl_resize( MBPIMPL( mb ),
             cnt_memblock_impl_size( MBPIMPL( mb ) ) + count );
}

int cnt_memblock_append( CntMemblock *mb, const void *data, size_t len )
{
    assert( mb != NULL );
    return cnt_memblock_impl_append( MBPIMPL( mb ), data, len );
}


/// ========================== DESTROY AND HASH ========================== ///
static void destroy( CntObject *obj )
{
    CntMemblock *container;

    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    container = CNT_OBJECT_CONTAINER( obj, CntMemblock );

    cnt_memblock_destroy( container );
}

static unsigned int hash( const CntObject *obj )
{
    CntMemblock *container;
    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    container = CNT_OBJECT_CONTAINER( obj, CntMemblock );

    return tdb_hash( cnt_memblock_begin( container ),
                     cnt_memblock_size( container ) );
}

