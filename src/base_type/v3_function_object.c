#include <v3_core.h>
#include <v3_eval.h>

v3_function_object_t *
v3_function_create(v3_ctx_t *ctx, v3_string_object_t *name, v3_number_object_t *arg_count)
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

    return afunc;
}

v3_function_object_t *
v3_function_create_native(v3_ctx_t *ctx, v3_string_object_t *name, v3_number_object_t *arg_count, v3_func_pt pt)
{
    v3_function_object_t *func;
    func = v3_function_create(ctx, name, arg_count);
    if (func == NULL) return NULL;

    func->is_native = 1;
    func->native_func = pt;
    return func;
}

v3_function_object_t *
v3_function_from_node(v3_ctx_t *ctx, v3_function_node_t *node, v3_list_t *scope)
{
    v3_function_object_t *func;

    if (func_node->id != NULL) {
        strobj = v3_str2string(&func_node->id->name);
    } else {
        strobj = v3_strobj("");
    }

    arg_count = func_node->params == NULL ? 0 : func_node->params->length;

    func_obj = v3_function_from_node(ctx, strobj, v3_numobj(arg_count), func_node);
    func = v3_function_create(ctx, name, arg_count);
    if (func == NULL) return NULL;

    func->func_node = node;

    call_object = v3_object_create(ctx, 5);
    if (call_object == NULL) return NULL;
    func_obj->call_obj = call_object;

    // set arguments into call_object
    CHECK_FCT(v3_object_set(call_object, v3_strobj("arguments"), argument));

    if (func_node->params != NULL) {
        for (i = 0; i < func_node->params.length; i++) {
            assert(func_node->params[i].type == V3_SYNTAX_IDENTIFIER);
            ident_node = (v3_idetifier_t *)func_node->params[i];
            v3_obj_set(call_object, v3_str2string(ident_node->name), &v3_undefined);
        }
    }

    for (i = 0; i < block->body->length; i++) {
        node = block->body[i];
        // TODO: when type is if, while, switch, try
        if (node->type == V3_SYNTAX_VARIABLE_DECLARATION) {
            var_stmt = node;

            for (i = 0; i < statement->declarations->length; i++) {
                if (statement->kind.data == dec_kind_var.data) {
                    v3_obj_set(call_object, v3_str2string(&dec[i]->id->name), &v3_undefined);
                } 
            }
        }
    }

    func_obj->scope = v3_list_clone(frame->scope);

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

