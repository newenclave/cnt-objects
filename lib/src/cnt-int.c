
#include "include/cnt-int.h"

static void destroy( struct CntObject *obj );
static unsigned int hash( const struct CntObject *obj );

#ifdef _MSC_VER

static const CntTypeInfo cnt_int_type = {
    CNT_OBJ_INT, // .id_
    destroy,     // .destroy_
    hash         // .hash_
};

#else

static const CntTypeInfo cnt_int_type = {
    .id_        = CNT_OBJ_INT,
    .destroy_   = destroy,
    .hash_      = hash
};

#endif

CntInt *cnt_int_new_from_int( int64_t value )
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

static void destroy( struct CntObject *obj )
{
    CNT_OBJECT_ASSERT_TYPE( obj, CNT_OBJ_INT );
    free( CNT_OBJECT_CONTAINER( obj, CntInt ) );
}

static unsigned int hash( const CntObject *obj )
{
    CNT_OBJECT_ASSERT_TYPE( obj, CNT_OBJ_INT );
    int64_t value = CNT_OBJECT_CONTAINER( obj, CntInt )->value_;
    return tdb_hash(&value, sizeof(value));
}

int64_t cnt_int_get_value( const CntInt *obj )
{
    return obj->value_;
}
