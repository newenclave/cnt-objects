#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

#include "lib/include/cnt-allocator.h"
#include "lib/src/cnt-array-impl.h"
#include "lib/src/cnt-heap-impl.h"


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
    assert( len == sizeof( double ) );

    *((double *)dst) = *((const double *)src);

    printf( "copy: %lf\n", *((double *)dst) );

    return dst;
}

void int_del( void *ptr, size_t len )
{
    double *i;
    assert( len == sizeof( double ) );

    i = (double *)ptr;
    printf( "destroy: %lf\n", *i );
}

int compare( const void *l, const void *r, size_t len )
{
    assert( len == sizeof( double ) );
    return *((const double *)l) < *((const double *)r)
            ? -1
            : *((const double *)r) < *((const double *)l);
}

void swap( void *l, void *r, size_t len )
{
    double tmp;
    assert( len == sizeof( double ) );

              tmp  = *((double *)l);
    *((double *)l) = *((double *)r);
    *((double *)r) = tmp;
}

void init( void *src, size_t cnt, size_t len  )
{
    double *r;
    assert( len == sizeof( double ) );

    r = ((double *)src);
    while( cnt-- ) {
        *r++ = 99.999;
    }
}

CntElementTraits inttrait = {
    sizeof( double ), init, int_del, int_cpy, compare, swap
};

CntElementTraits inttrait_def = { sizeof( double ) };

int main( )
{
    CntAllocator my_alloc = cnt_default_allocator;
    CntHeapImpl *a = cnt_heap_impl_new( &inttrait, &my_alloc );
    size_t i;
    double d;
    double *r;

    srand( 12312 );

    for( i=0; i<100; ++i ) {
        double t = rand( );
        cnt_heap_impl_push( a, &t );
    }

    while( cnt_heap_impl_size( a ) > 0 ) {
        double t = *((double *)cnt_heap_impl_top( a ));
        printf( "top : %lf size: %lu\n", t, cnt_heap_impl_size( a ) );
        cnt_heap_impl_pop( a );
    }

    cnt_heap_impl_free( a );

    return 0;
}

