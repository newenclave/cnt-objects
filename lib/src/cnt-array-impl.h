#ifndef CNT_ARRAY_IMPL_H
#define CNT_ARRAY_IMPL_H

#include "cnt-memblock-impl.h"

typedef struct CntArrayImpl {
    CntMemblockImpl        mmblock_;
    size_t                 element_size_;
} CntArrayImpl;


#endif // CNTARRAYIMPL_H

