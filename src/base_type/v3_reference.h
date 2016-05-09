#ifndef _V3_REFERENCE_TYPE_H
#define _V3_REFERENCE_TYPE_H 1

typedef struct {
    v3_base_object_t    *value;
    v3_str_t            name;
} v3_reference_t;

#define v3_ref(ctx, name, value) \
    ctx->ret->name = name; \
    ctx->ret->value = (v3_base_object_t *)value; \

#endif
