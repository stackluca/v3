#include <v3_core.h>

static v3_base_object_t *
v3_Function_construct(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args);
static v3_int_t v3_init_Function_prototype(v3_ctx_t *ctx);

v3_object_t             *Function_prototype;

v3_int_t v3_init_Function(v3_ctx_t *ctx)
{
    v3_function_object_t    *Function;

    Function = v3_function_create_native(ctx, v3_strobj("Function"), 
                                    v3_numobj(1), v3_Function_construct);
    if (Function == NULL) return V3_ERROR;                                    

    v3_init_Function_prototype(ctx);
    
    v3_function_set_prototype(ctx, Function, Function_prototype);

    Function->base.base.__proto__ = Function_prototype;
    // v3_object_set((v3_object_t *)&Function, "prototype", (v3_base_object_t *)&Function_prototype);

    // v3_object_set(ctx->global, "Function", (v3_base_object_t *)Function);

    // v3_object_set((v3_object_t *)&Function_prototype, "constructor", (v3_base_object_t *)function);
    // v3_object_set((v3_object_t *)&Function_prototype, "toString", to_string);
    // v3_object_set((v3_object_t *)&Function_prototype, "toSource", to_source);
    // v3_object_set((v3_object_t *)&Function_prototype, "apply", apply);
    // v3_object_set((v3_object_t *)&Function_prototype, "call", call);
    // v3_object_set((v3_object_t *)&Function_prototype, "bind", bind);
    // function_prototype.base.__proto__ = Object_prototype;

    // v3_object_set(function.prototype, "toFixed", v3_function_toFixed);
    return V3_OK;
}

static v3_int_t v3_init_Function_prototype(v3_ctx_t *ctx)
{
    Function_prototype = v3_object_create(ctx, 5);
    if (Function_prototype == NULL) return V3_ERROR;

    // v3_object_set(ctx, Number_prototype, "toString", to_string);
    //v3_object_set(Number_prototype, "toFixed", v3_number_toFixed);
    //number_prototype.__proto__ = Object_prototype;
    return V3_OK;
}

static v3_base_object_t *
v3_Function_construct(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    return V3_NOT_SUPPORT;
}
