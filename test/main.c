#include <stdio.h>

//#include "lib/include/cnt-common.h"

#include "lib/include/cnt-int.h"

int main( )
{
    CntInt *new_int = cnt_int_new_from_int( 44445 );

    printf( "new int value from: %ld with hash %u\n",
            cnt_int_get_value(new_int), CNT_OBJECT_HASH( new_int ) );

    CNT_DECREF( new_int );

    return 0;
}

