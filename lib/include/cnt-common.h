#ifndef CNT_COMMON_H
#define CNT_COMMON_H

#define FIELD_OFFSET( type, field ) \
    ((unsigned long)( &((const type *)0)->field ))

#define CONTAINER_OF( ptr, type, field ) \
    ((type *)((const char *)(ptr) - FIELD_OFFSET(type, field)))

#endif // CNTCOMMON_H
