#ifndef _V3_POOL_H
#define _V3_POOL_H 

#include <v3_core.h>

struct v3_pool_s {
    
    struct v3_pool_s *next;
};

extern void *v3_palloc(v3_pool_t *pool, size_t size);
extern void v3_pfree(v3_pool_t *pool, void *value);

extern v3_pool_t *v3_pool_create(size_t size);
extern void v3_pool_destory(v3_pool_t *pool);
#endif // _V3_POOL_H 
