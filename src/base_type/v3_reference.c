#include <v3_core.h>
#include <v3_reference.h>
#include "../v3_exception.h"

void v3_ref_init(v3_reference_t *ref)
{
    ref->base.type = V3_DATA_TYPE_REF;
    ref->scope = (v3_base_object_t *)&v3_null;
    ref->name = NULL;
}

v3_reference_t *v3_ref_create(v3_ctx_t *ctx, v3_string_object_t *name, v3_base_object_t *scope) 
{
    v3_reference_t  *ref;
    ref = v3_palloc(ctx->pool, sizeof(*ref));
    if (ref == NULL) return ref;

    v3_ref_init(ref);

    ref->scope = scope;
    ref->name = name;

    return ref;
}

v3_base_object_t *v3_ref_get_value(v3_ctx_t *ctx, v3_base_object_t *aref)
{
    v3_base_object_t    *scope;
    v3_reference_t      *ref;

    if (aref->type != V3_DATA_TYPE_REF) return aref;

    ref = (v3_reference_t *)aref;
    scope = ref->scope;

    if (scope == (v3_base_object_t *)&v3_null) {
        // return ReferenceError(ctx);
        v3_set_error(ctx, v3_ReferenceError, "reference error");
        return NULL;
    }
    
    return v3_find_property(scope, ref->name);
}

v3_base_object_t *v3_ref_put_value(v3_ctx_t *ctx, v3_base_object_t *aref, v3_base_object_t *value)
{
    v3_base_object_t    *scope;
    v3_reference_t      *ref;

    if (aref->type != V3_DATA_TYPE_REF) {
        v3_set_error(ctx, v3_ReferenceError, "put value expect ref");
        return NULL;
    }

    ref = (v3_reference_t *)aref;
    scope = ref->scope;

    if (scope == (v3_base_object_t *)&v3_null) {
        v3_object_set(ctx->global, ref->name, value);
    } else {
        v3_object_set(to_obj(scope), ref->name, value);
    }

    return value;
}

