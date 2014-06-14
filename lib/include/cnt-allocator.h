#ifndef CNT_ALLOCATOR_H
#define CNT_ALLOCATOR_H

#include <stdlib.h>

typedef struct CntAllocator {
    void * (* allocate)( size_t );
    void (* deallocate)( void * );
} CntAllocator;

#ifdef _MSC_VER

#define DefaultAllocator \
    {                    \
        malloc,          \
        free             \
    }

#else

#define DefaultAllocator        \
    {                           \
        .allocate   = malloc,   \
        .deallocate = free      \
    }

#endif

#endif // CNTALLOCATOR_H
