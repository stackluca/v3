#include <v3_core.h>
#include <assert.h>

v3_object_t             *Object_prototype;

static v3_int_t v3_init_object_prototype(v3_ctx_t *ctx);
static v3_base_object_t * Object(v3_ctx_t *ctx);
static v3_base_object_t *toString(v3_ctx_t *ctx);

v3_int_t v3_init_Object(v3_ctx_t *ctx)
{
    v3_function_object_t    *object;
    v3_int_t                rc;
#if 0
    Object = v3_function_create_native(ctx, v3_strobj("Object"), 
                                    v3_numobj(1), v3_Object_new);

    if (Object == NULL) return V3_ERROR;

    Object.is_native = 1;
    Object.func = v3_Object_new;
#endif

    // v3_object_set(ctx, ctx->global, v3_strobj("Object"), Object);

    v3_obj_set_native_func(ctx, ctx->global, Object, 1);
    object = (v3_function_object_t *)v3_object_get(ctx->global, v3_strobj("Object"));
    assert(object != NULL);

    Object_prototype = v3_object_create(ctx, 10);
    if (Object_prototype == NULL) return V3_ERROR;

    rc = v3_init_object_prototype(ctx);
    if (rc != V3_OK) return rc;

    v3_function_set_prototype(ctx, object, Object_prototype);
    return rc;
}

static v3_int_t v3_init_object_prototype(v3_ctx_t *ctx)
{
    Object_prototype->base.__proto__ = NULL;
    v3_obj_set_native_func(ctx, Object_prototype, toString, 1);

    return V3_OK;
}

static v3_base_object_t *
Object(v3_ctx_t *ctx)
{
    v3_obj_set(ctx->frame->this, v3_strobj("prototype"), Object_prototype);
    return ctx->frame->this;
}

static v3_base_object_t 
*toString(v3_ctx_t *ctx)
{
    return (v3_base_object_t *)v3_strobj("Object");
}
