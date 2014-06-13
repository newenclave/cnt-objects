
#include "include/cnt-memblock.h"

#include <memory.h>
#include <string.h>

static void destroy( struct CntObject *obj );
static unsigned int hash( const struct CntObject *obj );

static const size_t void_ptr_size      =   sizeof(void *);
static const size_t void_ptr_size_mask =   sizeof(void *) - 1;

#define CNT_MBLOCK_AT(ptr, pos) (((char *)ptr) + (pos))
#define CNT_MBLOCK_DEF_INC(size) (size + (size >> 1))

#define CNT_MBLOCK_AVAILABLE_LOCAL( mb )                            \
      ((mb)->impl_->capacity_ - (mb)->impl_->used_)

#define CNT_MBLOCK_FIX_SIZE( new_size )                             \
      ((new_size + void_ptr_size_mask) & (~(void_ptr_size_mask)))

#define CNT_MBLOCK_FIX_SIZE0( new_size )                            \
      ( new_size <= void_ptr_size)                                  \
      ? void_ptr_size                                               \
      : CNT_MBLOCK_FIX_SIZE( new_size )

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

typedef struct CntMemblockImpl {

    union block_data {
        void *ptr_;
        char *str_;
    }        data_;

    size_t   capacity_;
    size_t   used_;

} CntMemblockImpl;

static void *block_memset ( void *data, int c, size_t len )
{
    return memset( data, c, len );
}

static void *block_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

static void *block_malloc(size_t size)
{
    return malloc(size);
}

static void block_free_ptr(void *ptr)
{
    free(ptr);
}

static void *block_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy( dest, src, n );
}

static void *block_memmove(void *dest, const void *src, size_t n)
{
    return memmove( dest, src, n );
}

static size_t block_calc_prefer_size( size_t old_capa, size_t desired_size )
{
    size_t new_capa = CNT_MBLOCK_FIX_SIZE( CNT_MBLOCK_DEF_INC( old_capa ) );
    desired_size = CNT_MBLOCK_FIX_SIZE( desired_size );

    return ( new_capa > desired_size ) ? new_capa : desired_size;
}

static CntMemblockImpl *create_impl( size_t reserve_size )
{
    CntMemblockImpl *new_impl =
            (CntMemblockImpl *)block_malloc( sizeof(*new_impl) );

    if( new_impl ) {

        const size_t new_size = CNT_MBLOCK_FIX_SIZE0( reserve_size );

        new_impl->data_.ptr_ = block_malloc( new_size );

        if( new_impl->data_.ptr_ ) {

            new_impl->capacity_ = new_size;
            new_impl->used_     = 0;

        } else {
            block_free_ptr( new_impl );
            new_impl = NULL;
        }
    }

    return new_impl;
}

static void memblock_free( CntMemblock *container )
{
    block_free_ptr( container->impl_->data_.ptr_ );
    block_free_ptr( container->impl_ );
    block_free_ptr( container );
}

CntMemblock *cnt_memblock_new( )
{
    return cnt_memblock_new_reserved( void_ptr_size );
}

CntMemblock *cnt_memblock_new_reserved( size_t reserve_size )
{
    CntMemblock *new_inst = (CntMemblock *)block_malloc( sizeof(*new_inst) );

    if( new_inst ) {

        CNT_OBJECT_INIT( new_inst, &cnt_this_object_type );

        new_inst->impl_ = create_impl(  reserve_size >= void_ptr_size
                                      ? reserve_size
                                      : void_ptr_size);
        if( !new_inst->impl_ ) {
            block_free_ptr( new_inst );
            new_inst = NULL;
        }
    }
    return new_inst;
}


size_t cnt_memblock_size( const CntMemblock *mb )
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );

    return mb->impl_->used_;
}

size_t cnt_memblock_capacity( const CntMemblock *mb )
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );

    return mb->impl_->capacity_;
}

size_t cnt_memblock_available( const CntMemblock *mb )
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );

    return mb->impl_->capacity_ - mb->impl_->used_;
}

void  *cnt_memblock_begin( CntMemblock *mb )
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );

    return mb->impl_->data_.ptr_;
}

void  *cnt_memblock_end( CntMemblock *mb )
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );

    return CNT_MBLOCK_AT( mb->impl_->data_.ptr_, mb->impl_->used_);
}

void  *cnt_memblock_at( CntMemblock *mb, size_t position )
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );
    assert( mb->impl_->used_ >= position );

    return CNT_MBLOCK_AT(mb->impl_->data_.ptr_, position );
}

const void  *cnt_memblock_const_begin(const CntMemblock *mb)
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );

    return mb->impl_->data_.ptr_;
}

const void  *cnt_memblock_const_end(const CntMemblock *mb)
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );

    return CNT_MBLOCK_AT( mb->impl_->data_.ptr_, mb->impl_->used_);

}

const void  *cnt_memblock_const_at(const CntMemblock *mb, size_t position)
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );
    assert( mb->impl_->used_ >= position );

    return CNT_MBLOCK_AT(mb->impl_->data_.ptr_, position );
}

int cnt_memblock_reserve ( CntMemblock *mb, size_t new_size )
{
    char *new_data;

    assert( mb != NULL );
    assert( mb->impl_ != NULL );

    new_data = mb->impl_->data_.ptr_;

    assert( new_data != NULL );

    if( new_size > mb->impl_->capacity_ ) {

        new_size = block_calc_prefer_size( mb->impl_->capacity_, new_size );
        new_data = (char *)block_realloc( new_data, new_size );

        if( new_data ) {
            mb->impl_->data_.ptr_ = new_data;
            mb->impl_->capacity_  = new_size;
        }
    }

    return (new_data != NULL);
}

int cnt_memblock_resize  (CntMemblock *mb, size_t new_size)
{
    int result = cnt_memblock_reserve( mb, new_size );
    if( result ) mb->impl_->used_ = new_size;

    return result;
}

int cnt_memblock_push_back( CntMemblock *mb, char c)
{
    void *tail = cnt_memblock_create_back( mb, 1 );
    if( tail != NULL ) {
        *(char *)tail = c;
    }
    return tail != 0;
}

void cnt_memblock_clear(CntMemblock *mb)
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );
    mb->impl_->used_ = 0;
}

void cnt_memblock_swap ( CntMemblock *lmb, CntMemblock *rmb )
{
    CntMemblockImpl *tmpimpl;
    assert( lmb != NULL );
    assert( rmb != NULL );

    tmpimpl    = lmb->impl_;
    lmb->impl_ = rmb->impl_;
    rmb->impl_ = tmpimpl;
}

void cnt_memblock_zero( CntMemblock *mb )
{
    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );

    block_memset( mb->impl_->data_.ptr_, 0, mb->impl_->used_ );
}

void *cnt_memblock_create_back( CntMemblock *mb, size_t count )
{
    size_t old_size;
    void *tail = NULL;
    int res;

    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );

    old_size = mb->impl_->used_;

    res = cnt_memblock_resize( mb, old_size + count );

    if( 0 != res ) {
        tail = CNT_MBLOCK_AT(mb->impl_->data_.ptr_, old_size);
    }
    return tail;
}

void *cnt_memblock_create_front( CntMemblock *mb, size_t count )
{
    return cnt_memblock_create_insertion( mb, 0, count );
}

void *cnt_memblock_create_insertion( CntMemblock *mb,
                                     size_t position, size_t count )
{
    void *block = NULL;

    assert( mb != NULL );
    assert( mb->impl_ != NULL );
    assert( mb->impl_->data_.ptr_ != NULL );

    if( position == mb->impl_->used_ ) {

        block = cnt_memblock_create_back( mb, count );

    } else if( CNT_MBLOCK_AVAILABLE_LOCAL( mb ) < count ) {

        size_t new_size
                = block_calc_prefer_size( mb->impl_->capacity_,
                                          mb->impl_->used_ + count );

        CntMemblock *new_block = cnt_memblock_new_reserved( new_size );

        if( new_block ) {

            size_t new_tail_shift  = position + count;
            new_block->impl_->used_ = mb->impl_->used_ + count;

            if( position ) {
                block_memcpy( new_block->impl_->data_.ptr_,
                              mb->impl_->data_.ptr_, position );
            }
            block_memcpy( CNT_MBLOCK_AT(new_block->impl_->data_.ptr_,
                                        new_tail_shift),
                          CNT_MBLOCK_AT(mb->impl_->data_.ptr_, position),
                          mb->impl_->used_ - position);

            cnt_memblock_swap( mb, new_block );

            memblock_free( new_block );

            block = CNT_MBLOCK_AT(mb->impl_->data_.ptr_, position);
        }
    } else {

        void *from = CNT_MBLOCK_AT(mb->impl_->data_.ptr_ , position);

        block_memmove( CNT_MBLOCK_AT(from, count), from,
                       mb->impl_->used_ - position);

        mb->impl_->used_ += count;
        block = from;
    }

    return block;
}


/// ========================== DESTROY AND HASH ========================== ///
static void destroy( struct CntObject *obj )
{
    CntMemblock *container;

    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    container = CNT_OBJECT_CONTAINER( obj, CntMemblock );

    memblock_free( container );
}

static unsigned int hash( const struct CntObject *obj )
{
    CntMemblock *container;
    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    container = CNT_OBJECT_CONTAINER( obj, CntMemblock );

    return tdb_hash( container->impl_->data_.ptr_, container->impl_->used_ );
}

