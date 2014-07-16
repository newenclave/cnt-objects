
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
    assert( allocator != NULL );
    assert( traits != NULL );
    assert( traits->compare != NULL );
    assert( traits->copy != NULL );
    assert( traits->element_size > 0 );

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
    assert( allocator != NULL );
    assert( traits != NULL );
    assert( traits->compare != NULL );
    assert( traits->copy != NULL );
    assert( traits->element_size > 0 );
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
    if( aat->root_ ) {
        aa_tree_free_node( aat, aat->root_ );
    }
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
    CntAATreeNode    *l    =   t->links_[AA_LINK_LEFT];

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


/**
 *  common 'insert' function
 *  rerurns: 0 - failed ( *inserted == NULL )
 *           1 - success inserted ( *inserted == new_element_ptr )
 *           2 - element is already present ( *inserted == old_element_ptr )
 *
**/
static int aa_tree_node_insert( CntAATree *aat,
                                CntAATreeNodePtr *top,
                                const void *data,
                                CntAATreeNodePtr *inserted )
{
    CntAATreeNode *top_node = *top;
    int res = 0;
    if( NULL == top_node ) {
        top_node = aa_tree_create_node( aat, data );
        if( top_node ) {
            *top = *inserted = top_node;
            res = 1;
        } else {
            *inserted = NULL;
            res = 0;
        }
    } else {
        int cmp_res = aat->traits_->compare( data, top_node->data_.ptr_,
                                             aat->traits_->element_size );
        if( 0 == cmp_res ) {
            res       = 2;
            *inserted = top_node;
        } else {
            res = aa_tree_node_insert( aat,
                                       &top_node->links_[cmp_res > 0],
                                        data, inserted);
        }

        if( res == 1 ) {
            *top = split(skew(top_node));
        }
    }
    return res;
}

/// probably useless
#define aa_tree_node_check_levels(t)                                   \
         (t->links_[AA_LINK_RIGHT] &&                                  \
             (t->links_[AA_LINK_RIGHT]->level_ < (t->level_-1)))       \
            ||                                                         \
         (t->links_[AA_LINK_LEFT]  &&                                  \
            (t->links_[AA_LINK_LEFT]->level_  < (t->level_-1)))

/**
 *  rebalances tree
 *  return new ptr to node
 *
**/
static CntAATreeNode *aa_tree_node_rebalance( CntAATreeNode *t )
{
    if( aa_tree_node_check_levels( t ) ) {

        --t->level_;
        if( t->links_[AA_LINK_RIGHT]->level_ > t->level_ )
            t->links_[AA_LINK_RIGHT]->level_ = t->level_;

        t                        = skew ( t );
        t->links_[AA_LINK_RIGHT] = skew ( t->links_[AA_LINK_RIGHT] );
        t->links_[AA_LINK_RIGHT]
         ->links_[AA_LINK_RIGHT] = skew ( t->links_[AA_LINK_RIGHT]
                                           ->links_[AA_LINK_RIGHT] );
        t                        = split( t );
        t->links_[AA_LINK_RIGHT] = split( t->links_[AA_LINK_RIGHT] );
    }
    return t;
}

static CntAATreeNode *aa_tree_node_take_left( CntAATreeNode * cur,
                                              CntAATreeNodePtr *bup)
{
    if (NULL == cur->links_[AA_LINK_LEFT] ) {
        *bup = cur;
        return cur->links_[AA_LINK_RIGHT];
    }

    cur->links_[AA_LINK_LEFT] =
            aa_tree_node_take_left(cur->links_[AA_LINK_LEFT], bup);

    return aa_tree_node_rebalance(cur);
}

static int aa_tree_node_delete( CntAATree *aat,
                                CntAATreeNodePtr *top, const void *data )
{
    int       result =  0;
    CntAATreeNode *t = *top;

    if( t ) {

        int cmp_res = aat->traits_->compare( data, t->data_.ptr_,
                                             aat->traits_->element_size );

        if( 0 == cmp_res ) {

            /// aa_tree_node_del

            CntAATreeNode *tmp = NULL;
            if( NULL == t->links_[AA_LINK_LEFT] ) {

                tmp = t->links_[AA_LINK_RIGHT];

            } else if( NULL == t->links_[AA_LINK_RIGHT] ) {

                tmp = t->links_[AA_LINK_LEFT];

            } else {

                t->links_[AA_LINK_RIGHT] =
                        aa_tree_node_take_left(t->links_[AA_LINK_RIGHT], &tmp);

                tmp->links_[AA_LINK_LEFT]  = t->links_[AA_LINK_LEFT];
                tmp->links_[AA_LINK_RIGHT] = t->links_[AA_LINK_RIGHT];
                tmp->level_                = t->level_;
            }

            if( aat->traits_->destroy ) {
                aat->traits_->destroy( t->data_.ptr_,
                                       aat->traits_->element_size );
            }

            aat->allocator_->deallocate( t->data_.ptr_ );
            aat->allocator_->deallocate( t );

            result = 1;
            t      = tmp;

        } else {
            result = aa_tree_node_delete( aat, &t->links_[cmp_res > 0], data );
        }

        *top = t;
    }
    return result;
}

static CntAATreeNode *aa_tree_node_find( const CntAATree *aat,
                                         CntAATreeNode *node, const void *data )
{
    int cmp_res;
    CntAATreeNode *res = NULL;

    if( node ) {
        cmp_res = aat->traits_->compare( data, node->data_.ptr_,
                                         aat->traits_->element_size );
        if( cmp_res == 0 ) {
            res = node;
        } else {
            res = aa_tree_node_find( aat, node->links_[cmp_res > 0], data );
        }
    }

    return res;
}

int cnt_aa_tree_insert ( CntAATree *aat, const void *data )
{
    CntAATreeNode *inserted = NULL;
    int res = 0;
    assert( aat != NULL );

    res = aa_tree_node_insert( aat, &aat->root_, data, &inserted );
    aat->count_ += (res == 1);

    return res == 1;
}

int cnt_aa_tree_insert_update ( CntAATree *aat, const void *data )
{
    CntAATreeNode *inserted = NULL;
    int res = 0;
    assert( aat != NULL );

    res = aa_tree_node_insert( aat, &aat->root_, data, &inserted );

    aat->count_ += (res == 1);

    if( res == 2 ) {

        if( aat->traits_->destroy ) {
            aat->traits_->destroy( inserted->data_.ptr_,
                                   aat->traits_->element_size );
        }

        if( aat->traits_->copy ) {
            aat->traits_->copy( inserted->data_.ptr_, data,
                                aat->traits_->element_size );
        } else {
            aat_memcopy( inserted->data_.ptr_, data,
                         aat->traits_->element_size );
        }
    }

    return res != 0;
}


void *cnt_aa_tree_find( CntAATree *aat, const void *data )
{
    CntAATreeNode *res;

    assert( aat != NULL );

    res = aa_tree_node_find( aat, aat->root_, data );

    return res ? res->data_.ptr_ : NULL;
}

const void *cnt_aa_tree_cfind( const CntAATree *aat, const void *data )
{
    CntAATreeNode *res;

    assert( aat != NULL );

    res = aa_tree_node_find( aat, aat->root_, data );

    return res ? res->data_.ptr_ : NULL;
}


size_t cnt_aa_tree_size( const CntAATree *aat )
{
    assert( aat != NULL );
    return aat->count_;
}

int cnt_aa_tree_delete( CntAATree *aat, const void *data )
{
    int res;
    assert( aat != NULL );
    res = aa_tree_node_delete( aat, &aat->root_, data );
    aat->count_ -= ( res == 1 );
    return res;
}


