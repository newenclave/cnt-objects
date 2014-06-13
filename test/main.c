#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"

int main( )
{

    CntInt *n = cnt_int_new_from_int( 1000 );

    printf( "data hash: %x\n", CNT_OBJECT_HASH( n ) );

    CNT_DECREF( n );
    return 0;
}

