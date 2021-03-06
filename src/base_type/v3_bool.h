#ifndef _V3_BOOL_TYPE_H
#define _V3_BOOL_TYPE_H 1
#include <v3_core.h>
typedef struct {
    v3_base_object_t        base;
    short                   value;
} v3_bool_object_t;

extern v3_bool_object_t v3_bool_true;
extern v3_bool_object_t v3_bool_false;

#define to_bool(value) ((v3_bool_object_t *)(value))
#endif
