
#include "include/cnt-memblock.h"

#include <memory.h>

static void destroy( struct CntObject *obj );
static unsigned int hash( const struct CntObject *obj );

static const size_t void_ptr_size      =   sizeof(void *);
static const size_t void_ptr_size_mask =   sizeof(void *) - 1;

#define CNT_MBLOCK_AT(ptr, pos) (((char *)ptr) + (pos))
#define CNT_MBLOCK_DEF_INC(size) (size + (size >> 1))

#define CNT_MBLOCK_AVAILABLE_LOCAL( mb )                            \
      ((mb)->capacity_ - (mb)->used_)

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



/// ========================== DESTROY AND HASH ========================== ///
static void destroy( struct CntObject *obj )
{
    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );

    CntMemblock *container = CNT_OBJECT_CONTAINER( obj, CntMemblock );

    block_free_ptr( container->impl_->data_.ptr_ );
    block_free_ptr( container->impl_ );
    block_free_ptr( container );

}

static unsigned int hash( const struct CntObject *obj )
{
    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    CntMemblock *container = CNT_OBJECT_CONTAINER( obj, CntMemblock );
    return tdb_hash( container->impl_->data_.ptr_, container->impl_->used_ );
}

