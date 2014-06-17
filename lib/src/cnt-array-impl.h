#ifndef CNT_ARRAY_IMPL_H
#define CNT_ARRAY_IMPL_H

#include "cnt-memblock-impl.h"
#include "include/cnt-element-traits.h"

typedef struct CntArrayImpl {
    CntMemblockImpl         mblock_;
    const CntElementTraits *traits_;
} CntArrayImpl;

CntArrayImpl *cnt_array_impl_new( const CntElementTraits *traits,
                                  const CntAllocator *allocator );

CntArrayImpl *cnt_array_impl_new_reserved( const CntElementTraits *traits,
                                           const CntAllocator *allocator,
                                           size_t count );

/**
 * 1 == success
 * 0 == failed
**/
int cnt_array_impl_reserve( CntArrayImpl *arr, size_t count );
int cnt_array_impl_resize(  CntArrayImpl *arr, size_t count );

void cnt_array_impl_free( CntArrayImpl *arr );

size_t cnt_array_impl_size( const CntArrayImpl *arr );
size_t cnt_array_impl_capacity( const CntArrayImpl *arr );
size_t cnt_array_impl_available( const CntArrayImpl *arr );

const CntElementTraits *cnt_array_impl_element_traits(const CntArrayImpl *arr);

size_t cnt_array_impl_foreach( CntArrayImpl *arr,
                            void (*call)(void *, void *), void *data );

size_t cnt_array_impl_cforeach( const CntArrayImpl *arr,
                            void (*c_call)(const void *, void *), void *data );

void cnt_array_impl_swap( CntArrayImpl *larr, CntArrayImpl *rarr );

/**
 * 1 == success
 * 0 == failed
**/
int cnt_array_impl_push_back (CntArrayImpl *arr, const void *element );
int cnt_array_impl_append (CntArrayImpl *arr, const void *src, size_t count );
int cnt_array_impl_extend( CntArrayImpl *arr, size_t count );

void *cnt_array_impl_create_back( CntArrayImpl *arr, size_t count );
void *cnt_array_impl_create_front( CntArrayImpl *arr, size_t count );
void *cnt_array_impl_create_insert( CntArrayImpl *arr,
                                    size_t posision, size_t count );

void  *cnt_array_impl_begin( CntArrayImpl *arr );
void  *cnt_array_impl_end( CntArrayImpl *arr );
void  *cnt_array_impl_at( CntArrayImpl *arr, size_t index );

const void *cnt_array_impl_cbegin( const CntArrayImpl *arr );
const void *cnt_array_impl_cend( const CntArrayImpl *arr );
const void *cnt_array_impl_cat( const CntArrayImpl *arr, size_t index);

#endif // CNTARRAYIMPL_H

