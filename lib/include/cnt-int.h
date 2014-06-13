#ifndef CNT_INT_H
#define CNT_INT_H

#include "cnt-object.h"

typedef struct CntInt {
    CntObject_BASE;
    int value_;
} CntInt;

CntInt *cnt_int_new( void );
CntInt *cnt_int_new_from_int( int value );

#endif // CNTINT_H
