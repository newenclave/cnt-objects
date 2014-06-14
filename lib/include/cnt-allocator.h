#ifndef CNT_ALLOCATOR_H
#define CNT_ALLOCATOR_H

#include <stdlib.h>

typedef struct CntAllocator {
    void * (* allocate)( size_t );
    void   (* deallocate)( void * );
    void * (* reallocate)( void *, size_t);
} CntAllocator;

extern const CntAllocator cnt_default_allocator;

#endif // CNTALLOCATOR_H
