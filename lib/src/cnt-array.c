
#include "include/cnt-array.h"
#include "include/cnt-element-traits.h"
#include "cnt-array-impl.h"


static void destroy( struct CntObject *obj );
static unsigned int hash( const struct CntObject *obj );
static CntObject * clone(const CntObject * obj);
static int compare( const struct CntObject *l, const struct CntObject *r );

#define cnt_this_object_type_id CNT_OBJ_ARRAY

CNT_DEFINE_OBJECT_TYPE( CntArray,  cnt_this_object_type_id );

#define ARRPIMPL( mb ) (mb)->impl_

static CntElementTraits arr_traits = {
    sizeof( CntObject * )
};

CntArray *cnt_array_new( void )
{
    return cnt_array_new_al( &cnt_default_allocator );
}

CntArray *cnt_array_new_al( const CntAllocator *allocator )
{
    CntArray *inst;

    assert( allocator != NULL );
    assert( allocator->allocate != NULL );
    assert( allocator->deallocate != NULL );
    assert( allocator->reallocate != NULL );

    inst = (CntArray *)CNT_CALL_ALLOCATE( allocator, sizeof(*inst) );

    if( inst ) {
        CNT_OBJECT_INIT( inst, &cnt_this_object_type, allocator );
        ARRPIMPL(inst) = cnt_array_impl_new( NULL, allocator );
    }
    return inst;
}


/**
 * ========== trait functions ==========
**/

static void destroy( struct CntObject *obj )
{
    CntArray *arr;

    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    arr = CNT_OBJECT_CONTAINER( CntArray, obj );
    cnt_array_impl_free( arr->impl_ );
    CNT_CALL_DEALLOCATE( obj->allocator_, arr );
}

static unsigned int hash( const struct CntObject *obj )
{
    size_t arr_size;
    size_t i;
    CntArray *arr;
    unsigned int res_hash = 0;

    CNT_OBJECT_ASSERT_TYPE( obj, cnt_this_object_type_id );
    arr = CNT_OBJECT_CONTAINER( CntArray, obj );

    arr_size = cnt_array_impl_size( arr->impl_ );

    for( i=0; i<arr_size; ++i) {

        const CntObject *next =
                *((const CntObject **)cnt_array_impl_cat( arr->impl_, i ));

        unsigned int element_hash = cnt_object_hash( next );

        res_hash = res_hash
            ? tdb_hash_start(  &element_hash, sizeof(element_hash) )
            : tdb_hash_update( &element_hash, sizeof(element_hash), res_hash );
    }
    return tdb_hash_final( res_hash );
}

static CntObject * clone(const CntObject * obj)
{

}

static int compare( const struct CntObject *l, const struct CntObject *r )
{

}

