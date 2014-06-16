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

int cnt_objects_compare( const CntObject *l, const CntObject *r )
{
    if( l->type_->id_ == r->type_->id_ ) {
        return l->type_->compare_( l, r );
    } else {
        return (l->type_->id_ < r->type_->id_) ? -1 : 1;
    }
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

const CntAllocator cnt_default_allocator = {
    cnt_allocate, cnt_deallocate, cnt_reallocate
};

#else

const CntAllocator cnt_default_allocator = {
    .allocate   = cnt_allocate,
    .deallocate = cnt_deallocate,
    .reallocate = cnt_reallocate,
};

#endif

