#ifndef CNT_COMMON_H
#define CNT_COMMON_H


#ifdef _WIN32

#if _MSC_VER >= 1600

// Visual Studio 2010+
#include <stdint.h>

#else

typedef signed      char   int8_t;
typedef unsigned    char  uint8_t;
typedef signed     short  int16_t;
typedef unsigned   short uint16_t;
typedef signed   __int32  int32_t;
typedef unsigned __int32 uint32_t;
typedef signed   __int64  int64_t;
typedef unsigned __int64 uint64_t;

#endif

#else

#include <stdint.h>

#endif

/**
 * tdb_hash(): based on the hash agorithm from gdbm, via tdb
 * (from module-init-tools)
**/
static unsigned int tdb_hash( const void *begin, size_t length, unsigned init )
{
    unsigned value;
    unsigned i;

    const unsigned char *data = (const unsigned char *)begin;

    for (value = init * length, i=0; length != 0; ++i, --length)
        value = (value + (data[i] << (i*5 % 24)));

    return (1103515243 * value + 12345);
}

static unsigned int tdb_hash_default( const void *begin, size_t length )
{
    return tdb_hash( begin, length, 0x238F13AF );
}

#define FIELD_OFFSET( type, field ) \
    ((unsigned long)( &((const type *)0)->field ))

#define CONTAINER_OF( ptr, type, field ) \
    ((type *)((const char *)(ptr) - FIELD_OFFSET(type, field)))

#endif // CNTCOMMON_H
