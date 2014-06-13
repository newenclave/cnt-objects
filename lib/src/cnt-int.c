#include <stdlib.h>
#include "include/cnt-int.h"

void destroy(struct CntObject *);

static const CntType cnt_int_type = {
    CNT_OBJ_INT, destroy
};

CntInt *cnt_int_new_from_int( int value )
{
    CntInt *inst = (CntInt *)malloc( sizeof(*inst) );
    if( inst ) {
        CNT_OBJECT_INIT( inst, &cnt_int_type );
        inst->value_ = value;
    }
    return inst;
}

CntInt *cnt_int_new( void )
{
    return cnt_int_new_from_int( 0 );
}

