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
    printf( "destroy: %lu\n", *i );
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
        //printf( "init data to %ul\n", 101 );
        *r++ = 101;
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

#define MAX_ITERATION 8

int main( )
{
    CntAllocator def_allocator = cnt_default_allocator;

    CntDequeImpl deq;

    MYTYPE i;


    cnt_deque_impl_init( &deq, &inttrait, &def_allocator );

    printf( "start\n" );

//    for( i=0; i<MAX_ITERATION; ++i ) {
//        cnt_deque_impl_push_front( &deq, &i );
//    }

//    printf( "start back\n" );

//    for( i=0; i<MAX_ITERATION; ++i ) {
//        cnt_deque_impl_push_back( &deq, &i );
//    }

//    printf( "start pop\n" );
//    while( !cnt_deque_impl_empty( &deq ) ) {
//        cnt_deque_impl_pop_back( &deq );
//        cnt_deque_impl_pop_front( &deq );
//    }

//    printf( "start push-pop\n" );

    for( i=0; i<MAX_ITERATION; ++i ) {
        cnt_deque_impl_push_back( &deq, &i );
        cnt_deque_impl_pop_front( &deq );
    }

    printf( "start deinit %lu\n", cnt_deque_impl_size( &deq ) );

    //cnt_deque_impl_deinit( &deq );

    return 0;
}

