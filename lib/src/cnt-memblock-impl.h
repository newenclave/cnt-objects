#ifndef CNT_MEMBLOCK_IMPL_H
#define CNT_MEMBLOCK_IMPL_H

#include <stdlib.h>

#include "include/cnt-allocator.h"

typedef struct CntMemblockImpl {
    union block_data {
        void *ptr_;
    }                   data_;
    size_t              capacity_;
    size_t              used_;
    const CntAllocator *allocator_;
} CntMemblockImpl;

/**
  * 1 == success
  * 0 == failed
  */
int  cnt_memblock_impl_init( CntMemblockImpl *mb, size_t length,
                             const CntAllocator *allocator );

int  cnt_memblock_impl_assign( CntMemblockImpl *mb,
                               const char *data, size_t length );

/**
 * destroy block data
 */
void cnt_memblock_impl_deinit( CntMemblockImpl *mb );

CntMemblockImpl *cnt_memblock_impl_new( const CntAllocator *allocator );

void cnt_memblock_impl_free( CntMemblockImpl *mb );

CntMemblockImpl *cnt_memblock_impl_new_from( const void *data, size_t length,
                                             const CntAllocator *allocate );

CntMemblockImpl *cnt_memblock_impl_new_reserved(size_t reserve_size,
                                                const CntAllocator *allocate );

size_t cnt_memblock_impl_size      (const CntMemblockImpl *mb);
size_t cnt_memblock_impl_capacity  (const CntMemblockImpl *mb);
size_t cnt_memblock_impl_available (const CntMemblockImpl *mb);

void  *cnt_memblock_impl_begin     (CntMemblockImpl *mb);
void  *cnt_memblock_impl_end       (CntMemblockImpl *mb);
void  *cnt_memblock_impl_at        (CntMemblockImpl *mb, size_t position);

const void  *cnt_memblock_impl_const_begin (const CntMemblockImpl *mb);
const void  *cnt_memblock_impl_const_end   (const CntMemblockImpl *mb);
const void  *cnt_memblock_impl_const_at    (const CntMemblockImpl *mb,
                                            size_t position);

/**
  * 1 == success
  * 0 == failed
  */
int  cnt_memblock_impl_reserve  (CntMemblockImpl *mb, size_t new_size);
int  cnt_memblock_impl_resize   (CntMemblockImpl *mb, size_t new_size);
int  cnt_memblock_impl_push_back(CntMemblockImpl *mb, char c);
int  cnt_memblock_impl_extend   (CntMemblockImpl *mb, size_t count);
int  cnt_memblock_impl_append   (CntMemblockImpl *mb,
                                 const void *data, size_t len);

void cnt_memblock_impl_clear   (CntMemblockImpl *mb);
void cnt_memblock_impl_swap    (CntMemblockImpl *lmb, CntMemblockImpl *rmb);
void cnt_memblock_impl_zero    (CntMemblockImpl *mb);

void  *cnt_memblock_impl_create_back( CntMemblockImpl *mb, size_t count );
void  *cnt_memblock_impl_create_front( CntMemblockImpl *mb, size_t count );
void  *cnt_memblock_impl_create_insert( CntMemblockImpl *mb,
                                   size_t position, size_t count );

void *cnt_memblock_impl_delete( CntMemblockImpl *mb,
                                size_t position, size_t count );
void  cnt_memblock_impl_reduce( CntMemblockImpl *mb, size_t count );


#endif // CNTMEMBLOCKIMPL_H
