#ifndef CNT_MEMBLOCK_H
#define CNT_MEMBLOCK_H

#include "cnt-object.h"

struct CntMemblockImpl;

typedef struct CntMemblock {
    CntObject_BASE;
    struct CntMemblockImpl *impl_;
} CntMemblock;

CntMemblock *cnt_memblock_new( );
CntMemblock *cnt_memblock_new_reserved( size_t reserve_size );

size_t cnt_memblock_size      (const CntMemblock *mb);
size_t cnt_memblock_capacity  (const CntMemblock *mb);
size_t cnt_memblock_available (const CntMemblock *mb);

void  *cnt_memblock_begin     (CntMemblock *mb);
void  *cnt_memblock_end       (CntMemblock *mb);
void  *cnt_memblock_at        (CntMemblock *mb, size_t position);

const void  *cnt_memblock_const_begin (const CntMemblock *mb);
const void  *cnt_memblock_const_end   (const CntMemblock *mb);
const void  *cnt_memblock_const_at    (const CntMemblock *mb, size_t position);

/**
  * 1 == success
  * 0 == failed
  */
int  cnt_memblock_reserve (CntMemblock *mb, size_t new_size);
int  cnt_memblock_resize  (CntMemblock *mb, size_t new_size);

void cnt_memblock_clear   (CntMemblock *mb);
void cnt_memblock_swap    (CntMemblock *lmb, CntMemblock *rmb);
void cnt_memblock_zero    (CntMemblock *mb);


int    cnt_memblock_push_back( CntMemblock *mb, char c);

void  *cnt_memblock_create_back( CntMemblock *mb, size_t count );
void  *cnt_memblock_create_front( CntMemblock *mb, size_t count );
void  *cnt_memblock_create_insertion( CntMemblock *mb,
                                       size_t position, size_t count );


#endif // CNTMEMBLOCK_H
