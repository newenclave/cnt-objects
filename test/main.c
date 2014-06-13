#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"
#include "lib/include/cnt-memblock.h"

int main( )
{

    CntInt *n = cnt_int_new_from_int( 1000 );
    CntMemblock *m = cnt_memblock_new_reserved( 128 );

    printf( "data hash: %x %x\n",
            CNT_OBJECT_HASH( n ),
            CNT_OBJECT_HASH( m ) );

    CNT_DECREF( n );
    CNT_DECREF( m );

    return 0;
}

