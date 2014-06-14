#ifndef CNT_INT_H
#define CNT_INT_H

#include "cnt-object.h"

typedef struct CntInt {
    CntObject_BASE;
    int64_t value_;
} CntInt;

CntInt *cnt_int_new( void );
CntInt *cnt_int_new_al( const CntAllocator *allocator );

CntInt *cnt_int_new_from_int( int64_t value );
CntInt *cnt_int_new_from_int_al( int64_t value, const CntAllocator *allocator );

int64_t cnt_int_get_value( const CntInt *obj );

#endif // CNTINT_H
