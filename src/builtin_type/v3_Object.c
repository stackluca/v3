#include <v3_core.h>

v3_object_t             *Object_prototype;
v3_function_object_t    *Object;

v3_int_t v3_init_Object(v3_ctx_t *ctx)
{
    Object = v3_function_create(ctx, v3_strobj("Object"), 
                                    v3_numobj(1), v3_Object_new);

    if (Object == NULL) return V3_ERROR;

    Object.is_native = 1;
    Object.func = v3_Object_new;

    v3_object_set(ctx, ctx->global, v3_strobj("Object"), Object);

    Object_prototype = v3_object_create(ctx, 10);
    if (Object_prototype == NULL) return V3_ERROR;

    rc = v3_init_object_prototype(ctx);
    if (rc != V3_OK) return rc;

    v3_set_function_prototype(ctx, Object, Object_prototype);
}

static v3_int_t v3_init_object_prototype(v3_ctx_t *ctx)
{
    v3_function_object_t    *toString;
    toString = v3_function_object_create(ctx, v3_strobj("toString"), 1,
                                        v3_Object_toString);
                                        
    if (toString == NULL) return V3_ERROR;

    Object_prototype->__proto__ = NULL;
    v3_object_set(ctx, Object_prototype, v3_strobj("toString"), toString);

    return V3_OK;
}

static v3_object_t *
v3_Object_new(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    return this;
}

static v3_base_object_t 
*v3_Object_toString(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    return v3_strobj("Object");
}
