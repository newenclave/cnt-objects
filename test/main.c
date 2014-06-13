#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

int main( )
{

    CntInt *n = cnt_int_new_from_int( 1000 );
    CntMemblock *m = cnt_memblock_new_reserved( 128 );
    char *data;

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

    data = (char *)cnt_memblock_create_insertion( m, 3, 4 );

    data[0] = '!';
    data[1] = '!';
    data[2] = '!';
    data[3] = '!';

    data = (char *)cnt_memblock_begin( m );
    printf( "data hash: %lu %s %x\n", cnt_memblock_size(m), data,
            CNT_OBJECT_HASH(m));

    CNT_DECREF( n );
    CNT_DECREF( m );

    return 0;
}

