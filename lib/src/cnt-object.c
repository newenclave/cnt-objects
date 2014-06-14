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

void * cnt_allocate( size_t size )
{
    return malloc( size );
}

void cnt_deallocate( void *ptr )
{
    free( ptr );
}

void * cnt_reallocate( void *ptr, size_t size)
{
    return realloc( ptr, size );
}


#ifdef _MSC_VER

const struct CntAllocator cnt_default_allocator = {
    cnt_allocate, cnt_deallocate, cnt_reallocate
};

#else

const CntAllocator cnt_default_allocator = {
    .allocate   = cnt_allocate,
    .deallocate = cnt_deallocate,
    .reallocate = cnt_reallocate,
};

#endif

