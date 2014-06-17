#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

#include "lib/include/cnt-allocator.h"
#include "lib/src/cnt-array-impl.h"


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
    assert( len == sizeof( int ) );

    *((int *)dst) = *((const int *)src);

    printf( "copy: %d\n", *((int *)dst) );

    return dst;
}

void int_del( void *ptr, size_t len )
{
    assert( len == sizeof( int ) );

    int *i = (int *)ptr;
    printf( "destroy: %d\n", *i );
}

CntElementTraits inttrait = {
    sizeof( int ), 0, int_del, int_cpy
};

CntElementTraits inttrait_def = { sizeof( int ) };

int main( )
{
    CntAllocator my_alloc = cnt_default_allocator;
    CntArrayImpl *a = cnt_array_impl_new( &inttrait, &my_alloc );
    size_t i;

    cnt_array_impl_resize( a, 10 );

    for( i=0; i<cnt_array_impl_size( a ); ++i ) {
        *((int *)cnt_array_impl_at( a, i )) = i;
    }

    printf( "Add some!\n" );

    cnt_array_impl_resize( a, 20 );

    for( i=10; i<cnt_array_impl_size( a ); ++i ) {
        *((int *)cnt_array_impl_at( a, i )) = i * 2;
    }

    printf( "Del some!\n" );

    cnt_array_impl_resize( a, 5 );

    printf( "Destroy all!\n" );

    cnt_array_impl_free( a );

    return 0;
}

