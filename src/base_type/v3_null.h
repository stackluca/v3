#ifndef _V3_NULL_TYPE_H
#define _V3_NULL_TYPE_H 1
#include <v3_core.h>

typedef struct {
    v3_base_object_t base;
} v3_null_t;

extern v3_null_t v3_null;
#define v3_object_is_null(obj) (v3_base_object_t *)obj == (v3_base_object_t *)&v3_null
#endif
