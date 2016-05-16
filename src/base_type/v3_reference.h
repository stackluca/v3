#ifndef _V3_REFERENCE_TYPE_H
#define _V3_REFERENCE_TYPE_H 1
#include <v3_core.h>
#include <v3_base_type.h>

typedef struct {
    v3_base_object_t    base;
    v3_base_object_t    *scope;
    v3_string_object_t  *name;
} v3_reference_t;

v3_reference_t *v3_ref_create(v3_ctx_t *ctx, v3_string_object_t *name, v3_base_object_t *scope);
v3_base_object_t *v3_ref_get_value(v3_ctx_t *ctx, v3_base_object_t *aref);
v3_base_object_t *v3_ref_put_value(v3_ctx_t *ctx, v3_base_object_t *aref, v3_base_object_t *value);
#endif
