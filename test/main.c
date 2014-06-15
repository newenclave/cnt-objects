#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

#include "lib/include/cnt-allocator.h"

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

int main( )
{

    CntAllocator my_alloc = cnt_default_allocator;

    CntMemblock *m = cnt_memblock_new_from_al( "123", 3, &my_alloc );
    CntMemblock *m0;
    char *data;
    int i;

    my_alloc.allocate   = my_alloc_call;
    my_alloc.reallocate = my_realloc_call;

    printf( "data size: %lu\n", cnt_memblock_size(m));

    cnt_memblock_push_back( m, '1' );
    cnt_memblock_push_back( m, '2' );
    cnt_memblock_push_back( m, '3' );
    cnt_memblock_push_back( m, '4' );
    cnt_memblock_push_back( m, '5' );
    cnt_memblock_push_back( m, '\0' );

    data = (char *)cnt_memblock_begin( m );

    printf( "data hash: %lu %s %x\n", cnt_memblock_size(m), data,
            CNT_OBJECT_HASH(m));

    data = (char *)cnt_memblock_create_insert( m, 3, 40 );

    for( i=0; i<40; ++i ) {
        data[i] = '!';
    }

    for( i=0; i<4000; ++i ) {
        cnt_memblock_push_back( m, (char)i );
    }

    data = (char *)cnt_memblock_begin( m );
    printf( "data hash: %lu %s %x\n", cnt_memblock_size(m), data,
            CNT_OBJECT_HASH(m));

    cnt_memblock_delete( m, 3, 40 );

    cnt_memblock_reduce( m, 1 );
    cnt_memblock_append( m, "123456789\0", 10 );

    m0 = CNT_OBJECT_CLONE( CntMemblock, m );

    data = (char *)cnt_memblock_begin( m0 );
    printf( "data hash: %lu %s %x\n", cnt_memblock_size(m0), data,
            CNT_OBJECT_HASH(m0));

    printf( "m ref = %d\n", m->base_.refcount_ );

    CNT_DECREF( m );

    printf( "m0 ref = %d\n", m0->base_.refcount_ );

    CNT_DECREF( m0 );

    return 0;
}

