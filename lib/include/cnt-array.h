#ifndef CNT_ARRAY_H
#define CNT_ARRAY_H

#include "cnt-object.h"

struct CntArrayImpl;

typedef struct CntArray {
    CntObject_BASE;
    struct CntArrayImpl *impl_;
} CntArray;

CntArray *cnt_array_new( void );
CntArray *cnt_array_new_al( const CntAllocator *allocator );



#endif // CNTARRAY_H
