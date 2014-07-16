
#include <assert.h>
#include <string.h>

#include "cnt-aa-tree.h"

enum { AA_BOTTOM_LEVEL_VALUE = 1 };

static void *aat_memcopy( void *dst, const void *src, size_t size )
{
    return memcpy( dst, src, size );
}

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

static CntAATreeNode *aa_tree_create_node( CntAATree *aat, const void *data )
{
    CntAATreeNode *new_data = (CntAATreeNode *)
            aat->allocator_->allocate( sizeof(*new_data) );

    if( new_data ) {

        new_data->data_.ptr_ = aat->allocator_
                                  ->allocate( aat->traits_->element_size );

        if( !new_data->data_.ptr_ ) {
            aat->allocator_->deallocate( new_data );
            new_data = NULL;
        } else {
            if( aat->traits_->copy ) {
                aat->traits_->copy( new_data->data_.ptr_,
                                    data,
                                    aat->traits_->element_size );
            } else {
                aat_memcopy( new_data->data_.ptr_,
                             data,
                             aat->traits_->element_size );
            }
            new_data->links_[0] = new_data->links_[1] = NULL;
            new_data->level_    = AA_BOTTOM_LEVEL_VALUE;
        }
    }
    return new_data;
}

static void aa_tree_free_node_destroy( CntAATree *aat, CntAATreeNode *node )
{
    aat->traits_->destroy( node->data_.ptr_, aat->traits_->element_size );
    aat->allocator_->deallocate( node->data_.ptr_ );
    if( node->links_[0] ) {
        aa_tree_free_node_destroy( aat, node->links_[0] );
    }
    if( node->links_[1] ) {
        aa_tree_free_node_destroy( aat, node->links_[1] );
    }
    aat->allocator_->deallocate( node );
}

static void aa_tree_free_node_non_destroy( CntAATree *aat, CntAATreeNode *node )
{
    aat->allocator_->deallocate( node->data_.ptr_ );
    if( node->links_[0] ) {
        aa_tree_free_node_non_destroy( aat, node->links_[0] );
    }
    if( node->links_[1] ) {
        aa_tree_free_node_non_destroy( aat, node->links_[1] );
    }
    aat->allocator_->deallocate( node );
}

static void aa_tree_free_node( CntAATree *aat, CntAATreeNode *node )
{
    if( aat->traits_->destroy ) {
        aa_tree_free_node_destroy(aat, node);
    } else {
        aa_tree_free_node_non_destroy(aat, node);
    }
}

void cnt_aa_tree_free( CntAATree *aat )
{
    assert( aat != NULL );
    aa_tree_free_node( aat, aat->root_ );
    aat->allocator_->deallocate( aat );
}

/**
 *  Destroy elements
**/
void cnt_aa_tree_deinit( CntAATree *aat )
{
    assert( aat != NULL );
    aa_tree_free_node( aat, aat->root_ );
}

/**
 *  skew and split
**/
static CntAATreeNode *skew( CntAATreeNode *t )
{
    CntAATreeNodePtr *left = &(t->links_[AA_LINK_LEFT]);
    CntAATreeNode    *l =   t->links_[AA_LINK_LEFT];

    if( (l != 0) && (t->level_ == l->level_) ) {
        *left     = t;
        t->links_[AA_LINK_LEFT]  = l->links_[AA_LINK_RIGHT];
        l->links_[AA_LINK_RIGHT] = t;
        return l;
    }
    return t;
}

static CntAATreeNode *split( CntAATreeNode *t )
{
    CntAATreeNodePtr *right = &(t->links_[AA_LINK_RIGHT]);
    CntAATreeNode    *r     =   t->links_[AA_LINK_RIGHT];

    if( 0 == r || 0 == r->links_[AA_LINK_RIGHT] ) {
        return t;
    }

    if( t->level_ == r->links_[AA_LINK_RIGHT]->level_ ) {
        *right    = t;

        t->links_[AA_LINK_RIGHT] = r->links_[AA_LINK_LEFT];
        r->links_[AA_LINK_LEFT]  = t;
        ++r->level_;
        return r;
    }
    return t;
}
/**
 *  =====================================
**/
