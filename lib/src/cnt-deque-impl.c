#include <assert.h>

#include "cnt-deque-impl.h"

#define DEQUE_ELEMENT_NEXT( ptr, element_size )         \
            (((char *)ptr) + (element_size))

#define DEQUE_ELEMENT_PREV( ptr, element_size )         \
            (((char *)ptr) - (element_size))

#define DEQUE_ELEMENT_AT( ptr, element_size, position ) \
            (((char *)ptr) + ((element_size) * (position)))

/**
 * creates new unit for deque
**/
static CntDequeUnit *deque_unit_create( CntDequeImpl* cnd, size_t elements )
{
    CntDequeUnit *new_unit =
            (CntDequeUnit *)cnd->allocator_->allocate(sizeof(*new_unit));

    if( new_unit ) {

        size_t const length = elements * cnd->traits_->element_size;
        void *new_border = cnd->allocator_->allocate( length );

        if( !new_border ) {
            cnd->allocator_->deallocate( new_unit );
            new_unit = NULL;
        } else {
            new_unit->border_[DEQ_SIDE_FRONT] = new_border;
            new_unit->border_[DEQ_SIDE_BACK] = ((char *)new_border) + length;
            new_unit->list_.links_[0] = new_unit->list_.links_[1] = NULL;
        }
    }
    return new_unit;
}

/**
 * sets up unit's pointer to desired position
**/
static void deque_init_position( CntDequeImpl *cnd, size_t reserve,
                                 enum cnt_deque_start_point position)
{
    void *ptr_new = NULL;
    void *front = cnd->sides_[DEQ_SIDE_FRONT].unit_->border_[DEQ_SIDE_FRONT];
    void *back  = cnd->sides_[DEQ_SIDE_FRONT].unit_->border_[DEQ_SIDE_BACK];

    size_t const size = cnd->traits_->element_size;

    switch( position ) {
    case DEQ_START_TOP:
        ptr_new = DEQUE_ELEMENT_NEXT(front, size);
        break;
    case DEQ_START_BOTTOM:
        ptr_new = DEQUE_ELEMENT_PREV(back, size);
        break;
    default:
        ptr_new = DEQUE_ELEMENT_AT(front, size, reserve >> 1);
        break;
    }
    cnd->sides_[DEQ_SIDE_FRONT].ptr_ =
        cnd->sides_[DEQ_SIDE_BACK].ptr_ =  ptr_new;
}

/**
 *  returns:
 *   0 - if end of elements reached
 *  !0 - otherwise
**/
static int deque_unit_free( CntDequeImpl *cnd,
                            CntDequeUnit *unit,
                            void *begin, const void *end )
{
    const void  *block_end     = unit->border_[DEQ_SIDE_BACK];
    size_t const element_size  = cnd->traits_->element_size;

    while( begin != end && begin != block_end ) {

        cnd->traits_->destroy( begin, cnd->traits_->element_size );
        begin = DEQUE_ELEMENT_NEXT(begin, element_size);

    }
    return (begin != end);
}

static void deque_list_free( CntDequeImpl *cnd )
{
    // free elements
    CntDequeUnit       *unit       = NULL;
    CntDequeSide       *front_side = &cnd->sides_[DEQ_SIDE_FRONT];
    CntDLinkedListHead *head       = &front_side->unit_->list_;

    if( cnd->traits_->destroy ) {
        void *begin     = NULL;
        const void *end = cnd->sides_[DEQ_SIDE_BACK].ptr_;

        while( head ) {

            unit = CONTAINER_OF( head, CntDequeUnit, list_ );
            begin = begin ? unit->border_[0] : front_side->ptr_;

            head = CNT_DLINKED_LIST_NEXT( head );
            if( !deque_unit_free( cnd, unit, begin, end ) ) {
                head = NULL;
            }
        }
    }

    // free units
    head = &front_side->unit_->list_;
    while( head->links_[0] ) head = head->links_[0];
    while( head ) {
        unit = CONTAINER_OF( head, CntDequeUnit, list_ );
        head = CNT_DLINKED_LIST_NEXT( head );
        cnd->allocator_->deallocate( unit->border_[DEQ_SIDE_FRONT] );
        cnd->allocator_->deallocate( unit );
    }
}

static int deque_init( CntDequeImpl *deq,
                       const CntElementTraits *traits,
                       const CntAllocator *allocator,
                       size_t init_size,
                       enum cnt_deque_start_point position)
{
    int res = 0;

    deq->traits_    = traits;
    deq->allocator_ = allocator;
    deq->count_     = 0;

    CntDequeUnit *unit = deque_unit_create( deq,
                                            init_size ? init_size + 1 : 4);
    if( unit ) {

        deq->sides_[DEQ_SIDE_FRONT].unit_ =
            deq->sides_[DEQ_SIDE_BACK].unit_ = unit;

        deque_init_position( deq, init_size + 1, position );

        res = 1;
    }

    return res;
}

static CntDequeImpl *deque_impl_new( const CntElementTraits *traits,
                                     const CntAllocator *allocator,
                                     size_t init_size,
                                     enum cnt_deque_start_point position)
{
    CntDequeImpl *inst;

    assert( traits != NULL );
    assert( allocator != NULL );

    inst = (CntDequeImpl *)allocator->allocate( sizeof(*inst) );

    if( inst && !deque_init( inst, traits, allocator, init_size, position ) ) {
        allocator->deallocate( inst );
        inst = NULL;
    }

    return inst;
}

CntDequeImpl *cnt_deque_impl_new( const CntElementTraits *traits,
                                  const CntAllocator *allocator )
{
    return deque_impl_new( traits, allocator, 8, DEQ_START_MIDDLE );
}

int cnt_deque_impl_init( CntDequeImpl *deq,
                         const CntElementTraits *traits,
                         const CntAllocator *allocator )
{
    assert( deq != NULL );
    return deque_init( deq, traits, allocator, 8, DEQ_START_MIDDLE );
}

void cnt_deque_impl_free( CntDequeImpl *deq )
{
    assert( deq != NULL );
    deque_list_free( deq );
    deq->allocator_->deallocate( deq );
}

void cnt_deque_impl_deinit( CntDequeImpl *deq )
{
    assert( deq != NULL );
    deque_list_free( deq );
}

