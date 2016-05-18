#include <v3_core.h>
#include <assert.h>
#include "../v3_exception.h"

static v3_int_t v3_init_prototype(v3_ctx_t *ctx);

static v3_base_object_t *TypeError(v3_ctx_t *ctx);

v3_object_t                     *TypeError_prototype;

v3_int_t v3_init_TypeError(v3_ctx_t *ctx)
{
    v3_function_object_t    *object;
    v3_int_t                rc;

    v3_obj_set_native_func(ctx, ctx->global, TypeError, 1);
    object = (v3_function_object_t *)v3_object_get(ctx->global, v3_strobj("TypeError"));
    assert(object != NULL);

    TypeError_prototype = v3_object_create(ctx, 10);
    if (TypeError_prototype == NULL) return V3_ERROR;

    rc = v3_init_prototype(ctx);
    if (rc != V3_OK) return rc;

    v3_function_set_prototype(ctx, object, TypeError_prototype);
    return rc;
}

static v3_int_t v3_init_prototype(v3_ctx_t *ctx)
{
    TypeError_prototype->base.__proto__ = Error_prototype;
    v3_obj_set(Error_prototype, v3_strobj("name"), v3_strobj( "TypeError"));
    return V3_OK;
}

static v3_base_object_t *
TypeError(v3_ctx_t *ctx)
{
    v3_object_t *error;
    error = (v3_object_t *)Error(ctx);

    v3_obj_set(ctx->frame->this, v3_strobj("prototype"), TypeError_prototype);
    return (v3_base_object_t *)error;
}
