#include <string.h>

#include "cnt-heap-impl.h"


#define ARRPTR( hp )        (&(hp)->array_)
#define ARRALOCATOR( hp )   (ARRPTR( hp )->mblock_.allocator_)
#define ARRTRAITS( hp )     (ARRPTR( hp )->traits_)


/**
 *  sift action
**/

static int hp_memcmp( const void *l, const void *r, size_t length )
{
    return memcmp( l, r, length );
}

static void hp_memswap( void *l, void *r, size_t length )
{
    char *lc = (char *)l;
    char *rc = (char *)r;
    while( length-- ) {
        char
          t = *lc;
        *lc = *rc;
        *rc = t;
    }
}

static void swap_elements( void *l, void *r, size_t length,
                           void (*swap)( void *, void *, size_t ))
{
    if( swap ) {
        swap( l, r, length );
    } else {
        hp_memswap( l, r, length );
    }
}

/**
 *
siftdown( array heap )
{
    heap_size = len(heap);
    next = 1;
    while( (next * 2) <= heap_size ) {
        minimum = (next * 2) - 1;
        if( minimum + 1 < heap_size ) {
            minimum = heap[minimum] < heap[minimum+1]
                      ? minimum : minimum + 1;
        }
        if( heap[minimum] < heap[next-1] )
            swap( heap[next-1], heap[minimum] );
        else
            break;
        next = minimum+1;
    }
}
*
**/

static void sift_down( CntHeapImpl *hp, const size_t heap_size,
                       int (* compare)( const void *, const void *, size_t ))
{
    CntArrayImpl *arr         = ARRPTR(hp);
    const size_t element_size = ARRTRAITS(hp)->element_size;

    size_t next = 1;

    while( next <= (heap_size >> 1) ) {

        const size_t children = next << 1;
        size_t minimum = children - 1;
        void   *src_elem;
        void   *min_elem;
        int     cmp;

        if( children < heap_size ) {
            const int cmp_children =
                    compare( cnt_array_impl_at( arr, minimum + 1 ),
                             cnt_array_impl_at( arr, minimum ),
                             element_size);

            if( cmp_children < 0) {
                minimum++;
            }
        }

        src_elem = cnt_array_impl_at( arr, next-1  );
        min_elem = cnt_array_impl_at( arr, minimum );

        cmp = compare( min_elem, src_elem, element_size);

        if( cmp < 0 ) {
            swap_elements( src_elem, min_elem, element_size,
                           ARRTRAITS(hp)->swap );

            next = minimum + 1;
        } else {
            next = heap_size; // break;
        }
    }
}

/**
siftup( array heap )
{
    heap_size = len(heap);
    while( heap_size / 2 ) {
        parent = (heap_size * 2);
        if( heap[heap_size - 1] < heap[parent - 1] ) {
            swap( heap[heap_size - 1], heap[parent - 1] );
            heap_size = parent;
        } else {
            break;
        }
    }
}
**/

static void sift_up( CntHeapImpl *hp, size_t heap_size,
                     int (* compare)( const void *, const void *, size_t ))
{
    CntArrayImpl *arr         = ARRPTR(hp);
    const size_t element_size = ARRTRAITS(hp)->element_size;

    size_t parent_index = (heap_size >> 1);

    while( parent_index ) {

        void *current = cnt_array_impl_at( arr, heap_size - 1 );
        void *parent  = cnt_array_impl_at( arr, parent_index - 1 );

        const int cmp_res = compare( current, parent, element_size );

        if( cmp_res < 0 ) {
            swap_elements( current, parent, element_size,
                           ARRTRAITS(hp)->swap );
            heap_size = parent_index;
            parent_index >>= 1;
        } else {
            parent_index = 0; // break
        }
    }
}

//=====================//

CntHeapImpl *cnt_heap_impl_new( const CntElementTraits *traits,
                                const CntAllocator *allocator )
{
    CntHeapImpl *inst;
    assert( traits != NULL );

    assert( allocator != NULL );
    assert( allocator->allocate != NULL );
    assert( allocator->deallocate != NULL );

    inst = (CntHeapImpl *)allocator->allocate( sizeof(*inst) );
    if(inst) {
        cnt_array_impl_init( ARRPTR(inst), traits, allocator );
    }

    return inst;
}

void cnt_heap_impl_free( CntHeapImpl *hp )
{
    assert( hp != NULL );

    void (*dealloc)(void *) = ARRALOCATOR(hp)->deallocate;

    cnt_array_impl_deinit( ARRPTR(hp) );

    dealloc( hp );
}


