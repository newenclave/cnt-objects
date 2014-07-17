#ifndef CNT_OBJECT_PTR_TRAITS_H
#define CNT_OBJECT_PTR_TRAITS_H

#include <string.h>
#include <assert.h>

#include "cnt-object.h"
#include "cnt-element-traits.h"

//void (* init)( void *, size_t, size_t );
//void (* destroy)( void *, size_t );
//void *(* copy)( void *, const void *, size_t );
//int  (* compare)( const void *, const void *, size_t );
//void (* swap)( void *, void *, size_t );


static void cnt_opt_init( void *obj_ptr, size_t count, size_t element_size )
{
    assert( obj_ptr != NULL );
    assert( element_size == sizeof( sizeof(CntObject*) ));
    memset( obj_ptr, count, element_size );
}

static void  cnt_opt_destroy( void *obj_ptr, size_t element_size )
{
    CntObject *obj;
    assert( obj_ptr != NULL );
    assert( element_size == sizeof( sizeof(CntObject*) ));
    obj = *((CntObject **)obj_ptr);
    cnt_object_decref(obj);
}

static void *cnt_opt_copy( void *obj_ptr, const void *data, size_t element_size)
{
    CntObject *obj;
    assert( obj_ptr != NULL );
    assert( element_size == sizeof( sizeof(CntObject*) ));
    obj = *((CntObject **)obj_ptr);

    cnt_object_incref( obj );

    return obj;
}

static int cnt_opt_compare( const void *l, const void *r, size_t element_size )
{
    const CntObject *lobj;
    const CntObject *robj;
    assert( l != NULL );
    assert( r != NULL );
    assert( element_size == sizeof( sizeof(CntObject*) ));

    lobj = *((const CntObject **)l);
    robj = *((const CntObject **)r);

    return cnt_objects_compare( lobj, robj );
}

static void  cnt_opt_swap( void *l, void *r, size_t element_size)
{
    CntObject *lobj;
    CntObject *robj;
    assert( l != NULL );
    assert( r != NULL );
    assert( element_size == sizeof( sizeof(CntObject*) ));

    lobj = *((CntObject **)l);
    robj = *((CntObject **)r);

    *((CntObject **)r) = lobj;
    *((CntObject **)l) = robj;
}

static const CntElementTraits cnt_object_ptr_traits = {
    sizeof(CntObject *),
    cnt_opt_init,
    cnt_opt_destroy,
    cnt_opt_copy,
    cnt_opt_compare,
    cnt_opt_swap
};

#endif // CNTOBJECTPTRTRAITS_H
