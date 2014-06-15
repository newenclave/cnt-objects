#ifndef CNT_ELEMENT_TRAITS_H
#define CNT_ELEMENT_TRAITS_H

#include <stdlib.h>

typedef struct CntElementTraits {

    size_t     element_size;

    /// destroy element
    void       (* destroy)( void * );

    /// copy element to another place;
    /// returns ptr to destination (1st parameter)
    void      *(* copy)( void *, const void *, size_t );

} CntElementTraits;

#define CNT_ELEMENT_COPY( trait, dst, src )                 \
        (trait)->copy( (dst), (src), (trait)->element_size )

#define CNT_ELEMENT_DESTROY( trait, src )                   \
        (trait)->destroy( src )

#endif // CNTELEMENTTRAITS_H
