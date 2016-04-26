#include <v3_core.h>

v3_function_object_t    Function;
v3_object_t             Function_prototype;

void v3_init_Function(v3_ctx_t *ctx)
{
    // v3_function_object_t    function_toFixed;
    // v3_function_object_t    parse_int;
    // v3_function_object_t    to_string;
    
    // Function.name = v3_string("Function");
    Function.length = 1;
    Function.base.base.__proto__ = &Function_prototype;
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
}
