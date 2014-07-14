#include <assert.h>
#include <string.h>

#include "cnt-deque-impl.h"

#define DEQUE_DEFAULT_INIT_ELEMENTS 8

#define DEQUE_ELEMENT_NEXT( ptr, element_size )         \
            (((char *)ptr) + (element_size))

#define DEQUE_ELEMENT_PREV( ptr, element_size )         \
            (((char *)ptr) - (element_size))

#define DEQUE_ELEMENT_AT( ptr, element_size, position ) \
            (((char *)ptr) + ((element_size) * (position)))

#define DEQUE_BLOCK_SIDE( unit, side ) ((unit)->border_[!(side)])

#define DEQUE_BLOCK_IS_SIDE( unit, ptr, side )          \
        DEQUE_BLOCK_SIDE(unit, side) == (ptr)

#define DEQUE_IS_BORDER( deq, side )                    \
        ((deq)->sides_[side].ptr_ ==                    \
         (deq)->sides_[side].unit_->border_[side] )

#define DEQUE_PTR_DIFF( l, r ) ((const char *)(l) - (const char *)(r))

//#define DEQUE_DEF_INC(size) (size)
#define DEQUE_DEF_INC(size) ((size) + ((size) >> 1))

#define DEQUE_UNIT_SIZE( unit )                           \
        DEQUE_PTR_DIFF ((unit)->border_[DEQ_SIDE_BACK],   \
                      (unit)->border_[DEQ_SIDE_FRONT])


static void *deq_memcopy( void *dst, const void *src, size_t size )
{
    return memcpy( dst, src, size );
}

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
 * sets up unit's pointer to the desired position
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

        cnd->traits_->destroy( begin, element_size );
        begin = DEQUE_ELEMENT_NEXT(begin, element_size);

    }
    return (begin != end);
}

static void deque_list_free( CntDequeImpl *cnd )
{
    CntDequeUnit       *unit       = NULL;
    CntDequeSide       *front_side = &cnd->sides_[DEQ_SIDE_FRONT];
    CntDLinkedListHead *head       = &front_side->unit_->list_;

    // free elements
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
    CntDequeUnit *unit;

    deq->traits_    = traits;
    deq->allocator_ = allocator;
    deq->count_     = 0;

    unit = deque_unit_create( deq, init_size ? init_size + 1
                                             : DEQUE_DEFAULT_INIT_ELEMENTS );

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

int cnt_deque_impl_empty( const CntDequeImpl *deq )
{
    assert( deq != NULL );
    return (deq->count_ == 0) ? 1 : 0;
}

size_t cnt_deque_impl_size ( const CntDequeImpl *deq )
{
    assert( deq != NULL );
    return deq->count_;
}

void *cnt_deque_impl_front(CntDequeImpl *deq )
{
    assert( deq != NULL );
    assert( deq->count_ != 0 );

    return deq->sides_[DEQ_SIDE_FRONT].ptr_;
}

void *cnt_deque_impl_back (CntDequeImpl *deq )
{
    void *ptr = NULL;

    assert( deq != NULL );
    assert( deq->count_ != 0 );

    ptr = (char *)(deq->sides_[DEQ_SIDE_BACK].ptr_) -
                   deq->traits_->element_size;
    return ptr;
}

const void *cnt_deque_impl_cfront(const CntDequeImpl *deq )
{
    assert( deq != NULL );
    assert( deq->count_ != 0 );

    return deq->sides_[DEQ_SIDE_FRONT].ptr_;
}

const void *cnt_deque_impl_cback (const CntDequeImpl *deq )
{
    const void *ptr = NULL;

    assert( deq != NULL );
    assert( deq->count_ != 0 );

    ptr = (const char *)(deq->sides_[DEQ_SIDE_BACK].ptr_) -
                         deq->traits_->element_size;
    return ptr;
}


static int deque_extend_side( CntDequeImpl *deq, int dir )
{
    CntDequeSide *side       = &deq->sides_[dir];
    CntDequeUnit *new_unit   =
            CONTAINER_OF( CNT_DLINKED_LIST_STEP( &side->unit_->list_, dir ),
                          CntDequeUnit, list_);
    int const deq_empty      = (deq->sides_[0].ptr_ == deq->sides_[1].ptr_);

    if( !new_unit ) {

        CntDLinkedListHead *other_list =
                CNT_DLINKED_LIST_STEP( &deq->sides_[!dir].unit_->list_, !dir );

        if( other_list ) {

            new_unit = CONTAINER_OF( other_list, CntDequeUnit, list_ );
            CNT_DLINKED_LIST_REMOVE( other_list );
            other_list->links_[0] = other_list->links_[1] = NULL;

        } else {

            const size_t old_size = DEQUE_UNIT_SIZE(side->unit_)
                                  / deq->traits_->element_size;

            const size_t new_size = DEQUE_DEF_INC(old_size);
            new_unit = deque_unit_create( deq, new_size );
        }
        if( new_unit ) {
            CNT_DLINKED_LIST_INSERT( &side->unit_->list_,
                                     &new_unit->list_, dir );
        }
    }

    if( new_unit ) {
        side->unit_ = new_unit;
        side->ptr_  = DEQUE_BLOCK_SIDE( new_unit, dir );
        if( deq_empty ) {
            deq->sides_[!dir].ptr_  = side->unit_->border_[!dir];
            deq->sides_[!dir].unit_ = side->unit_;
        }
    }

    return new_unit != NULL;
}

static void *cnt_dec_ptr_call( void **ptr, size_t size )
{
    char *tmp = (char *)*ptr;
    tmp  -= size;
    *ptr  = tmp;
    return tmp;
}

static void *cnt_inc_ptr_call( void **ptr, size_t size )
{
    char *tmp = (char *)*ptr;
    *ptr = tmp + size;
    return tmp;
}

static void *deque_create_at( CntDequeImpl *deq, int dir )
{
    typedef void *(* fix_ptr_call)( void **, size_t );

    void *new_ptr = NULL;
    int res       = 1;

    static const fix_ptr_call ptr_calls[2] = {
        cnt_dec_ptr_call, cnt_inc_ptr_call
    };

    if( DEQUE_IS_BORDER( deq, dir ) ) {
        res = deque_extend_side( deq, dir );
    }

    if( res ) {
        CntDequeSide *side = &deq->sides_[dir];
        new_ptr = ptr_calls[dir]( &side->ptr_, deq->traits_->element_size );
        if( deq->traits_->init ) {
            deq->traits_->init( new_ptr, 1, deq->traits_->element_size );
        }
        ++deq->count_;
    }
    return new_ptr;
}

void *cnt_deque_impl_create_front( CntDequeImpl *deq )
{
    assert( deq != NULL );
    return deque_create_at( deq, DEQ_SIDE_FRONT );
}

void *cnt_deque_impl_create_back( CntDequeImpl *deq )
{
    assert( deq != NULL );
    return deque_create_at( deq, DEQ_SIDE_BACK );
}

static void deque_pop( CntDequeImpl *deq, int dir )
{
    CntDequeSide *side = &deq->sides_[dir];

    void *old_and_new_side[2];
    void *new_side;
    size_t const elem_size = deq->traits_->element_size;

    old_and_new_side[0] = side->ptr_;
    old_and_new_side[1] = new_side =
            dir ? DEQUE_ELEMENT_PREV( side->ptr_, elem_size )
                : DEQUE_ELEMENT_NEXT( side->ptr_, elem_size );

    if( deq->traits_->destroy ) {
        deq->traits_->destroy( old_and_new_side[dir],
                               deq->traits_->element_size );
    }

    if( DEQUE_BLOCK_IS_SIDE(side->unit_, new_side, dir)
            && new_side != deq->sides_[!dir].ptr_ )
    {

        CntDLinkedListHead  *old_list = &side->unit_->list_;
        CntDLinkedListHead  *next_unit =
               CNT_DLINKED_LIST_STEP(&side->unit_->list_, !dir);

        side->unit_ = CONTAINER_OF(next_unit, CntDequeUnit, list_);
        new_side    = DEQUE_BLOCK_SIDE(side->unit_, !dir);

        if( NULL != (old_list = old_list->links_[dir]) ) {
            CntDequeUnit *old_un = CONTAINER_OF(old_list, CntDequeUnit, list_);

            CNT_DLINKED_LIST_REMOVE( old_list );

            deq->allocator_->deallocate( old_un->border_[DEQ_SIDE_FRONT] );
            deq->allocator_->deallocate( old_un );
        }

    }

    side->ptr_ = new_side;
    --deq->count_;
}

void cnt_deque_impl_pop_front( CntDequeImpl *deq )
{
    deque_pop( deq, DEQ_SIDE_FRONT );
}

void cnt_deque_impl_pop_back( CntDequeImpl *deq )
{
    deque_pop( deq, DEQ_SIDE_BACK );
}

static int deque_push( CntDequeImpl *deq, const void *element, int dir )
{
    void *new_ptr = deque_create_at( deq, dir );
    if( new_ptr ) {
        if( deq->traits_->copy ) {
            deq->traits_->copy( new_ptr, element, deq->traits_->element_size );
        } else {
            deq_memcopy( new_ptr, element, deq->traits_->element_size );
        }
    }
    return ( new_ptr != NULL );
}

int cnt_deque_impl_push_front( CntDequeImpl *deq, const void *element )
{
    return deque_push( deq, element, DEQ_SIDE_FRONT );
}

int cnt_deque_impl_push_back( CntDequeImpl *deq, const void *element )
{
    return deque_push( deq, element, DEQ_SIDE_BACK );
}
