#include <v3_core.h>

v3_function_object_t *
v3_function_create(v3_ctx_t *ctx, v3_string_object_t *name, v3_number_object_t *arg_count,
v3_func_pt func)
{
    v3_function_object_t    *afunc;
    afunc = v3_palloc(ctx->pool, sizeof(v3_function_object_t));
    if (afunc == NULL) return NULL;
    
    memset(afunc, 0, sizeof(*afunc));

    v3_object_init(ctx, &afunc->base, 5);

    afunc->base.base.type = V3_DATA_TYPE_FUNCTION;
    afunc->base.base.__proto__ = Function_prototype;

    afunc->name = name;
    afunc->length = arg_count;
    // afunc->is_native = 1;
    // afunc->native_func = func;

    return afunc;
}

v3_function_object_t *
v3_function_create_native(v3_ctx_t *ctx, v3_string_object_t *name, v3_number_object_t *arg_count, v3_func_pt pt)
{
    v3_function_object_t *func;
    func = v3_function_create(ctx, name, arg_count, pt);
    if (func == NULL) return NULL;

    func->is_native = 1;
    func->native_func = pt;
    return func;
}

v3_base_object_t *
v3_function_apply(v3_ctx_t *ctx, v3_function_object_t *func, v3_base_object_t *this, v3_arguments_t *arguments)
{
    if (func->is_native) {
        return func->native_func(ctx, this, arguments);
    } else {
        return NULL;
    }
}
