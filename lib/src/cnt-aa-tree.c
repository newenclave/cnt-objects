
#include <assert.h>

#include "cnt-aa-tree.h"


CntAATree *cnt_aa_tree_new( const CntElementTraits *traits,
                            const CntAllocator *allocator )
{
    CntAATree *inst;
    assert( traits != NULL );
    assert( allocator != NULL );

    inst = (CntAATree *)allocator->allocate( sizeof(*inst) );

    if( inst ) {
        inst->root_      = NULL;
        inst->count_     = 0;
        inst->allocator_ = allocator;
        inst->traits_    = traits;
    }

    return inst;
}

int cnt_aa_tree_init( CntAATree *aat,
                      const CntElementTraits *traits,
                      const CntAllocator *allocator )
{
    assert( traits != NULL );
    assert( allocator != NULL );
    assert( aat != NULL );

    aat->root_      = NULL;
    aat->count_     = 0;
    aat->allocator_ = allocator;
    aat->traits_    = traits;

    return 1;
}

void cnt_aa_tree_free( CntAATree *aat )
{
    assert( aat != NULL );

    //aat->allocator_->deallocate( aat );
}

/**
 *  Destroy elements
**/
void cnt_aa_tree_deinit( CntAATree *aat )
{
    ;;;
}
