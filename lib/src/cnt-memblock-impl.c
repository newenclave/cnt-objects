#ifndef CNT_MEMBLOCK_IMPL_C
#define CNT_MEMBLOCK_IMPL_C

#include <assert.h>
#include <memory.h>
#include <string.h>

#include "cnt-memblock-impl.h"


static const size_t void_ptr_size      =   sizeof(void *);
static const size_t void_ptr_size_mask =   sizeof(void *) - 1;

#define CNT_MBLOCK_AT(ptr, pos) (((char *)ptr) + (pos))
#define CNT_MBLOCK_DEF_INC(size) (size + (size >> 1))

//#define MBLOCK_IMPL( block ) (block)

#define CNT_MBLOCK_AVAILABLE_LOCAL( mb )                            \
      ((mb)->capacity_ - (mb)->used_)

#define CNT_MBLOCK_FIX_SIZE( new_size )                             \
      ((new_size + void_ptr_size_mask) & (~(void_ptr_size_mask)))

#define CNT_MBLOCK_FIX_SIZE0( new_size )                            \
      ( new_size <= void_ptr_size)                                  \
      ? void_ptr_size                                               \
      : CNT_MBLOCK_FIX_SIZE( new_size )

static void *block_memset ( void *data, int c, size_t len )
{
    return memset( data, c, len );
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
    desired_size    = CNT_MBLOCK_FIX_SIZE( desired_size );

    return ( new_capa > desired_size ) ? new_capa : desired_size;
}

int  cnt_memblock_impl_init( CntMemblockImpl *mb, size_t length,
                             const CntAllocator *allocator )
{
    const size_t new_size = CNT_MBLOCK_FIX_SIZE0( length );

    assert( mb != NULL );
    assert( allocator != NULL );
    assert( allocator->allocate != NULL );

    mb->data_.ptr_ = CNT_CALL_ALLOCATE( allocator, new_size );
    mb->allocator_ = allocator;

    if( mb->data_.ptr_ ) {
        mb->capacity_ = new_size;
        mb->used_     = 0;
    }

    return mb->data_.ptr_ ? 1 : 0;
}

void cnt_memblock_impl_deinit( CntMemblockImpl *mb )
{
    if( mb && mb->data_.ptr_ ) {

        assert( mb->allocator_ != NULL );
        assert( mb->allocator_->deallocate != NULL );

        CNT_CALL_DEALLOCATE( mb->allocator_, mb->data_.ptr_ );
    }
}

int  cnt_memblock_impl_assign( CntMemblockImpl *mb,
                               const char *data, size_t length )
{
    int res;
    assert( mb != NULL );
    res = cnt_memblock_impl_resize( mb, length );
    if( res ) {
        block_memcpy( cnt_memblock_impl_begin( mb ), data, length );
    }
    return res;
}

static CntMemblockImpl *create_impl( size_t reserve_size,
                                     const CntAllocator *allocator )
{
    CntMemblockImpl *new_impl;

    assert( allocator != NULL );
    assert( allocator->allocate != NULL );
    assert( allocator->deallocate != NULL );
    assert( allocator->reallocate != NULL );

     new_impl =
           (CntMemblockImpl *)CNT_CALL_ALLOCATE( allocator, sizeof(*new_impl));

    if( new_impl ) {
        if( !cnt_memblock_impl_init ( new_impl, reserve_size, allocator ) ) {
            CNT_CALL_DEALLOCATE( allocator, new_impl );
            new_impl = NULL;
        }
    }

    return new_impl;
}

static void memblock_free( CntMemblockImpl *container )
{
    cnt_memblock_impl_deinit( container );
    if( container ) {
        CNT_CALL_DEALLOCATE( container->allocator_, container );
    }
}

CntMemblockImpl *cnt_memblock_impl_new( const CntAllocator *allocator )
{
    return cnt_memblock_impl_new_reserved( void_ptr_size, allocator );
}

void cnt_memblock_impl_free( CntMemblockImpl *mb )
{
    memblock_free( mb );
}

CntMemblockImpl *cnt_memblock_impl_new_from( const void *data, size_t length,
                                             const CntAllocator *allocate )
{
    CntMemblockImpl *inst = cnt_memblock_impl_new_reserved(length, allocate);
    if( inst ) {
        inst->used_ = length;
        block_memcpy( inst->data_.ptr_, data, length );
    }
    return inst;
}

CntMemblockImpl *cnt_memblock_impl_new_reserved( size_t reserve_size,
                                                 const CntAllocator *allocate )
{
    CntMemblockImpl *new_inst = create_impl( reserve_size, allocate );
    return new_inst;
}

size_t cnt_memblock_impl_size( const CntMemblockImpl *mb )
{
    assert( mb != NULL );

    return mb->used_;
}

size_t cnt_memblock_impl_capacity( const CntMemblockImpl *mb )
{
    assert( mb != NULL );
    return mb->capacity_;
}

size_t cnt_memblock_impl_available( const CntMemblockImpl *mb )
{
    assert( mb != NULL );

    return mb->capacity_ - mb->used_;
}

void  *cnt_memblock_impl_begin( CntMemblockImpl *mb )
{
    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );

    return mb->data_.ptr_;
}

void  *cnt_memblock_impl_end( CntMemblockImpl *mb )
{
    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );

    return CNT_MBLOCK_AT( mb->data_.ptr_, mb->used_);
}

void  *cnt_memblock_impl_at( CntMemblockImpl *mb, size_t position )
{
    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );
    assert( mb->used_ >= position );

    return CNT_MBLOCK_AT( mb->data_.ptr_, position );
}

const void  *cnt_memblock_impl_const_begin(const CntMemblockImpl *mb)
{
    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );

    return mb->data_.ptr_;
}

const void  *cnt_memblock_impl_const_end(const CntMemblockImpl *mb)
{
    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );

    return CNT_MBLOCK_AT( mb->data_.ptr_, mb->used_);
}

const void  *cnt_memblock_impl_const_at(const CntMemblockImpl *mb,
                                        size_t position)
{
    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );
    assert( mb->used_ >= position );

    return CNT_MBLOCK_AT( mb->data_.ptr_, position );
}

int cnt_memblock_impl_reserve ( CntMemblockImpl *mb, size_t new_size )
{
    void *new_data;

    assert( mb != NULL );

    new_data = mb->data_.ptr_;

    if( new_size > mb->capacity_ ) {

        new_size = block_calc_prefer_size( mb->capacity_, new_size );
        new_data = CNT_CALL_REALLOCATE( mb->allocator_, new_data, new_size );

        if( new_data ) {
            mb->data_.ptr_ = new_data;
            mb->capacity_  = new_size;
        }
    }

    return (new_data != NULL);
}

int cnt_memblock_impl_resize  (CntMemblockImpl *mb, size_t new_size)
{
    int result = cnt_memblock_impl_reserve( mb, new_size );
    if( result ) mb->used_ = new_size;

    return result;
}

int cnt_memblock_impl_push_back( CntMemblockImpl *mb, char c)
{
    void *tail = cnt_memblock_impl_create_back( mb, 1 );
    if( tail != NULL ) {
        *(char *)tail = c;
    }
    return tail != 0;
}

void cnt_memblock_impl_clear(CntMemblockImpl *mb)
{
    assert( mb != NULL );
    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );
    mb->used_ = 0;
}

void cnt_memblock_impl_swap( CntMemblockImpl *lmb, CntMemblockImpl *rmb )
{
    void   *tmp_data;
    size_t  tmp_used;
    size_t  tmp_capa;

    assert( lmb != NULL );
    assert( rmb != NULL );

    tmp_data        = lmb->data_.ptr_;
    lmb->data_.ptr_ = rmb->data_.ptr_;
    rmb->data_.ptr_ = tmp_data;

    tmp_used        = lmb->used_;
    lmb->used_      = rmb->used_;
    rmb->used_      = tmp_used;

    tmp_capa        = lmb->capacity_;
    lmb->capacity_  = rmb->capacity_;
    rmb->capacity_  = tmp_capa;
}

void cnt_memblock_impl_zero( CntMemblockImpl *mb )
{
    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );

    block_memset( mb->data_.ptr_, 0, mb->used_ );
}

void *cnt_memblock_impl_create_back( CntMemblockImpl *mb, size_t count )
{
    size_t old_size;
    void *tail = NULL;
    int res;

    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );

    old_size = mb->used_;

    res = cnt_memblock_impl_resize( mb, old_size + count );

    if( 0 != res ) {
        tail = CNT_MBLOCK_AT(mb->data_.ptr_, old_size);
    }
    return tail;
}

void *cnt_memblock_impl_create_front( CntMemblockImpl *mb, size_t count )
{
    return cnt_memblock_impl_create_insert( mb, 0, count );
}

void *cnt_memblock_impl_create_insert( CntMemblockImpl *mb,
                                       size_t position, size_t count )
{
    void *block = NULL;

    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );

    if( position == mb->used_ ) {

        block = cnt_memblock_impl_create_back( mb, count );

    } else if( CNT_MBLOCK_AVAILABLE_LOCAL( mb ) < count ) {

        size_t new_size =
                block_calc_prefer_size( mb->capacity_, mb->used_ + count );

        CntMemblockImpl *new_block =
                cnt_memblock_impl_new_reserved( new_size, mb->allocator_);

        if( new_block ) {

            size_t new_tail_shift  = position + count;
            new_block->used_ = mb->used_ + count;

            if( position ) {
                block_memcpy( new_block->data_.ptr_, mb->data_.ptr_, position );
            }

            block_memcpy(
                CNT_MBLOCK_AT(new_block->data_.ptr_, new_tail_shift),
                CNT_MBLOCK_AT(mb->data_.ptr_, position),
                mb->used_ - position);

            cnt_memblock_impl_swap( mb, new_block );

            memblock_free( new_block );

            block = CNT_MBLOCK_AT(mb->data_.ptr_, position);
        }
    } else {

        void *from = CNT_MBLOCK_AT(mb->data_.ptr_ , position);

        block_memmove( CNT_MBLOCK_AT(from, count), from,
                       mb->used_ - position);

        mb->used_ += count;
        block = from;
    }

    return block;
}

void *cnt_memblock_impl_delete( CntMemblockImpl *mb,
                                size_t position, size_t count )
{
    void    *tail_begin;
    size_t   tail_len;
    void    *res;

    assert( mb != NULL );
    assert( mb->data_.ptr_ != NULL );
    assert( mb->used_ >= position + count );

    tail_begin  = CNT_MBLOCK_AT( mb->data_.ptr_, position );

    tail_len    = mb->used_ - (position + count);

    res         = block_memmove( tail_begin,
                                 CNT_MBLOCK_AT(tail_begin, count),
                                 tail_len );
    mb->used_  -= count;

    return res;
}

void cnt_memblock_impl_reduce( CntMemblockImpl *mb, size_t count )
{
    assert( mb != NULL );
    assert( mb->used_ >= count );

    mb->used_ -= count;
}

int cnt_memblock_impl_extend( CntMemblockImpl *mb, size_t count )
{
    return cnt_memblock_impl_resize( mb, mb->used_ + count );
}

int cnt_memblock_impl_append( CntMemblockImpl *mb,
                              const void *data, size_t len )
{
    void *tail = cnt_memblock_impl_create_back( mb, len );

    if( tail ) {
        block_memcpy( tail, data, len );
    }

    return tail != NULL;
}

#endif // CNTMEMBLOCKIMPL_C
