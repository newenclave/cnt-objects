#ifndef CNT_ALLOCATOR_H
#define CNT_ALLOCATOR_H

#include <stdlib.h>

typedef struct CntAllocator {
    void * (* allocate)( size_t );
    void   (* deallocate)( void * );
    void * (* reallocate)( void *, size_t);
} CntAllocator;

#define CNT_CALL_ALLOCATE(   al,      size ) al->allocate( size )
#define CNT_CALL_DEALLOCATE( al, ptr       ) al->deallocate( ptr )
#define CNT_CALL_REALLOCATE( al, ptr, size ) al->reallocate( ptr, size )

extern const CntAllocator cnt_default_allocator;

#endif // CNTALLOCATOR_H

