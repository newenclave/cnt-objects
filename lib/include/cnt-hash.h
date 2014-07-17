#ifndef CNT_HASH_H
#define CNT_HASH_H

/**
 * tdb_hash(): based on the hash agorithm from gdbm, via tdb
 * (from module-init-tools)
**/
static unsigned int tdb_hash( const void *begin, size_t length, unsigned init )
{
    unsigned value;
    unsigned i;

    const unsigned char *data = (const unsigned char *)begin;

    for( value = init * length, i=0; length != 0; ++i, --length) {
        value = (value + (data[i] << (i*5 % 24)));
    }

    return (1103515243 * value + 12345);
}

static unsigned int tdb_hash_update( const void *begin,
                                     size_t length, unsigned init )
{
    unsigned value;
    unsigned i;

    const unsigned char *data = (const unsigned char *)begin;

    for (value = init * length, i=0; length != 0; ++i, --length) {
        value = (value + (data[i] << (i*5 % 24)));
    }

    return value;
}

static unsigned int tdb_hash_start( const void *begin, size_t length)
{
    return tdb_hash_update( begin, length, 0x238F13AF );
}

static unsigned int tdb_hash_final( unsigned value )
{
    return (1103515243 * value + 12345);
}

static unsigned int tdb_hash_default( const void *begin, size_t length )
{
    return tdb_hash( begin, length, 0x238F13AF );
}

#endif // CNTHASH_H
