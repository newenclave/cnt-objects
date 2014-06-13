#ifndef CNT_OBJECT_H
#define CNT_OBJECT_H

#include <stdlib.h>
#include <assert.h>
#include "cnt-common.h"

typedef enum {
     CNT_OBJ_NONE = 0
    ,CNT_OBJ_INT
    ,CNT_OBJ_MEMBLOCK

    ,CNT_OBJ_MAX
} cnt_object_type_id;

static const char * cnt_type_id_to_string( cnt_object_type_id id )
{
    switch ( id ) {

    case CNT_OBJ_NONE:
        return "none";

    case CNT_OBJ_INT:
        return "int";

    case CNT_OBJ_MEMBLOCK:
        return "memory block";

    case CNT_OBJ_MAX:
        return "__max__";
    }
}

struct CntObject;

typedef struct CntTypeInfo {
    cnt_object_type_id id_;
    void (*destroy_)( struct CntObject * );
    unsigned int (*hash_)( const struct CntObject * );
} CntTypeInfo;

typedef struct CntObject {
    const CntTypeInfo *type_;
    unsigned int       refcount_;
} CntObject;

#define CntObject_BASE  \
        CntObject base_

#define CNT_OBJECT_CONTAINER( base_obj, container_type ) \
        CONTAINER_OF( base_obj, container_type, base_ )

#define CNT_OBJECT_BASE( obj ) (&(obj)->base_)

#define CNT_OBJECT_ASSERT_TYPE( obj, obj_type ) \
        assert( obj != NULL );                  \
        assert( obj->type_ != NULL );           \
        assert( obj->type_->id_ == obj_type )

#define CNT_OBJECT_INIT( obj, obj_type )    \
        obj->base_.refcount_  = 1;          \
        obj->base_.type_      = obj_type

void         cnt_object_decref( CntObject *obj );
void         cnt_object_incref( CntObject *obj );
unsigned int cnt_object_hash( const CntObject *obj );

#define CNT_DECREF( obj ) cnt_object_decref( CNT_OBJECT_BASE(obj) )
#define CNT_INCREF( obj ) cnt_object_incref( CNT_OBJECT_BASE(obj) )

#define CNT_OBJECT_HASH( obj ) cnt_object_hash( CNT_OBJECT_BASE(obj) )
#define CNT_OBJECT_TYPE( obj ) ( CNT_OBJECT_BASE(obj)->type_->id_ )

#define CNT_OBJECT_TYPE_STRING( obj ) \
        cnt_type_id_to_string( CNT_OBJECT_BASE(obj)->type_->id_ )

#endif // CNTOBJECT_H
