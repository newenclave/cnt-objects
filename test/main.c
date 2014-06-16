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
    CntMemblock *myblock;
    CntInt *myblock2;

    my_alloc.allocate = my_alloc_call;
    my_alloc.reallocate = my_realloc_call;

    myblock =  cnt_memblock_new_from( "123", 3 );
    myblock2 = cnt_int_new( );

    printf( "cmp: %s %s %d\n",
            CNT_OBJECT_NAME( myblock ), CNT_OBJECT_NAME( myblock2 ),
            CNT_OBJECTS_COMPARE( myblock, myblock2 ) );

    CNT_DECREF(myblock);
    CNT_DECREF(myblock2);

    return 0;
}

