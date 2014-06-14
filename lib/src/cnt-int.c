
#include "include/cnt-int.h"

static void destroy( struct CntObject *obj );
static unsigned int hash( const struct CntObject *obj );

#define cnt_this_object_type_id CNT_OBJ_INT

#ifdef _MSC_VER

static const CntTypeInfo cnt_int_type = {
    cnt_this_object_type_id, // .id_
    destroy,                 // .destroy_
    hash                     // .hash_
};

#else

static const CntTypeInfo cnt_int_type = {
    .id_        = cnt_this_object_type_id,
    .destroy_   = destroy,
    .hash_      = hash
};

#endif

CntInt *cnt_int_new_from_int_al( int64_t value, const CntAllocator *allocator )
{
    CntInt *inst;

    assert( allocator != NULL );
    assert( allocator->allocate != NULL );
    assert( allocator->deallocate != NULL );
    assert( allocator->reallocate != NULL );

    inst = (CntInt *)allocator->allocate( sizeof(*inst) );
    if( inst ) {
        CNT_OBJECT_INIT_AL( inst, &cnt_int_type, allocator );
        inst->value_ = value;
    }
    return inst;

}

CntInt *cnt_int_new_from_int( int64_t value )
{
    return cnt_int_new_from_int_al( value, &cnt_default_allocator );
}

CntInt *cnt_int_new( void )
{
    return cnt_int_new_from_int( 0 );
}

CntInt *cnt_int_new_al( const CntAllocator *allocator )
{
    return cnt_int_new_from_int_al( 0, allocator );
}

int64_t cnt_int_get_value( const CntInt *obj )
{
    return obj->value_;
}

/// ========================== DESTROY AND HASH ========================== ///
static void destroy( struct CntObject *obj )
{
    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    obj->allocator_->deallocate( CNT_OBJECT_CONTAINER( obj, CntInt ) );
}

static unsigned int hash( const CntObject *obj )
{
    int64_t value;
    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );

    value = CNT_OBJECT_CONTAINER( obj, CntInt )->value_;
    return tdb_hash( &value, sizeof(value) );
}
