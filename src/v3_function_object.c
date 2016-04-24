
void v3_init_Function()
{
    v3_function_object_t    function;
    v3_function_object_t    function_toFixed;
    v3_object_t             function_prototype;
    v3_function_object_t    parse_int;
    v3_function_object_t    to_string;
    
    function.name = "Function";
    function.length = 1;
    function.__proto__ = function_prototype;
    v3_object_set(function, "prototype", function_prototype);

    v3_object_set(v3_global, "Function", function);

    v3_object_set(function_prototype, "constructor", function);
    v3_object_set(function_prototype, "toString", to_string);
    v3_object_set(function_prototype, "toSource", to_source);
    v3_object_set(function_prototype, "apply", apply);
    v3_object_set(function_prototype, "call", call);
    v3_object_set(function_prototype, "bind", bind);
    function_prototype.__proto__ = Object_prototype;

    v3_object_set(function.prototype, "toFixed", v3_function_toFixed);
}

v3_function_object_t 
v3_create_native_func(v3_ctx_t *ctx, char *name, size_t arg_count, v3_func_pt *pt)
{
    v3_function_object_t *func;
    func = v3_palloc(ctx->pool, sizeof(v3_function_object_t));
    if (func == NULL) return NULL;
    func->name = name;
    func->length = arg_count;
    func->is_native = 1;
    func->native_func = pt;
    return func;
}
