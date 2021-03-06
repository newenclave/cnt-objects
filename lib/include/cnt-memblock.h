#ifndef CNT_MEMBLOCK_H
#define CNT_MEMBLOCK_H

#include "cnt-object.h"

struct CntMemblockImpl;

typedef struct CntMemblock {
    CntObject_BASE;
    struct CntMemblockImpl *impl_;
} CntMemblock;

CntMemblock *cnt_memblock_new( void );
CntMemblock *cnt_memblock_new_from( const void *data, size_t length );
CntMemblock *cnt_memblock_new_reserved( size_t reserve_size );

/**
  * Create with allocator
  */
CntMemblock *cnt_memblock_new_al        ( const CntAllocator *allocator );
CntMemblock *cnt_memblock_new_from_al   ( const void *data, size_t length,
                                          const CntAllocator *allocator );
CntMemblock *cnt_memblock_new_reserved_al( size_t reserve_size,
                                           const CntAllocator *allocator );

size_t cnt_memblock_size      (const CntMemblock *mb);
size_t cnt_memblock_capacity  (const CntMemblock *mb);
size_t cnt_memblock_available (const CntMemblock *mb);

void  *cnt_memblock_begin     (CntMemblock *mb);
void  *cnt_memblock_end       (CntMemblock *mb);
void  *cnt_memblock_at        (CntMemblock *mb, size_t position);

const void  *cnt_memblock_cbegin (const CntMemblock *mb);
const void  *cnt_memblock_cend   (const CntMemblock *mb);
const void  *cnt_memblock_cat    (const CntMemblock *mb, size_t position);

/**
  * 1 == success
  * 0 == failed
  */
int  cnt_memblock_reserve  (CntMemblock *mb, size_t new_size);
int  cnt_memblock_resize   (CntMemblock *mb, size_t new_size);
int  cnt_memblock_push_back(CntMemblock *mb, char c);
int  cnt_memblock_extend   (CntMemblock *mb, size_t count);
int  cnt_memblock_append   (CntMemblock *mb, const void *data, size_t len);

void cnt_memblock_clear   (CntMemblock *mb);
void cnt_memblock_swap    (CntMemblock *lmb, CntMemblock *rmb);
void cnt_memblock_zero    (CntMemblock *mb);

void  *cnt_memblock_create_back( CntMemblock *mb, size_t count );
void  *cnt_memblock_create_front( CntMemblock *mb, size_t count );
void  *cnt_memblock_create_insert( CntMemblock *mb,
                                   size_t position, size_t count );

void *cnt_memblock_delete( CntMemblock *mb, size_t position, size_t count );
void  cnt_memblock_reduce( CntMemblock *mb, size_t count );

#endif // CNTMEMBLOCK_H
