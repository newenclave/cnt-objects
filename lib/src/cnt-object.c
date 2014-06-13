#include "include/cnt-object.h"
#include <stdlib.h>

void cnt_object_decref( CntObject *obj )
{
    if( obj && (--obj->refcount_ == 0) ) {
        assert( obj->type_           != NULL );
        assert( obj->type_->destroy_ != NULL );
        if( --obj->refcount_ == 0 ) {
            obj->type_->destroy_( obj );
        }
    }
}

void cnt_object_incref( CntObject *obj )
{
    assert( obj != NULL );
    ++obj->refcount_;
}

