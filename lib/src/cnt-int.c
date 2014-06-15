
#include "include/cnt-int.h"

static void destroy( struct CntObject *obj );
static unsigned int hash( const struct CntObject *obj );
static CntObject * clone(const CntObject * obj);

#define cnt_this_object_type_id CNT_OBJ_INT

#ifdef _MSC_VER

static const CntTypeInfo cnt_this_object_type = {
    cnt_this_object_type_id, // .id_
    destroy,                 // .destroy_
    hash,                    // .hash_
    clone                    // .clone_
};

#else

static const CntTypeInfo cnt_this_object_type = {
    .id_        = cnt_this_object_type_id,
    .destroy_   = destroy,
    .hash_      = hash,
    .clone_     = clone
};

#endif

CntInt *cnt_int_new_from_int_al( int64_t value, const CntAllocator *allocator )
{
    CntInt *inst;

    assert( allocator != NULL );
    assert( allocator->allocate != NULL );
    assert( allocator->deallocate != NULL );
    assert( allocator->reallocate != NULL );

    inst = (CntInt *)CNT_CALL_ALLOCATE( allocator, sizeof(*inst) );
    if( inst ) {
        CNT_OBJECT_INIT( inst, &cnt_this_object_type, allocator );
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
    CNT_CALL_DEALLOCATE( obj->allocator_, CNT_OBJECT_CONTAINER( CntInt, obj ));
}

static unsigned int hash( const CntObject *obj )
{
    int64_t value;
    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );

    value = CNT_OBJECT_CONTAINER( CntInt, obj )->value_;
    return tdb_hash( &value, sizeof(value) );
}

static CntObject * clone( const CntObject *obj )
{
    int64_t value;
    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    CntInt *cloned = cnt_int_new_from_int( value );
    return CNT_OBJECT_BASE( cloned );
}

