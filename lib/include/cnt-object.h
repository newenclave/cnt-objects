#ifndef CNT_OBJECT_H
#define CNT_OBJECT_H

typedef enum {
     CNT_OBJ_NONE = 0
    ,CNT_OBJ_INT

    ,CNT_OBJ_MAX
} object_type_id;

struct CntObject;

typedef struct CntType {
    object_type_id id_;
    void (*destroy_)(struct CntObject *);
} CntType;

typedef struct CntObject {
    const CntType *type_;
    unsigned long  refcount_;
} CntObject;

#define CntObject_BASE  \
        CntObject base_

#define CNT_BASE_OBJECT( obj ) (&(obj)->base_)

#define CNT_OBJECT_INIT( obj, obj_type )    \
        obj->base_.refcount_  = 1;          \
        obj->base_.type_      = obj_type

void cnt_object_decref( CntObject *obj );
void cnt_object_incref( CntObject *obj );

//#define CNT_DECREF( obj ) cnt_object_decref( CNT_BASE_OBJECT(obj) )
//#define CNT_INCREF( obj ) cnt_object_incref( CNT_BASE_OBJECT(obj) )

#endif // CNTOBJECT_H
