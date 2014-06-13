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

#endif // CNTMEMBLOCK_H
