#ifndef _V3_LIST_H
#define _V3_LIST_H 
#include <v3_core.h>

typedef struct v3_list_part_s v3_list_part_t;
struct v3_list_part_s {
    void            *value;
    v3_list_part_t  *next;
};

struct v3_list_s {
    v3_list_part_t      *elts;
    size_t              nelts;
    v3_pool_t           *pool;
};

v3_list_t *v3_list_create(v3_pool_t *pool);
v3_int_t v3_list_prepend(v3_list_t *list, void *value);
v3_list_t *v3_list_clone(v3_ctx_t *ctx, v3_list_t *src);
void v3_list_pop(v3_list_t *list);
#endif
