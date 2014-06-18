#ifndef CNT_ELEMENT_TRAITS_H
#define CNT_ELEMENT_TRAITS_H

#include <stdlib.h>

typedef struct CntElementTraits {

    const size_t element_size;

    /**
     *  init n elements
     *  init( src, n, element_size );
    **/
    void (* init)( void *, size_t, size_t );

    /**
     *  free element's data. but NOT element pointer
     *  destroy( src, element_size )
    **/
    void (* destroy)( void *, size_t );

    /**
     *   copy element to another place;
     *   returns: dst
     *   copy( dst, src, element_size )
    **/
    void *(* copy)( void *, const void *, size_t );

    /**
     *  returns:
     *      <0 left  < right
     *       0 left == right
     *      >0 left  > right
     *   compare( left, right, element_size )
    **/
    int (* compare)( const void *, const void *, size_t );

    /**
     *   swaps 2 elements
     *   swap( left, right, element_size )
    **/
    void (* swap)( void *, void *, size_t );

} CntElementTraits;

#define CNT_ELEMENT_COPY( trait, dst, src )                     \
        (trait)->copy( (dst), (src), (trait)->element_size )

#define CNT_ELEMENT_DESTROY( trait, src )                       \
        (trait)->destroy( src, (trait)->element_size )

#define CNT_ELEMENT_INIT( trait, src )                          \
        (trait)->init( src, 1, (trait)->element_size )

#define CNT_ELEMENTS_INIT( trait, src, count )                   \
        (trait)->init( src, count, (trait)->element_size )

#define CNT_EMPTY_ELEMENT_TRAITS( size ) { size }

#endif // CNTELEMENTTRAITS_H
