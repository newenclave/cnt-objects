
#include "include/cnt-memblock.h"

static void destroy( struct CntObject *obj );
static unsigned int hash( const struct CntObject *obj );

#ifdef _MSC_VER

static const CntTypeInfo cnt_int_type = {
    CNT_OBJ_INT, // .id_
    destroy,     // .destroy_
    hash         // .hash_
};

#else

static const CntTypeInfo cnt_int_type = {
    .id_        = CNT_OBJ_INT,
    .destroy_   = destroy,
    .hash_      = hash
};

#endif

typedef struct CntMemblockImpl {

    union block_data {
        void *ptr_;
        char *str_;
    }        data_;

    size_t   capacity_;
    size_t   used_;

} CntMemblockImpl;

static void destroy( struct CntObject *obj )
{

}

static unsigned int hash( const struct CntObject *obj )
{

}

CntMemblock *cnt_memblock_new( )
{

}

CntMemblock *cnt_memblock_new_reserved( size_t reserve_size )
{

}
