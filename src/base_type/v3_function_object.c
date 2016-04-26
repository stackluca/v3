#include <v3_core.h>


v3_function_object_t *
v3_function_create(v3_ctx_t *ctx, v3_string_object_t *name, v3_number_object_t *arg_count,
v3_func_pt *func)
{
    v3_function_object_t    *afunc;
    afunc = v3_palloc(ctx->pool, sizeof(v3_function_object_t));
    if (afunc == NULL) return NULL;
    
    memset(afunc, 0, sizeof(*afunc));

    v3_object_init(afunc, 5);

    afunc->name = name;
    afunc->length = arg_count;
    afunc->is_native = 1;
    afunc->func = func;

    afunc->__proto__ = Function_prototype;

    return afunc;
}

v3_function_object_t *
v3_create_native_func(v3_ctx_t *ctx, char *name, size_t arg_count, v3_func_pt *pt)
{
    v3_function_object_t *func;
    func = v3_palloc(ctx->pool, sizeof(v3_function_object_t));
    if (func == NULL) return NULL;

    func->name.data = name;
    func->name.length = strlen(name);
    func->length = arg_count;
    func->is_native = 1;
    // func->native_func = pt;
    return func;
}
