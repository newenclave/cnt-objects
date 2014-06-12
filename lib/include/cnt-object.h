#ifndef CNT_OBJECT_H
#define CNT_OBJECT_H

struct CntObject;

typedef struct CntType {

    void (*destroy)(struct CntObject);

} CntType;

#endif // CNTOBJECT_H
