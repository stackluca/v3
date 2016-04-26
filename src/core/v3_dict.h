#ifndef _V3_DICT_H
#define _V3_DICT_H 
#include <v3_core.h>

typedef struct v3_dict_slot_s v3_dict_slot_t;

struct v3_dict_slot_s {
    v3_str_t            key;
    void                *value;
    v3_dict_slot_t      *next;
};

typedef struct {
    v3_dict_slot_t      *head; 
    size_t              len;
} v3_dict_bucket_t;

struct v3_dict_s{
    v3_dict_bucket_t    *buckets;
    size_t              capacity;
    v3_pool_t           *pool;
#ifdef V3_DEBUG
    v3_int_t            count;
#endif
};

extern v3_dict_t *v3_dict_create(v3_pool_t *pool, size_t capacity);
// extern void v3_dict_t *v3_dict_free(v3_pool_t *pool, size_t capacity);
extern v3_int_t v3_dict_set(v3_dict_t *dict, v3_str_t *key, void *value);
extern void *v3_dict_get(v3_dict_t *dict, v3_str_t *key);
#endif
