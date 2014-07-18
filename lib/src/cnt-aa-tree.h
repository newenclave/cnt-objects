#ifndef CNT_AA_TREE_H
#define CNT_AA_TREE_H

#include <stdlib.h>

#include "include/cnt-element-traits.h"
#include "include/cnt-allocator.h"

enum aa_tree_link_position {
     AA_LINK_LEFT   = 0
    ,AA_LINK_RIGHT  = 1
};

typedef struct CntAATreeNode {
    struct CntAATreeNode    *links_[2];
    union  node_data {
        void    *ptr_;
        char    *str_;
        size_t   number_;
    }                        data_;
    size_t                   level_;
} CntAATreeNode;

typedef CntAATreeNode *CntAATreeNodePtr;

typedef struct CntAATree {
    const CntAllocator     *allocator_;
    const CntElementTraits *traits_;
    CntAATreeNode          *root_;
    size_t                  count_;
} CntAATree;


CntAATree *cnt_aa_tree_new( const CntElementTraits *traits,
                            const CntAllocator *allocator );

int cnt_aa_tree_init( CntAATree *aat,
                      const CntElementTraits *traits,
                      const CntAllocator *allocator );

void cnt_aa_tree_free( CntAATree *aat );

/**
 *  Destroy elements
**/
void cnt_aa_tree_deinit( CntAATree *aat );

/**
 *  1 = success
 *  0 = failed ( alloc failed or element already esists)
**/
int cnt_aa_tree_insert ( CntAATree *aat, const void *data );

/**
 *  1 = success inserted or updated
 *  0 = failed ( memory alloc failed )
**/
int cnt_aa_tree_insert_update ( CntAATree *aat, const void *data );


void       *cnt_aa_tree_find (       CntAATree *aat, const void *data );
const void *cnt_aa_tree_cfind( const CntAATree *aat, const void *data );


size_t cnt_aa_tree_size( const CntAATree *aat );

/**
 *  1 = removed
 *  0 = not found
**/
int cnt_aa_tree_delete( CntAATree *aat, const void *data );


#endif // CNT_AA_TREE_H
