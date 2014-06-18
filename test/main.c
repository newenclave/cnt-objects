#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

#include "lib/include/cnt-allocator.h"
#include "lib/src/cnt-array-impl.h"
#include "lib/src/cnt-heap-impl.h"

typedef size_t MYTYPE;

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
    //printf( "destroy: %lf\n", *i );
}

int compare( const void *l, const void *r, size_t len )
{
    assert( len == sizeof( MYTYPE ) );

//    printf( "compare %lf, %lf\n", *((const MYTYPE *)l), *((const MYTYPE *)r) );

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

int main( )
{
    CntAllocator my_alloc = cnt_default_allocator;
    CntHeapImpl *a = cnt_heap_impl_new( &inttrait, &my_alloc );
    size_t i;
    MYTYPE d;
    MYTYPE *r;

    srand( 12312 );

    for( i=0; i<1000; ++i ) {
        MYTYPE t = rand( ) % 1000;
        cnt_heap_impl_push( a, &t );
        cnt_heap_impl_push( a, &i );
    }

    printf( "Heap: %lu\n", cnt_heap_impl_size( a ) );

    while( cnt_heap_impl_size( a ) > 0 ) {
        MYTYPE t = *((MYTYPE *)cnt_heap_impl_top( a ));
        printf( " %4lu", t );
        if( cnt_heap_impl_size( a ) % 20 == 1  ) {
            printf( "\n" );
        }
        cnt_heap_impl_pop( a );
    }

    printf( "\n" );

    cnt_heap_impl_free( a );

    return 0;
}

