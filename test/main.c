#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

#include "lib/include/cnt-allocator.h"
#include "lib/src/cnt-array-impl.h"


void *my_alloc_call( size_t len )
{
    void *ptr = malloc( len );
    printf( "Allocate %lu bytes @%llx\n", len, ptr );
    return ptr;
}

void *my_realloc_call( void *ptr, size_t len )
{
    void *ptr_new = realloc( ptr, len );
    printf( "Reallocate %lu bytes @%llx->@%llx\n", len, ptr, ptr_new );
    return ptr_new;
}

void int_del( void *ptr )
{
    (void)(ptr);
}

void *int_cpy( void *dst, const void *src, size_t len )
{
    (void)(len);
    *((int *)dst) = *((const int *)src);
    return dst;
}

const CntElementTraits inttrait = {
    sizeof( int ), int_del, int_cpy
};

int main( )
{

    CntAllocator my_alloc = cnt_default_allocator;
    CntArrayImpl *myarr;

    my_alloc.allocate = my_alloc_call;
    my_alloc.reallocate = my_realloc_call;

    myarr = cnt_array_impl_new( &inttrait, &my_alloc );

    void *p = cnt_array_impl_begin( myarr );

    cnt_array_impl_free( myarr );

    return 0;
}

