#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

#include "lib/include/cnt-allocator.h"
#include "lib/src/cnt-array-impl.h"
#include "lib/src/cnt-heap-impl.h"
#include "lib/src/cnt-deque-impl.h"

typedef size_t MYTYPE;

#include "include/cnt-list.h"

void *my_alloc_call( size_t len )
{
    void *ptr = malloc( len );
    printf( "Allocate %lu bytes @%lx\n", len, (unsigned long)ptr );
    return ptr;
}

void *my_realloc_call( void *ptr, size_t len )
{
    void *ptr_new = realloc( ptr, len );
    printf( "Reallocate %lu bytes @%lx->@%lx\n", len,
            (unsigned long)ptr, (unsigned long)ptr_new );
    return ptr_new;
}


void *int_cpy( void *dst, const void *src, size_t len )
{
    assert( len == sizeof( MYTYPE ) );

    *((MYTYPE *)dst) = *((const MYTYPE *)src);

    //printf( "copy: %lu\n", *((MYTYPE *)dst) );

    return dst;
}

void int_del( void *ptr, size_t len )
{
    MYTYPE *i;
    assert( len == sizeof( MYTYPE ) );

    i = (MYTYPE *)ptr;
    //printf( "destroy: %lu\n", *i );
}

int compare( const void *l, const void *r, size_t len )
{
    assert( len == sizeof( MYTYPE ) );

//    printf( "compare %lu, %lu\n", *((const MYTYPE *)l), *((const MYTYPE *)r) );

    return *((const MYTYPE *)l) < *((const MYTYPE *)r)
            ? -1
            : *((const MYTYPE *)r) < *((const MYTYPE *)l);
}

void swap( void *l, void *r, size_t len )
{
    MYTYPE tmp;
    assert( len == sizeof( MYTYPE ) );

              tmp  = *((MYTYPE *)l);
    *((MYTYPE *)l) = *((MYTYPE *)r);
    *((MYTYPE *)r) = tmp;
}

void init( void *src, size_t cnt, size_t len  )
{
    MYTYPE *r;
    assert( len == sizeof( MYTYPE ) );

    r = ((MYTYPE *)src);
    while( cnt-- ) {
        *r++ = 99;
    }
}

CntElementTraits inttrait = {
    sizeof( MYTYPE ), init, int_del, int_cpy, compare, swap
};

CntElementTraits inttrait_def = { sizeof( MYTYPE ) };

typedef struct test_list {
    CntDLinkedListHead l;
    int b;
} test_list;


int main( )
{
    CntAllocator def_allocator = cnt_default_allocator;

    CntDequeImpl deq;

    cnt_deque_impl_init( &deq, &inttrait, &def_allocator );

    printf( "deq count: %u\n", deq.count_ );

    cnt_deque_impl_deinit( &deq );

    return 0;
}

