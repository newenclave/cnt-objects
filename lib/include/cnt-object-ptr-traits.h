#ifndef CNT_OBJECT_PTR_TRAITS_H
#define CNT_OBJECT_PTR_TRAITS_H

#include <string.h>
#include <assert.h>

#include "cnt-object.h"
#include "cnt-element-traits.h"

static void cnt_opt_init( void *obj_ptr, size_t count, size_t element_size )
{
    assert( obj_ptr != NULL );
    assert( element_size == sizeof( sizeof(CntObject*) ));
    memset( obj_ptr, 0, count * element_size );
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

    obj = *((CntObject **)obj_ptr) = *((CntObject **)data);

    cnt_object_incref( obj );

    return obj;
}

static int cnt_opt_compare( const void *l, const void *r, size_t element_size )
{
    assert( l != NULL );
    assert( r != NULL );
    assert( element_size == sizeof( sizeof(CntObject*) ));

    return cnt_objects_compare( *((const CntObject **)l),
                                *((const CntObject **)r) );
}

static void  cnt_opt_swap( void *l, void *r, size_t element_size)
{
    CntObject *tmp;

    assert( l != NULL );
    assert( r != NULL );
    assert( element_size == sizeof( sizeof(CntObject*) ));

    tmp = *((CntObject **)l);
    *((CntObject **)l) = *((CntObject **)r);
    *((CntObject **)r) = tmp;
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
