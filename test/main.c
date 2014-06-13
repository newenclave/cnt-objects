#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

int main( )
{

    CntInt *n = cnt_int_new_from_int( 1000 );
    CntMemblock *m = cnt_memblock_new_reserved( 128 );
    char *data;
    int i;

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

    data = (char *)cnt_memblock_begin( m );
    printf( "data hash: %lu %s %x\n", cnt_memblock_size(m), data,
            CNT_OBJECT_HASH(m));

    cnt_memblock_delete( m, 3, 40 );

    cnt_memblock_append( m, "123456789\0", 10 );

    data = (char *)cnt_memblock_begin( m );
    printf( "data hash: %lu %s %x\n", cnt_memblock_size(m), data,
            CNT_OBJECT_HASH(m));

    CNT_DECREF( n );
    CNT_DECREF( m );

    return 0;
}

