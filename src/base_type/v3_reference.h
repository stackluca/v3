#ifndef _V3_REFERENCE_TYPE_H
#define _V3_REFERENCE_TYPE_H 1
#include <v3_core.h>
#include <v3_base_type.h>

typedef struct {
    v3_base_object_t    base;
    v3_base_object_t    *scope;
    v3_string_object_t  *name;
} v3_reference_t;

#define v3_ref(ctx, name, value) \
    ctx->ret->name = name; \
    ctx->ret->value = (v3_base_object_t *)value; \

#define v3_ref_base(ref) ((v3_base_object_t *)(ref)->value)

v3_reference_t *v3_ref_create(v3_ctx_t *ctx, const char *name, size_t len, v3_base_object_t *value);

#endif
