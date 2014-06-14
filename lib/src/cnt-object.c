#include "include/cnt-object.h"
#include <stdlib.h>

void cnt_object_decref( CntObject *obj )
{
    if( obj && (--obj->refcount_ == 0) ) {
        assert( obj->type_           != NULL );
        assert( obj->type_->destroy_ != NULL );
        obj->type_->destroy_( obj );
    }
}

void cnt_object_incref( CntObject *obj )
{
    assert( obj != NULL );
    assert( obj->refcount_ > 0 );

    ++obj->refcount_;
}

unsigned int cnt_object_hash( const CntObject *obj )
{
    assert( obj != NULL);
    assert( obj->type_ != NULL );
    assert( obj->type_->hash_ != NULL );
    return obj->type_->hash_( obj );
}

const CntAllocator cnt_default_allocator = DefaultAllocator;

