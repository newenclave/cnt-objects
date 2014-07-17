#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

#include "lib/include/cnt-allocator.h"
#include "lib/src/cnt-array-impl.h"
#include "lib/src/cnt-heap-impl.h"
#include "lib/src/cnt-deque-impl.h"
#include "lib/src/cnt-aa-tree.h"

#include "lib/include/cnt-object-ptr-traits.h"

typedef size_t MYTYPE;

size_t alloc_calls   = 0;
size_t free_calls    = 0;
size_t init_calls    = 0;
size_t copy_calls    = 0;
size_t destroy_calls = 0;

#include "include/cnt-list-head.h"

void *my_alloc_call( size_t len )
{
    void *ptr = malloc( len );
    alloc_calls++;
    //printf( "Allocate %lu bytes @%lx\n", len, (unsigned long)ptr );
    return ptr;
}

void my_free_call( void *ptr )
{
    free_calls++;
    free(ptr);
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

    copy_calls++;
    //printf( "copy: %lu\n", *((MYTYPE *)dst) );

    return dst;
}

void int_del( void *ptr, size_t len )
{
    MYTYPE *i;
    assert( len == sizeof( MYTYPE ) );
    destroy_calls++;
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

    init_calls++;
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

#define MAX_ITERATION 2000000

int main( )
{
    CntAllocator def_allocator = cnt_default_allocator;
    CntDequeImpl deq;

    MYTYPE i;
    CntAATree *aat;

    def_allocator.allocate   = my_alloc_call;
    def_allocator.deallocate = my_free_call;

    aat = cnt_aa_tree_new( &inttrait, &def_allocator );

    for( i=0; i<MAX_ITERATION; ++i ) {
        cnt_aa_tree_insert( aat, &i );
        cnt_aa_tree_insert_update( aat, &i );
    }

    for( i=0; i<MAX_ITERATION; ++i ) {
        cnt_aa_tree_delete( aat, &i );
    }

    printf( "array size: %lu\n", cnt_aa_tree_size( aat ) );
    cnt_aa_tree_free( aat );

    goto tstop;


//    cnt_deque_impl_init( &deq, &inttrait, &def_allocator );
    cnt_deque_impl_init( &deq, &inttrait_def, &def_allocator );

    printf( "start\n" );

    for( i=0; i<MAX_ITERATION; ++i ) {
        cnt_deque_impl_push_front( &deq, &i );
    }

    printf( "start back\n" );

    for( i=0; i<MAX_ITERATION; ++i ) {
        cnt_deque_impl_push_back( &deq, &i );
    }

    printf( "start pop 1\n" );
    while( !cnt_deque_impl_empty( &deq ) ) {
        cnt_deque_impl_pop_back( &deq );
        cnt_deque_impl_pop_front( &deq );
    }

    printf( "start push-pop\n" );

    cnt_deque_impl_push_front( &deq, &i );
    for( i=0; i<MAX_ITERATION; ++i ) {
        cnt_deque_impl_push_back( &deq, &i );
        cnt_deque_impl_pop_front( &deq );
    }

    printf( "start push-pop 2\n" );

    for( i=0; i<MAX_ITERATION; ++i ) {
        cnt_deque_impl_push_back( &deq, &i );
        cnt_deque_impl_pop_front( &deq );
    }

    printf( "start deinit %lu\n", cnt_deque_impl_size( &deq ) );

    cnt_deque_impl_deinit( &deq );

tstop:
    printf( "alloc  : %lu\n"
            "free   : %lu\n"
            "init   : %lu\n"
            "copy   : %lu\n"
            "delete : %lu\n"
            ,
            alloc_calls, free_calls, init_calls, copy_calls, destroy_calls);

    return 0;
}

