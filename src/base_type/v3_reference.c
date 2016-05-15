#include <v3_core.h>

void v3_ref_init(v3_reference_t *ref)
{
    ref->base.type = V3_DATA_TYPE_REF;
    ref->value = &v3_null;
    ref->name.data = "";
    ref->name.length = 0;
}

v3_reference_t *v3_ref_create(v3_ctx_t *ctx, const char *name, size_t len, v3_base_object_t *value) 
{
    v3_reference_t  *ref;
    ref = v3_palloc(ctx->pool, sizeof(*ref));
    if (ref == NULL) return ref;

    v3_ref_init(ref);

    ref->value = value;
    ref->name.data = name;
    ref->name.length = len;

    return ref;
}

v3_base_object_t *v3_ref_get_value(v3_ctx_t *ctx, v3_base_object_t *aref)
{
    v3_base_object_t    *value;
    v3_reference_t      *ref;

    if (ref->base.type != V3_DATA_TYPE_REF) return ref;

    ref = (v3_reference_t *)aref;
    value = ref->value;
    if (value == &v3_null) return ReferenceError(ctx);
    
}

